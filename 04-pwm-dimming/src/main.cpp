#include <Arduino.h>
#include <math.h>

const uint8_t NUM_PINS = 3; // Number of microncontroller pins used for charlieplexing
const uint8_t NUM_ROWS = NUM_PINS; // Number of rows in led matrix in schematics
const uint8_t NUM_LEDS = NUM_PINS*(NUM_PINS - 1);
const uint8_t MAX_BRIGHTNESS = 32;
volatile uint8_t led_brightness[NUM_LEDS];

void set_led_brightness(uint8_t led_idx, uint8_t brightness) {
  led_brightness[led_idx] = brightness;
}

void set_all_brightness(uint8_t brightness) {
  memset((char*)led_brightness, brightness, NUM_LEDS);
}

void pulse_all() {
  for (uint8_t i=1; i < MAX_BRIGHTNESS; ++i) {
    set_all_brightness(i);
    delay(50);
  }
  for (uint8_t i=MAX_BRIGHTNESS; i > 0; --i) {
    set_all_brightness(i);
    delay(50);
  }
}


const uint8_t WAVE_TABLE_LEN = 120;
const float PERIOD_S = 1.2f; // Wave period in seconds
const uint8_t MIN_WAVE_BRIGHTNESS = 2;
const uint8_t MAX_WAVE_BRIGHTNESS = MAX_BRIGHTNESS;

const uint8_t VISIBLE_WAVE_SEGMENT = WAVE_TABLE_LEN / 2; // One half of a sine wave
const uint8_t wave_segment_step = VISIBLE_WAVE_SEGMENT / NUM_LEDS;

uint8_t wave_table[WAVE_TABLE_LEN];
const uint16_t wave_step_delay_millis = (uint16_t)((float)PERIOD_S * 1000 / WAVE_TABLE_LEN);

// Initialize wave table with sine wave.
void init_wave_table() {

    const uint8_t wave_amplitude = (MAX_WAVE_BRIGHTNESS - MIN_WAVE_BRIGHTNESS) / 2;
    const uint8_t wave_offset = MIN_WAVE_BRIGHTNESS + wave_amplitude;

    for (uint8_t i = 0; i < WAVE_TABLE_LEN; ++i) {
        float t = (2.0f * (float)M_PI * i) / WAVE_TABLE_LEN;
        wave_table[i] = (uint8_t) round((sin(t) * wave_amplitude) + wave_offset);
    }
}

void falling_wave() { 
  for (uint8_t i = 0; i < WAVE_TABLE_LEN; ++i) {
      uint8_t idx_0 = i;
      uint8_t idx_1 = (i + wave_segment_step) % WAVE_TABLE_LEN;
      uint8_t idx_2 = (i + wave_segment_step * 2) % WAVE_TABLE_LEN;
      uint8_t idx_3 = (i + wave_segment_step * 3) % WAVE_TABLE_LEN;
      uint8_t idx_4 = (i + wave_segment_step * 4) % WAVE_TABLE_LEN;
      uint8_t idx_5 = (i + wave_segment_step * 5) % WAVE_TABLE_LEN;

      set_led_brightness(0, wave_table[idx_0]);
      set_led_brightness(1, wave_table[idx_1]);
      set_led_brightness(2, wave_table[idx_2]);
      set_led_brightness(3, wave_table[idx_3]);
      set_led_brightness(4, wave_table[idx_4]);
      set_led_brightness(5, wave_table[idx_5]);
      
      delay(wave_step_delay_millis);
  }
}

//
// See https://www.arduinoslovakia.eu/application/timer-calculator
//
void setup_interrupts() {
  // NOTE: Code for Arduino Uno, code must be changed for ATtiny

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

const uint8_t LINE_A_BIT = 0x01;
const uint8_t LINE_B_BIT = 0x02;
const uint8_t LINE_C_BIT = 0x04;

/* State variables used in ISR are prefixed with i_ */
uint8_t i_ramp = MAX_BRIGHTNESS - 1; // ramp that counts from 0 to MAX_BRIGHTNESS - 1
uint8_t i_row_idx = NUM_ROWS - 1; // index of row that is currently turned on
uint8_t i_led0_brightess = 0; // brightness level for first led on row i_row_idx
uint8_t i_led1_brightess = 0; // brightness level for second led on row i_row_idx
uint8_t i_col_enable_mask = 0; // Bitmask, 1 bit for each of the two leds on row i_row_idx
uint8_t i_row_outputs = 0; // Bitmask, one bit is set the for current row i_row_idx
uint8_t i_row_mask = 0; // Bitmask, one bit is set for all columns with index less than current i_row_idx

ISR(TIMER1_COMPA_vect) {
  ++i_ramp;
  if (i_ramp >= MAX_BRIGHTNESS) {
    i_ramp = 0;
    ++i_row_idx;

    if (i_row_idx >= NUM_ROWS) {
      i_row_idx = 0;
    }

    const uint8_t first_led_in_row_idx = TODO: Multiply with 2 using bitshift operator (do not use * operator, ISR should be fast)
    i_row_outputs = TODO: Set bit i_row_idx, other bits shall be zero.  (e.g. i_row_idx=1 should give i_row_outpus=0x02 (0b00000010))
    i_row_mask = TODO: Set all bits up to but not including bit i_row_idx (e.g. i_row_idx=1 should give i_row_mask=0x01 (0b00000001))
    
    i_led0_brightess = led_brightness[first_led_in_row_idx];
    i_led1_brightess = led_brightness[first_led_in_row_idx + 1];
    
    i_col_enable_mask = 0x03;  // All columns enabled
  }

  if (i_ramp == i_led0_brightess)
    TODO turn off bit 0 in i_col_enable_mask
  if (i_ramp == i_led1_brightess)
    TODO turn off bit 1 in i_col_enable_mask

  
  uint8_t column_outputs = (i_col_enable_mask & i_row_mask) | (i_col_enable_mask & ~i_row_mask) << 1;
  uint8_t output_pins = column_outputs != 0 ? column_outputs | i_row_outputs : 0;

  DDRB = output_pins;
  PORTB = output_pins != 0 ? i_row_outputs : 0; 
}

void setup() {
  DDRB = 0;
  PORTB = 0;
  setup_interrupts();  

  set_all_brightness(MAX_BRIGHTNESS);
  init_wave_table();
  pulse_all();
}

void loop() {
  falling_wave();
}

