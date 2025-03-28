These Scripts are used in Arduinos microcontroller in various locations within the NDST Tools, including Subsea and surface. They can be edited and uploaded via the Arduino IDE. The description below describe the functions of each script in the repositories, including the location of the Arduino device running the script.

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

**Joystick_Test_June27_2019_draft.ino**

_This is a code stub, the intention of this code is to allow for an Arduino Joystick connected to BOOTS MiniZeus control box to be used to control the ROS Pan and Tilt Rotator on BOOTS. This rotator communicated via RS-485, and is accesible via the 4-port MOXA surface/subsea pairs on BOOTS. Documentation for the communications protocol for this unit can be found under the R Drive References and Specifications folder._

**EPOD Temp, Pressure, Humidity Sensor**

This script reads an Adafruit BME280 temp, pressure and humidity sensor over the i2c bus (default i2c address 0x77). It reads temp in degrees celsius, pressure is in millibars, humidity is percent humidity. The wiring diagram for a generic Arduino UNO is shown below - note that the Arduino Nano that is installed in both the BOOTS and YellowEye EPODS to support this function use pins A4 (SDA pin) and A5 (SCL pin) instead. This requires the <Wire.h> library.

This string is concatenated into an ASCII .CSV string, and output at 1Hz on the Arduino hardware serial inteface (Pin 1 for Tx output from the Arduino board). The string terminates with a <CR><LF>

![adafruit_products_BME280_arduino_I2C_breadboard_bb](https://github.com/user-attachments/assets/36918e5b-8202-4312-813d-7e0ca6c4d636)

**WaterLinke DVL A125 PD4 to ASCII**

This script reads in a PD4 formatted binary data format for DVL data, converts the 47 byte long message to ASCII text, and outputs a commma seperated value string. The altitude of each of the DVL's four beam are 1reports (cm) is calculated, and an average altitudes (average of all four beams) is calcualted as well. Data is output from the A125 DVL at 9600-8-N-

Velocity (in the DVL's local frame of reference) in the X (Fwd/Aft), Y (Port/Stbd) and Z (Up/down) axes. An 'E Velocity' value is also calculated, which is the standard deviation of the velocity in all 3 axes. Velocity readings are all in units of mm/s. The DVL's time of ping, bottom lock status, and internal sound speed are also output in the comma seperated values string. 

Data are output as fast as the Arduino unit can process the incoming binary data - it practice, this is about 4 Hz on an Arduino Uno. This arduino unit is located inside the YellowEye SCU.
