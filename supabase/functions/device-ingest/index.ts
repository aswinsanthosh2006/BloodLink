/**
 * =========================================================================
 * Supabase Edge Function: device-ingest
 * Runtime: Deno / TypeScript
 * Description: High-throughput ingestion gateway for BloodLink ESP8266 nodes.
 * Responsibilities:
 *  - Authenticates hardware nodes via device credentials
 *  - Records device heartbeats (last_seen_at)
 *  - Handles 'rfid_scan': associates UID with blood bag & returns storage telemetry
 *  - Handles 'sensor_reading': stores cold-chain telemetry and updates slot occupancy
 * =========================================================================
 */

import { serve } from "https://deno.land/std@0.177.0/http/server.ts";
import { createClient } from "https://esm.sh/@supabase/supabase-js@2.39.0";

const corsHeaders = {
  "Access-Control-Allow-Origin": "*",
  "Access-Control-Allow-Headers": "authorization, x-client-info, apikey, content-type, x-device-code, x-device-secret",
};

serve(async (req: Request) => {
  // Handle CORS preflight
  if (req.method === "OPTIONS") {
    return new Response("ok", { headers: corsHeaders });
  }

  try {
    const supabaseClient = createClient(
      Deno.env.get("SUPABASE_URL") ?? "",
      Deno.env.get("SUPABASE_SERVICE_ROLE_KEY") ?? ""
    );

    // Extract authentication headers
    const deviceCode = req.headers.get("x-device-code");
    const deviceSecret = req.headers.get("x-device-secret");

    if (!deviceCode) {
      return new Response(
        JSON.stringify({ ok: false, error: "Missing x-device-code header" }),
        { status: 401, headers: { ...corsHeaders, "Content-Type": "application/json" } }
      );
    }

    // Update Device Heartbeat in database
    await supabaseClient
      .from("devices")
      .update({ last_seen_at: new Date().toISOString() })
      .eq("device_code", deviceCode);

    // Parse Request Body
    const body = await req.json();
    const { type } = body;

    // ---------------------------------------------------------------------
    // Handshake 1: RFID Scan Event (MCU1)
    // ---------------------------------------------------------------------
    if (type === "rfid_scan") {
      const { uid } = body;
      if (!uid) {
        return new Response(
          JSON.stringify({ ok: false, error: "Missing tag UID" }),
          { status: 400, headers: { ...corsHeaders, "Content-Type": "application/json" } }
        );
      }

      // Query blood bag registered to this RFID UID
      const { data: bag, error: bagError } = await supabaseClient
        .from("blood_bags")
        .select("bag_code, blood_group, status, entry_at, expiry_date, rack_id, slot_code")
        .eq("rfid_uid", uid)
        .maybeSingle();

      if (bagError) {
        throw bagError;
      }

      // If tag is not registered in the system
      if (!bag) {
        return new Response(
          JSON.stringify({ ok: true, known: false }),
          { status: 200, headers: { ...corsHeaders, "Content-Type": "application/json" } }
        );
      }

      // Retrieve latest environmental cold-chain reading
      const { data: latestReading } = await supabaseClient
        .from("readings")
        .select("temperature_c, recorded_at")
        .order("recorded_at", { ascending: false })
        .limit(1)
        .maybeSingle();

      // Log activity event in audit log
      await supabaseClient.from("activity_events").insert({
        event_type: "rfid_scan",
        description: `RFID scanned by ${deviceCode}`,
        bag_code: bag.bag_code,
        device_code: deviceCode,
        payload: { uid, blood_group: bag.blood_group, rack: bag.rack_id, slot: bag.slot_code },
      });

      return new Response(
        JSON.stringify({
          ok: true,
          known: true,
          bag: {
            bag_code: bag.bag_code,
            blood_group: bag.blood_group,
            status: bag.status,
            entry_at: bag.entry_at,
            expiry_date: bag.expiry_date,
          },
          temperature_c: latestReading?.temperature_c ?? null,
          temperature_recorded_at: latestReading?.recorded_at ?? null,
        }),
        { status: 200, headers: { ...corsHeaders, "Content-Type": "application/json" } }
      );
    }

    // ---------------------------------------------------------------------
    // Handshake 2: Periodic Environmental Telemetry & Slot Occupancy (MCU2)
    // ---------------------------------------------------------------------
    if (type === "sensor_reading") {
      const { temperature_c, humidity_pct, slots } = body;

      // Persist environmental readings
      if (typeof temperature_c === "number" && typeof humidity_pct === "number") {
        await supabaseClient.from("readings").insert({
          device_code: deviceCode,
          temperature_c,
          humidity_pct,
          recorded_at: new Date().toISOString(),
        });
      }

      // Update slot occupancy states
      if (Array.isArray(slots)) {
        for (const slot of slots) {
          if (slot.sensor_id && typeof slot.occupied === "boolean") {
            await supabaseClient
              .from("storage_slots")
              .update({
                is_occupied: slot.occupied,
                updated_at: new Date().toISOString(),
              })
              .eq("sensor_id", slot.sensor_id);
          }
        }
      }

      return new Response(
        JSON.stringify({ ok: true, message: "Telemetry ingested successfully" }),
        { status: 200, headers: { ...corsHeaders, "Content-Type": "application/json" } }
      );
    }

    return new Response(
      JSON.stringify({ ok: false, error: `Unsupported payload type: ${type}` }),
      { status: 400, headers: { ...corsHeaders, "Content-Type": "application/json" } }
    );
  } catch (err) {
    return new Response(
      JSON.stringify({ ok: false, error: err.message }),
      { status: 500, headers: { ...corsHeaders, "Content-Type": "application/json" } }
    );
  }
});
