// Requires remote XBee in loopback mode (DIN connected to DOUT)
//Code implemented by Kevin Delgado, Tarun Repala, Arthur Shweitzer
//RGB Color Sensor code taken from instructables.com/id/Using-an-RGB-LED-to-Detect-Colours/?ALLSTEPS

#include <SoftwareSerial.h>
#include <Servo.h>                           // Include servo library

#define Rx 10                                // DOUT to pin 10
#define Tx 11                                // DIN to pin 11 
SoftwareSerial Xbee (Rx, Tx);
SoftwareSerial mySerial = SoftwareSerial(255, Tx);


Servo servoLeft;                             // Declare left servo signal
Servo servoRight;                            // Declare right servo signal


boolean isCommunicating = false;
boolean isXbeeOn = false;


// Define colour sensor LED pins
int ledArray[] = {2,3,8};

// boolean to know if the balance has been set
boolean balanceSet = false;

//place holders for colour detected
int red = 0;
int green = 0;
int yellow = 0;

//floats to hold colour arrays
float colourArray[] = {0,0,0};
float whiteArray[] = {241.00,208.00,184.00};
float blackArray[] = {68.00,73.00,60.00};

//place holder for average
int avgRead;

int counter =0;
int finalHitch = 0;
int theCount =0;


int incomingInt = 0;
int robotNumber = 0;
int maxIter = 1000;
int iterCount = 0;
char myID = 'o';
char robotIDs[] = {'y', 'b', 'o', 'r', 'g'};
boolean done = false;

void setup(){
 
  //setup the outputs for the colour sensor
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(8,OUTPUT);
 
  //begin serial communication
  Serial.begin(9600);
 
  servoLeft.attach(13);                             
  servoRight.attach(12);  
  
  delay (100);
 
}

//main program loop
void loop(){
    if(!done){
      lineFollow();
    }
    else{
      sendSignal();
    }
    delay(25); 
}

void still()  {
  servoLeft.writeMicroseconds(1500);         // Left wheel stop
  servoRight.writeMicroseconds(1500);        // Right wheel stop
}

void forward(){
  servoLeft.writeMicroseconds(1600);         // Left wheel counterclockwise
  servoRight.writeMicroseconds(1400);        // Right wheel clockwise
}

void slightForward(){
  servoLeft.writeMicroseconds(1550);         // Left wheel counterclockwise
  servoRight.writeMicroseconds(1450);        // Right wheel clockwise
}

void back()  {
  servoLeft.writeMicroseconds(1400);         // Left wheel clockwise
  servoRight.writeMicroseconds(1600);        // Right wheel counterclockwise
}

void hardLeft()  {
  servoLeft.writeMicroseconds(1480);         // Left wheel clockwise
  servoRight.writeMicroseconds(1450);        // Right wheel clockwise
}

void slightLeft()  {
  servoLeft.writeMicroseconds(1500);         // Left wheel clockwise
  servoRight.writeMicroseconds(1460);        // Right wheel clockwise
}

void perpLeft()  {
  servoLeft.writeMicroseconds(1450);         // Left wheel clockwise
  servoRight.writeMicroseconds(1450);        // Right wheel clockwise
}

void hardRight()  {
  servoLeft.writeMicroseconds(1550);         // Left wheel counterclockwise
  servoRight.writeMicroseconds(1520);        // Right wheel counterclockwise
}

void slightRight()  {
  servoLeft.writeMicroseconds(1540);         // Left wheel counterclockwise
  servoRight.writeMicroseconds(1500);        // Right wheel counterclockwise
}

void perpRight()  {
  servoLeft.writeMicroseconds(1550);         // Left wheel counterclockwise
  servoRight.writeMicroseconds(1550);        // Right wheel counterclockwise
}

