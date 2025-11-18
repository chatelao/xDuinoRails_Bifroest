#include <Arduino.h>
#include <xDuinoRails_Turnouts.h>

// Define the two-way turnout
xDuinoRails_Turnout turnout1(
    1,
    "Zweiwegweiche",
    xDuinoRails_Turnout::MOTOR_COIL,
    D1, D2, // Coil pins
    D3, D4  // Sensor pins
);

// Define the Märklin three-way turnout
xDuinoRails_ThreeWayTurnout turnout2(
    2,
    "Dreiwegweiche",
    D5, D6, // Coil pins A
    D7, D8, // Sensor pins A
    D9, D10, // Coil pins B
    D11, D12  // Sensor pins B
);

void setup() {
    Serial.begin(115200);
    Serial.println("xDuinoRails Turnout Example: Basic-2-3");

    turnout1.begin();
    turnout2.begin();

    // Initially set the three-way turnout to straight
    turnout2.setPosition(0);
}

void loop() {
    // Update the state of all turnouts
    turnout1.update();
    turnout2.update();

    // Example logic to cycle through turnout positions
    static unsigned long lastToggleTime = 0;
    static int state = 0;
    if (millis() - lastToggleTime > 5000) { // Every 5 seconds
        lastToggleTime = millis();

        switch (state) {
            case 0:
                Serial.println("Setting Zweiwegweiche to Position 1");
                turnout1.setPosition(1);
                Serial.println("Setting Dreiwegweiche to Straight (0)");
                turnout2.setPosition(0);
                break;
            case 1:
                Serial.println("Setting Zweiwegweiche to Position 2");
                turnout1.setPosition(2);
                break;
            case 2:
                 Serial.println("Setting Dreiwegweiche to Left (1)");
                turnout2.setPosition(1);
                break;
            case 3:
                Serial.println("Setting Dreiwegweiche to Right (2)");
                turnout2.setPosition(2);
                break;
        }

        state = (state + 1) % 4;
    }
}
