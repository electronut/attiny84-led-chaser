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

char pinTX = PB1;
volatile unsigned char currByte = 0;
volatile int sendingData = 0;

// initialize 
void init_serial()
{
  // set TX pin as output
  DDRB = (1 << pinTX);

  // set up timer1:
  cli();
  // 16 bit timer 
  // Divide by 1
  TCCR1B |= (1<<CS10);
  // Count cycles - (1/9600)*8000000 
  OCR1A = 833;   
  // Put Timer/Counter1 in CTC mode
  TCCR1B |= 1<<WGM12;
  // set interrupt flag
  TIMSK1 |= 1<<OCIE1A;
  sei();
}

// send a byte
void send_byte(char data)
{
  // set flag
  sendingData = 1;
  // set current data byte
  currByte = data;
  // wait till done
  while(sendingData);
}

// which bit was sent last? (0-10) 
// -1 implies none sent
volatile char bitNum = -1;

// 16-bit timer CTC handler - Interrupt Service Routine
ISR(TIM1_COMPA_vect)      
{
  // serial data sent as:
  //
  // 9600 8 N 1 
  //
  // start(low)-0-1-2-3-4-5-6-7-stop(high)-idle(high)-idle(high)
  // 12 bits sent per packet

  // idle => high
  if(!sendingData) {
    PORTB |= (1 << pinTX);
  }
  else {

    if(bitNum < 0) {
      // start bit - low
      PORTB &= ~(1 << pinTX);
      // set bit number
      bitNum = 0;
    }
    else if(bitNum >= 8) {      
      // stop bit - high
      PORTB |= (1 << pinTX);
      // increment
      bitNum++;

      // send 2 idle - high - bits
      // not necessary strictly speaking
      if(bitNum == 10) {
        // done 
        sendingData = 0;
        // unset bit number
        bitNum = -1;
      }
    }
    else { 
      // data bits:

      // bit num is in [0, 7]

      // extract relevant bit from data
      char dataBit = currByte & (1 << bitNum);
      if(dataBit) {
        PORTB |= (1 << pinTX);
      }
      else { 
        PORTB &= ~(1 << pinTX);
      }

      // update bit number
      bitNum++;
    }
  }
}


// write null terminated string
void send_str(const char* str)
{
  int len = strlen(str);
  int i;
  for (i = 0; i < len; i++) {
    send_byte(str[i]);
  }   
}

// ADC conversion
int getADC()
{
  // start conversion
  ADCSRA |= (1 << ADSC);
  
  // loop till done
  while(ADCSRA & (1 << ADSC));

#if 0  
  unsigned char adcl = ADCL;
  unsigned char adch = ADCH;
  
  int result = (adch << 8) | adcl; 
#endif

  int result = ADC;

  return result;
}


volatile unsigned char pwmValue = 0;

ISR(TIM0_OVF_vect)
{

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


#if 0

volatile float rmsAmplitude = 0;
volatile int sumSquares = 0;
volatile int nSamples = 0;
const int maxSamples = 20;
volatile int adcVal = 0;
volatile int count = 0;


ISR(TIM0_OVF_vect)
{
  // for clock running at 8000000 Hz with prescalar of  256
  // 8-bit counter overflows = 8000000/256/256 = 122 times every second

  // get ADC value

  // start conversion
  ADCSRA |= (1 << ADSC);
  // loop till done
  while(ADCSRA & (1 << ADSC));
  
  // use h/l regs
  unsigned char adcl = ADCL;
  unsigned char adch = ADCH;
  int result = (adch << 8) | adcl; 

  adcVal = result;

  // calculate RMS amplitude
  if (nSamples == maxSamples) {
    // compute RMS
    rmsAmplitude = sqrt(sumSquares/maxSamples);
    // reset sum and no of samples
    sumSquares = 0;
    nSamples = 0;
  }
  else {
    // sum squares
    sumSquares += (result - 512)*(result - 512);
  }
  nSamples++;

  int val = 512 - adcVal;

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

#endif


int main (void)
{
  // set output
  DDRB = (1 << PB2) | (1 << PB0) | (1 << PB1);
  DDRA |= 
    (1 << PA1) | (1 << PA2) |
    (1 << PA3) | (1 << PA4) |
    (1 << PA5) | (1 << PA6) |
    (1 << PA7);

  //#if 0
  //init_serial();

  // enable ADC 
  ADCSRA |= (1 << ADEN);

  // set ADC port

  // set 8-bit timer
  cli();

  // set prescaler to 256
  TCCR0B |= (1 << CS00);
  // set overflow interrupt enable
  TIMSK0 |= (1 << TOIE0);

  sei();
  //#endif


  // loop
  int count = 0;

  while (1) {
  
#if 0
    char str[16];
    sprintf(str, "%d\n", getADC());
    send_str(str);
#endif

    pwmValue++;

    _delay_ms(20);
  }
 
  return 1;
}
