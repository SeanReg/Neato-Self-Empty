
// Include the AccelStepper library:
#include <AccelStepper.h>
#include <Arduino.h>
#include "DebouncedSwitch.cpp"
#include <State.h>
#include <Configuration.h>
#include <NewPing.h>

void stateMachineUpdate();

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(STEPPER_INTERFACE_TYPE, DRIVER_STEP_PIN, DRIVER_DIR_PIN);

DebouncedSwitch homeSwitch = DebouncedSwitch(HOME_SWITCH_PIN);

NewPing distanceSensor = NewPing(DISTANCE_SENSOR_TRIGGER_PIN, DISTANCE_SENSOR_INPUT_PWM);

State currentState = UNKNOWN;
State lastState = UNKNOWN;

unsigned long vacuumStartTime = 0; 

unsigned long robotRoamTime = 0;

void setup() {
  pinMode(VACUUM_RELAY_PIN, OUTPUT);
  digitalWrite(VACUUM_RELAY_PIN, LOW);

  // Set the maximum speed in steps per second:
  stepper.setEnablePin(DRIVER_ENABLE_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.setMaxSpeed(1000);

  Serial.begin(9600);
  Serial.println("Setup");
}

void loop() {
  // Set the speed in steps per second:
  stateMachineUpdate();

  // Step the motor with a constant speed as set by setSpeed():
  //stepper.runSpeed();
  homeSwitch.run();
}

void blockToPoistion(long position) {
    stepper.enableOutputs();
    stepper.moveTo(position);
    stepper.setSpeed(MOVEMENT_SPEED);
    while (stepper.distanceToGo() != 0) {
      stepper.runSpeedToPosition();
    }
    stepper.disableOutputs();
}

void stateMachineUpdate() {
  if (lastState != currentState) {
    Serial.println(currentState);
    lastState = currentState;
  }

  switch (currentState) {
  case UNKNOWN:
    currentState = SEARCHING_FOR_HOME;
    break;
  case IDLE:
    //Robot has left
    if (distanceSensor.convert_cm(distanceSensor.ping_median(8)) > MAX_PARK_DISTANCE_CM) {
      currentState = ROBOT_ROAMING;
    }
    break;
  case ROBOT_ROAMING:
    if (robotRoamTime == 0) {
      robotRoamTime = millis();
    }

    //robot returned
    if (distanceSensor.convert_cm(distanceSensor.ping_median(8)) <= MAX_PARK_DISTANCE_CM) {

      if (millis() >= robotRoamTime + MIN_ROAMING_TIME_MS) {
        //Robot was gone for awhile so start the vacuum procedure
        currentState = MOVE_TO_VACUUM;
      } else {
        //Lost robot for short time, don't vacuum
        currentState = IDLE;
      }

      robotRoamTime = 0;
    }
    break;
  case MOVE_TO_IDLE:
    //Raise axis away from Vacuum
    blockToPoistion(IDLE_POSITION);
    
    currentState = IDLE;
    break;
  case SEARCHING_FOR_HOME:
    //Move down slowly
    stepper.enableOutputs();
    stepper.setSpeed(STEPPER_SEARCH_SPEED);
    stepper.runSpeed();

    if (homeSwitch.getState()) {
      stepper.setCurrentPosition(0);
      stepper.disableOutputs();
      currentState = MOVE_TO_IDLE;
    }
    break;
  case MOVE_TO_VACUUM:
    //Lower the vacuum arm
    blockToPoistion(0);

    currentState = VACUUM;

  case VACUUM:
    if (vacuumStartTime == 0) {
      //Turn on vacuum
      digitalWrite(VACUUM_RELAY_PIN, HIGH);
      vacuumStartTime = millis();
    }

    if (millis() > vacuumStartTime + VACUUM_RUNTIME_MS) {
      //Turn off vacuum
      digitalWrite(VACUUM_RELAY_PIN, LOW);
      vacuumStartTime = 0;

      //Go to idle
      currentState = MOVE_TO_IDLE;
    }
  default:
    break;
  }
}
