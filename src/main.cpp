#include <Arduino.h>
#include <xDuinoRails_Turnouts.h>
#include <NmraDcc.h>

// DCC Pin Definition
#define DCC_PIN D0

// Initialize the DCC object
NmraDcc Dcc;

// Define the two-way turnout with sensors
// Note: Pin definitions are for the Seeed XIAO RP2040
// Using D1, D2 for Coils, D3, D4 for Sensors
xDuinoRails_Turnout turnout1(
    1,
    "Zweiwegweiche",
    xDuinoRails_Turnout::MOTOR_COIL,
    D1, D2, // Coil pins
    D3, D4  // Sensor pins
);

// Define the BEMF-controlled turnout using the new config struct
BEMF_Config bemf_config = {
    .pwm_a_pin = D7,
    .pwm_b_pin = D8,
    .bemf_a_pin = A2, // Must be an ADC pin. Using A2 to avoid conflict with turnout2
    .bemf_b_pin = A3  // Must be an ADC pin. Using A3 to avoid conflict with turnout2
};
xDuinoRails_Turnout turnout3(
    3,
    "BEMF-Weiche",
    bemf_config
);

// Commented out to avoid pin conflicts during DCC testing
// Define the Märklin three-way turnout
/*
xDuinoRails_ThreeWayTurnout turnout2(
    2,
    "Dreiwegweiche",
    D5, D6,    // Coil pins A
    A3, A2, // Sensor pins A
    D9, D10,   // Coil pins B
    A1, A0  // Sensor pins B
);
*/


// Callback for DCC Accessory Packet
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {
    Serial.print("DCC Command - Addr: ");
    Serial.print(Addr);
    Serial.print(", Dir: ");
    Serial.print(Direction);
    Serial.print(", Power: ");
    Serial.println(OutputPower);

    // Only act on "Output On" commands (OutputPower = 1) if using momentary switches,
    // but for turnouts, usually the command comes with OutputPower=1.
    if (OutputPower == 1) {
        if (Addr == 1) {
            Serial.print("Switching Turnout 1 to ");
            Serial.println(Direction ? "Straight (0)" : "Turn (1)");
            // Note: NMRA direction 0/1 mapping to setPosition 0/1 depends on wiring
            // Usually 1=Thrown (Turn), 0=Closed (Straight)
            // But let's assume Direction 1 corresponds to Position 1, Direction 0 to Position 0.
            // Check library spec: Direction 1 is "Activate", 0 is "Deactivate"?
            // Actually, for Basic Accessory Decoder Packet:
            // The least significant bit of the address + the CD bit determines the output.
            // NmraDcc library abstracts this.
            // Direction: 1 = "On/Closed/Green", 0 = "Off/Thrown/Red" (standard varies).

            // Let's just map Direction 0 -> Position 0, Direction 1 -> Position 1.
            turnout1.setPosition(Direction ? 1 : 0);
        } else if (Addr == 3) {
            Serial.print("Switching Turnout 3 to ");
            Serial.println(Direction ? "Position 2" : "Position 1");
            turnout3.setPosition(Direction ? 2 : 1);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("xDuinoRails Turnout Example: RP2040 with DCC");

    // Configure DCC
    // Setup the DCC Pin with pullup enabled
    Dcc.pin(0, DCC_PIN, 1);

    // Initialize DCC with Manufacturer ID, Version, Flags, and OpsMode Address
    Dcc.init(MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0);

    Serial.print("DCC Initialized on Pin ");
    Serial.println(DCC_PIN);

    turnout1.begin();
    //turnout2.begin();
    turnout3.enablePolling(true);
    turnout3.begin();
}

void loop() {
    // Process DCC commands
    Dcc.process();

    // Update the state of all turnouts
    turnout1.update();
    //turnout2.update();
    turnout3.update();

    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime > 1000) {
        Serial.print("Turnout 3 Position: ");
        Serial.println(turnout3.getPosition());
        lastPrintTime = millis();
    }
}
