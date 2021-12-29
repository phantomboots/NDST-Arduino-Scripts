/* Aug 10, 2020
 * This script reads in Pitc and Roll values from RedShift labs model UM7 IMUs. One IMU is located in the Phantom SubSea MuxCan
 * and the other is mounted to the MiniZeus camera. The data as read in as NMEA strings formateed as $PCHRA,<time>,<roll>,<pitch>,<yaw>,*CRC.
 * The Yaw angle is analagous the Phantom's heading, and is discarded at this stage.
 * The NMEA string header is stripped off, as is this CRC value. Then roll, pitch are parsed out.
 * 
 * Data are send as TTL RS-232 from each UM7 to the Arduino Nano at 1 Hz. Data from sensor are combined into a new string, which is formatted as
 * 
 * $MUX_ZEUS_IMUS,<ZeusRoll>,<ZeusPitch>,<MuxRoll>,<MuxPitch>,<RollDif>,<PitchDif>
 * 
 * The Roll, Pitch and Dif are calculated as the Zeus angle minus the Mux angle, for each of the three Euler angles.
 */


#include <NeoSWSerial.h>
#include <AltSoftSerial.h>

//This runs on Arduino Nano pin 8,9 ONLY. IT CANNOT BE SWITCHED!
AltSoftSerial MuxIMU;

//Seperate buffers for the Mux and Zeus IMU read-in data, but can be the same size. 45 chars is enough to hold entire incoming NMEA msg.

const byte numChars = 45;
char receivedCharsMux[numChars]; // an array to store the received data for the MuxIMU
char receivedCharsZeus[numChars]; //an array to store the received data for the ZeusIMU

char tempCharsMux[45];  //temporary array to store the chars from receivedChars
char tempCharsZeus[45];  //temporary array to store the chars from receivedChars

//Boolean to determine if full NMEA msg has been read in.
boolean newData = false;

//Variables to hold the parsed out data from the two IMUs
char MuxHeader[] = "";
float MuxTimestamp = 0.0;
float MuxRoll = 0.0;
float MuxPitch = 0.0;
float MuxYaw = 0.0;
char MuxCRC[] = "";

char ZeusHeader[] = "";
float ZeusTimestamp = 0.0;
float ZeusRoll = 0.0;
float ZeusPitch = 0.0;
float ZeusYaw = 0.0;
char ZeusCRC[] = "";

float PitchDif = 0.0;
float RollDif = 0.0;
float YawDif = 0.0;


// software serial #1: RX = digital pin 10, TX = digital pin 11. This one is selectable.
NeoSWSerial ZeusIMU(10, 11);

void setup() {
 Serial.begin(9600);
 MuxIMU.begin(9600);
 ZeusIMU.begin(9600); 
}

void loop() {

//Parse MuxCan NMEA msg and extract variables
  MuxWithStartEndMarkers();
  showMuxData();

//Parse ZeusCan NMEA msg and extract variable, calculate dif variables, then format the msg.
  ZeusWithStartEndMarkers();
  showZeusData();

}

void showMuxData() {
 if (newData == true) {
 strcpy(tempCharsMux, receivedCharsMux);
 parseIMUMux();
 newData = false;
 }
}

void showZeusData() {
 if (newData == true) {
 strcpy(tempCharsZeus, receivedCharsZeus);
 parseIMUZeus();
 newData = false;
 }
 else {
 delay(500);
 }
 
 float PitchDif = MuxPitch - ZeusPitch;
 float RollDif = MuxRoll - ZeusRoll;
 float YawDif = MuxYaw - ZeusYaw;

 /* Apply offsets here, for the SubSea Can IMU and the Zeus IMU NOTE: the labels are reversed, the ZeusIMU is located in the
 SubSea Can and the MuxImu is located on the MiniZeus!!!!*/

 /*ZeusRoll = - 11 degrees
   ZeusPitch = + 3 degrees
   MuxRoll = no offset
   MuxPitch = -3 degrees
 MuxPitch = MuxPitch - 3;*/

 Serial.print("$ZEUS_CAN_IMUS,");
 Serial.print(MuxRoll);
 Serial.print(",");
 Serial.print(MuxPitch);
 Serial.print(",");
 Serial.print(ZeusRoll-11);
 Serial.print(",");
 Serial.print(ZeusPitch + 3);
 Serial.print(",");
 Serial.print(RollDif);
 Serial.print(",");
 Serial.println(PitchDif-3);
}


void MuxWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    static byte startMarker = 36; //ASCII character code for '$'
    static byte endMarker = 42;   //ASCII character code for '*'
    char rc;

    while (MuxIMU.available() > 0 && newData == false) {
        rc = MuxIMU.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedCharsMux[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedCharsMux[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void ZeusWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    static byte startMarker = 36; //ASCII character code for '$'
    static byte endMarker = 42;   //ASCII character code for '*'
    char rc;
 
    while (ZeusIMU.available() > 0 && newData == false) {
        rc = ZeusIMU.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedCharsZeus[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedCharsZeus[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}



void parseIMUMux() {

    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(tempCharsMux, ",");      // get the first part - the header
  strcpy(MuxHeader, strtokIndx); // copy it to Header

  strtokIndx = strtok(NULL, ",");  //this continues where the previous call left off, get the next part, the timestamp
  MuxTimestamp = atof(strtokIndx);     // convert this part to a float
  
  strtokIndx = strtok(NULL, ","); // get the next part, the roll
  MuxRoll = atof(strtokIndx);     // convert this part to an float and store it
  
  strtokIndx = strtok(NULL, ",");  //get the next part, the pitch
  MuxPitch = atof(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ",");  //get the next part, the yaw
  MuxYaw = atof(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ",");  //get the last part, the CRC
  strcpy(MuxCRC, strtokIndx);     // convert this part to a char copy it to the 'CRC' value

}

void parseIMUZeus() {

    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(tempCharsZeus, ",");      // get the first part - the header
  strcpy(ZeusHeader, strtokIndx); // copy it to Header

  strtokIndx = strtok(NULL, ",");  //this continues where the previous call left off, get the next part, the timestamp
  ZeusTimestamp = atof(strtokIndx);     // convert this part to a float
  
  strtokIndx = strtok(NULL, ","); // get the next part, the roll
  ZeusRoll = atof(strtokIndx);     // convert this part to an float and store it
  
  strtokIndx = strtok(NULL, ",");  //get the next part, the pitch
  ZeusPitch = atof(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ",");  //get the next part, the yaw
  ZeusYaw = atof(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ",");  //get the last part, the CRC
  strcpy(ZeusCRC, strtokIndx);     // convert this part to a char copy it to the 'CRC' value

}
