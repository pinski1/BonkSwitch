# Bonk Switch - Design Verification Test Plan
Need to test the design to verify it meets the specification.

## Equipment Required

|Equipment|Model|Serial Number|
|---------|-----|-------------|
|PSU, 60V 3A|||
|XT60 cable|||
|XT30 10Ω load|||
|Multi-meter|||
|Oscilloscope|||
|Oscilloscope probes|||
|USB Micro cable|||
|Thermal Camera|||

## Test Plan

|Test|Description|Criteria|Pass |Notes|
|----|-----------|--------|-----|-----|
|Visually inspect the board|Is anything out of place?|Shorts|❌| |
|||Tombstoning|❌| |
|||PCB Faults|❌| |
|||Cold joints|❌| |
|||Serial Number present|❌| |
|Test for Shorts|Multi-meter on conitinuity mode||
|||OC between VCC (TP3) & GND|❌| |
|||OC between +24V (TP1) & GND|❌| |
|||OC between +5V (TP2) & GND|❌| |
|||OC between +3V3 (A1.p2) & GND|❌| |

Remove the Feather for the following tests.

Apply current limited +40V @ 3A power via the XT60.

|Test|Description|Criteria|Pass |Notes|
|----|-----------|--------|-----|-----|
|    |           |        |❌| |
|Measure the voltage|VCC (TP3) & GND |VCC ±5%|❌| |
||+5V (TP2) & GND|+4.75V & +5.25V|❌| |
||+24V (TP1) & GND|0V & +0.08V|❌| |
|Check LEDs|+5V LED (LD2)|lit|❌| |
||+24V LED (LD1)|unlit|❌| |
||Fault LED (LD3)|unlit|❌| |
||SW1 LED (LD4)|unlit|❌| |
||SW2 LED (LD5)|unlit|❌| |

Place a jumper between pin 25 and pin 4 on the Feather to enable the +24V SMPS.

|Test|Description|Criteria|Pass |Notes|
|----|-----------|--------|-----|-----|
|Measure the voltage|+24V (TP1) & GND |+21.60V & +26.40V|❌| |
|Check LEDs|+24V LED (LD1)|lit|❌| |
|Check Board Temp|USe thermal camera|No hot spots|❌| |

Remove power to board and jumper.

Program Feather with test script.

Fit Feather to board.

Apply current limited +40V @ 3A power via the XT60.

|Test|Description|Criteria|Pass |Notes|
|----|-----------|--------|-----|-----|
|Feather powered|Check LEDs on feather|lit|❌| |
|Feather communicating|Plug USB cable in|Characters on terminal|❌| |
|ADC reading correctly|VCC_MON|raw: xxx to xxx|❌| |
||+5V_MON|raw: xxx to xxx|❌| |
||I_SW1_MON|raw: xxx to xxx|❌| |
||I_SW2_MON|raw: xxx to xxx|❌| |
|Feather controls +24V|Enable +24V SMPS|TP1 +21.60V & +26.40V|❌| |
|Check LEDs|Fault LED (LD3)|lit|❌| |
|ADC reading correctly|+24V_MON|raw: xxx to xxx|❌| |

* FrSky RX testing
   * recieving FrSky packets
   * able to bind FrSky to RX
   * can see sticks moving
* MOSFET switch testing
   * check MOSFETs are driven on with minimal ringing
   * check MOSFET on resistance is reasonable
   * check SW1 is feather controlled
   * check SW2 is feather controlled
   * check against load of 10Ω
   * check current if I_SW\*_MON
* confirm operation when VCC >50V and <35V

## Test Conclusion

1. Fault with design
   1. PCB mod
   2. PCB fix
  


