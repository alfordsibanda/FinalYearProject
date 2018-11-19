//https://arduino.stackexchange.com/questions/37387/can-i-make-delaymicroseconds-more-accurate
#include <util/delay.h>
//_delay_us(tSamp2);
char serialData[10];
boolean toggle = false; // To toggle the clock pin

long toMicro = 1000000;
long FreqOffset = 100000;
int PRBSOffset = 10;
int periodOffset = 10;

int PRBS = 10;
int notPRBS = 11;
int CLOCK = 6;
/*
char bufferCMD[1];
char bufferFREQ[5];
char bufferPRBS[2];

*/
int CMD;
long frequency;
long duration;
double period;
int PRBSbits;
int periodLen;

// https://blog.kurttomlinson.com/posts/prbs-pseudo-random-binary-sequence
// PRBS Polynomials
int polynomial[] = {
  0b1100,
  /* PRBS4: x^4 + x^3 + 1*/
  0b10100,
  /* PRBS5: x^5 + x^3 + 1*/
  0b110000,
  /* PRBS6: x^6 + x^5 + 1*/
  0b1100000,
  /* PRBS7: x^7 + x^6 + 1*/
  0b10111000,
  /* PRBS8: x^8 + x^6 + x^5 + x^4 + 1*/
  0b100010000,
  /* PRBS9: x^9 + x^5 + 1*/
  0b1001000000,
  /* PRBS10: x^10 + x^7 + 1*/
  0b10100000000,
  /* PRBS11: x^11 + x^9 + 1*/
  0b111000001000,
  /* PRBS12: x^12 + x^11 + x^10 + x^4 + 1*/
  0b1110010000000,
  /* PRBS13: x^13 + x^12 + x^11 + x^8 + 1*/
  0b11100000000010
  /* PRBS14: x^14 + x^13 + x^12 + x^2 + 1*/
};

/* Start States 
   Used as the seed for different PRBS bits. Any non-zero value will work
*/
int start_state[] = {
  0b1111,
  0b11111,
  0b111111,
  0b1111111,
  0b11111111,
  0b111111111,
  0b1111111111,
  0b11111111111,
  0b111111111111,
  0b1111111111111,
  0b11111111111111
};

int PRBS_periodLen[] = {
  15,
  31,
  63,
  127,
  255,
  511,
  1023,
  2047,
  4095,
  8191,
  16383,
};

void setup() {
  // put your setup code here, to run once:
  // USING PORT_MANIPULATION TO REDUCE DELAYS ON PINS OUTPUTS
  // GIVE MOTIVATION BEHIND THIS
  // https://www.arduino.cc/en/Reference/PortManipulation

  DDRB = B00001101;
  DDRC = B00010000;
  DDRD = B01000000;
  //pinMode(PRBS, OUTPUT); // PRBS pin set as output
  //pinMode(notPRBS, OUTPUT); // !PRBS pin set as output
  //pinMode(CLOCK, OUTPUT); // CLOCK pin set as output. To check if bits are transmitted at set clock frequency
  //Serial.begin(115200); // Baud rate for serial communication. Yet to observe what different values do
  Serial.begin(1000000); // Baud rate for serial communication. Yet to observe what different values do
  /*
  Low baud rate introduced transients in clock signal
  */

}

void loop() {
  // put your main code here, to run repeatedly:

  while (Serial.available() > 0) { // While the input buffer is not empty

    char bufferCMD;
    char bufferFREQ[6];
    char bufferPRBS[2];
    char bufferPeriod[2];
    char Duration[5];

    //Serial.readBytes(bufferCMD,1);
    //String CMD = String(bufferCMD);

    //Serial.print("\n***Data Available***");
    //Serial.print("\n====================");

    bufferCMD = Serial.read();
    //Serial.print(bufferCMD);

    CMD = bufferCMD;

    if (CMD == 'y') {
      //Serial.print("\n__TESTING FUNCTION__\n");

      // PRBS
      String PRBS = Serial.readStringUntil(',');
      //PRBSbits = (PRBS.toInt()) - PRBSOffset;
      PRBSbits = (PRBS.toInt());

      // FREQUENCY
      String FREQUENCY = Serial.readStringUntil(',');
      //frequency = atol(FREQUENCY.c_str()) - FreqOffset;
      frequency = atol(FREQUENCY.c_str());

      // DURATION
      String DURATION = Serial.readStringUntil('z');
      duration = atol(DURATION.c_str());
      
//  Serial.print("\n\nPRE PROCESSING");
//  Serial.print("\nPRBS: ");
//  Serial.print(PRBS);
//
//  Serial.print("\nFrequency: ");
//  Serial.print(FREQUENCY);
//
//  Serial.print("\nDuration: ");
//  Serial.print(DURATION);
//
//  Serial.print("\n\nPOST PROCESSING");
//  Serial.print("\nPRBS: ");
//  Serial.print(PRBSbits);
//
//  Serial.print("\nFrequency: ");
//  Serial.print(frequency);
//
//  Serial.print("\nDuration: ");
//  Serial.print(duration);

      // TEST DURATION
      //Serial.readBytesUntil(NULL, bufferPeriod, 2);
                //Serial.readBytes(bufferPeriod, 2);
      //String period = String(bufferPeriod);
      //periodLen = (period.toInt()-1000)/100;//%1000)%100;
      //Serial.print("\nPeriod Length: ");
      //Serial.print(periodLen);
      
      bitshifter(PRBSbits,frequency,duration);
      
    } else {
      Serial.println("ELSE in the loop...");
      delayMicroseconds(200);
      SYS_pause();
      delayMicroseconds(200);
    }
  }
  SYS_pause();
}

