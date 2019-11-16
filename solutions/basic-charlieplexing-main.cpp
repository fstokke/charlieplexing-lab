#include <Arduino.h>

const uint8_t NUM_LEDS=6;

const uint8_t LINE_A = PB0;
const uint8_t LINE_B = PB1;
const uint8_t LINE_C = PB2;


// DDRB direction config for each LED (1 = output)
const char led_dir[NUM_LEDS] = {
  ( 1 << LINE_A | 1 << LINE_B ), // LED 0
  ( 1 << LINE_A | 1 << LINE_C ), // LED 1
  ( 1 << LINE_B | 1 << LINE_A ), // LED 2
  ( 1 << LINE_B | 1 << LINE_C ), // LED 3
  ( 1 << LINE_C | 1 << LINE_A ), // LED 4
  ( 1 << LINE_C | 1 << LINE_B ), // LED 5
}; 

// PORTB output config for each LED (1 = High, 0 = Low)
const char led_out[NUM_LEDS] = {
  ( 1 << LINE_A ), // LED 0
  ( 1 << LINE_A ), // LED 1
  ( 1 << LINE_B ), // LED 2
  ( 1 << LINE_B ), // LED 3
  ( 1 << LINE_C ), // LED 4
  ( 1 << LINE_C ), // LED 5
};

void light_led(uint8_t led_idx) { // led_idx must be from 0 to NUM_LEDS-1
	DDRB = led_dir[led_idx];
	PORTB = led_out[led_idx];
}

void leds_off() {
	DDRB = 0;
	PORTB = 0;	
}

void setup() {
  leds_off();
}

void loop() {
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    light_led(i);
    delay(500);
  }
}


