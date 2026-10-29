# BloodLink Web Dashboard

The web management dashboard for **BloodLink** provides centralized inventory visibility, rack slot occupancy status, cold-chain temperature telemetry, and device health monitoring.

---

## 🌐 Live Deployment

* **URL**: [https://blood-link-ashy.vercel.app](https://blood-link-ashy.vercel.app)
* **Hosting Platform**: Vercel
* **Frontend Framework**: React (Single Page Application)
* **Backend Integration**: Supabase JavaScript Client (PostgreSQL PostgREST + Realtime subscriptions)

---

## 🖥️ User Interface Overview

| View / Module | Visual Demonstration | Core Functions |
| :--- | :--- | :--- |
| **Storage Overview** | ![Overview](../../docs/images/dashboard_overview.jpeg) | Metric cards for total registered units, available units, units expiring within 7 days, and current cold-chain temperature. |
| **Storage Slots** | ![Storage](../../docs/images/dashboard_storage.jpeg) | Visual grid of rack compartments (`RACK-1` slots `S1-01` to `S2-02`) displaying real-time occupancy reported by IR optical sensors. |
| **Activity Log** | ![Activity Log](../../docs/images/dashboard_activity_log.jpeg) | Timestamped audit trail tracking each physical RFID tag scan, recording device, and affected inventory unit. |
| **Connectivity** | ![Connectivity](../../docs/images/dashboard_connectivity.jpeg) | Real-time device health monitor calculating active connection status based on heartbeat updates received by Supabase. |

---

## ⚙️ Architecture & Data Synchronization

1. **Inventory Queries**: The dashboard queries Supabase tables (`blood_bags`, `storage_slots`, `readings`, `devices`) using read-only public anon keys backed by Row Level Security (RLS).
2. **Search & Filter**: Allows operators to locate units by bag code (e.g., `BB01`), blood group (`O+`, `A+`, etc.), or assigned compartment slot.
3. **Heartbeat Visibility**: Highlights edge node status (`0/2 devices active` or `2/2 devices active`) to immediately alert clinic technicians if a sensor node loses Wi-Fi connectivity.
