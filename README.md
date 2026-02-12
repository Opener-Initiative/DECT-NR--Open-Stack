# C Code DECT NR+ Implementation over nRF91

This repository contains work-in-progress source code and resources for a communication system prototype implementing the **DECT-2020** standard on the **Nordic nRF91X1** System-in-Package (SiP).

The project models the standard using a **finite state machine (FSM)** and integrates the modem via the official **nrf_modem_dect_phy.h** API to ensure real-time, low-latency operation.

The code was developed by the Universidad Politécnica de Cartagena (UPCT) in cooperation with the Ostfalia University of Applied Sciences as part of the Franco-German joint research project "Media and Event production via Resilient Communication on IoT Infrastructure" (MERCI, grant number 01MJ22016D).

Please place any questions, suggestions and enhancements in the [issue tracker](https://github.com/Opener-Initiative/DECT-NR--Open-Stack/issues) or the [pull requests](https://github.com/Opener-Initiative/DECT-NR--Open-Stack/pulls).

## 🧩 Context Overview

- **Objective:** Develop a DECT NR+ C code implementation modeling ETSI TS 103 636 Parts 1–5.
- **Hardware:** Nordic nRF91X1 (processor + modem).
- **Software Architecture:**
   - FSM controlling system states and protocol flow.
   - I/O Module to inject and extract data from the network.
   - Data buffers for applications to interact with DECT NR+ network.
   - Package Core, composed of packet processor and packet generator.
   - Package queues, TX queue and RX queue.
   - Modem handler to coordinate
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
│   ├── build/                      # Sample build folder
│   ├── overlay/                    # Overlays for builds
│   ├── CMakeLists.txt
│   └── prj.conf
└── Documents/
      └── ETSI_TS_103636_xx.pdf       # Reference specifications
```

## 🛠️ Usage
Application mode, sensor or gateway, is selected in prj.conf with CONFIG_SENSOR= y / n(N meaning gateway mode)
For a simple approach to the preconfigured Demo, precompiled files for nrf9151-DevKit are available in the demoHex folder. Program *GATEWAY.hex* in the FT device and *SENSOR.hex* in the PT device that will have attached the recommended sensor.

## ⚙️ Building and Running

1. **Requirements**
    - Nordic SDK and toolchain (minimum v3.0.0)
    - DECT modem firmware (minimum v1.1.0, Nordic provides the firmware free of charge, please contact Nordic sales)
    - ~~(Optional) OLED Display SSD1306 0.91 inches~~
    - (Optional) Environmental sensor HTU21D

The DECT NR+ is supported in specific modem firmware. You need to reprogram the modem firmware on Nordic nrf91 DevKits. For example with https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-Desktop
The stack implementation uses Nordic Connect SDK (NCS for short), please see https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html how to install the NCS for Zephyr developement on Nordic hardware.  

2. **Build**
    - Compile with VS Code + nRF Connect extension, or use west/cmake.

4. **Run**
    - Use the physical hardware to view the real-time demonstrator ~~with the display~~, or run a console using nRF Connect or any serial terminal.
    - Observe FSM transitions and modem events in real time using serial terminal, and monitor data transmission over a DECT NR+ network.

## Optional Hardware Setup

### Release v0.1.0

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

## Release v0.2.0

Removed support for OLED Display, data received shall be displayed using a serial terminal.


```text
+-----------------------+
|     IoT Sensor        |
|     HTU21D            |
+-----------------------+
| VIN | GND | SCL | SDA |
+-----+-----+-----+-----+ 
    |     |     |     |
    |     |     |     |
+-----+-----+-----+-----+ 
| VDD | GND |P0.25|P0.21|
+-----------------------+ 
|        nRF9161        |
+-----------------------+
```


## 🚀 Current Features

* Basic FSM structure for DECT NR+ operation.
* Implementation of all headers defined in the Technical Specification.
* Association procedure with ID exchange and network creation.
* Plain application data transmitted and received; data sent periodically every 8 seconds.
* Example drivers for connecting peripherals to the nRF9151 SiP via I2C.
* Modular architecture ready for PHY/MAC/DLC layer expansion.

---

## 🧾 References

* [ETSI TS 103 636 Series – DECT-2020 NR Specification](https://www.etsi.org/deliver/etsi_ts/103600_103699/10363601/)
* [Nordic Semiconductor nRF9151-DevKit Documentation](https://www.nordicsemi.com/Products/Development-hardware/nRF9151-DK)
* [Nordic Semiconductor nrfConnect for Desktop](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-Desktop)
* [Nordic Semiconductor nrfConnect SDK](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html)
* [Zephyr Project](https://zephyrproject.org/)
  

