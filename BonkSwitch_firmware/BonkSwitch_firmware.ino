/** BonkSwitch
 *
 * See https://www.ti.com/tool/TIDA-00289
 *
 *
 * Written for the [Adafruit Feather M0 Basic Proto](https://www.adafruit.com/product/2772)
 * Using https://github.com/bolderflight/SBUS
 */
#include <elapsedMillis.h>
#include <SBUS.h>

/** Settings **/
const unsigned int loop_duration_us = 10000; // 100Hz → 10ms
// forward duration
// return duration
// RC channel settings


/** Pin Map **/
#define PIN_VCC_MON       (A0)
#define PIN_24V_MON       (A5)
#define PIN_5V_MON        (A4)
#define PIN_I_SW1_MON     (A1)
#define PIN_I_SW2_MON     (A2)
#define PIN_EN_PSU        (13)  // active low
#define PIN_EN_SW         (12)
#define PIN_SW1_EN        (11)
#define PIN_SW2_EN        (10)
#define PIN_FAULT         (5)
#define PIN_SBUS_IN       (0)

/** Prototypes **/
unsigned int convert_ADC(int channel);
SBUS rc_rx(Serial1);
elapsedMicros loop_elapsed_us;

/** Globals **/
enum _states {s_Failsafe, s_Idle, s_Firing, s_Returning} current_state;
volatile unsigned int counter_packet_missed = 0x00;
bool flag_packet_rdy;
uint16_t channels[16];
bool failSafe;
bool lostFrame;
unsigned int mon_vcc;
unsigned int mon_24v;
unsigned int mon_5v;
unsigned int mon_sw1;
unsigned int mon_sw2;
unsigned int eqv_12v;
unsigned int coil_current[2][20];
unsigned int coil_curr_index = 0;
String debug_buffer;


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
  digitalWrite(PIN_EN_SW, HIGH);
  digitalWrite(PIN_SW1_EN, LOW);
  digitalWrite(PIN_SW2_EN, LOW);
  digitalWrite(PIN_FAULT, HIGH);

  for (int i = 0; i < 20; i++)
  {
    coil_current[0][i] = 0x00;
    coil_current[1][i] = 0x00;
  }

  // USB UART debug setup
  Serial.begin(115200);
  Serial.println("BonkSwitch!");
  debug_buffer.reserve(128);
  while (Serial.available()) Serial.read();

  // SBus setup
  rc_rx.begin();

  // State Machine setup
  current_state = s_Failsafe;

  // finished setup
  digitalWrite(PIN_FAULT, LOW);
}

