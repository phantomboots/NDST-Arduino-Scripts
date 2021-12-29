These Scripts are used in Arduinos microcontroller in various locations within the Phantom system, including Subsea and surface. They can be edited and uploaded via the Arduino IDE.

**phantomHeadingDepthLeakAndUmbTurns.ino**

This script runs on an Arduino UNO located within the Phantom's surface Mux console. The arduion reads in analog output signals from Phantom power slab to ascertain Phantom's magnetic Heading, Depth, Leak Detection and Umbilical Turns. Heading is encoded as a pulse duration from the Power Slab, and the Depth is encoded as an analog voltage from 0 to 1.1V. 

Leak detection is encoded by the Power Slab as a repeating voltage oscillation of +/- 75 mV. If this state is detected, the script triggers a repeating beep tone output via a 5V DC PC beeper. A check is done to ensure that this state does not trigger during brief power oscillations during the Mux Console and/or Power Slab boot up phase.

Umbilical turns are calculated based on whether the Phantom's heading has 'wrapped around' (i.e. one full compass rotation) from 0 to 360. This is done by checking for a change in heading of +/- 90 degrees over one calculation cycles (i.e. less than 1 second). The Phantom's attitude cannot change this quickly in the real world, and this rapid variation between update cycles can thus be used to determine when the compass value rolls over 360. A check is made again depth reading; if the depth is less than 5 m, roll over could be due to the ship turning. This state is rejected, and only rotation while the vehicle is subsea is used as a valid state option. 


