#define SAMPLE_RATE 8 // 8kHz (WM8731@12.288MHz, Arduino@16MHz) ) mkc 9/15/13
#define BYPASS 0 // Bypass LINEIN
#define DACSEL 1 // Select DAC for audio loopback
#define INSEL 1 // enable MIC input
#define ADCHPD 0
//#define LHPVOL 221
//#define RHPVOL 221

//const int ledPin =  A0;
const int ledPin =  6; //Non-Arduino-MCU
const int buzz_pin = A3; 
const int buzz_led =  7;

const int resetPin =  4;

int SH=1;
int SL=0;



#ifndef AudioCodec_h // include guard
#define AudioCodec_h

volatile int16_t d_in;
volatile int16_t d_out;
volatile uint8_t hb=1;
volatile uint8_t hbyte,lbyte,xbyte,rbyte;
volatile uint8_t e,f;
volatile uint8_t tx_sync=0;
volatile uint8_t button_val, buzz_val=0;
volatile uint8_t start_audio=0;
volatile int16_t header_audio[8];
volatile uint8_t audio_bt=0;
volatile uint8_t checker, buzz_ucnt, buzz_dcnt=0;
volatile uint8_t f_count=0;
volatile uint8_t read_pin, bzread_pin=0;
volatile uint8_t end_audio=0;
static inline void AudioCodec_init(void); //***Remove these 2 functions in case of error
static inline void AudioCodec_data(volatile int* _lin, volatile int* _rin,volatile int _lout,volatile int _rout);
char saved_header[]={58,48,49,57,49,57,65}; //:01919A
char buzz_high[]={58,48,49,57,66,57,65}; //:019B9A
//char saved_footer[]={58,58,48,49,57,56,57,68}; //::01989D
char buzz_low[]={58,48,49,57,66,57,68}; //:019B9D

#include <Wire.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <PinChangeInt.h>
#define button_pin 2
#define buzz_btn A4 //A0 previously

#ifndef SAMPLE_RATE
  #define SAMPLE_RATE 8
#endif

#ifndef BYPASS
  #define BYPASS 1
#elif (BYPASS == 0)||(BYPASS == 1)
#else
  #error BYPASS value not defined
#endif

#ifndef DACSEL
  #define DACSEL 1
#elif (DACSEL == 0)||(DACSEL == 1)
#else
  #error DACSEL value not defined
#endif

#ifndef INSEL
  #define INSEL 0
#elif (INSEL == 0)||(INSEL == 1)
#else
  #error INSEL value not defined
#endif

#ifndef ADCHPD
  #define ADCHPD 0
#elif (ADCHPD == 0)||(ADCHPD == 1)
#else
  #error ADCHPD value not defined
#endif

// CORE CLOCK Variables   USBNORMAL=0;BOSR=0;SR0=1;SR1=1;SR2=0;SR3=0;CLKDIV2=0;CLKODIV2=0;
uint8_t USBNORMAL=0;
uint8_t BOSR=0;
uint8_t SR0=1;
uint8_t SR1=1;
uint8_t SR2=0;
uint8_t SR3=0;
uint8_t CLKDIV2=0;
uint8_t CLKODIV2=0;

// setup variables for ADC

#if ADCS == 0
  // do nothing
#endif
//{58,48,49,57,49,57,65}; //:01919A
//{58,48,49,57,56,57,68}; //:01989D
void interruptFunction() 
{
  if(button_val==0)
  {
    
    button_val=1;
  
  }
  
  if (button_val==2)
  {
  read_pin=digitalRead(button_pin);  
  if(read_pin==1){button_val=3;}
  else if(read_pin==0){delay(50);read_pin=digitalRead(button_pin);if(read_pin==1){button_val=3;}}
  else if(read_pin==0){button_val=2;}
  }
}

void buzz_Function() 
{ //Serial.println("here");
  if(buzz_val==0)
  {
    buzz_val=1;
  }
  
  if (buzz_val==2)
  {
  bzread_pin=digitalRead(buzz_btn);  
  if(bzread_pin==1){buzz_val=3;}
  else if(bzread_pin==0){delay(50);bzread_pin=digitalRead(buzz_btn);if(bzread_pin==1){buzz_val=3;}}
  else if(bzread_pin==0){buzz_val=2;}
  }
}

