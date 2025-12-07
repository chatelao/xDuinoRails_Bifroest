# Feature Comparison

This document provides a detailed comparison between `xDuinoRails` and major commercial turnout decoders such as the ESU SwitchPilot, Viessmann 5212, Roco 10775, and others.

## Comparison Matrix

| Feature | **xDuinoRails** | **ESU SwitchPilot 1/2** | **Viessmann 5212/5211** | **Roco 10775 / Z21 Switch** | **Märklin m83** | **Digikeijs DR4018** |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Primary Use** | Turnout Control System (Custom) | DCC/MM Accessory Decoder | DCC/MM Accessory Decoder | DCC Accessory Decoder | DCC/MM/mfx Decoder | DCC Accessory Decoder |
| **Motor Types** | **Unified Servo & Dual-Coil** | Dual-Coil (Servo via extension/V2) | Dual-Coil (Separate 5280 for Servo) | Dual-Coil (Z21 can do generic) | Dual-Coil (Servo via 60821) | Dual-Coil / Lights / Generic |
| **Outputs** | Scalable (RP2040 GPIO dependent) | 4x Transistor (Coil) + 2x Servo | 4x Dual-Coil (8 outputs) | 8x Outputs (4 Turnouts) | 4x Turnouts (8 outputs) | 16x Outputs (8 Turnouts) |
| **Position Feedback** | **BEMF (Sensorless)** or End-Switches | RailCom (Digital Ack) | None (Simple) | RailCom (Z21) | mfx (Auto-registration) | None / Simple |
| **BEMF Detection** | **Yes (Unique Feature)** | No (Current sensing only for overload) | No | No | No | No |
| **Control Logic** | Non-blocking State Machine | Firmware (Black box) | Hardware/Firmware | Firmware | Firmware | Firmware |
| **Servo Movement** | Smooth "Soft Switch" | Smooth (Configurable speed) | N/A (requires specific module) | N/A (requires specific module) | N/A (requires adapter) | Basic / Presets |
| **Coil Activation** | **Pulsed with OFF-window (for BEMF)** | Adjustable Pulse Time | Pulse | Adjustable Pulse Time | Adjustable Pulse Time | Presets |
| **3-Way Turnout** | **Native Class (Composite)** | Configurable via mapping | Configurable wiring | Configurable wiring | Configurable wiring | Configurable presets |
| **Protection** | Timeout & Stall Detection | Overload / Short Circuit | Short Circuit | Short Circuit | Short Circuit | Short Circuit |
| **Configurability** | **Full C++ Source Code** | CV Programming / Buttons | CVs / Switches | CVs / Z21 App | mfx / CVs / Switches | CVs / Presets |

## Key Differentiators

### 1. Sensorless BEMF Feedback
This is the most significant differentiator. While commercial decoders use RailCom or mfx to report "command received" or sometimes current draw, `xDuinoRails` actively uses Back-EMF (BEMF) to detect if the solenoid slug has actually moved, without needing external microswitches. This is a feature typically found in high-end locomotive decoders, not turnout decoders.

### 2. Unified Architecture
`xDuinoRails` handles both Servos and Coils natively in the same class structure, whereas competitors often require separate modules (e.g., Viessmann 5212 vs 5280) or adapters (Märklin 60821) to mix motor types.

### 3. Open Flexibility
As a library, it offers infinite customization of the logic (e.g., custom lighting sequences linked to turnouts) compared to the fixed CV-based options of commercial units.

## Missing Features (vs. Commercial)

*   **Integrated DCC/MM Decoding:** `xDuinoRails` is currently a control library. It does not have a built-in DCC signal decoder (listening to track packets) implemented in the example, meaning it acts as a controller that needs an interface (like Serial/USB or a separate DCC library) to receive commands, whereas the competitors connect directly to the track.
*   **Integrated Power Stages:** Commercial units come with integrated power stages (MOSFETs/Relays) on board. `xDuinoRails` requires external driver hardware (e.g., L298N, Darlington arrays) to drive the coils.
*   **Plug-and-Play Configuration:** Commercial units can be configured via programming tracks or buttons (CVs). `xDuinoRails` relies on compiling C++ code for configuration.
