//Initialize pins to use for MiniZeus Zoom, focus and aperture

int zoomPin = 0;
int focusPin = 2;
int aperturePin = 4;
String header = "$MINIZEUS,";

void setup() {
  Serial.begin(9600);
}

void loop() {
 //Read in 0 to 5V signal from MiniZeus 
 
  int zoomReading = analogRead(zoomPin);
  int focusReading = analogRead(focusPin);
  int apertureReading = analogRead(aperturePin);

 //Remap 1024 bit analog reading to a 0-100 scale
 
  int zoomScaled = map(zoomReading,0,1023,0,100);
  int focusScaled = map(focusReading,0,1023,0,100);
  int apertureScaled = map(apertureReading,0,1023,0,100);

//Print the remapped values

  String message = String(header + "Z," + zoomScaled + "," + "F," + 
                        focusScaled + "," + "A," + apertureScaled);
  Serial.println(message);
  delay(200);                      
}