static inline void AudioCodec_init(void)
{

  // setup spi peripheral
  digitalWrite(10, LOW); // set ss pin to output low
  pinMode (10, OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0); 
 
  //DDRB |= (1 << 2);
  //PORTB &= ~(1<<PORTB2);
  //spi_init();
  
  ///////////////I2C Setup/////////////////////
 
  Wire.begin();
  Wire.beginTransmission(0x1a);
  Wire.write(0x0c); // power reduction register
  Wire.write((uint8_t)0x00); // turn everything on
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x0e); // digital data format
  Wire.write(0x03); // 16-bit DSP Mode B
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x08); // analog audio pathway configuration
  Wire.write((uint8_t)((DACSEL << 4)|(BYPASS << 3)|(INSEL << 2)));
  //Wire.write((uint8_t)(SIDETONE << 5));
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x10); // WM8731 CORE CLOCK config
  Wire.write((uint8_t)((CLKODIV2<<7)|(CLKDIV2<<6)|(SR3<<5)|(SR2<<4)|(SR1<<3)|(SR0<<2)|(BOSR<<1)|(USBNORMAL<<0)));
  Wire.endTransmission();

  Wire.beginTransmission(0x1a);
  Wire.write(0xc); // LININ, CLKOUT power down
  Wire.write(0x41);
  Wire.endTransmission();

  Wire.beginTransmission(0x1a);
  Wire.write(0x12); // codec enable
  Wire.write(0x01);
  Wire.endTransmission();
  // setup ADCs
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x0a); // digital audio path configuration
  Wire.write((uint8_t)ADCHPD);
  Wire.endTransmission();
  
  /*
  Wire.beginTransmission(0x1a);
  Wire.write(0x04); // left headphone out register
  Wire.write((uint8_t)LHPVOL);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x06); // right headphone out register
  Wire.write((uint8_t)RHPVOL);
  Wire.endTransmission();
 */
   
    
  ////////////////////////////////////////////
  
  // setup timer1 for codec clock division
  TCCR1A = 0x00; // set to CTC mode
  TCCR1B = 0x0a; // set to CTC and Timer1 prescaler DIV8, 0xa (internal clock 16MHz/8
  TCCR1C = 0x00; // not used
  TCNT1H = 0x00; // clear the counter
  TCNT1L = 0x00;
  
  #if SAMPLE_RATE == 8
    OCR1AH = 0x00; // set the counter top
    OCR1AL = 0xfa; 
  //  OCR1AH = 0x01; // set the counter top
  //  OCR1AL = 0x5b; // 15B
  #endif
    //  TIMSK1 = 0x02; // turn on compare match interrupt
  TIMSK1 |= (1 << OCIE1A); // turn on compare match interrupt
  
  // turn off all enabled interrupts (delay and wire)
  TIMSK0 = 0x00;
  TWCR = 0x00;

  sei(); // turn on interrupts
}

#if ADCS == 0
  ;
  //{
   //; // do nothing
  //}
#endif


