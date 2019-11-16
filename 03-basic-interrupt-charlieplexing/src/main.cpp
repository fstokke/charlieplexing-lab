#include <Arduino.h>

const uint8_t NUM_LEDS = 6;

const uint8_t LINE_A = PB0;
const uint8_t LINE_B = PB1;
const uint8_t LINE_C = PB2;

// Declare as volatile to ensure that compiler does not optimize away 
// updates/read because it does not see activity in interrupt routine

volatile uint32_t led_state = 0; // Bit mask with NUM_LEDS bits. bit N is set if LED N is on.
volatile uint8_t led_idx = 0;

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
  led_state = 0;
	DDRB = 0;
	PORTB = 0;
}

void running_led() {
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    led_state = (1 << i);
    delay(500);
  }
}

void inverse_running_led() {
  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    TODO: light all leds except led with index i
    delay(500);
  }
}

void bargraph_pulse() {
  for (int8_t i = 0; i <= NUM_LEDS; ++i) {
    led_state = (1 << i) - 1; // Set all LEDs up to but not including LED at index i
    delay(150);

  }
  for (int8_t i = NUM_LEDS-1; i >=0; --i) {
    led_state = (1 << i) - 1;
    delay(150);
  }
}


// Set up interrupt frequency of 10 kHz
void setup_interrupts() {
 noInterrupts();
  // Clear registers

  // TCCR1A – Timer/Counter1 Control Register A
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 10000 Hz (16000000/((24+1)*64))
  OCR1A = 24; // Timer 1 output compare register -- interupt is generated when timer reaches this value
  // CTC
  TCCR1B |= (1 << WGM12); // Enable CTC mode - Clears/resets Timer 1 on interrupt
  // Clock prescaler 64 
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

// This is the interrupt routine. 
ISR(TIMER1_COMPA_vect) {
  if (TODO: Test if bit led_idx is set in led_state) {
    	DDRB = led_dir[led_idx];
    	PORTB = led_out[led_idx];
  } else {
      DDRB = 0;
      PORTB = 0;
  }

  ++led_idx;
  if (led_idx >= NUM_LEDS) {
    led_idx = 0;
  }
}

void setup() {
  leds_off();
  setup_interrupts();
}

void loop() {

  for (uint8_t i = 0; i < 3; ++i) {
    running_led();
  }

  for (uint8_t i = 0; i < 3; ++i) {
    inverse_running_led();
  }
  
  for (uint8_t i = 0; i < 3; ++i) {
    bargraph_pulse();
  }
}
