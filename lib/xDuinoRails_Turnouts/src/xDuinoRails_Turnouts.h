#ifndef xDuinoRails_Turnouts_h
#define xDuinoRails_Turnouts_h

#include <Arduino.h>
#include <Servo.h>

class xDuinoRails_Turnout {
public:
    enum MotorType {
        MOTOR_SERVO,
        MOTOR_COIL
    };

    // Unified constructor
    xDuinoRails_Turnout(int id, const char* name, MotorType motorType, int pin1, int pin2, int sensorPin1, int sensorPin2, int angleMin = 30, int angleMax = 150);

    void begin();
    void update();
    void setPosition(int position); // 1 for position 1, 2 for position 2

private:
    enum State {
        STATE_IDLE,
        STATE_MOVING_TO_POS1,
        STATE_MOVING_TO_POS2
    };

    void stopMotor();

    // General properties
    int _id;
    const char* _name;
    MotorType _motorType;
    State _state;
    int _targetPosition; // 0: unset, 1: pos1, 2: pos2

    // Motor-specific data
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
    } _motor;

    // Sensor pins
    int _sensorPin1;
    int _sensorPin2;

    // Timing
    unsigned long _moveStartTime;
    unsigned long _lastMoveTime;

    // Constants
    static const unsigned long TIMEOUT_MS = 5000;
    static const int SERVO_STEP_DELAY = 20;
    static const int COIL_PULSE_ON_MS = 50;
    static const int COIL_PULSE_OFF_MS = 150;
};

class xDuinoRails_ThreeWayTurnout {
public:
    xDuinoRails_ThreeWayTurnout(
        int id,
        const char* name,
        // Pins for the first coil pair (e.g., for 'left' position)
        int coilPin1_A, int coilPin2_A, int sensorPin1_A, int sensorPin2_A,
        // Pins for the second coil pair (e.g., for 'right' position)
        int coilPin1_B, int coilPin2_B, int sensorPin1_B, int sensorPin2_B
    );

    void begin();
    void update();

    // Positions: 0 for straight, 1 for left, 2 for right
    void setPosition(int position);
    ~xDuinoRails_ThreeWayTurnout();

private:
    int _id;
    const char* _name;

    // Two standard turnouts to control the two coil pairs
    xDuinoRails_Turnout* _turnoutA;
    xDuinoRails_Turnout* _turnoutB;

    int _targetPosition; // 0: straight, 1: left, 2: right
};

#endif
