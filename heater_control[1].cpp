#include "heater_control.h"
#include <Arduino.h>

// System variables
static SystemState current_state = STATE_IDLE;
static float current_temperature = 0.0;
static bool heater_status = false;

// Hardware pins - updated for Wokwi compatibility
const int HEATER_PIN = 2;
const int STATUS_LED = 13;  // Changed from LED_BUILTIN to pin 13
const int BUZZER_PIN = 3;

void system_init() {
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    digitalWrite(HEATER_PIN, LOW);
    digitalWrite(STATUS_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    Serial.begin(115200);
    Serial.println("Heater Control System Initialized");
}

void update_temperature(float current_temp) {
    current_temperature = current_temp;
}

SystemState get_current_state() {
    return current_state;
}

void control_heater(bool turn_on) {
    heater_status = turn_on;
    digitalWrite(HEATER_PIN, turn_on ? HIGH : LOW);
    digitalWrite(STATUS_LED, turn_on ? HIGH : LOW);
}

void handle_overheat() {
    control_heater(false);
    digitalWrite(BUZZER_PIN, HIGH);
    current_state = STATE_OVERHEAT;
    
    // In a real system, this might trigger a latch that requires manual reset
    Serial.println("!!! OVERHEAT CONDITION - SYSTEM SHUTDOWN !!!");
}

void system_loop() {
    // State transition logic
    switch(current_state) {
        case STATE_IDLE:
            if (current_temperature < TEMP_LOW_THRESHOLD) {
                control_heater(true);
                current_state = STATE_HEATING;
                Serial.println("State: Heating");
            }
            break;
            
        case STATE_HEATING:
            if (current_temperature >= TEMP_HIGH_THRESHOLD) {
                control_heater(false);
                current_state = STATE_STABILIZING;
                Serial.println("State: Stabilizing");
            }
            break;
            
        case STATE_STABILIZING:
            if (current_temperature <= TEMP_LOW_THRESHOLD) {
                control_heater(true);
                current_state = STATE_HEATING;
                Serial.println("State: Heating");
            } else if (abs(current_temperature - (TEMP_LOW_THRESHOLD + TEMP_HIGH_THRESHOLD)/2) < 1.0) {
                current_state = STATE_TARGET_REACHED;
                Serial.println("State: Target Reached");
            }
            break;
            
        case STATE_TARGET_REACHED:
            if (current_temperature < TEMP_LOW_THRESHOLD - 2.0 || 
                current_temperature > TEMP_HIGH_THRESHOLD + 2.0) {
                current_state = STATE_IDLE;
                Serial.println("State: Idle");
            }
            break;
            
        case STATE_OVERHEAT:
            // Stay in this state until manual reset
            break;
    }
    
    // Safety check (applies to all states)
    if (current_temperature >= TEMP_DANGER_THRESHOLD) {
        handle_overheat();
    }
    
    // Log current status
    Serial.print("Temp: ");
    Serial.print(current_temperature);
    Serial.print("C, Heater: ");
    Serial.println(heater_status ? "ON" : "OFF");
    
    delay(1000); // Main loop delay
}