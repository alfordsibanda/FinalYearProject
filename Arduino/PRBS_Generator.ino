long toMicro = 1000000;

int CMD;
long frequency;
long duration;
double period;
int PRBSbits;
int periodLen;

// PRBS Polynomials
int polynomial[] = {
  0b1100, /* PRBS4: x^4 + x^3 + 1 */
  0b10100, /* PRBS5: x^5 + x^3 + 1 */
  0b110000, /* PRBS6: x^6 + x^5 + 1 */
  0b1100000, /* PRBS7: x^7 + x^6 + 1 */
  0b10111000, /* PRBS8: x^8 + x^6 + x^5 + x^4 + 1 */
  0b100010000, /* PRBS9: x^9 + x^5 + 1 */
  0b1001000000, /* PRBS10: x^10 + x^7 + 1 */
  0b10100000000, /* PRBS11: x^11 + x^9 + 1 */
  0b111000001000, /* PRBS12: x^12 + x^11 + x^10 + x^4 + 1 */
  0b1110010000000, /* PRBS13: x^13 + x^12 + x^11 + x^8 + 1 */
  0b11100000000010  /* PRBS14: x^14 + x^13 + x^12 + x^2 + 1 */
};

/* Start States 
   Used as the seed for different PRBS bits. Any non-zero value will work
*/
int start_state[] = {
  0b1111,           // PRBS4
  0b11111,          // PRBS5
  0b111111,         // PRBS6
  0b1111111,        // PRBS7
  0b11111111,       // PRBS8
  0b111111111,      // PRBS9
  0b1111111111,     // PRBS10
  0b11111111111,    // PRBS11
  0b111111111111,   // PRBS12
  0b1111111111111,  // PRBS13
  0b11111111111111  // PRBS14
};

int PRBS_periodLen[] = {
  15, 	 // PRBS4
  31, 	 // PRBS5	
  63, 	 // PRBS6
  127,	 // PRBS7
  255,	 // PRBS8
  511,	 // PRBS9
  1023,	 // PRBS10
  2047,	 // PRBS11
  4095,	 // PRBS12
  8191,  // PRBS13
  16383, // PRBS14
};

void setup() {
  /* INITIALISE */

  DDRB = B00001101; // !PRBS, PRBS, Gate Drive Disable
  DDRC = B00010000; // Sample Frequency
  DDRD = B01000000; // PRBS Clock
  Serial.begin(1000000); // Baud rate for serial communication
  
  /*   Low baud rate introduced transients in clock signal  */

}

void loop() {
  /* MAIN LOOP */

  while (Serial.available() > 0) { // While the input buffer is not empty

    char bufferCMD;
    char bufferFREQ[6];
    char bufferPRBS[2];
    char bufferPeriod[2];
    char Duration[5];

    bufferCMD = Serial.read();
    CMD = bufferCMD;

    if (CMD == 'y') {
	  // PRBS
      String PRBS = Serial.readStringUntil(',');
      PRBSbits = (PRBS.toInt());

      // FREQUENCY
      String FREQUENCY = Serial.readStringUntil(',');
      frequency = atol(FREQUENCY.c_str());

      // DURATION
      String DURATION = Serial.readStringUntil('z');
      duration = atol(DURATION.c_str());
      bitshifter(PRBSbits,frequency,duration);
      
    } 
	else {  
      delayMicroseconds(200);
      SYS_pause();
      delayMicroseconds(200);
    }
  }
  SYS_pause();
}

void bitshifter( int PRBS_bits, long frequency, long duration) {

  /*
  PRBS_bits have been offset by 4 due to array indexing
  */
  int taps = polynomial[PRBS_bits - 4];
  int lfsr = start_state[PRBS_bits - 4];
  int loc_start = start_state[PRBS_bits - 4];
  int PRBS_Period = PRBS_periodLen[PRBS_bits-4];

  long tSamp = ((long)toMicro  / ((long)frequency * 2 ));
  long tSamp2 = tSamp-1.5;
  
  PORTD = B00000000;
  PORTC = B00000000;

  int lsb;
  long tStart = micros();

  long durMS = duration*1000;
  long LStart = (long)tStart;

  while ((micros() - LStart ) < durMS)
  {
    delayMicroseconds(tSamp2); 
    lsb = lfsr & 1; /* Get LSB (i.e., the output bit). */
    lfsr >>= 1; /* Shift entire PRBs sequence */ 
    
    if (lsb == 1) { /* Only apply toggle mask if output bit is 1. */
      lfsr ^= taps; /* Apply toggle mask, value has 1 at bits corresponding to taps, 0 elsewhere. */
     // ONES
     PORTC ^= B00010000;
     PORTB = B00000100;
    }
    else{
     //ZEROS
     PORTC ^= B00010000;
     PORTB = B00001000;
    }

  if(lfsr==loc_start)
  {
    PORTD ^= B01000000;
  }

  if((Serial.available() > 0) && (Serial.read()=='x')){
    SYS_pause();
    break;
    }
   
  delayMicroseconds(tSamp2);
  PORTC ^= B00010000; 
  }
  
}

void SYS_pause() {
  delayMicroseconds(200);
  PORTB = B00000001; // Open both switches using Gate Drive Disable Pin
  delayMicroseconds(200);
  PORTC = B00000000;
}
