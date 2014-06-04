/*
  Modify from Liquid Crystal example
  For 8 x 7 segment module
  Using BBFuino from Cytron Technologies with 8x2 character LCD
 
  The circuit:
 * 8x2 character LCD connection to BBFuino
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 * 8 x7 segment module to BBFuino
 * VCC to 5V
 * GND to GND
 * DIO to D10
 * SCK to D11
 * RCK to D12
 */
 
 //1 st 7 segment for fare 
 //2nd 7 segment for speed and distance
 
  
// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Time.h>

#define LATCH  12  //pin 12 of BBFuino connect to RCK of 8x7segment module 
#define CLOCK  11  //pin 11 of BBFuino connect to SCK of 8x7segment module 
#define DATA   10  //pin 10 of BBFuino connect to DIO of 8x7segment module 
#define LED    13  //LED is connected to pin 13 of Arduino
#define  MultiplexDelay  1  //delay for multiplexing between digit on 8x7segment module
#define LEFT   0  // define the value for left justify
#define RIGHT  1  // define the value for right justify
#define BLANK 11  //array element to make 7segment blank
#define LATCH2  2  //pin 2 of BBFuino connect to RCK of 8x7segment module 
#define CLOCK2  3  //pin 3 of BBFuino connect to SCK of 8x7segment module 
#define DATA2  13  //pin 13 of BBFuino connect to DIO of 8x7segment module 
// initialize the library with the numbers of the interface pins
#define START 1
#define END 2

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
unsigned int totalDistance=0;
unsigned long totalTime=0;
unsigned long totalIncome=0;

int distance=20;
double cost=12000;
int time=20;
double speedVal=30;

boolean startJourneyVal=false;
boolean endJourneyVal=false;
boolean waitingVal=false;
boolean onJourney=false;//have to implement the mechanism

//messages to display when start event occurs
boolean sMessage=false;


// array to activate particular digit on the 8x7segment module
// it is the common anode of 7 segment
byte anode[8] = { 0b10000000,  //digit 1 from right
                    0b01000000,  //digit 2 from right
                    0b00100000,  //digit 3 from right
                    0b00010000,  //digit 4 from right
                    0b00001000,  //digit 5 from right
                    0b00000100,  //digit 6 from right
                    0b00000010,  //digit 7 from right
                    0b00000001   //digit 8 from right                                         
                    };

//array for decimal number, it is the cathode, please refer to the datasheet.
//therefore a logic low will activete the particular segment
                    //PGFEDCBA, segment on 7 segment, P is the dot
byte cathode[12] = {0b11000000,  // 0
                    0b11111001,  // 1
                    0b10100100,  // 2
                    0b10110000,  // 3
                    0b10011001,  // 4
                    0b10010010,  // 5
                    0b10000010,  // 6
                    0b11111000,  // 7
                    0b10000000,  // 8
                    0b10010000,  // 9  
                    0b01111111,  //dot                  
                    0b11111111   //blank
                    };     
 
//fucntion to send the serial data out to two 74HC595 serial to parallel shift register and activate the 7 segment.                  
void display8x7segment(byte datapin, byte clockpin, byte latchpin, byte digit, byte number)
{
    digitalWrite(latchpin, LOW);
    shiftOut(datapin, clockpin, MSBFIRST, digit); // clears the right display
    shiftOut(datapin, clockpin, MSBFIRST, number); // clears the left display
    digitalWrite(latchpin, HIGH);  
}

