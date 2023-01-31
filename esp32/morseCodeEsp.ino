#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define ENCODER_BUTTON 13
#define LED_PIN 15
#define DELAY_BUTTON 10

struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button button = {ENCODER_BUTTON, false};
bool button_action_required;
bool ledTurnOn;
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 
unsigned long ledTime = 0;
int state;


bool delay_passed() {
  return button_time - last_button_time > DELAY_BUTTON;
}

void IRAM_ATTR handleKey() {
  button_time = millis();
  state = digitalRead(button.PIN);
  bool btnPressed = (state == LOW && !button.pressed && delay_passed());
  bool btnReleased = (state == HIGH && button.pressed && delay_passed());
  
  if (btnPressed) {
    button.pressed   = true;
    last_button_time = button_time;
  } else if (btnReleased) {
    button.pressed   = false;
    last_button_time = button_time;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(button.PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(button.PIN, handleKey, CHANGE);
  Serial.println("Basic Encoder Test:");
}

void ledBlink() {
  if(!ledTurnOn && button_action_required) {
    digitalWrite(LED_PIN, HIGH);
    ledTime = millis();
    ledTurnOn = true;
  }
  
  if(ledTurnOn && millis() - ledTime >  50) {
    digitalWrite(LED_PIN, LOW);
    ledTurnOn = false;
    button_action_required = false;
  }
}

void onButtonPressed() {
  ledBlink();
}

void onButtonReleased() {
  digitalWrite(LED_PIN, LOW);
  ledTurnOn = false;
  button_action_required = true;
}

void loop() {
  if (button.pressed && button_action_required) {
    onButtonPressed();
  } else if(!button.pressed && !button_action_required) {
    onButtonReleased();
  }
}
