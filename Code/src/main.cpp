/*Example sketch to control a stepper motor with A4988 stepper motor driver, AccelStepper library and Arduino: continuous rotation. More info: https://www.makerguides.com */

// Include the AccelStepper library:
#include <AccelStepper.h>
#include <Arduino.h>
#include "DebouncedSwitch.cpp"
#include <State.h>
#include <Configuration.h>

void stateMachineUpdate();

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(STEPPER_INTERFACE_TYPE, DRIVER_STEP_PIN, DRIVER_DIR_PIN);

DebouncedSwitch homeSwitch = DebouncedSwitch(HOME_SWITCH_PIN);

State currentState = UNKNOWN;

unsigned long vacuumStartTime = 0; 

void setup() {
  // Set the maximum speed in steps per second:
  stepper.setEnablePin(DRIVER_ENABLE_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.setMaxSpeed(1000);

  Serial.begin(9600);
  Serial.println("Setup 2");
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
    stepper.move(position);
    stepper.setSpeed(MOVEMENT_SPEED);
    while (stepper.distanceToGo() != 0) {
      stepper.runSpeedToPosition();
    }
    stepper.disableOutputs();
}

void stateMachineUpdate() {
  //Serial.println(currentState);

  switch (currentState) {
  case UNKNOWN:
    if (homeSwitch.getState()) {
      currentState = MOVE_TO_IDLE;
    } else {
      currentState = SEARCHING_FOR_HOME;
    }
    break;
  case MOVE_TO_IDLE:
    //Raise axis away from Vacuum
    stepper.setCurrentPosition(0);
    blockToPoistion(IDLE_POSITION);
    
    currentState = IDLE;
    break;
  case SEARCHING_FOR_HOME:
    //Move down slowly
    stepper.enableOutputs();
    stepper.setSpeed(STEPPER_SEARCH_SPEED);
    stepper.runSpeed();

    if (homeSwitch.getState()) {
      stepper.disableOutputs();
      currentState = MOVE_TO_IDLE;
    }
    break;
  case MOVE_TO_VACUUM:
    blockToPoistion(0);

    currentState = VACUUM;

  case VACUUM:
    if (vacuumStartTime == 0) {
      //Turn on vacuum

      vacuumStartTime = millis();
    }

    if (millis() > vacuumStartTime + VACUUM_RUNTIME_MS) {
      //Turn off vacuum
      vacuumStartTime = 0;

      //Go to idle
      currentState = MOVE_TO_IDLE;
    }
  default:
    break;
  }
}
