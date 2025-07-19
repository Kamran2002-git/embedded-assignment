#ifndef HEATER_CONTROL_H
#define HEATER_CONTROL_H

// Temperature thresholds (in Celsius)
#define TEMP_LOW_THRESHOLD   20.0
#define TEMP_HIGH_THRESHOLD  30.0
#define TEMP_DANGER_THRESHOLD 35.0

// System states
typedef enum {
    STATE_IDLE,
    STATE_HEATING,
    STATE_STABILIZING,
    STATE_TARGET_REACHED,
    STATE_OVERHEAT
} SystemState;

// Function prototypes
void system_init();
void update_temperature(float current_temp);
SystemState get_current_state();
void system_loop();

#endif // HEATER_CONTROL_H