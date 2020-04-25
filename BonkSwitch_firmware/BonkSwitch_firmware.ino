/** BonkSwitch
 *
 *
 *
 *
 *
 */

//#include <> // SBus library
// elapsed time library

/** Settings */
// forward duration
// return duration
// RC channel settings

// VCC conversion
// 24V conversion
// 5V conversion
// I_SW conversion

/** Pin Map */
#define PIN_VCC_MON		(A0)
#define PIN_24V_MON		(A5)
#define PIN_5V_MON		(A9)
#define PIN_I_SW1_MON	(A1)
#define PIN_I_SW2_MON	(A2)
#define PIN_EN_PSU		(13)
#define PIN_EN_SW		(12)
#define PIN_SW1_EN		(11)
#define PIN_SW2_EN		(10)
#define PIN_FAULT		(5)
#define PIN_SBUS_IN		(0)

/** Prototypes */
unsigned int get_VCC(void);
unsigned int get_24V(void);
unsigned int get_5V(void);
unsigned int get_I_SW(byte channel);
unsigned int set_SW(byte channel);

/** Globals */
enum _states {s_Failsafe, s_Idle, s_Firing, s_Returning};


void setup(void) {
  
  pinMode(PIN_VCC_MON, INPUT);
  pinMode(PIN_24V_MON, INPUT);
  pinMode(PIN_5V_MON, INPUT);
  pinMode(PIN_I_SW1_MON, INPUT);
  pinMode(PIN_I_SW2_MON, INPUT);
  pinMode(PIN_EN_PSU, OUTPUT);
  pinMode(PIN_EN_SW, OUTPUT);
  pinMode(PIN_SW1_EN, OUTPUT);
  pinMode(PIN_SW2_EN, OUTPUT);
  pinMode(PIN_FAULT, OUTPUT);
  
  digitalWrite(PIN_EN_PSU, HIGH);
  digitalWrite(PIN_EN_SW, LOW);
  digitalWrite(PIN_SW1_EN, LOW);
  digitalWrite(PIN_SW2_EN, LOW);
  digitalWrite(PIN_FAULT, LOW);
  
  // USD UART debug setup
  
  // SBus setup
  
  // State Machine setup
  
  
}


void loop(void) {
}

/**                                     Functions                                     */

/**
 *
 *
 */
unsigned int get_VCC(void) {
  unsigned int temp_volts = 0x00;
  
  // read ADC on channel PIN_VCC_MON
  // save to temp_volts
  // convert to millivolts via conversion factor
  return temp_volts;
}

/**
 *
 *
 */
unsigned int get_24V(void) {
  unsigned int temp_volts = 0x00;
  
  // read ADC on channel PIN_24V_MON
  // save to temp_volts
  // convert to millivolts via conversion factor
  return temp_volts;
}

/**
 *
 *
 */
unsigned int get_5V(void) {
  unsigned int temp_volts = 0x00;
  
  // read ADC on channel PIN_5V_MON
  // save to temp_volts
  // convert to millivolts via conversion factor
  return temp_volts;
}

/**
 *
 *
 */
unsigned int get_I_SW(byte channel) {
  unsigned int temp_amps = 0x00;
  
  switch (channel)
  {
    case 1:
      // read ADC on channel 1 into temp_amps
      break;
    case 2:
      // read ADC on channel 2 into temp_amps
      break;
    default:
      temp_amps = 0xFFFF; // error
  };

  // convert to milliamps via conversion factor
  return temp_volts;
}














