# Smart Clinic Appointment & Patient Management System (SCAPMS)

A modern desktop application built with **C++17** and **Qt 6 Widgets** designed to streamline patient records, appointment scheduling, waiting queue management, and reporting for clinical environments.

---

## 🚀 Features & Architecture

* **Desktop Dashboard:** Serves as the startup screen featuring summary cards, custom-painted doughnut charts, age distribution bars, a daily activity summary, and today's schedule.
* **Core Workflows (Sidebar Navigation):**
  * **Patient Management:** Handle patient records with support for inheritance (`GeneralPatient`, `EmergencyPatient`, `FollowUpPatient`).
  * **Appointment Scheduling:** Book, view, and track appointment statuses.
  * **Waiting Queue:** Manage patient queues dynamically via `ClinicManager`.
  * **Search & Sort:** Quick lookup and organization using the dedicated `SearchEngine`.
  * **Reports & Analytics:** Generate visual reports based on clinical data.
* **Robust File Persistence:** Fully implemented JSON-based data persistence to save and load patient and appointment records reliably between sessions.
* **Smart ID Generation:** Incorporates a "Hard Sync" strategy during startup/file load to scan for the absolute maximum existing ID, preventing unexpected ID jumps or collisions.

---

## 📸 Screenshots

Project previews and user interface walkthroughs are organized in the [`/screenshots`](./screenshots) directory:
* Main Dashboard view
* Patient & Appointment workflows
* Search and Queue interfaces

---

## 🛠️ Build & Installation

Open this folder in **Qt Creator** with a Qt 6 Widgets desktop kit (C++17), or build via command line using **CMake**:

```bash
cmake -S . -B build
cmake --build build
