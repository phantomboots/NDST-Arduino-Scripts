// *********************************************************************
// *********************************************************************
// *                                                                   *
// * Phantom Heading, Depth, Leak Detection and Umbilical Turn Counter *
// *                                                                   *
// *********************************************************************
// *********************************************************************
//
//
// last updated March 3 2021
//
//
// This program performs several functions. The Ardiuno interrupts analog sigals from the Phantom power slab to ascertain the 
// Phantom ROV's Heading, Depth, Leak Detection and number of Umbilical turns and outputs as Serial data.
//  
//
// * Heading   -> Read in the 5V DC pulse from the Phantom Aux output port (Pin 1 = Common, Pin 4 = Heading Signal)
//                The heading information is carried while the signal on Pin 4 of the Aux output cable is LOW. Measure this duration (msecs) and convert to
//                heading information. Each degree is approx. 182us of low pulse width. Total pulse width is 100ms.
//
// * Depth     -> Depth is measured from the Phantom Aux outport (Pin 3) on Arduino analog pin 5. Each milivolt of signal is proportional to 1 foot of depth.
//                This value is converted to meters for serial display. Leak Detection is also alerted via Depth min. When a leak is detected a connection is
//                shorted and triggers a +/-75mV osciallation. The function phantomDepth() uses the Quickstats.h library function stddev(samples[],m) to analyze a 
//                9 part float array for a standard devivation of greater than 20(mV or ft) for more than a period of time.
//
// * Umbilical -> This Displays the number of turns the Phantom ROV has done on this dive so you ascertain if the cable is twisting too much.
//   Turns   
//
// example output @ heading 120degree, 30m below surface and 0 umbilical turns: 
// $ROV_HDT_DPT_UTN,120,30,0


#include "AltSoftSerial.h"
#include "QuickStats.h"
#include "math.h"

#define PHANTOMTIMECONST 182  // time in microseconds per degree that Phantom Heading PW output will be low

#define HDTPULSEPIN 7         // ardiuno Uno input pin
#define DEPTHPIN 5            // analog pin A5 to read depth value
#define LEAKBEEPPIN 4         // digital PWM pin 9 to power computer speaker


AltSoftSerial altSerial;  // Create an additional serial port instance, using AltSoftSerial library
QuickStats Stats;         // Create a stats object

String header = "$ROV_HDT_DPT_UTN,";     // Serial String output header

// variables for phanthom Heading
unsigned long phantomRovPulseLowUSecs; 
unsigned long phantomHeadingDegree;

// variables for phantom Depth and Leak Detection
int phantomDepthMeters;
int leak_time = 0;
float leak_array[10];
float depth_array[9];

// variables for phantom Umbillical Turns Counter
int phantomUmbilicalTurns = 0;
int headingNow, headingThen, headingDifference = 0;


void setup() 
{
   pinMode(HDTPULSEPIN, INPUT); // Reads PWM from Phantom slab to ascertain vehicle heading
   analogReference(INTERNAL);   // Sets the upper end of the analogRead() function to 1.1 V, so full range of measure is 1.1/1024 or 1 mV per increment.
   
   Serial.begin(9600);          // Initialize serial communications at 9600 baud. 
   altSerial.begin(9600);       // Initialize alternate serial communications at 9600 baud. 

   delay(1000);
   
   phantomInit();
}


void loop() 
{
  phantomHeading();             // Obtains ROV heading and stores in variable phantomHeadingDegree
  phantomDepth();               // Obtains ROV depth and stores in variable phantomDepthMeters. Will also monitor for vehicle leak will alert with 600Hz tone output from pin4 to a computer speaker.
  phantomUmbillicalTurns();     // Increments/Decrements a turn counter for every revolution that the ROV performs. This is stored in variable phantomUmbilicalTurns.
  phantomOutput();              // Outputs all ascertained data as a Serial string via altSerial. Example string -> $ROV_HDT_DPT_UTN,120,30,0 

  delay(100);
}

void phantomInit()
{
  altSerial.println("Phantom Heading, Depth, Leak Detection and Umbilical Turn Counter Initialize..."); 
  delay(500);
  phantomHeading();             // Run function to ascertain heading so you can load headThen variable with initial state
  headingThen = phantomHeadingDegree;
}

void phantomHeading()
{
  //Read in the pulse lengths.
  noInterrupts();                // Disable Interrupts so as not to distrub the pulse width time measurement.
  phantomRovPulseLowUSecs = pulseIn(HDTPULSEPIN, LOW, 500000UL);
  phantomHeadingDegree = (phantomRovPulseLowUSecs / PHANTOMTIMECONST);
  interrupts();                  // Enable Interrupts as measurement is completed
}

void phantomDepth()
{
  //Take 9 readings in succession, and then average them. Help to remove some of the variability in the signal. Check this array for larger fluctuations in depth; if they are detected, trigger the leak detector.
  for(int i=8; i > -1; i--)
  {
    if(i == 0)
    {
      depth_array[i] = analogRead(DEPTHPIN);
    } 
    else 
    {
      depth_array[i] = depth_array[i -1];
    }
  }

  float depth_avg = Stats.average(depth_array, 9);
  float meters = (depth_avg * 0.3584) - 42; //Scales 0-1023 A to D values to a depth in meters. Empirically derived values, based on passing small incremental voltages through benchtop power supply.
  
  phantomDepthMeters = int(meters + 0.5);   //Round the depth to nearest meter.

  //To catch leak detection behaviour, store depth readings in an array of 9 values, continuously update this array. If wild fluctuations are
  float leak_val = Stats.stdev(depth_array, 9);

  //If the standard deviation of the leak value array exceeds 20, there is a large variation in the depths, and this should be reflected on the 
  //power slab's display as well. 
  
  if(leak_val < 20)  //If the leak value is active, and it has been for more than 5 seconds, trigger the leak sensor response.
  {
    leak_time = 0;
  } 
  else 
  {
    leak_time = leak_time + 1; //Increment by approximately the number of milliseconds per progam loop.
    if(leak_time > 20)         //If the leak value is still active, trigger the response.
    {
      tone(LEAKBEEPPIN, 600, 1000);
    }
  }
}

void phantomUmbillicalTurns()
{
   //Compare current heading value to the one from previous iteration of the loop, find the difference.

  int headingCorrect = wrap360(phantomHeadingDegree);
  
  headingNow = headingCorrect;
  headingDifference = headingNow - headingThen;

  if(headingDifference == 0)
  {}
  else if(headingDifference < -90) //If difference between last and current value is highly negative, increment the counter. Choose a value of 90, since
  {                      //unlikely that the vehicle will turn more than 90 degrees/sec in water.
    phantomUmbilicalTurns++ ;
  }
  else if(headingDifference > 90) //If difference between last and current value is highly positive, decrement the counter
  {
    phantomUmbilicalTurns-- ;
  }
  
  headingThen = headingNow;
  
  //To protect against errant umbilical turn flip/flops at surface, lock the umbilical turns to 0 when the depth is less than 10 m.
  if(phantomDepthMeters < 5)
  {
    phantomUmbilicalTurns = 0;
  }

}

void phantomOutput()
{
  //Print Output String ->  $ROV_HDT_DPT_UTN,xxx,xxx,x
  altSerial.print(header);
  altSerial.print(phantomHeadingDegree);
  altSerial.print(",");
  altSerial.print(phantomDepthMeters);
  altSerial.print(",");
  altSerial.print(phantomUmbilicalTurns);
  altSerial.println();
}

int wrap360(int direction)
{
  while(direction > 359) direction -= 360;
  while(direction < 0) direction += 360;
  return direction;
}