void processHashMark()  {                         //If you're on your course
    if(counter<3){
    still();
    delay(25);
    forward();
    delay(500);
    still();
    checkColour();
    checkColour();
    checkColour();
    printColour();
    theCount = theCount + presentValue();
    display(presentColor(), theCount);
    counter++;
    delay(20);
   }
    
    else if(counter == 3)  {      //If you're on the long line
      still();     
      getRobotNumber();
      display("Orange Squadron", robotNumber);
      waitForSignal();  // Wait until other robot sends the right signal
      counter++;
      delay(100);
      
    }
    else if(counter < 4 + (5 - robotNumber)) {   //If you're going to final mark
      counter++;
      delay(100);
    }
    else if(counter == 4 + (5 - robotNumber)) {  //If you've reached your mark
      still();
      delay(50);
      servoLeft.detach();             // Attach left signal to pin 13
      servoRight.detach();            // Attach right signal to pin 12
      done = true;
      delay(100);      // Indicate that you're done with the IDC
    }
    slightForward();    //Move forward off the hashmark
    delay(100);
}
  
  ///////
  void lineFollow(){
  if(RCcolor(4) == "black" && RCcolor(5) == "white" && RCcolor(6) == "white" && RCcolor(7) == "white"){ //hard right
    hardRight();
  }
  else if(RCcolor(4) == "black" && RCcolor(5) == "black" && RCcolor(6) == "white" && RCcolor(7) == "white"){ //slight right
    slightRight();
  }
  else if(RCcolor(4) == "white" && RCcolor(5) == "black" && RCcolor(6) == "black" && RCcolor(7) == "white"){ //go forward
    forward();
  }
  else if(RCcolor(4) == "white" && RCcolor(5) == "white" && RCcolor(6) == "black" && RCcolor(7) == "black"){ //slight left
    slightLeft();
  }
  else if(RCcolor(4) == "white" && RCcolor(5) == "white" && RCcolor(6) == "white" && RCcolor(7) == "black"){ // hard left
    hardLeft();
  }
  else if(RCcolor(4) == "black" && RCcolor(5) == "black" && RCcolor(6) == "black" && RCcolor(7) == "black"){ //stop
    processHashMark();

  }
  else if(RCcolor(4) == "black" && RCcolor(5) == "black" && RCcolor(6) == "black" && RCcolor(7) == "white"){ //perp right
    perpRight();

  }
  else if(RCcolor(4) == "white" && RCcolor(5) == "black" && RCcolor(6) == "black" && RCcolor(7) == "black"){ //perp left
    perpLeft();

  }
  else{     //all white, go forward
    forward();
  }
 } 
  ///////
  
  long RCTime(int sensorIn){
   long duration = 0;
   pinMode(sensorIn, OUTPUT);     // Make pin OUTPUT
   digitalWrite(sensorIn, HIGH);  // Pin HIGH (discharge capacitor)
   delay(1);                      // Wait 1ms
   pinMode(sensorIn, INPUT);      // Make pin INPUT
   digitalWrite(sensorIn, LOW);   // Turn off internal pullups
   while(digitalRead(sensorIn)){  // Wait for pin to go LOW
      duration++;
   }
   return duration;
}

String RCcolor(int sensorIn){
  if(RCTime(sensorIn) > 100){
    return "black";
  }
  else{
    return "white";
  }
}

void listenForSignal(){
 if(Xbee.available()) {            // Is data available from XBee?
   char id = Xbee.read();
   if(checkID(id)){
     char incoming[2];
     Xbee.readBytesUntil('t',incoming,2);
     incomingInt = incoming[0] - '0';
     if(incomingInt > 0 && incomingInt < 10)  {
         Serial.println(incomingInt);       //   send to Serial Monitor
      }
    }
  }
}

// This is a method to send signals in a format that only the other robots
// on this team will recognize. This will filter out noise from other teams
void sendSignal()  {
  Xbee.begin(9600);
  while(done) {
    String outputStr = String(String(myID) + String(robotNumber) + String('t'));
  //if(Serial.available()) {          // Is serial data available?
    //char outgoing = Serial.read();  // Read character, send to XBee
    Xbee.print(outputStr);
    Serial.println(outputStr);
  }
  Xbee.end();
  
  //First character represents the robotID, second is the robotNumber.
  //and third is the terminating character.
}

// This is a helper method for listenForSignal. Checks if the first character
// from an incoming signal is a valid robotID
boolean checkID(char c)  {
  for(int i = 0; i < sizeof(robotIDs); i++){
    char id = robotIDs[i];
    if(id == c)
      return true;
  }
  return false;
}

// Method for making the robot wait if it isn't first and
// if the robot before it hasn't sent its signal, which should occur
// when the robot before has reached its final hashmark.
// The robot will also wait for a maximum of 25(delay time) * 400(maxIterations)
// milliseconds until it goes ahead
void waitForSignal()  {
  if(robotNumber != 1)  {
    servoLeft.detach();
    servoRight.detach();
    Xbee.begin(9600);
    while(((incomingInt != robotNumber - 1) && iterCount < 500 + maxIter * (robotNumber-2)))  {
      still();
      listenForSignal();
      delay(25);
      iterCount++;
    }
    Xbee.end();
    servoLeft.attach(13);
    servoRight.attach(12);
  }
}
  
  
/////////
void checkBalance(){
  //check if the balance has been set, if not, set it
  if(balanceSet == false){
    setBalance();
  }
}