// codec data transfer function
static inline void AudioCodec_data(volatile int* _lin, volatile int* _rin,volatile int _lout,volatile int _rout) 
{
  int _out_temp = _lout;
  PORTB |= (1<<PORTB2);  // toggle ss pin // PORTB2 IS PIN 10
  asm volatile ("out %0, %B1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  PORTB &= ~(1<<PORTB2); // toggle ss pin
  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("out %0, %A1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  asm volatile ("in r3, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  _out_temp = _rout;
  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("out %0, %B1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  asm volatile ("in r2, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  asm volatile ("movw %0, r2" : "=r" (*_lin) : );

  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("out %0, %A1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  asm volatile ("in r3, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("in r2, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  asm volatile ("movw %0, r2" : "=r" (*_rin) : );
  
}
#endif // end include guard

volatile int left_in = 0; // in from codec (LINE_IN)
volatile int right_in = 0;
volatile int left_out = 0; // out to codec (HP_OUT)
volatile int right_out = 0;

void setup() 
 {
 digitalWrite(resetPin,HIGH);  
 AudioCodec_init(); 
 //Serial.begin(57600);
 Serial.begin(115200);
 pinMode(ledPin,OUTPUT);digitalWrite(ledPin,LOW);
 pinMode(buzz_pin,OUTPUT);digitalWrite(buzz_pin,LOW);
 pinMode(button_pin, INPUT);digitalWrite(button_pin,HIGH);
 pinMode(buzz_btn, INPUT);digitalWrite(buzz_btn,HIGH);
 attachPinChangeInterrupt(button_pin, interruptFunction, CHANGE); 
 attachPinChangeInterrupt(buzz_btn, buzz_Function, CHANGE);
 }
 
void loop() { ; }

ISR(TIMER1_COMPA_vect, ISR_NAKED) 
{
  AudioCodec_data(&left_in, &right_in, left_out, right_out);
   
  //if(button_val==1){Serial.print("::::::01919A");digitalWrite(ledPin,HIGH);button_val=2;} //Send the header
  if(button_val==1){Serial.print("::01919A");Serial.print("::01919A");digitalWrite(ledPin,HIGH);button_val=2;} //Send the header
  
  if (button_val==2)
  { 
    
    d_out=right_in;
    if(tx_sync==0) { e='\r'; f='\n'; } else {
    e = ((d_out & 0xFF00)>>8); 
    f = (d_out & 0x00FF); }
    //Serial.print(button_val);Serial.print('\n');
    Serial.write(e);
    Serial.write(f);
    tx_sync++;
    f_count=0;
  }
  
  else if(button_val>=3)
  {
   Serial.print(":::::::");
   digitalWrite(ledPin,LOW);button_val=0;start_audio=0;audio_bt=0;
   //Serial.begin(115200);
   Serial.flush();
  }
  
  if(buzz_val==1){digitalWrite(buzz_led, HIGH); digitalWrite(buzz_pin,HIGH);buzz_val=2;}
  
  if (buzz_val==2)
  { 
    digitalWrite(buzz_pin,HIGH);
    digitalWrite(buzz_led,HIGH);
  }
  
  else if(buzz_val>=3)
  {
   digitalWrite(buzz_pin,LOW);
   digitalWrite(buzz_led, LOW);
   buzz_val=0; 
  }
  
 while (Serial.available() > 0) {
  
 rbyte=Serial.read();  
 //Serial.println(rbyte); 
 if(start_audio==0) {
 if((rbyte==58)){audio_bt=0;}
 header_audio[audio_bt]= rbyte;
 audio_bt++;
 }

 if(hb==1)
 {
    hbyte=rbyte;
    //Serial.println(hbyte);
    
    hb=0;
    if(xbyte=='\r' && hbyte=='\n') { hb=1; }
  }
  
  else 
  {
    lbyte=rbyte;
    xbyte=lbyte;
    hb=1;
    //Serial.write(hbyte);Serial.write(lbyte); //Just for the link test:::Remove it, when in use
    d_in=(hbyte<<8)|lbyte; 
  }
    
    if(audio_bt==7)
    {     
      checker=0;
      buzz_ucnt=0; buzz_dcnt=0;
      for(uint8_t p=0;p<=6;p++)
      {
        if(header_audio[p]==saved_header[p]){checker++;}
        if(header_audio[p]==buzz_high[p]){buzz_ucnt++;}
        if(header_audio[p]==buzz_low[p]){buzz_dcnt++;}
       
      }
      if(checker==7){start_audio=1;audio_bt=0;digitalWrite(ledPin,HIGH);}
      if(buzz_ucnt==7){digitalWrite(buzz_pin,HIGH);}
      if(buzz_dcnt==7){digitalWrite(buzz_pin,LOW);}
      else{audio_bt=0;}
     }
     
    if(d_in==((58<<8)|58))
    {
      if(end_audio<=2)
      {
      end_audio++;
      if(end_audio==3)
      {
        //button_val=0;
        digitalWrite(ledPin,LOW);start_audio=0;audio_bt=0;end_audio=0;
        Serial.flush();//**
        //Serial.begin(115200);
      }
      }  
    }
    else{end_audio=0;}
    
      
   if (start_audio==1)
   {  
      left_out = d_in; // put sinusoid on left channel
      
      right_out = (-d_in) ; // put inverted sinusoid out on right channel
      
   }
   

 }    
 

    reti(); // dont forget to return from the interrupt
    
}
