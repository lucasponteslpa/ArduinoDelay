/* Arduino Audio Loopback Test
 *
 * Arduino Realtime Audio Processing
 * 2 ADC 8-Bit Mode
 * analog input 1 is used to sample the audio signal
 * analog input 0 is used to control an audio effect
 * PWM DAC with Timer2 as analog output
 
 
 
 * KHM 2008 / Lab3/  Martin Nawrath nawrath@khm.de
 * Kunsthochschule fuer Medien Koeln
 * Academy of Media Arts Cologne
 
 */


#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


int ledPin = 13;                 // LED connected to digital pin 13
int testPin = 7;


boolean div32;
boolean div16;
boolean div8;
// vars altered by interrupt
volatile boolean f_sample;
volatile byte badc0;
volatile byte badc1;
volatile byte ibb;
volatile byte aux;





int ii;
byte dd[1500];  // Audio Memory Array 8-Bit

void setup()
{
  pinMode(ledPin, OUTPUT);      // sets the digital pin as output
  pinMode(testPin, OUTPUT);
  
  // set adc prescaler  to 64 for 19kHz sampling frequency
  //cbi(ADCSRA, ADPS2);
  //sbi(ADCSRA, ADPS1);
  //sbi(ADCSRA, ADPS0);




  sbi(ADMUX,ADLAR);  // 8-Bit ADC in ADCH Register
  sbi(ADMUX,REFS0);  // VCC Reference
  cbi(ADMUX,REFS1);
  cbi(ADMUX,MUX0);   // Set Input Multiplexer to Channel 0
  cbi(ADMUX,MUX1);
  cbi(ADMUX,MUX2);
  cbi(ADMUX,MUX3);


  // Timer2 PWM Mode set to fast PWM 
  cbi (TCCR2A, COM2A0);
  sbi (TCCR2A, COM2A1);
  sbi (TCCR2A, WGM20);
  sbi (TCCR2A, WGM21);

  cbi (TCCR2B, WGM22);




  // Timer2 Clock Prescaler to : 1 
  sbi (TCCR2B, CS20);
  cbi (TCCR2B, CS21);
  cbi (TCCR2B, CS22);

  // Timer2 PWM Port Enable
  sbi(DDRB,3);                    // set digital pin 11 to output

  //cli();                         // disable interrupts to avoid distortion
  cbi (TIMSK0,TOIE0);              // disable Timer0 !!! delay is off now
  sbi (TIMSK2,TOIE2);              // enable Timer2 Interrupt
  
  //Serial.print("ADC offset=");     // trim to 127
  ii=badc1;  
 
  //Serial.println(ii);


}

int i = 0;


void loop()
{
  while (!f_sample) {         // wait for Sample Value from ADC
  }                           // Cycle 15625 KHz = 64uSec 
  PORTD = PORTD  | 128;       //  Test Output on pin 7
  f_sample=false;

  aux = badc1 + dd[i+1]; //SOMA AMOSTRA ATUAL COM O ECO DE AMPLITUDE PELA METADE(SO TEM UM IMPULSO) 
  if(i+1<=1499)OCR2A=aux;                // output audio to PWM port (pin 11)
  else OCR2A = badc1;
  // variable delay controlled by potentiometer    
  // when distortion then delay / processing time is too long   
 // for (cnta=0; cnta <= badc0; cnta++) { 
  //  ibb = ibb * 5;              
 // }

  PORTD = PORTD  ^ 128;       //  Test Output on pin 7
} // loop

//******************************************************************
// Timer2 Interrupt Service at 62.5 KHz
// here the audio and pot signal is sampled in a rate of:  16Mhz / 256 / 2 / 2 = 15625 Hz
// runtime : xxxx microseconds
ISR(TIMER2_OVF_vect) {

  PORTB = PORTB  | 1 ;

  div32=!div32;                      // divide timer2 frequency / 2 to 31.25kHz
  if (div32){ 
    div8=!div8;  
    if (div8) //MAIS UM CONDICIONAL PARA DIMINUIR A FREQUENCIA DE AMOSTRAGEM E TORNAR O DELAY APARENTE 
      {
          div16=!div16;  // 
          if (div16) {                     // sample channel 0 and 1 alternately so each channel is sampled with 15.6kHz
             badc0=ADCH;                    // get ADC channel 0
             sbi(ADMUX,MUX0);               // set multiplexer to channel 1 
          }
          else
          {
            badc1=ADCH;                    // get ADC channel 1
            cbi(ADMUX,MUX0);               // set multiplexer to channel 0
            f_sample=true;
            i++;//ANDAR COM O BUFFER
            dd[i] = badc1*0.5;//ARMAZENA AMOSTRA ATUAL
            
            if (i == 1499)
            {
              i = 0;
            }
          }
      }
    ibb++;                          // short delay before start conversion
    ibb--; 
    ibb++; 
    ibb--;   
    sbi(ADCSRA,ADSC);               // start next conversion
  }
    
}