////////
void setBalance(){
  //set white balance
   delay(5000);                              //delay for five seconds, this gives us time to get a white sample in front of our sensor
  //scan the white sample.
  //go through each light, get a reading, set the base reading for each colour red, green, and blue to the white array
  for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);
     delay(100);
     getReading(5);          //number is the number of scans to take for average, this whole function is redundant, one reading works just as well.
     whiteArray[i] = avgRead;
     digitalWrite(ledArray[i],LOW);
     delay(100);
  }
  //done scanning white, now it will pulse blue to tell you that it is time for the black (or grey) sample.
   //set black balance
    delay(5000);              //wait for five seconds so we can position our black sample 
  //go ahead and scan, sets the colour values for red, green, and blue when exposed to black
  for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);
     delay(100);
     getReading(5);
     blackArray[i] = avgRead;
     //blackArray[i] = analogRead(2);
     digitalWrite(ledArray[i],LOW);
     delay(100);
  }
   //set boolean value so we know that balance is set
  balanceSet = true;
  delay(5000);     //delay another 5 seconds to let us catch up
  }
  //////
void checkColour(){
    for(int i = 0;i<=2;i++){
     digitalWrite(ledArray[i],HIGH);  //turn or the LED, red, green or blue depending which iteration
     delay(100);                      //delay to allow CdS to stabalize, they are slow
     getReading(5);                  //take a reading however many times
     colourArray[i] = avgRead;        //set the current colour in the array to the average reading
     float greyDiff = whiteArray[i] - blackArray[i];                    //the highest possible return minus the lowest returns the area for values in between
     colourArray[i] = (colourArray[i] - blackArray[i])/(greyDiff)*255; //the reading returned minus the lowest value divided by the possible range multiplied by 255 will give us a value roughly between 0-255 representing the value for the current reflectivity(for the colour it is exposed to) of what is being scanned
     
     digitalWrite(ledArray[i],LOW);   //turn off the current LED
     delay(100);
  }
 
 
}
///////
void getReading(int times){
  int reading;
  int tally=0;
  //take the reading however many times was requested and add them up
for(int i = 0;i < times;i++){
   reading = analogRead(0);
   tally = reading + tally;
   delay(10);
}
//calculate the average and set it
avgRead = (tally)/times;
}
///////
//prints the colour in the colour array, in the next step, we will send this to processing to see how good the sensor works.
void printColour(){
Serial.print("R = ");
Serial.println(int(colourArray[0]));
Serial.print("G = ");
Serial.println(int(colourArray[1]));
Serial.print("B = ");
Serial.println(int(colourArray[2]));
Serial.println(presentColor());
//delay(2000);
}

String presentColor(){
  
  if ((int(colourArray[0]) - int(colourArray[1])) > 150){
    return "RED";
    Serial.println("RED");
  }
  else if (int(colourArray[0]) < 160){
    return "GREEN";
    Serial.println("Green");
  }
  else if ((int(colourArray[0])-int(colourArray[2])) > 45){
    return "YELLOW";
    Serial.println("yellow");
  }
  else{
    return "WHITE";
    Serial.println("white");
  }
}

void display(String myColor,  int count) {
    
  pinMode(Tx, OUTPUT);
  digitalWrite(Tx, HIGH);
  
  mySerial.begin(9600);
  delay(100);
  mySerial.write(12);                 // Clear             
  mySerial.write(17);                 // Turn backlight on
  delay(5);                           // Required delay
  mySerial.print(myColor);  // First line
  mySerial.write(13);                 // Form feed
  mySerial.print(count);   // Second line
  mySerial.write(212);                // Quarter note
  mySerial.write(220);                // A tone
  delay(3000);                        // Wait 3 seconds
  mySerial.write(18);                 // Turn backlight off

}
int presentValue(){
  
  if ((int(colourArray[0]) - int(colourArray[1])) > 150){
    return 0;
  }
  else if (int(colourArray[0]) < 160){
    return 3;
  }
  else if ((int(colourArray[0])-int(colourArray[2])) > 45){
    return 2;
  }
  else{
    return 1;
  }
}

int getRobotNumber(){
  robotNumber = theCount;
  return robotNumber;
}



