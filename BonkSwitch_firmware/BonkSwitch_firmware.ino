/** BonkSwitch
 *
 *
 *
 *
 * Written for the [Adafruit Feather M0 Basic Proto](https://www.adafruit.com/product/2772)
 * Using https://github.com/bolderflight/SBUS
 */
#include <elapsedMillis.h>
#include "SBUS.h"

/** Settings */
const unsigned int loop_duration_us = 10000; // 100Hz
// forward duration
// return duration
// RC channel settings


/** Pin Map */
#define PIN_VCC_MON        (A0)
#define PIN_24V_MON        (A5)
#define PIN_5V_MON        (A9)
#define PIN_I_SW1_MON	(A1)
#define PIN_I_SW2_MON	(A2)
#define PIN_EN_PSU        (13)
#define PIN_EN_SW        (12)
#define PIN_SW1_EN        (11)
#define PIN_SW2_EN        (10)
#define PIN_FAULT        (5)
#define PIN_SBUS_IN        (0)

/** Prototypes */
unsigned int convert_ADC(int channel);
unsigned int set_SW(byte channel);
SBUS rc_rx(Serial1);
elapsedMicros loop_elapsed_us;

/** Globals */
enum _states {s_Failsafe, s_Idle, s_Firing, s_Returning} current_state;
volatile unsigned int counter_packet_missed = 0x00;
volatile unsigned int counter_BIST = 0x00; // built in self-test
unsigned int value_bist;
bool flag_packet_missed;
uint16_t channels[16];
bool failSafe;
bool lostFrame;

unsigned int coil_current[2][20];
unsigned int coil_curr_index = 0;



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
  digitalWrite(PIN_FAULT, HIGH);
  
  // USB UART debug setup
  Serial.begin(115200);
  Serial.println("BonkSwitch!");
  
  // SBus setup
  rc_rx.begin();
  
  // State Machine setup
  current_state = s_Failsafe;
  
  
  // finished setup
  digitalWrite(PIN_FAULT, LOW);
}


void loop(void) {
  
  if(loop_elapsed_us > loop_duration_us)
  {
    loop_elapsed_us -= loop_duration_us;
    
    // Built in self-test
    switch (counter_BIST)
    {
      case 10:
        value_bist = convert_ADC(PIN_VCC_MON);
		if (value_bist < 24000 || value_bist > 61200)
		{
			Serial.println("FAULT: VCC is more than 20% out of range!");
			digitalWrite(PIN_FAULT, HIGH);
		}
        break;
      case 20:
        value_bist = convert_ADC(PIN_24V_MON);
		if ((value_bist < 19200 || value_bist > 28800) && digitalRead(PIN_EN_PSU) == LOW)
		{
			Serial.println("FAULT: +24V is more than 20% out of range!");
			digitalWrite(PIN_FAULT, HIGH);
		}
		else if (value_bist > 800 && digitalRead(PIN_EN_PSU) == HIGH)
		{
			Serial.println("FAULT: +24V is disabled but powered!");
			digitalWrite(PIN_FAULT, HIGH);
		}
		break;
      case 30:
        value_bist = convert_ADC(PIN_5V_MON);
		if(value_bist < 4000 || value_bist > 6000)
		{
			Serial.println("FAULT: +5V is more than 20% out of range!");
			digitalWrite(PIN_FAULT, HIGH);
		}
        break;
      default:
        // do nothing
    }
    if(counter_BIST >= 39) counter_BIST = 0;
    else counter_BIST += 1;
    
    // get latest coil currents
	coil_current[0][coil_curr_index] = convert_ADC(PIN_I_SW1_MON);
	coil_current[1][coil_curr_index] = convert_ADC(PIN_I_SW2_MON);
	if(coil_curr_index == 19) coil_curr_index = 0;
	else coil_curr_index += 1;
	// over current protection?
	// solenoid armature position estimation
	
    // get latest RC packet
    flag_packet_missed = rc_rx.read(&channels[0], &failSafe, &lostFrame)
    if(flag_packet_missed == TRUE) counter_packet_missed += 1;
    else if(counter_packet_missed < 5) counter_packet_missed = 0;
    else counter_packet_missed -= 5;
    
    switch (current_state)
    {
      case s_Idle:
	    // do nothing until channel is wiggled
        // can go to s_Firing or s_Failsafe
        // current_state = s_Firing;
        // current_state = s_Failsafe;
        break;
      case s_Firing:
		// enable firing solenoid
		// ensure it doesn't over heat
        // can go to s_Returning or s_Failsafe
        // current_state = s_Returning;
        // current_state = s_Failsafe;
        break;
      case s_Returning:
   		// enable retract solenoid
		// ensure it doesn't over heat
        // can go to s_Idle or s_Failsafe
        // current_state = s_Idle;
        // current_state = s_Failsafe;
        break;
      case s_Failsafe:
      default:
        digitalWrite(PIN_FAULT, HIGH);
        Serial.println("Hit failsafe!");
        // release both solenoids
        // monitor inputs for problems
        
        // can go to s_Idle
        // current_state = s_Idle;
      
    }
  if(loop_elapsed_us > loop_duration_us) Serial.println("Duration exceeded!");
  }
  
}

/**                                     Functions                                     */


/** Returns the voltage/current measured on a pin in milliVolts/milliAmps.
  * @param The pin to take the ADC reading from.
  * @returns The value in milliVolts/milliAmps.
  */
inline unsigned int convert_ADC(int channel) {

  unsigned int adc_val = 0x00;
  const unsigned int _count = 0x04;
  const unsigned int _conv_vcc = 53711; // (3.3V/1024)/(30kΩ/500kΩ)/1µV
  const unsigned int _conv_24v = 30270; // (3.3V/1024)/(56kΩ/526kΩ)/1µV
  const unsigned int _conv_5v = 6445; // (3.3V/1024)/(30/500)/1µV
  const unsigned int _conv_i_sw = 16113; // (3.3V/1024)/(20*0.01Ω)/1µV
  
  for(int i = 0; i < _count; i++) adc_val += analogRead(channel);
  adc_vale /= _count;
  
  switch (channel)
  {
    case PIN_VCC_MON:
      // convert to VCC, in microVolts
      adc_value *= _conv_vcc;
      break;
    case PIN_24V_MON:
      // convert to 24V, in microVolts
      adc_value *= _conv_24v;
      break;
    case get_5V:
      // convert to 5V, in microVolts
      adc_value *= _conv_5v;
      break;
    case PIN_I_SW1_MON:
    case PIN_I_SW2_MON:
      // convert to I_SW*, in microVolts
      adc_value *= _conv_i_sw;
      break;
    default:
      adc_val = 0xFFFFFFFF; // impossibly large number
      // error!
  }
  return adc_val/1000; // return milliV
}