//function to display value on 8x7 segment display according to the justify state
void displayNumber8x7segment(byte justify, unsigned long value)
{

   byte decimal[8] = {0};   
   value = value % 100000000;  //ensure the value is within 8 digits only
   decimal[7] = value / 10000000;  //extract digit 7 from value
   value = value % 10000000;       //extract the rest of 7 digit value
   decimal[6] = value / 1000000;
   value = value % 1000000;
   decimal[5] = value / 100000;
   value = value % 100000;
   decimal[4] = value / 10000;
   value = value % 10000;
   decimal[3] = value / 1000;
   value = value % 1000;
   decimal[2] = value / 100;
   value = value % 100;
   decimal[1] = value / 10;
   decimal[0] = value % 10;
   byte zero = 0;
   if (justify == RIGHT)
   {   
     for(byte e = 8; e > 0 ; e --)
     {
       if(zero == 0)
       {
         if(decimal[e-1] != 0)          
         {
           display8x7segment(DATA, CLOCK, LATCH, anode[e-1], cathode[decimal[e-1]]);
           display8x7segment(DATA2, CLOCK2, LATCH2, anode[e-1], cathode[decimal[e-1]]);
           zero = 1;     
         }
       }
       else{ display8x7segment(DATA, CLOCK, LATCH, anode[e-1], cathode[decimal[e-1]]);
             display8x7segment(DATA2, CLOCK2, LATCH2, anode[e-1], cathode[decimal[e-1]]);
       }
     
     delay(MultiplexDelay); 
     }
   }
   else  //if justify == left
   { 
     byte d = 0;     
     for(byte e = 8; e > 0; e --)
     {       
       if(zero == 0)
       {
         if(decimal[e-1] != 0)         
         {
           display8x7segment(DATA, CLOCK, LATCH, anode[7], cathode[decimal[e-1]]);
           display8x7segment(DATA2, CLOCK2, LATCH2, anode[7], cathode[decimal[e-1]]);
           zero = 1;
           d ++;     
           delay(MultiplexDelay); 
         }
       }
       else 
       {
         display8x7segment(DATA, CLOCK, LATCH, anode[7-d], cathode[decimal[e-1]]);
         display8x7segment(DATA2, CLOCK2, LATCH2, anode[7-d], cathode[decimal[e-1]]);
         d ++;
         delay(MultiplexDelay); 
       }     
       
     }
     
   }
}

void fare(byte justify, unsigned long value)
{

   byte decimal[8] = {0};   
   value = value % 100000000;  //ensure the value is within 8 digits only
   decimal[7] = value / 10000000;  //extract digit 7 from value
   value = value % 10000000;       //extract the rest of 7 digit value
   decimal[6] = value / 1000000;
   value = value % 1000000;
   decimal[5] = value / 100000;
   value = value % 100000;
   decimal[4] = value / 10000;
   value = value % 10000;
   decimal[3] = value / 1000;
   value = value % 1000;
   decimal[2] = value / 100;
   value = value % 100;
   decimal[1] = value / 10;
   decimal[0] = value % 10;
   byte zero = 0;
   if (justify == RIGHT)
   {   
     for(byte e = 8; e > 0 ; e --)
     {
       if(zero == 0)
       {
         if(decimal[e-1] != 0)          
         {
           display8x7segment(DATA, CLOCK, LATCH, anode[e-1], cathode[decimal[e-1]]);
           
           zero = 1;     
         }
       }
       else{ display8x7segment(DATA, CLOCK, LATCH, anode[e-1], cathode[decimal[e-1]]);
             
       }
     
     delay(MultiplexDelay); 
     }
   }
   else  //if justify == left
   { 
     byte d = 0;     
     for(byte e = 8; e > 0; e --)
     {       
       if(zero == 0)
       {
         if(decimal[e-1] != 0)         
         {
           display8x7segment(DATA, CLOCK, LATCH, anode[7], cathode[decimal[e-1]]);
           
           zero = 1;
           d ++;     
           delay(MultiplexDelay); 
         }
       }
       else 
       {
         display8x7segment(DATA, CLOCK, LATCH, anode[7-d], cathode[decimal[e-1]]);
         d ++;
         delay(MultiplexDelay); 
       }     
       
     }
     
   }
}
void speedAndDistance(unsigned long speedVal, unsigned long value)
{

   byte decimal[8] = {0};   
   value = value % 10000;  //ensure the value is within 8 digits only
   decimal[7] = value / 1000;  //extract digit 7 from value
   value = value % 1000;       //extract the rest of 7 digit value
   decimal[6] = value / 100;
   value = value % 100;
   decimal[5] = value / 10;
   decimal[4] = value % 10;
   
   speedVal = speedVal % 10000;
   decimal[3] = speedVal / 1000;
   speedVal = speedVal % 1000;
   decimal[2] = speedVal / 100;
   speedVal = speedVal % 100;
   decimal[1] = speedVal / 10;
   decimal[0] = speedVal % 10;
   byte zero = 0;
   //Right side
   {   
     for(byte e = 8; e > 4 ; e --)
     {
       if(zero == 0)
       {
         if(decimal[e-1] != 0)          
         {
           display8x7segment(DATA2, CLOCK2, LATCH2, anode[e-1], cathode[decimal[e-1]]);
           
           zero = 1;     
         }
       }
       else{ display8x7segment(DATA2, CLOCK2, LATCH2, anode[e-1], cathode[decimal[e-1]]);
             
       }
     
     delay(MultiplexDelay); 
     }
   }
    // left side
   { 
     byte d = 0;     
     for(byte e = 3; e > 0; e --)
     {       
       if(zero == 0)
       {
         if(decimal[e-1] != 0)         
         {
           display8x7segment(DATA2, CLOCK2, LATCH2, anode[2], cathode[decimal[e-1]]);
           
           zero = 1;
           d ++;     
           delay(MultiplexDelay); 
         }
       }
       else 
       {
         display8x7segment(DATA2, CLOCK2, LATCH2, anode[2-d], cathode[decimal[e-1]]);
         d ++;
         delay(MultiplexDelay); 
       }     
       
     }
     
   }
}

  
void setup() {
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(LATCH2, OUTPUT);
  pinMode(CLOCK2, OUTPUT);
  pinMode(DATA2, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LATCH, HIGH);  
  digitalWrite(LED, LOW);  //off LED  
  
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(8, 2);
  // Print a message to the LCD.
  //lcd.print("8x7seg");
  //welcome();
 // delay(1000);  //delay for 1 second
 
  Wire.begin(4);//join i2c bus with address #4
  Wire.onReceive(receiveEvent);//register event
  Serial.begin(9600);//start serial for output
}

