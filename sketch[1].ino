#include <Arduino.h>
#include "heater_control.h"

// Simulated temperature reading
float read_temperature_sensor() {
    static float temp = 15.0;
    
    if (get_current_state() == STATE_HEATING) {
        temp += 0.5;
    } else {
        temp -= 0.2;
    }
    
    temp += (random(-10, 10) / 10.0);
    return temp;
}

void setup() {
    system_init();
}

void loop() {
    float current_temp = read_temperature_sensor();
    update_temperature(current_temp);
    system_loop();
}