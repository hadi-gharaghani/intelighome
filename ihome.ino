#include<SoftwareSerial.h>
#include <LedControl.h>
#include <Servo.h>


//value ......
int DIN = 5;
int CS =  6;
int CLK = 7;
char value = 0;
//.....
int leftRightPos = 0;         // set a variable to store the servo position
int index = 0;                // the index of the current reading
int total = 0;                // the total of all readings
int average = 0;              // the average
int echoPin = 8;              // the SRF05's echo pin
int initPin = 9;              // the SRF05's init pin
unsigned long pulseTime = 0;  // variable for reading the pulse
unsigned long distance = 0;   // variable for storing distance
const int numReadings = 10;   // set a variable for the number of readings to take


Servo leftRightServo;      
SoftwareSerial alfha(2,3);
LedControl lc = LedControl(DIN,CLK,CS,0);

void setup(){
   leftRightServo.attach(11);
   pinMode(initPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(12,OUTPUT);
   
   Serial.begin(9600);
   alfha.begin(9600);
   alfha.println(" Start");
 
   lc.shutdown(0,false);
   lc.setIntensity(0,8);
   lc.clearDisplay(0);
  
   byte w[8] ={0x00,0x5E,0x00,0x04,0x02,0x52,0x0C,0x00};
   printByte(w);
   digitalWrite(12,1);
}
void loop() {  
   
   byte on[8] =         {0x08,0x14,0x2A,0xDD,0xDD,0x2A,0x14,0x08,};
   byte off[8] =        {0x08,0x14,0x22,0xC1,0xC1,0x22,0x14,0x08,};
   byte secure[8] =     {0x20,0x60,0xC0,0x60,0x30,0x18,0x0C,0x06,};
   byte not_secure[8] = {0x83,0xC2,0x66,0x18,0x18,0x24,0x67,0xC1,};
   byte omg[8] =        {0x00,0x5E,0x00,0x04,0x02,0x52,0x0C,0x00};
   
   if(alfha.available() > 0){
   value = alfha.read();
   alfha.print(value);
   alfha.print("\n");
   
   //off lamp clicked on application do this !
   if(value == '1'){
        lc.clearDisplay(0);
        digitalWrite(12,0);
        printByte(on);
   }
   //on lamp clicked off application do this !
   else if(value == '0'){
        lc.clearDisplay(0);
        digitalWrite(12,1); 
        printByte(off);
      }
   
   
   //not-secure butten clicked on application do this !
   else if(value == '2'){
       
        lc.clearDisplay(0);
        digitalWrite(12,1);
        printByte(secure);
        
   do{   
   for(leftRightPos = 0; leftRightPos < 180; leftRightPos+=10) {  // going left to right.
      leftRightServo.write(leftRightPos);
      
      for (index = 0; index<=numReadings;index++) {            // take x number of readings from the sensor and average them
        digitalWrite(initPin, LOW);
        delayMicroseconds(50);
        digitalWrite(initPin, HIGH);                           // send signal
        delayMicroseconds(50);                                 // wait 50 microseconds for it to return
        digitalWrite(initPin, LOW);                            // close signal
        
        pulseTime = pulseIn(echoPin, HIGH);                    // calculate time for signal to return
        distance = pulseTime/58;                               // convert to centimetres
        total = total + distance;                              // update total
        
        if(total <= 70){
          lc.clearDisplay(0);
          printByte(omg);
          digitalWrite(12,0); 
          delay(500);
          digitalWrite(12,1);
          delay(500);
          digitalWrite(12,0);
          delay(1000);
          digitalWrite(12,1);        
          
        }
        delay(10);
      }
      
    average = total/numReadings;                               // create average reading
 
    if (index >= numReadings)  {                               // reset the counts when at the last item of the array
      index = 0;
      total = 0;
    }
    Serial.print("X");                                         // print leading X to mark the following value as degrees
    Serial.print(leftRightPos);                                // current servo position
    Serial.print("V");                                         // preceeding character to separate values
    Serial.println(average);                                   // average of sensor readings
  }
  
  /*
  start going right to left after we got to 180 degrees
  same code as above
  */
  for(leftRightPos = 180; leftRightPos > 0; leftRightPos-=10) {// going right to left
    leftRightServo.write(leftRightPos);
    
    for (index = 0; index<=numReadings;index++) {
      digitalWrite(initPin, LOW);
      delayMicroseconds(50);
      digitalWrite(initPin, HIGH);
      delayMicroseconds(50);
      digitalWrite(initPin, LOW);
      
      pulseTime = pulseIn(echoPin, HIGH);
      distance = pulseTime/58;
      total = total + distance;
      
      if(total <= 70){
         lc.clearDisplay(0);
         printByte(omg);
         digitalWrite(12,0); 
         delay(500);
         digitalWrite(12,1);
         delay(500);
         digitalWrite(12,0);
         delay(1000);
         digitalWrite(12,1);
         
        }
      delay(10);
     }
    average = total/numReadings;
    if (index >= numReadings)  {
      index = 0;
      total = 0;
     }
    Serial.print("X");
    Serial.print(leftRightPos);
    Serial.print("V");
    Serial.println(average);
     }
      value = alfha.read();
    } while(value != '3');
    printByte(not_secure);  
   }
   //secure butten clicked on application do this !
   else if(value == '3')
   {
     lc.clearDisplay(0);
     digitalWrite(12,1);
     printByte(not_secure);
   }
  }
}

   //just setting
void printByte(byte character [])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
}