void loop(){ 
  welcome();
  showLicenseDate();
 // startJourney();//temporary
 // waiting();//temporary
 // endJourney();//temporary
 // summery();//temporary
  while(true){
    if(startJourneyVal){
      if(sMessage){
        startJourney();
        sMessage=false;
      }
      running();
      
      if(endJourneyVal){
        endJourney();
      }
      else if(waitingVal){
        waiting();
      }
    }
    
    else commonDisplay();
    
  /*  display1();
    delay(1000);
    display2();
    delay(1000);
    display3();*/
  }
  //1st demo, 8x7segment will display decimal value from 0 to 9 and dot from 1st digit (most right) until the last digit (most right) 
     
  //delay(1000);  //delay 1 second
  
  //2nd demo, 8x7segment will display same decimal from 0 to 9 and dot across all 8 digit
   
 // delay(1000);  //delay 1 second
  
  //3rd demo, 8x7segment will display a decimal value increasing like normal counter.
  
 // delay(1000);
  
}
void display1(){
  for(byte i = 0; i < 8; i++)
  {
    for(byte k = 0; k < 11; k++)
    {
       display8x7segment(DATA, CLOCK, LATCH, anode[i], cathode[k]);  
       display8x7segment(DATA2, CLOCK2, LATCH2, anode[i], cathode[k]); 
       lcd.setCursor(0,1);  //move cursor to 2nd line
       lcd.print(k);  
       lcd.print(' '); 
       delay(300);
    }
  }

}
void display2(){
  for(byte k = 0; k < 11; k++)
    {     
        display8x7segment(DATA, CLOCK, LATCH, 0xff, cathode[k]);  //activate all digit  
        display8x7segment(DATA2, CLOCK2, LATCH2, 0xff, cathode[k]);  //activate all digit
        lcd.setCursor(0,1);  //move cursor to 2nd line
        lcd.print(k);  
        lcd.print(' ');  
        delay(300);    
    } 

}
void display3(){
  for (unsigned long value = 0; value < 100000000; value ++)
  {    
     for(byte i = 0; i < 10 ; i ++)
      {
      lcd.setCursor(0,1);  //move cursor to 2nd line
      lcd.print(value);
      displayNumber8x7segment(RIGHT, value);  //display the value in right justify format    
     }       
  }

}
void welcome(){
  lcd.clear();
 
  lcd.print("Welcome");
  lcd.setCursor(2,1);
  lcd.print(" To Taxi Meter");
  delay(3000);
  lcd.clear();

}
void startJourney(){
  lcd.clear();
  lcd.print("Your Journey");
  lcd.setCursor(3,1);
  lcd.print("Starts");
  delay(2000);
  lcd.clear();
}
void running(){
  //display fare distance and speed
  lcd.print("FARE    :");
  lcd.print(cost);
  lcd.setCursor(0,1);
  lcd.print("DISTANCE:");
  lcd.print(distance);
  displayFare();
  displaySpeed();
  displayDistance();
  delay(2000);
  //display in the 7 segments also
 
  lcd.clear();
}
void endJourney(){
  lcd.clear();
  lcd.print("Your Journey");
  lcd.setCursor(3,1);
  lcd.print("Ends");
  delay(2000);
  lcd.clear();
 // lcd.setCursor(6,0);
  lcd.print("FARE    :");
  lcd.print(cost);
  lcd.setCursor(0,1);
  lcd.print("DISTANCE:");
  lcd.print(distance);
  delay(20000);
  //display in the 7 segments also
  displayFare();
  displayWaitingTime();
  displayDistance();
  
  lcd.clear();
}
void waiting(){
  lcd.clear();
  lcd.print("Waiting");
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  lcd.print(time);
  delay(2000);
  lcd.clear();
  // lcd.setCursor(6,0);
  lcd.print("FARE:  ");
  lcd.print(cost);
  lcd.setCursor(0,1);
  lcd.print("TIME:  ");
  lcd.print(time);
  //Display in the 7 segment
  displayFare();
  displayWaitingTime();
  displayDistance();
  
  //to display 2
  delay(2000);
  lcd.clear();
}
void summery(){//summery of the day
  lcd.clear();
  lcd.print("Summery");
  delay(2000);
  lcd.clear();
  lcd.print("Distance: ");
  lcd.print(totalDistance);
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  lcd.print(totalTime);
  delay(2000);
  lcd.clear();
  lcd.print("Total Income");
  lcd.setCursor(0,1);
  lcd.print(totalIncome);
  delay(2000);
  //display in the 7 segment
  //off the 7 segment display
  //can include the time also
  lcd.clear();
}
void receiveEvent(int howMany)
{
 /* while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }*/
  distance=Wire.read();
  cost=Wire.read();
  time=Wire.read();
  speedVal=Wire.read();
  int x = Wire.read();    // receive byte as an integer
  
 // Serial.println(x);         // print the integer
  if(x==START){
    //start journey 
    startJourneyVal=true;
  }
  else if(x==END){
    //end journey
    totalDistance+=distance;
    totalIncome+=cost;
    startJourneyVal=false;
    endJourneyVal=true;
    
  }
  
  //update cost waiting time and distance accordingly
  
 
}
void showLicenseDate(){
  //this will show the license date and other dates
  lcd.clear();
  lcd.print("Next Licens Date");
  lcd.setCursor(3,1);
  lcd.print("2014-06-28");
  delay(4000);
  lcd.clear();
}

void commonDisplay(){
  lcd.clear();
  lcd.print("Common Display");
  delay(300);
  lcd.clear();
}

void displayFare(){
//delay(3000);
//fare(RIGHT,400);
while(!endJourneyVal & !waitingVal){
  fare(RIGHT, cost);
  speedAndDistance(speedVal,distance);
}
}
void displaySpeed(){


}
void displayWaitingTime(){

}
void displayDistance(){


}


