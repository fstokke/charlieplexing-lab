#include <Arduino.h>


void setup_interrupts() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 4 Hz (16000000/((X+1)*256))
  OCR1A = TODO; // TODO: Calculate correct value
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 256
  TCCR1B |= (1 << CS12);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

// This is the interrupt routine. 
ISR(TIMER1_COMPA_vect) {
  PORTB ^= 0x01;
}

void setup() {
  DDRB = 0x03; // Configure PB0 and PB1 as output pins
  PORTB = 0;
  setup_interrupts();
}

void loop() {
}
