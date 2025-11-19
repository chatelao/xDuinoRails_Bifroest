#include "xDuinoRails_Turnouts.h"

// Initialize the static pointers and flags
xDuinoRails_Turnout* xDuinoRails_Turnout::_active_bemf_turnout = nullptr;
bool xDuinoRails_Turnout::_bemf_motor_active = false;

// --- Constructors ---

// Original constructor for Servo and Coil
xDuinoRails_Turnout::xDuinoRails_Turnout(int id, const char* name, MotorType motorType, int pin1, int pin2, int sensorPin1, int sensorPin2, int angleMin, int angleMax)
    : _id(id), _name(name), _motorType(motorType), _sensorPin1(sensorPin1), _sensorPin2(sensorPin2),
      _state(STATE_IDLE), _targetPosition(0), _bemfEndDetected(false) {
    if (_motorType == MOTOR_SERVO) {
        new (&_motor.servo) Servo();
        _motor.servo.pin = pin1;
        _motor.servo.angleMin = angleMin;
        _motor.servo.angleMax = angleMax;
        _motor.servo.currentAngle = angleMin;
    } else if (_motorType == MOTOR_COIL) {
        _motor.coil.pin1 = pin1;
        _motor.coil.pin2 = pin2;
    }
}

// Overloaded constructor for BEMF
xDuinoRails_Turnout::xDuinoRails_Turnout(int id, const char* name, MotorType motorType, int pwm_a, int pwm_b, int bemf_a, int bemf_b)
    : _id(id), _name(name), _motorType(motorType), _sensorPin1(-1), _sensorPin2(-1),
      _state(STATE_IDLE), _targetPosition(0), _bemfEndDetected(false) {
    if (_motorType == MOTOR_COIL_BEMF) {
        _motor.bemf.pwm_a_pin = pwm_a;
        _motor.bemf.pwm_b_pin = pwm_b;
        _motor.bemf.bemf_a_pin = bemf_a;
        _motor.bemf.bemf_b_pin = bemf_b;
    }
}


void xDuinoRails_Turnout::begin() {
    if (_motorType == MOTOR_SERVO) {
        _motor.servo.servo.attach(_motor.servo.pin);
        _motor.servo.servo.write(_motor.servo.currentAngle);
        pinMode(_sensorPin1, INPUT_PULLUP);
        pinMode(_sensorPin2, INPUT_PULLUP);
    } else if (_motorType == MOTOR_COIL) {
        pinMode(_motor.coil.pin1, OUTPUT);
        pinMode(_motor.coil.pin2, OUTPUT);
        digitalWrite(_motor.coil.pin1, LOW);
        digitalWrite(_motor.coil.pin2, LOW);
        pinMode(_sensorPin1, INPUT_PULLUP);
        pinMode(_sensorPin2, INPUT_PULLUP);
    } else if (_motorType == MOTOR_COIL_BEMF) {
        hal_motor_init(_motor.bemf.pwm_a_pin, _motor.bemf.pwm_b_pin, _motor.bemf.bemf_a_pin, _motor.bemf.bemf_b_pin, on_bemf_update);
    }
}

void xDuinoRails_Turnout::setPosition(int position) {
    if (position == 1 || position == 2) {
        _targetPosition = position;
    }
}

void xDuinoRails_Turnout::stopMotor() {
    if (_motorType == MOTOR_COIL) {
        digitalWrite(_motor.coil.pin1, LOW);
        digitalWrite(_motor.coil.pin2, LOW);
    } else if (_motorType == MOTOR_COIL_BEMF) {
        hal_motor_set_pwm(0, false);
        _active_bemf_turnout = nullptr;
        _bemf_motor_active = false;
    }
    _state = STATE_IDLE;
}

void xDuinoRails_Turnout::on_bemf_update(int raw_bemf) {
    if (_active_bemf_turnout) {
        // Simple stall detection: if BEMF is below a threshold for some time
        static int stall_count = 0;
        if (raw_bemf < 10) { // This threshold needs tuning
            stall_count++;
        } else {
            stall_count = 0;
        }

        if (stall_count > 5) { // This count needs tuning
            _active_bemf_turnout->_bemfEndDetected = true;
            stall_count = 0;
        }
    }
}

