-- =========================================================================
-- BloodLink Database Schema Initialization
-- Platform: Supabase / PostgreSQL 15+
-- =========================================================================

-- Enable UUID extension
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "pgcrypto";

-- -------------------------------------------------------------------------
-- 1. Devices Table
-- Tracks edge microcontroller nodes, authentication credentials, and heartbeats
-- -------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS public.devices (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    device_code TEXT NOT NULL UNIQUE,
    device_name TEXT,
    secret_hash TEXT,
    status TEXT DEFAULT 'active',
    last_seen_at TIMESTAMPTZ,
    created_at TIMESTAMPTZ DEFAULT now()
);

-- Seed Initial Hardware Nodes
INSERT INTO public.devices (device_code, device_name, status)
VALUES 
    ('ESP8266-RFID-01', 'BloodLink RFID & OLED Controller', 'active'),
    ('ESP8266-SENSOR-01', 'BloodLink Environmental & Slot Sensor Node', 'active')
ON CONFLICT (device_code) DO NOTHING;

-- -------------------------------------------------------------------------
-- 2. Storage Slots Table
-- Defines physical rack layout, slot identities, and optical IR sensor mappings
-- -------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS public.storage_slots (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    rack_id TEXT NOT NULL DEFAULT 'RACK-1',
    slot_code TEXT NOT NULL,
    assigned_blood_group TEXT,
    sensor_id TEXT,
    is_occupied BOOLEAN DEFAULT false,
    updated_at TIMESTAMPTZ DEFAULT now(),
    CONSTRAINT uq_rack_slot UNIQUE (rack_id, slot_code)
);

-- Seed 4-Compartment Demonstration Model
INSERT INTO public.storage_slots (rack_id, slot_code, assigned_blood_group, sensor_id, is_occupied)
VALUES
    ('RACK-1', 'S1-01', 'O+', 'IR-01', false),
    ('RACK-1', 'S1-02', 'A+', 'IR-02', false),
    ('RACK-1', 'S2-01', 'B+', NULL, false),
    ('RACK-1', 'S2-02', 'AB+', NULL, false)
ON CONFLICT (rack_id, slot_code) DO NOTHING;

-- -------------------------------------------------------------------------
-- 3. Blood Bags Inventory Table
-- Core inventory repository linking physical RFID UID to clinical blood unit data
-- -------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS public.blood_bags (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    bag_code TEXT NOT NULL UNIQUE,
    rfid_uid TEXT NOT NULL UNIQUE,
    blood_group TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'available', -- 'available', 'reserved', 'issued', 'expired'
    rack_id TEXT DEFAULT 'RACK-1',
    slot_code TEXT,
    entry_at TIMESTAMPTZ DEFAULT now(),
    expiry_date DATE NOT NULL,
    donor_id TEXT,
    created_at TIMESTAMPTZ DEFAULT now(),
    updated_at TIMESTAMPTZ DEFAULT now()
);

-- Seed Demonstration Blood Bag (BB01)
INSERT INTO public.blood_bags (bag_code, rfid_uid, blood_group, status, rack_id, slot_code, expiry_date)
VALUES 
    ('BB01', '93 2A 14 0B', 'O+', 'available', 'RACK-1', 'S1-01', CURRENT_DATE + INTERVAL '35 days')
ON CONFLICT (bag_code) DO NOTHING;

-- -------------------------------------------------------------------------
-- 4. Sensor Readings Telemetry Table
-- Time-series log of cold-chain temperature and ambient humidity
-- -------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS public.readings (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    device_code TEXT REFERENCES public.devices(device_code) ON DELETE SET NULL,
    temperature_c NUMERIC(5, 2) NOT NULL,
    humidity_pct NUMERIC(5, 2) NOT NULL,
    recorded_at TIMESTAMPTZ DEFAULT now()
);

CREATE INDEX IF NOT EXISTS idx_readings_recorded_at ON public.readings(recorded_at DESC);

-- -------------------------------------------------------------------------
-- 5. Activity Log Table
-- Immutable audit trail capturing RFID scans, stock transactions, and device events
-- -------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS public.activity_events (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    event_type TEXT NOT NULL, -- 'rfid_scan', 'slot_update', 'telemetry', 'alert'
    description TEXT NOT NULL,
    bag_code TEXT,
    device_code TEXT,
    payload JSONB,
    created_at TIMESTAMPTZ DEFAULT now()
);

CREATE INDEX IF NOT EXISTS idx_activity_events_created_at ON public.activity_events(created_at DESC);

-- -------------------------------------------------------------------------
-- 6. Alerts Table
-- System exceptions, cold-chain threshold violations, and expiry alerts
-- -------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS public.alerts (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    alert_type TEXT NOT NULL, -- 'temperature_exceeded', 'near_expiry', 'device_offline'
    severity TEXT NOT NULL DEFAULT 'warning', -- 'info', 'warning', 'critical'
    message TEXT NOT NULL,
    resolved BOOLEAN DEFAULT false,
    created_at TIMESTAMPTZ DEFAULT now()
);

-- =========================================================================
-- ROW LEVEL SECURITY (RLS) POLICIES
-- =========================================================================

ALTER TABLE public.devices ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.storage_slots ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.blood_bags ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.readings ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.activity_events ENABLE ROW LEVEL SECURITY;
ALTER TABLE public.alerts ENABLE ROW LEVEL SECURITY;

-- Allow anonymous/authenticated read access for web dashboard consumption
CREATE POLICY "Public Read Access on devices" ON public.devices FOR SELECT USING (true);
CREATE POLICY "Public Read Access on storage_slots" ON public.storage_slots FOR SELECT USING (true);
CREATE POLICY "Public Read Access on blood_bags" ON public.blood_bags FOR SELECT USING (true);
CREATE POLICY "Public Read Access on readings" ON public.readings FOR SELECT USING (true);
CREATE POLICY "Public Read Access on activity_events" ON public.activity_events FOR SELECT USING (true);
CREATE POLICY "Public Read Access on alerts" ON public.alerts FOR SELECT USING (true);

-- Mutations are restricted to service_role (used by Supabase Edge Functions)
CREATE POLICY "Service Role Mutations on devices" ON public.devices FOR ALL USING (auth.role() = 'service_role');
CREATE POLICY "Service Role Mutations on storage_slots" ON public.storage_slots FOR ALL USING (auth.role() = 'service_role');
CREATE POLICY "Service Role Mutations on blood_bags" ON public.blood_bags FOR ALL USING (auth.role() = 'service_role');
CREATE POLICY "Service Role Mutations on readings" ON public.readings FOR ALL USING (auth.role() = 'service_role');
CREATE POLICY "Service Role Mutations on activity_events" ON public.activity_events FOR ALL USING (auth.role() = 'service_role');
CREATE POLICY "Service Role Mutations on alerts" ON public.alerts FOR ALL USING (auth.role() = 'service_role');
