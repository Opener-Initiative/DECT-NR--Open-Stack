# C Code DECT NR+ Implementation over nRF9161

This repository contains work-in-progress source code and resources for a communication system prototype implementing the **DECT-2020** standard on the **Nordic nRF9161** System-in-Package (SiP).

The project models the standard using a **finite state machine (FSM)** and integrates the modem via the official **nRF Modem API** to ensure real-time, low-latency operation.

## 🧩 Context Overview

- **Objective:** Develop a DECT NR+ C code implementation modeling ETSI TS 103 636 Parts 1–5.
- **Hardware:** Nordic nRF9161 (processor + LTE modem).
- **Software Architecture:**
   - FSM controlling system states and protocol flow.
   - Dedicated threads for RX/TX and modem handling.
   - Event queues, semaphores, and work queues for synchronization.
   - Packet header definitions and functions for assembling and disassembling.
   - Functions to pack and unpack DECT messages with multiple headers.
- **Focus:** Create a solid base for a telecommunication technology capable of transmitting information in defined scenarios, implementing characteristics from ETSI TS 103 636.
- **Current demos:** IoT Transmission Data

## 📂 Repository Structure

```text
├── DECT-Release/
│   ├── src
│   │   ├── app                     # Entry point – system init, FSM start
│   │   ├── common                  # Tools and auxiliary functions
│   │   ├── config                  # Configuration files
│   │   ├── drivers                 # Example drivers
│   │   ├── fsm                     # FSM logic and routines
│   │   ├── headers                 # DECT NR+ header formatting
│   │   ├── protocol                # Protocol procedures
│   │   └── main.c
│   ├── build/                      # Sample build configuration
│   ├── overlay/                    # Overlays for builds
│   ├── CMakeLists.txt
│   ├── prj.conf
│   └── htu21d_sensor.overlay
└── Documents/
      └── ETSI_TS_103636_xx.pdf       # Reference specifications
```

## 🛠️ Usage

Download the file attached to the release and upload it to the nRF9161DK using the nRF Connect Programmer application. Ensure that the board is pre-flashed with the DECT modem firmware for proper operation.

## ⚙️ Building and Running

1. **Requirements**
    - Nordic SDK and toolchain (minimum v2.5.0)
    - DECT modem firmware (minimum v0.5.0-110 prealpha)
    - (Optional) OLED Display SSD1306 0.91 inches
    - (Optional) Environmental sensor HTU21D

2. **Build**
    Compile with VS Code + nRF Connect extension, or use west/cmake.

3. **Run**
    - Use the physical hardware to view the real-time demonstrator with the display, or run a console using nRF Connect or any serial terminal.
    - Observe FSM transitions and modem events in real time, and monitor data transmission over a DECT NR+ network.

## Optional Hardware Setup

Although the demo sends dummy data by default, it is recommended to connect a real data source to test the demonstrator’s functionalities. To connect both display and sensor, use the diagram below or configure as needed:

```text
+-----------------------+        +-----------------------+
|   OLED Display        |        |     IoT Sensor        |
|   SSD1306             |        |     HTU21D            |
+-----------------------+        +-----------------------+
| VCC | GND | SCL | SDA |        | VIN | GND | SCL | SDA |
+-----+-----+-----+-----+        +-----+-----+-----+-----+ 
    |     |     |     |              |     |     |     |
    |     |     |     |              |     |     |     |
+-----+-----+-----+-----+        +-----+-----+-----+-----+ 
| 5V  | GND |P0.31|P0.30|        | VDD | GND |P0.2 |P0.28|
+-----------------------+        +-----------------------+ 
|        nRF9161        |        |        nRF9161        |
+-----------------------+        +-----------------------+
```

With this setup, you can view association updates on the display and use real data collected by the HTU21D sensor.

## 🚀 Current Features

* Basic FSM structure for DECT NR+ operation.
* Implementation of all headers defined in the Technical Specification.
* Association procedure with ID exchange and network creation.
* Plain application data transmitted and received; data sent periodically every 5 seconds.
* Example drivers for connecting peripherals to the nRF9161 SiP via I2C.
* Modular architecture ready for PHY/MAC/DLC layer expansion.

---

## 🧾 References

* [ETSI TS 103 636 Series – DECT-2020 NR Specification](https://www.etsi.org/deliver/etsi_ts/103600_103699/10363601/)
* [Nordic Semiconductor nRF9161 Documentation](https://www.nordicsemi.com/Products/nRF9161)
* [Zephyr Project](https://zephyrproject.org/)

