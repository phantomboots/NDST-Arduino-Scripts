// Arduino pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 1; // analog pin connected to X output
const int Y_pin = 3; // analog pin connected to Y output

//Output strings to use to actuate the Panel and tilt. Node A is the Pan function, node B is the tilt function.

String Pan_left = "A>010";
String Pan_right = "A<010";
String Pan_brake = "As064";
String Tilt_up = "B<010";
String Tilt_down = "B>010";
String Tilt_brake = "Bs032";

void setup() {
//  pinMode(SW_pin, INPUT);
//  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
}

void loop() {

//Read the status of the switch
  
//  Serial.print("Switch:  ");
//  Serial.print(digitalRead(SW_pin));
//  Serial.print("\n");


//Read the input from the joystick in the x-axis, re-map it to a binary value

  int X_val = analogRead(X_pin);
  int X_scaled = map(X_val,0,1023,0,10);
  if (X_scaled > 6) {
     Serial.print(Tilt_up);
     delay(500);
     Serial.print(Tilt_brake);
  }
  if (X_scaled < 3) {
     Serial.print(Tilt_down);
     delay(500);
     Serial.print(Tilt_brake);
  }

//Read the input from the joystick in the y-axis, re-map it to a binary value

  int Y_val = analogRead(Y_pin);
  int Y_scaled = map(Y_val,0,1023,0,10);
  if (Y_scaled > 6) {
      Serial.print(Pan_left);
      delay(500);
      Serial.print(Pan_brake);
  }
  if (Y_scaled < 3) {
      Serial.print(Pan_right);
      delay(500);
      Serial.print(Pan_brake);
  }

  delay(500);
}
