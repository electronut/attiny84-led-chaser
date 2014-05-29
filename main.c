///
// A simple program for the ATtiny84 that blinks an LED.
//
// electronut.in
//


#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
#define F_CPU 8000000


// mode = 0 => audio
// mode = 1 => cylon
volatile char mode = 0;


volatile int nTimer0 = 0;
volatile char pin = 0;
// Timer 0 overflow interrupt handler
ISR(TIM0_OVF_vect)
{
  if (mode == 1) {
    // for clock running at 8000000 Hz with prescalar of 1024
    // 8-bit counter overflows = 8000000/1024/256 = 30.52 every second
    if (nTimer0 == 31) {
      // toggle PB1
      PORTA = 0;
      PORTB = 0;
      if (pin == 0) {
        PORTB |= (1 << PB0);
      }
      PORTA |= (1 << pin++);                    
      if (pin > 7) {
        pin = 0;
      }
      // reset
      nTimer0 = 0;
    }
 
    // increment count
    nTimer0++;
  }
}

volatile int adcVal = 0;
volatile int qcount = 0;

// ADC interrupt
ISR(ADC_vect)
{
   // use h/l regs
  unsigned char adcl = ADCL;
  unsigned char adch = ADCH;
  int result = (adch << 8) | adcl; 
  
  adcVal = 512 - result;

  // do next conversion
  ADCSRA |= (1 << ADSC);
}

// switch on/off LED pins according to intensity
void setLEDS(int val) 
{
  if(val > 32) {
    PORTB |= (1 << PB0);
  }
  else {
    PORTB &= ~(1 << PB0);
  }

  if(val > 64) {
    PORTA |= (1 << PA1);
  }
  else {
    PORTA &= ~(1 << PA1);
  }

  if(val > 80) {
    PORTA |= (1 << PA2);
  }
  else {
    PORTA &= ~(1 << PA2);
  }

  if(val > 100) {
    PORTA |= (1 << PA3);
  }
  else {
    PORTA &= ~(1 << PA3);
  }

  if(val > 120) {
    PORTA |= (1 << PA4);
  }
  else {
    PORTA &= ~(1 << PA4);
  }

  if(val > 140) {
    PORTA |= (1 << PA5);
  }
  else {
    PORTA &= ~(1 << PA5);
  }

  if(val > 160) {
    PORTA |= (1 << PA6);
  }
  else {
    PORTA &= ~(1 << PA6);
  }

  if(val > 180) {
    PORTA |= (1 << PA7);
  }
  else {
    PORTA &= ~(1 << PA7);
  }
}

int main (void)
{
  // set output
  DDRB = (1 << PB0);
  DDRA |= (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | 
    (1 << PA5) | (1 << PA6) | (1 << PA7);


  // disable interrupts
  cli();

  // ************************************
  // set up ADC:

  // set divisor to 8
  ADCSRA |= (1<<ADPS1)|(1<<ADPS0);
  // enable ADC
  ADCSRA |= (1 << ADEN);
  // enable auto conversion, enable interrupt
  //ADCSRA |= (1 << ADATE);
  ADCSRA |= (1 << ADIE);
  // convert first value
  ADCSRA |= (1 << ADSC);

  // ************************************
  
  // set 8-bit timer
  // set prescaler to 1024
  TCCR0B |= (1 << CS02) | (1 << CS00);
  // set overflow interrupt enable
  TIMSK0 |= (1 << TOIE0);

  // enable interrupts
  sei();

  //#endif



  while (1) {

    if (adcVal < 32) {
      qcount++;
      // switch to vis mode
      mode = 0;
    }
    else {
      qcount = 0;
    }

    if (qcount > 100) {
      mode = 1;
    }

    if (mode == 0) {
      setLEDS(adcVal);
    }

    _delay_ms(5);
  }
 
  return 1;
}
