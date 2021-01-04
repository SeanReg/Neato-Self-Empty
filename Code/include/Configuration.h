
//PIN DEFINITIONS
#define DRIVER_DIR_PIN 2
#define DRIVER_STEP_PIN 3
#define DRIVER_ENABLE_PIN 4
#define STEPPER_INTERFACE_TYPE 1

#define HOME_SWITCH_PIN 8

#define VACUUM_RELAY_PIN 5

#define DISTANCE_SENSOR_INPUT_PWM 6
#define DISTANCE_SENSOR_TRIGGER_PIN 7

//Dock distance
#define MAX_PARK_DISTANCE_CM 7

//Min time for vacuum to be gone before considered roaming
#define MIN_ROAMING_TIME_MS 10 * 60 * 1000L

//Stepper movement speeds
#define MOVEMENT_SPEED 400
#define STEPPER_SEARCH_SPEED MOVEMENT_SPEED
#define STEPS_PER_MM 160L

//IDLE Position when not in use
#define IDLE_POSITION -100L * STEPS_PER_MM

//How long to run the vacuum for
#define VACUUM_RUNTIME_MS 20 * 1000 