void bitshifter( int PRBS_bits, long frequency, long duration) {
//void bitshifter(int PRBS_bits, int frequency, int CYCLES/runTime) {

  /*
  PRBS_bits have been offset by 4 due to array indexing
  */
  int taps = polynomial[PRBS_bits - 4];
  int lfsr = start_state[PRBS_bits - 4];
  int loc_start = start_state[PRBS_bits - 4];
  int PRBS_Period = PRBS_periodLen[PRBS_bits-4];

  //double tSamp = ((double)toMicro  / ((double)frequency * 2 ));
  //double tSamp2 = tSamp -4.4;

  long tSamp = ((long)toMicro  / ((long)frequency * 2 ));
  long tSamp2 = tSamp-1.5;
  
  //long temp;
  //temp = ((float)toMicro / frequency); // Delay in microseconds

  //uint32_t duration = 4 * 1000; // 5 seconds as a start..., to use input from user

  //Serial.print("\nSampling time:");
  //Serial.print(tSamp);
  //tSamp = tSamp/2;
  
  //int CYCLES = 0; // Change to either a time based run or cycle based variable
  //int period = 0;
  //int prbs = 0;
  //int lsb;
  
  //PORTD = B00001000;
  //delayMicroseconds(tSamp);
  //PORTD = B00000000;

 // while(CYCLES != period_Len){
  PORTD = B00000000;
  PORTC = B00000000;

  int lsb;
  //uint32_t tStart = millis();
  //uint32_t tStart = micros();
  long tStart = micros();

  long durMS = duration*1000;
  long LStart = (long)tStart;

   // Might change to do-while....
  while ((micros() - LStart ) < durMS)
  {
  //int period = 0;
  //int prbs = 0;
  //int lsb;
  
//  do {
    //PORTD ^= B01000000;
    //PORTC ^= B00010000;
   // delayMicroseconds(tSamp/2);
    //++period; // Number of bits to represent PRBS period length 2^n - 1
    //toggle = !toggle;
    
    //PORTD ^= B00001000;
    //delayMicroseconds(tSamp2); 
    delayMicroseconds(tSamp2);
    //_delay_us(tSamp2);
    //PORTC ^= B00010000;
    lsb = lfsr & 1; /* Get LSB (i.e., the output bit). */
    //prbs = prbs + lsb;
    lfsr >>= 1; /* Shift register */ //Shift entire PRBs sequence
    
    //delayMicroseconds(tSamp);
    
    if (lsb == 1) { /* Only apply toggle mask if output bit is 1. */
      lfsr ^= taps; /* Apply toggle mask, value has 1 at bits corresponding to taps, 0 elsewhere. */
      // ONES
     PORTC ^= B00010000;
      //PORTC = B00010000;
      PORTB = B00000100;
    }
    else{
      //ZEROS
     PORTC ^= B00010000;
      //PORTC = B00010000;
      PORTB = B00001000;
    }
    //delayMicroseconds(tSamp);
    //PORTC = B00000000;
    //PORTC ^= B00010000;

  //} while (lfsr != loc_start);
  //CYCLES++;
  
  //delayMicroseconds(tSamp);
  //delayMicroseconds(tSamp2);
  //PORTC ^= B00010000;

  if(lfsr==loc_start)
  {
    PORTD ^= B01000000;
  }

  //if((Serial.available() > 0) ){//){&& (Serial.read()=='x')){
  if((Serial.available() > 0) && (Serial.read()=='x')){
  //  Serial.println("\nTEST INTERRUPT");
    SYS_pause();
    break;
    }
   
  delayMicroseconds(tSamp2);
  PORTC ^= B00010000; 
//  }
//  if( (micros() - tStart) > duration)
//  {
//    Serial.println("\nTEST COMPLETE");  
  }
  
}

void SYS_pause() {
  delayMicroseconds(200);
  PORTB = B00000001; // Open both switches using Gate Drive Disable Pin
  delayMicroseconds(200);
  PORTC = B00000000;
}
