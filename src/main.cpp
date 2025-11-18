#include <Arduino.h>
#include <Servo.h>

// Define motor types
enum MotorType {
  MOTOR_SERVO,
  MOTOR_COIL
};

// Define the Turnout structure
struct Turnout {
  int id;
  const char* name;
  MotorType motorType;

  union {
    struct {
      int pin;
      int angleMin;
      int angleMax;
      int currentAngle;
      Servo servo;
    } servo;
    struct {
      int pin1;
      int pin2;
    } coil;
  } motor;

  int sensorPin1;
  int sensorPin2;

  enum State {
    STATE_IDLE,
    STATE_MOVING_TO_POS1,
    STATE_MOVING_TO_POS2
  } state;

  int targetPosition;

  unsigned long lastMoveTime;
  unsigned long moveStartTime;
};

// --- Function Prototypes ---
void setup();
void loop();
void updateTurnout(Turnout* turnout);
void setTurnoutPosition(Turnout* turnout, int position);

// --- Constants ---
const unsigned long TIMEOUT_MS = 5000; // 5 seconds timeout for motor movement
const int SERVO_STEP_DELAY = 20; // ms between servo steps
const int COIL_PULSE_ON_MS = 50; // ms for coil pulse
const int COIL_PULSE_OFF_MS = 150; // ms between coil pulses

// --- Turnout Definitions ---
const int numTurnouts = 2;
Turnout turnouts[numTurnouts] = {
  {
    .id = 1,
    .name = "Weiche 1 (Servo)",
    .motorType = MOTOR_SERVO,
    .motor = { .servo = { .pin = D3, .angleMin = 30, .angleMax = 150, .currentAngle = 30 } },
    .sensorPin1 = D4,
    .sensorPin2 = D5,
    .state = Turnout::STATE_IDLE,
    .targetPosition = 0
  },
  {
    .id = 2,
    .name = "Weiche 2 (Spule)",
    .motorType = MOTOR_COIL,
    .motor = { .coil = { .pin1 = D6, .pin2 = D7 } },
    .sensorPin1 = D8,
    .sensorPin2 = D9,
    .state = Turnout::STATE_IDLE,
    .targetPosition = 0
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Turnout Control System Initializing");

  for (int i = 0; i < numTurnouts; ++i) {
    if (turnouts[i].motorType == MOTOR_SERVO) {
      turnouts[i].motor.servo.servo.attach(turnouts[i].motor.servo.pin);
      turnouts[i].motor.servo.servo.write(turnouts[i].motor.servo.currentAngle);
    } else {
      pinMode(turnouts[i].motor.coil.pin1, OUTPUT);
      pinMode(turnouts[i].motor.coil.pin2, OUTPUT);
      digitalWrite(turnouts[i].motor.coil.pin1, LOW);
      digitalWrite(turnouts[i].motor.coil.pin2, LOW);
    }
    pinMode(turnouts[i].sensorPin1, INPUT_PULLUP);
    pinMode(turnouts[i].sensorPin2, INPUT_PULLUP);
  }
}

void loop() {
  for (int i = 0; i < numTurnouts; ++i) {
    updateTurnout(&turnouts[i]);
  }

  static unsigned long lastToggleTime = 0;
  static int target = 1;
  if (millis() - lastToggleTime > 7000) {
    Serial.print("Setting target to ");
    Serial.println(target);
    setTurnoutPosition(&turnouts[0], target);
    setTurnoutPosition(&turnouts[1], target);
    target = (target == 1) ? 2 : 1;
    lastToggleTime = millis();
  }
}

void stopMotor(Turnout* turnout) {
  if (turnout->motorType == MOTOR_COIL) {
    digitalWrite(turnout->motor.coil.pin1, LOW);
    digitalWrite(turnout->motor.coil.pin2, LOW);
  }
  // For servos, stopping is just not sending more write commands
  turnout->state = Turnout::STATE_IDLE;
}


void updateTurnout(Turnout* turnout) {
  bool sensor1_active = digitalRead(turnout->sensorPin1) == LOW;
  bool sensor2_active = digitalRead(turnout->sensorPin2) == LOW;

  // State machine logic
  switch (turnout->state) {
    case Turnout::STATE_IDLE:
      if (turnout->targetPosition == 1 && !sensor1_active) {
        turnout->state = Turnout::STATE_MOVING_TO_POS1;
        turnout->moveStartTime = millis();
        Serial.print("Bewegung gestartet: ");
        Serial.println(turnout->name);
      } else if (turnout->targetPosition == 2 && !sensor2_active) {
        turnout->state = Turnout::STATE_MOVING_TO_POS2;
        turnout->moveStartTime = millis();
        Serial.print("Bewegung gestartet: ");
        Serial.println(turnout->name);
      }
      break;

    case Turnout::STATE_MOVING_TO_POS1:
      if (sensor1_active) {
        stopMotor(turnout);
        Serial.print("Position 1 erreicht: ");
        Serial.println(turnout->name);
      } else if (millis() - turnout->moveStartTime > TIMEOUT_MS) {
        stopMotor(turnout);
        Serial.print("Timeout: ");
        Serial.println(turnout->name);
      } else {
        // Continue moving
        if (turnout->motorType == MOTOR_SERVO) {
          if (millis() - turnout->lastMoveTime > SERVO_STEP_DELAY) {
            if (turnout->motor.servo.currentAngle > turnout->motor.servo.angleMin) {
              turnout->motor.servo.currentAngle--;
              turnout->motor.servo.servo.write(turnout->motor.servo.currentAngle);
            }
            turnout->lastMoveTime = millis();
          }
        } else { // MOTOR_COIL
          if (millis() - turnout->lastMoveTime > (COIL_PULSE_ON_MS + COIL_PULSE_OFF_MS)) {
            digitalWrite(turnout->motor.coil.pin1, HIGH);
            turnout->lastMoveTime = millis();
          }
          if (millis() - turnout->lastMoveTime > COIL_PULSE_ON_MS) {
            digitalWrite(turnout->motor.coil.pin1, LOW);
          }
        }
      }
      break;

    case Turnout::STATE_MOVING_TO_POS2:
      if (sensor2_active) {
        stopMotor(turnout);
        Serial.print("Position 2 erreicht: ");
        Serial.println(turnout->name);
      } else if (millis() - turnout->moveStartTime > TIMEOUT_MS) {
        stopMotor(turnout);
        Serial.print("Timeout: ");
        Serial.println(turnout->name);
      } else {
        // Continue moving
        if (turnout->motorType == MOTOR_SERVO) {
          if (millis() - turnout->lastMoveTime > SERVO_STEP_DELAY) {
            if (turnout->motor.servo.currentAngle < turnout->motor.servo.angleMax) {
              turnout->motor.servo.currentAngle++;
              turnout->motor.servo.servo.write(turnout->motor.servo.currentAngle);
            }
            turnout->lastMoveTime = millis();
          }
        } else { // MOTOR_COIL
          if (millis() - turnout->lastMoveTime > (COIL_PULSE_ON_MS + COIL_PULSE_OFF_MS)) {
            digitalWrite(turnout->motor.coil.pin2, HIGH);
            turnout->lastMoveTime = millis();
          }
          if (millis() - turnout->lastMoveTime > COIL_PULSE_ON_MS) {
            digitalWrite(turnout->motor.coil.pin2, LOW);
          }
        }
      }
      break;
  }
}

void setTurnoutPosition(Turnout* turnout, int position) {
  turnout->targetPosition = position;
}
