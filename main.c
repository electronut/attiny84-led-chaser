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

#if 0
volatile unsigned char pwmValue = 0;

volatile float rmsAmplitude = 0;
volatile int sumSquares = 0;
volatile int nSamples = 0;
const int maxSamples = 20;
volatile int adcVal = 0;
volatile int count = 0;
volatile int movingAvg = 0;


ISR(TIM0_OVF_vect)
{

  if (mode == 1) {

    static unsigned char tCount = 255;

    // overflowed from 255 to 0
    if(++tCount == 0) {
      // turn all PortA pins off
      PORTA = 0;
    
      // turn on pin 
      if (pwmValue != 0) {
        PORTA |= (1 << PA1);
      }
    }
    else {
      // turn off pin when count exceeds PWM value
       if (tCount > pwmValue) {
        PORTA &= ~(1 << PA1);
      }
    }
  }
  else {
    // for clock running at 8000000 Hz with prescalar of  256
    // 8-bit counter overflows = 8000000/256/256 = 122 times every second

    // get ADC value
    
    // set as input
    DDRA &= ~(1 << PA0); 
    PORTA &= ~(1 << PA0); // pull up turned off 

    // enable ADC 
    ADCSRA |= (1 << ADEN);

    // start conversion
    ADCSRA |= (1 << ADSC);
    // loop till done
    while(ADCSRA & (1 << ADSC));
  
    // use h/l regs
    unsigned char adcl = ADCL;
    unsigned char adch = ADCH;
    int result = (adch << 8) | adcl; 

    adcVal = result;

    // disable ADC 
    ADCSRA &= ~(1 << ADEN);
    
    // set pin as output
    DDRA |= (1 << PA0); 

    int val = 512 - adcVal;

    if(val > 32) {
      PORTA |= (1 << PA0);
    }
    else {
      PORTA &= ~(1 << PA0);
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
}
#endif

volatile int adcVal = 0;

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

#if 0
  // set 8-bit timer
  // set prescaler to 256
  TCCR0B |= (1 << CS02);
  // set overflow interrupt enable
  TIMSK0 |= (1 << TOIE0);
#endif

  // enable interrupts
  sei();

  //#endif



  while (1) {
  
    setLEDS(adcVal);

    _delay_ms(5);
  }
 
  return 1;
}