void loop(void) {

  if (loop_elapsed_us > loop_duration_us)
  {
    loop_elapsed_us -= loop_duration_us;

    // get voltage rails values
    mon_vcc = convert_ADC(PIN_VCC_MON);
    mon_24v = convert_ADC(PIN_24V_MON);
    mon_5v = convert_ADC(PIN_5V_MON);

    // calculate 12V equivalent
    eqv_12v = ((12000 * 256) / mon_24v);

    // get latest coil currents
    mon_sw1 = convert_ADC(PIN_I_SW1_MON);
    coil_current[0][coil_curr_index] = mon_sw1;
    mon_sw2 = convert_ADC(PIN_I_SW2_MON);
    coil_current[1][coil_curr_index] = mon_sw2;
    if (coil_curr_index == 19) coil_curr_index = 0;
    else coil_curr_index += 1;
    // over current protection?
    // solenoid armature position estimation

    // get latest RC packet
    flag_packet_rdy = rc_rx.read(&channels[0], &failSafe, &lostFrame);
    if(flag_packet_rdy == true) counter_packet_missed = 0x00;
    else if(counter_packet_missed < 20) counter_packet_missed ++;
    
    // get Serial commands
    for (int i = 0; i < 65; i++)
    {
      if (Serial.available() > 0) debug_buffer += char(Serial.read());
      else break;
    }
    if (debug_buffer.indexOf('\r') > 0)
    {
      if (debug_buffer.substring(0, debug_buffer.indexOf('\r')) == "STATUS")
      {
        Serial.println("BonkSwitch Status:");
        Serial.print("VCC is: "); Serial.print(mon_vcc / 1000, DEC); Serial.print('.'); Serial.print(mon_vcc % 1000, DEC); Serial.print("V\r\n");
        Serial.print("+24V is: "); Serial.print(mon_24v / 1000, DEC); Serial.print('.'); Serial.print(mon_24v % 1000, DEC); Serial.print("V\r\n");
        Serial.print("+5V is: "); Serial.print(mon_5v / 1000, DEC); Serial.print('.'); Serial.print(mon_5v % 1000, DEC); Serial.print("V\r\n");
        Serial.print("SW1 current is: "); Serial.print(mon_sw1, DEC); Serial.print("mA\r\n");
        Serial.print("SW2 current is: "); Serial.print(mon_sw2, DEC); Serial.print("mA\r\n");
        switch (current_state)
        {
          case s_Idle:
            Serial.println("Current state is: Idle");
            break;
          case s_Firing:
            Serial.println("Current state is: Firing");
            break;
          case s_Returning:
            Serial.println("Current state is: Returning");
            break;
          case s_Failsafe:
          default:
            Serial.println("Current state is: Failsafe!");
        }
        // SBUS status?
        //Serial.print(""); 
        Serial.print("The +24V PSU is "); Serial.println(!digitalRead(PIN_EN_PSU) ? "enabled" : "disabled");
        Serial.print("The switches are "); Serial.println(digitalRead(PIN_EN_SW) ? "enabled" : "disabled");
        Serial.print("Switch 1 is "); Serial.println(digitalRead(PIN_SW1_EN) ? "enabled" : "disabled");
        Serial.print("Switch 2 is "); Serial.println(digitalRead(PIN_SW2_EN) ? "enabled" : "disabled");
        Serial.print("There is "); Serial.println(digitalRead(PIN_FAULT) ? "a fault!" : "not a fault");
      }
      else if (debug_buffer.substring(0, debug_buffer.indexOf('\r')) == "PSU EN")
      {
        Serial.println("Enabling +24V PSU");
        digitalWrite(PIN_EN_PSU, LOW);
      }
      else if (debug_buffer.substring(0, debug_buffer.indexOf('\r')) == "PSU DIS")
      {
        Serial.println("Disabling +24V PSU");
        digitalWrite(PIN_EN_PSU, HIGH);
      }
      else if (debug_buffer.substring(0, debug_buffer.indexOf('\r')) == "SW1 EN")
      {
        Serial.println("Enabling SW1");
        digitalWrite(PIN_SW1_EN, HIGH);
      }
      else if (debug_buffer.substring(0, debug_buffer.indexOf('\r')) == "SW1 DIS")
      {
        Serial.println("Disabling SW1");
        digitalWrite(PIN_SW1_EN, LOW);
      }
      else if (debug_buffer.substring(0, debug_buffer.indexOf('\r')) == "SW2 EN")
      {
        Serial.println("Enabling SW2");
        digitalWrite(PIN_SW2_EN, HIGH);
      }
      else if (debug_buffer.substring(0, debug_buffer.indexOf('\r')) == "SW2 DIS")
      {
        Serial.println("Disabling SW2");
        digitalWrite(PIN_SW2_EN, LOW);
      }
      debug_buffer.remove(0);
    }

    /** weapon state machine **/
    switch (current_state)
    {
      case s_Idle:
        digitalWrite(PIN_EN_PSU, LOW);
        digitalWrite(PIN_FAULT, LOW);
        digitalWrite(PIN_SW1_EN, LOW); analogWrite(PIN_SW1_EN, 0);
        digitalWrite(PIN_SW2_EN, LOW); analogWrite(PIN_SW2_EN, 0);
        
        if(counter_packet_missed > 5 || channels[0] < 100) current_state = s_Failsafe;
        else if (channels[0] > 1500)  current_state = s_Firing;
        else if (channels[0] < 700)   current_state = s_Returning;
        break;
      case s_Firing:
        // enable firing solenoid
        digitalWrite(PIN_SW1_EN, HIGH);
        
        // ensure it doesn't over heat
        delay(3);
        analogWrite(PIN_SW1_EN, eqv_12v);
        
        if(counter_packet_missed > 5 || channels[0] < 100) current_state = s_Failsafe;
        else if (channels[0] > 700 && channels[0] < 1500)  current_state = s_Idle;
        break;
      case s_Returning:
        // enable retract solenoid
        digitalWrite(PIN_SW2_EN, HIGH);
        
        // ensure it doesn't over heat
        delay(3);
        analogWrite(PIN_SW2_EN, eqv_12v);
        
        if(counter_packet_missed > 5 || channels[0] < 100) current_state = s_Failsafe;
        else if (channels[0] > 700 && channels[0] < 1500)  current_state = s_Idle;
        break;
      case s_Failsafe:
      default:
        digitalWrite(PIN_FAULT, HIGH);
        digitalWrite(PIN_EN_PSU, HIGH);
        digitalWrite(PIN_SW1_EN, LOW);
        digitalWrite(PIN_SW2_EN, LOW);
        
        // monitor inputs for problems

        if(counter_packet_missed == 0) current_state = s_Idle;
    }

    if (loop_elapsed_us > loop_duration_us) Serial.println("Duration exceeded!");
  }

}

/**                                     Functions                                    **/


/** Returns the voltage/current measured on a pin in milliVolts/milliAmps.
    @param The pin to take the ADC reading from.
    @returns The value in milliVolts/milliAmps.
*/
inline unsigned int convert_ADC(int channel) {

  unsigned int adc_value = 0x00;
  const unsigned int _count = 0x04;
  const unsigned int _conv_vcc = 53711;  // (3.3V/1024)/(30kΩ/500kΩ)/1µV
  const unsigned int _conv_24v = 30270;  // (3.3V/1024)/(56kΩ/526kΩ)/1µV
  const unsigned int _conv_5v = 7813;    // (3.3V/1024)/(33kΩ/80kΩ)/1µV
  const unsigned int _conv_i_sw = 16113; // (3.3V/1024)/(20*0.01Ω)/1µV

  for (unsigned int i = 0; i < _count; i++) adc_value += analogRead(channel);
  adc_value /= _count;

  switch (channel) {
    case PIN_VCC_MON:
      adc_value *= _conv_vcc;
      break;
    case PIN_24V_MON:
      adc_value *= _conv_24v;
      break;
    case PIN_5V_MON:
      adc_value *= _conv_5v;
      break;
    case PIN_I_SW1_MON:
    case PIN_I_SW2_MON:
      adc_value *= _conv_i_sw;
      break;
    default:
      adc_value = 0xFFFFFFFF; // impossibly large number
  }
  return adc_value / 1000; // return milliVolts/milliAmps
}
