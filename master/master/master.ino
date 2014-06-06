/*
start journey, end journey and waiting messages will be send to the slave at the initializing point only. 
Other times values will be send accordingly
*/
// set pin numbers:
#include<Wire.h>
#define START 1
#define END 2
#define WAITING 3
#define DISTANCE 0.75
#define INITIAL_KM 50
#define KM 30
#define WAITING_HOUR 30


unsigned long rpm=0;
unsigned long rpmPrev=0;

const int startPin = 2;    // the number of the pushbutton pin
const int endPin=4;
const int ledPin = 13;      // the number of the LED pin
const int ledPin2=12;
const int ledPin3=11;
const int input =8;      //input of the taxi meter sensor

//Variables to store temporary values
int distance=90;
int cost=89;
int time=678;  //time per 5 rounds
int speedVal=111;
int timerValue=0;
int waitingTime=0;

//Variables to store temporary states
boolean startJourneyVal=false;
boolean endJourneyVal=false;
boolean waitingVal=false;
boolean onJourney=false;//have to implement the mechanism

// Variables will change:
int startLedState = HIGH;         // the current state of the output pin
int endLedState=HIGH;
int inputLedState=HIGH;

int startButtonState;             // the current reading from the input pin
int endButtonState;
int inputButtonState;
int lastStartButtonState = LOW;   // the previous reading from the input pin
int lastEndButtonState=LOW;
int lastInputButtonState=LOW;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastStartDebounceTime = 0;  // the last time the output pin was toggled
long lastEndDebounceTime=0;
long lastInputDebounceTime=0;
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(startPin, INPUT);
  pinMode(endPin,INPUT);
  pinMode(input,INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2,OUTPUT);
  pinMode(ledPin3,INPUT);

  // set initial LED state
  digitalWrite(ledPin, startLedState);
  digitalWrite(ledPin2,endLedState);
  
  //communication between the slave starts
  Wire.begin();
}
byte x=0;
void loop() {
  

while(true){
    
  // read the state of the switch into a local variable:
  int reading = digitalRead(startPin);
  int readingEnd=digitalRead(endPin);
  int readingInput=digitalRead(input);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  //check the start button state
  // If the switch changed, due to noise or pressing:
  if (reading != lastStartButtonState) {
    // reset the debouncing timer
    lastStartDebounceTime = millis();
  }
  if ((millis() - lastStartDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != startButtonState) {
      startButtonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (startButtonState == HIGH) {
        startLedState = !startLedState;//remove this line from the code not necessary
        startJourneyVal=true;
        
      }
    }
  }
  
  if ((millis() - lastStartDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != startButtonState) {
      startButtonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (startButtonState == HIGH) {
        startLedState = !startLedState;//remove this line from the code not necessary
        startJourneyVal=true;
        
        Wire.beginTransmission(4);//transmit to device #4
        Wire.write(distance);
        Wire.write(cost);
        Wire.write(time);
        Wire.write(speedVal);
        Wire.write(START);
        Wire.endTransmission();
        
      }
    }
  }
   if (readingEnd != lastEndButtonState) {
    // reset the debouncing timer
    lastEndDebounceTime = millis();
  }  
  
  //check the end button state 
   if ((millis() - lastEndDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingEnd != endButtonState) {
      endButtonState = readingEnd;

      // only toggle the LED if the new button state is HIGH
      if (endButtonState == HIGH) {
        endLedState = !endLedState;
        endJourneyVal=true;
        Wire.beginTransmission(4);//transmit to device #4
        //Wire.write("x is ");
        Wire.write(distance);
        Wire.write(cost);
        Wire.write(time);
        Wire.write(END);
        
        Wire.endTransmission();
        
      }
    }
  }
  
  // set the LEDs:
  digitalWrite(ledPin, startLedState);
  digitalWrite(ledPin2,endLedState);
  // save the reading.  Next time through the loop,
  // it'll be the lastStartButtonState:
  lastStartButtonState = reading;
  lastEndButtonState=readingEnd;
  
    if ((millis() - lastInputDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingInput != inputButtonState) {
      inputButtonState = readingInput;

      // only toggle the LED if the new button state is HIGH
      if (inputButtonState == HIGH) {
        inputLedState = !inputLedState;
       rpm
        
      }
    }
  }
    //calculate the journey
    
    
  if(startJourneyVal){
    rpmPrev=rpm;
      calculateDistance();
      calculateSpeed();
      calculateFare();
      
      
      if(endJourneyVal){
        startJourneyVal=false;
        endJourneyVal=false;
        
      }
      if(waitingVal){
        
        calculateWaitingTime();
        calculateFare();
      }
      sendToSlave();
  }
  //calculation for waiting
  //enable waitng and call for appropriate methods to do the rest
  //
  
}//end of the while loop
  
}
void calculateDistance(){
  rpm++;
  distance=rpam*DISTANCE;
}
void calculateWaitingTime(){
  if(rpmVal==rpm){
    waitingVal=true;
    //timer on  
  }
  //waitingTime=timer

}
void calculateSpeed(){
  //timerValue update from the timer
  if(rpm!=0 & rpm%5 == 0){
    time=timerValue;//this should be updated with the timer value
    speedVal=5*DISTANCE/time;
    timerValue=0;
    //reset the timer
  }

}
void calculateFare(){
  if(distance<=1000)
    cost=INITIAL_KM;
  else{
    cost+=(distance-1000)*KM;
  }

}
void sendToSlave(){
  

}
void clear(){
  rpm=0;
  distance=0;
  
}
