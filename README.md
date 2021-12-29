These Scripts are used in Arduinos microcontroller in various locations within the Phantom system, including Subsea and surface. They can be edited and uploaded via the Arduino IDE.

**phantomHeadingDepthLeakAndUmbTurns.ino**

This script runs on an Arduino UNO located within the Phantom's surface Mux console. The arduion reads in analog output signals from Phantom power slab to ascertain Phantom's magnetic Heading, Depth, Leak Detection and Umbilical Turns. Heading is encoded as a pulse duration from the Power Slab, and the Depth is encoded as an analog voltage from 0 to 1.1V. 

Leak detection is encoded by the Power Slab as a repeating voltage oscillation of +/- 75 mV. If this state is detected, the script triggers a repeating beep tone output via a 5V DC PC beeper. A check is done to ensure that this state does not trigger during brief power oscillations during the Mux Console and/or Power Slab boot up phase.

Umbilical turns are calculated based on whether the Phantom's heading has 'wrapped around' (i.e. one full compass rotation) from 0 to 360. This is done by checking for a change in heading of +/- 90 degrees over one calculation cycles (i.e. less than 1 second). The Phantom's attitude cannot change this quickly in the real world, and this rapid variation between update cycles can thus be used to determine when the compass value rolls over 360. A check is made again depth reading; if the depth is less than 5 m, roll over could be due to the ship turning. This state is rejected, and only rotation while the vehicle is subsea is used as a valid state option. 

**UM7_ReadIn_ZeusandMux.ino**

This script runs on an Arduinon Nano located with the Phantom's subsea can. The script reads in Euler angles calculated from two UM7-LT IMUs boards (https://www.pololu.com/product/2763). One IMU is potted in Scotchcast and is affixed to the side of the of the Phantom's HD camera. The other IMU is located with within the Phantom's subsea can. Both IMUs communicate with the Arduino UNO using TTL serial, at a 9600-8-N-1. Pitch and roll values are read in at a rate of 1 Hz. 

These data are combines by the Arduino Nano, and output as a custom ASCII string, also at 9600-8-N-1. A MAX232 chip is used to convert from Arduino TTL serial levels to RS232 serial levels. These data are send up the tether and output over a data channel on the Phantom's 16 port MOXA. 


**MiniZeus_DA_Converter.ino**

This simple script reads analog voltages outputs from a MiniZeus Mark I camera control PCB. The cameras zoom, focus and aperture levels are output as voltages in the range of 0 to 5V. The script is used with the MiniZues Mark I controller PCBs that are used with both the Phantom and BOOTs.

These values are read in via an Arduino UNO located with the Phantom's auxilliary control box; with BOOTS, an Arduino UNO is located in the factory supplied control enclosure from Insite Pacific. Analog readings in the 0 to 1023 scale are remapped to a value between 0 to 95%. In this context, 0 is aperture fully closed, 0 is fully zoomed out (widest view) _focus values are typically around 50%, consult Insite Pacific documentation for a better understanding of focus range_. These data are formatted as a custom ASCII string, and output as RS232 serial data strings at 9600-8-N-1 at a rate of about 8 Hz.