void xDuinoRails_Turnout::update() {
    bool sensor1_active = false;
    bool sensor2_active = false;

    if (_motorType != MOTOR_COIL_BEMF) {
        sensor1_active = digitalRead(_sensorPin1) == LOW;
        sensor2_active = digitalRead(_sensorPin2) == LOW;
    }

    switch (_state) {
        case STATE_IDLE:
            if (_targetPosition == 1 && !sensor1_active) {
                if (_motorType == MOTOR_COIL_BEMF && _bemf_motor_active) return; // Concurrency lock

                _state = STATE_MOVING_TO_POS1;
                _moveStartTime = millis();
                if (_motorType == MOTOR_COIL_BEMF) {
                    _active_bemf_turnout = this;
                    _bemf_motor_active = true;
                    _bemfEndDetected = false;
                    hal_motor_set_pwm(255, true); // Full speed, forward
                }
                Serial.print("Bewegung gestartet: ");
                Serial.println(_name);
            } else if (_targetPosition == 2 && !sensor2_active) {
                if (_motorType == MOTOR_COIL_BEMF && _bemf_motor_active) return; // Concurrency lock

                _state = STATE_MOVING_TO_POS2;
                _moveStartTime = millis();
                if (_motorType == MOTOR_COIL_BEMF) {
                    _active_bemf_turnout = this;
                    _bemf_motor_active = true;
                    _bemfEndDetected = false;
                    hal_motor_set_pwm(255, false); // Full speed, reverse
                }
                Serial.print("Bewegung gestartet: ");
                Serial.println(_name);
            }
            break;

        case STATE_MOVING_TO_POS1:
            if ((_motorType != MOTOR_COIL_BEMF && sensor1_active) || (_motorType == MOTOR_COIL_BEMF && _bemfEndDetected)) {
                stopMotor();
                Serial.print("Position 1 erreicht: ");
                Serial.println(_name);
            } else if (millis() - _moveStartTime > TIMEOUT_MS) {
                stopMotor();
                Serial.print("Timeout: ");
                Serial.println(_name);
            } else {
                if (_motorType == MOTOR_SERVO) {
                    if (millis() - _lastMoveTime > SERVO_STEP_DELAY) {
                        if (_motor.servo.currentAngle > _motor.servo.angleMin) {
                            _motor.servo.currentAngle--;
                            _motor.servo.servo.write(_motor.servo.currentAngle);
                        }
                        _lastMoveTime = millis();
                    }
                } else if (_motorType == MOTOR_COIL) {
                    if (millis() - _lastMoveTime > (COIL_PULSE_ON_MS + COIL_PULSE_OFF_MS)) {
                        digitalWrite(_motor.coil.pin1, HIGH);
                        _lastMoveTime = millis();
                    }
                    if (millis() - _lastMoveTime > COIL_PULSE_ON_MS) {
                        digitalWrite(_motor.coil.pin1, LOW);
                    }
                }
            }
            break;

        case STATE_MOVING_TO_POS2:
            if ((_motorType != MOTOR_COIL_BEMF && sensor2_active) || (_motorType == MOTOR_COIL_BEMF && _bemfEndDetected)) {
                stopMotor();
                Serial.print("Position 2 erreicht: ");
                Serial.println(_name);
            } else if (millis() - _moveStartTime > TIMEOUT_MS) {
                stopMotor();
                Serial.print("Timeout: ");
                Serial.println(_name);
            } else {
                if (_motorType == MOTOR_SERVO) {
                    if (millis() - _lastMoveTime > SERVO_STEP_DELAY) {
                        if (_motor.servo.currentAngle < _motor.servo.angleMax) {
                            _motor.servo.currentAngle++;
                            _motor.servo.servo.write(_motor.servo.currentAngle);
                        }
                        _lastMoveTime = millis();
                    }
                } else if (_motorType == MOTOR_COIL) {
                    if (millis() - _lastMoveTime > (COIL_PULSE_ON_MS + COIL_PULSE_OFF_MS)) {
                        digitalWrite(_motor.coil.pin2, HIGH);
                        _lastMoveTime = millis();
                    }
                    if (millis() - _lastMoveTime > COIL_PULSE_ON_MS) {
                        digitalWrite(_motor.coil.pin2, LOW);
                    }
                }
            }
            break;
    }
}

// --- ThreeWayTurnout Implementation ---
// (This part remains unchanged as it uses standard coil turnouts)
xDuinoRails_ThreeWayTurnout::xDuinoRails_ThreeWayTurnout(
    int id, const char* name,
    int coilPin1_A, int coilPin2_A, int sensorPin1_A, int sensorPin2_A,
    int coilPin1_B, int coilPin2_B, int sensorPin1_B, int sensorPin2_B)
    : _id(id), _name(name), _targetPosition(0)
{
    _turnoutA = new xDuinoRails_Turnout(id * 10 + 1, "3-Way-A", xDuinoRails_Turnout::MOTOR_COIL, coilPin1_A, coilPin2_A, sensorPin1_A, sensorPin2_A);
    _turnoutB = new xDuinoRails_Turnout(id * 10 + 2, "3-Way-B", xDuinoRails_Turnout::MOTOR_COIL, coilPin1_B, coilPin2_B, sensorPin1_B, sensorPin2_B);
}

xDuinoRails_ThreeWayTurnout::~xDuinoRails_ThreeWayTurnout() {
    delete _turnoutA;
    delete _turnoutB;
}

void xDuinoRails_ThreeWayTurnout::begin() {
    _turnoutA->begin();
    _turnoutB->begin();
}

void xDuinoRails_ThreeWayTurnout::setPosition(int position) {
    if (position >= 0 && position <= 2) {
        _targetPosition = position;
        switch (_targetPosition) {
            case 0: // Straight
                _turnoutA->setPosition(1);
                _turnoutB->setPosition(1);
                break;
            case 1: // Left
                _turnoutA->setPosition(2);
                _turnoutB->setPosition(1);
                break;
            case 2: // Right
                _turnoutA->setPosition(1);
                _turnoutB->setPosition(2);
                break;
        }
    }
}

void xDuinoRails_ThreeWayTurnout::update() {
    _turnoutA->update();
    _turnoutB->update();
}
