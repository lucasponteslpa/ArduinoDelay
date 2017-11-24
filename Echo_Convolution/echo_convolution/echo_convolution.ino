/* 
 *
 */


#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

boolean div32;
boolean div16;
boolean div8;

volatile boolean f_sample;
volatile byte audio_in;
volatile byte audio_out;

byte echo_buffer[1500];

void setup()
{
  sbi(ADMUX,ADLAR);  // 8-Bit ADC no registrador ADCH
  sbi(ADMUX,REFS0);  // VCC Referencia
  cbi(ADMUX,MUX0);   // Seta multiplexador da entrada para o canal 0
  cbi(ADMUX,MUX1);


  // setar modo Timer2 PWM 
  sbi (TCCR2A, COM2A1);
  sbi (TCCR2A, WGM21);

  cbi (TCCR2B, WGM22);

  // Timer2 Clock Prescaler to : 1 
  sbi (TCCR2B, CS20);
  cbi (TCCR2B, CS22);

  // habilitar porta do Timer2 PWM
  sbi(DDRB,3);                    // seta o pino digital 11 como saida
  sbi (TIMSK2,TOIE2);              // liga Timer2 Interrupt

}

int i = 0;


void loop()
{
  while (!f_sample);         // esperar pelo valor de amostra vindo do ADC                          
  f_sample=false;

  audio_out = audio_in + echo_buffer[i+1]; //SOMA AMOSTRA ATUAL COM O ECO DE AMPLITUDE PELA METADE(SO TEM UM IMPULSO) 
  if(i+1<=1499)OCR2A=audio_out;                // output audio na  porta PWM (pin 11)
  else OCR2A = audio_in;
}
//na inturrupcao amostra é retirada em: 16Mhz / 256 / 2 / 2/ 2 = 7812 Hz
ISR(TIMER2_OVF_vect) {

  div32=!div32;              
  if (div32){ 
    div8=!div8;  
    if (div8) //MAIS UM CONDICIONAL PARA DIMINUIR A FREQUENCIA DE AMOSTRAGEM E TORNAR O DELAY APARENTE 
      {
          div16=!div16;  // 
          if (div16) {                  
             sbi(ADMUX,MUX0);               // seta o multiplexador para o canal 1 
          }
          else
          {
            audio_in=ADCH;                    // pega ADC do canal 1
            cbi(ADMUX,MUX0);               // seta o multiplexador para o canal 0
            f_sample=true;
            i++;//ANDAR COM O BUFFER
            echo_buffer[i] = audio_in*0.5;//ARMAZENA AMOSTRA ATUAL
            
            if (i == 1499)
            {
              i = 0;
            }
          }
      }   
    sbi(ADCSRA,ADSC);
  }
    
}

