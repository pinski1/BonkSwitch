# Bonk Switch - Design Verification Test Plan
Need to test the design to verifiy it meets the specification.

## Test Plan

|Test|Description|Criteria|Pass |Fail |Notes|
|----|-----------|--------|-----|-----|-----|
|Visually inspect the board|Is anything out of place?|Shorts| [ ] | [ ] | |
|||Tombstoning| [ ] | [ ] | |
|||PCB Faults| [ ] | [ ] | |
|||Cold joints| [ ] | [ ] | |
|||Serial Number present| [ ] | [ ] | |

* VCC/+24V/+5V/+3V3 not shorted to GND

Remove the Feather for the following tests.

Apply current limited +40V @ ?mA power via the XT60.

|Test|Description|Criteria|Pass |Fail |Notes|
|----|-----------|--------|-----|-----|-----|
|    |           |        | [ ] | [ ] |     |


* Measure the voltage at:
  * TP3		VCC ±5%
  * TP2		+5V ±5%
  * TP1		0V
* +5V LED (LD2) lit
* +24V LED (LD1) unlit
* Fault LED (LD3) unlit
* SW1 LED (LD4) unlit
* SW2 LED (LD5) unlit

Place a jumper between pin 25 and pin 4 on the Feather to enable the +24V SMPS.

* Measure the voltage at:
  * TP1		+24V ±10%
* no hot spots with all rails powered
* +24V LED (LD1) lit

Remove power to board and jumper.

Program Feather with test script.

Fit Feather to board.

Apply current limited +40V @ ?mA power via the XT60.

|Test|Description|Criteria|Pass |Fail |Notes|
|----|-----------|--------|-----|-----|-----|
|    |           |        | [ ] | [ ] |     |


* Verify LEDs on feather light up
* connect to feather via USB serial
* record voltages/raw ADC values for
  * VCC_MON
  * +5V_MON
  * +24V_MON
* verify the feather can control the +24V supply
* verify the fault LED (LD3) lights

* Something about FrSky reciever testing

* MOSFET switch testing

* confirm operation when VCC >50V and <35V

## Test Conclusion





