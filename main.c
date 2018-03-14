#include<Servo.h> 
#include <Wire.h>

#define buzzerON digitalWrite(8, HIGH);
#define buzzerOFF digitalWrite(8, LOW);

int engineStart=0;
int throttlePosition=0;
int slope=0;
int accMode=0;
int revMode=0;
int sunroofMode=0;
int temp=0;
int distance = 0;
int srDistance = 0;
int mode=0;

Servo myServo; 
int servoPin = 13; 
int switchPin = 5; 
int sunroofSensorPin = 10; 
float travellingTime; 
float DistanceinCm; 

int revSensorPin = 11;
float pingTime;
float targetDistance; 
int motorPin = 12;
int buzzPin = 8;
int accSensorPin = 7;
int engSwitch = 2;
int accSwitch = 3;
int revSwitch = 4;
int sunroofSwitch = 5;
int frontWheel = 6;
int backWheel = 12;


void setup()
{
  digitalWrite(motorPin,LOW); 
  Wire.begin();
  
  pinMode(accSensorPin, INPUT);  //pinMode(9, OUTPUT);
  pinMode(engSwitch, INPUT);//engine on
  pinMode(accSwitch, INPUT);//acc on
  pinMode(revSwitch, INPUT);//reverse on
  pinMode(sunroofSwitch, INPUT);//sunroof on
  pinMode(buzzPin, OUTPUT);
  
  myServo.attach(servoPin);   

  pinMode(motorPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  
  //Motor is running
  //digitalWrite(motorPin, HIGH);
 
  
  Serial.begin(9600);
}

long readUltrasonicDistance(int pin)
{
  pinMode(pin, OUTPUT);                
  digitalWrite(pin, LOW);
  delayMicroseconds(2);                
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  /* Reads the pin, and returns the sound wave travel time in microseconds*/
  return pulseIn(pin, HIGH);
}


void loop()
{          

  	Wire.beginTransmission(8); 
  
    engineStart = digitalRead(engSwitch);		    
  	accMode = digitalRead(accSwitch);
    revMode = digitalRead(revSwitch);
    sunroofMode = digitalRead(sunroofSwitch);
  
  	if(engineStart==1)
     {
           // engineON;
        send_mode(0);
      
        digitalWrite(frontWheel,HIGH);
        digitalWrite(backWheel,HIGH);
      		
      		if(accMode == 1 && revMode ==0 && sunroofMode ==0)				
            {
              Serial.println("ACC MODE");
              digitalWrite(frontWheel,HIGH);
    		  digitalWrite(backWheel,HIGH);
              distance = 0.01723 * readUltrasonicDistance(accSensorPin);	
              //accModeON 
              acc();
              send_mode(1);
            }
            else if(revMode == 1 && accMode == 0 && sunroofMode ==0)
            {
                
              Serial.println("REVERSE PARKING MODE");
              /*pinMode(pingPin, INPUT);
              pingTime = pulseIn(pingPin, HIGH);
              targetDistance = pingTime/29/2;*/

              targetDistance = 0.01723 * readUltrasonicDistance(revSensorPin);
              rev(targetDistance);
              send_mode(2);
            }
            else if(sunroofMode == 1 && accMode == 0 && revMode ==0)
            {
              Serial.println("Sunroof Control Mode");
              srDistance = 0.01723 * readUltrasonicDistance(sunroofSensorPin);
              sunroof(srDistance);
              
            }
    }
  	else
    {	
      	//engineOFF;
      	//accModeOFF; 
      	digitalWrite(buzzPin, LOW);
      	analogWrite(frontWheel,0);
        analogWrite(backWheel,0);
      send_mode(6);
      
    }
 	delay(100); 				
      
}
void vehicleSpeed(void)
{
  	throttlePosition = analogRead(1)/4; 		
  	slope = analogRead(0); 		                           	  	                                                       	 
    if(slope < 50)
      throttlePosition = throttlePosition + 40;
  	if(slope > 50 && slope < 200)
      throttlePosition = throttlePosition + 30;
  	if(slope > 200 && slope < 500)
      throttlePosition = throttlePosition + 20;
  	if(slope > 500 && slope < 600)
      throttlePosition = throttlePosition;
  	if(slope > 600 && slope < 700)
      throttlePosition = throttlePosition - 20;
  	if(slope > 700 && slope < 900)
      throttlePosition = throttlePosition - 30;
  	if(slope > 900)
      throttlePosition = throttlePosition - 40;
  	if(throttlePosition > 255)
      throttlePosition = 255;
  	
  	temp = throttlePosition;
    analogWrite(frontWheel,throttlePosition);
    analogWrite(backWheel,throttlePosition);
}
void acc()
{	
  throttlePosition = temp;
  if(distance<20)							
  {  
      	analogWrite(frontWheel,1);
        analogWrite(backWheel,1);  
      	buzzerON
  }
  else
  {
      	if(distance>100)
        {  
      		throttlePosition = throttlePosition; 		
          	buzzerOFF
        }
      	else
        {
  		if(distance>20 && distance<50)
     		throttlePosition = throttlePosition/4; 			
  		if(distance>50 && distance<75)
      		throttlePosition = throttlePosition/2;  		
  		if(distance>75 && distance<100)
      		throttlePosition = (throttlePosition/4)*3; 		
              	buzzerON
        }
       if(throttlePosition > 255)
      		throttlePosition = 255;
  	   analogWrite(frontWheel,throttlePosition);
       analogWrite(backWheel,throttlePosition);
      }
}

void rev(float targetDistance)
{
    if(targetDistance<40)
    {
      analogWrite(frontWheel,0);
      analogWrite(backWheel,0);
      buzzerON
    }
  	else if(targetDistance>40 && targetDistance<255)
    {
      digitalWrite(frontWheel,153);   //half speed
      digitalWrite(backWheel,153);   //half speed
      buzzerON
    }
    else
    {
      buzzerOFF
    }
      
}

void sunroof(int srDistance)
{
  if(digitalRead(servoPin) == LOW) 
  {  
 
  if(srDistance <= 110) 
    { 
      myServo.write(90); 
      Serial.println("Obstacle found!!");
      send_mode(4);
    }     
   else 
    { 
     myServo.write(0); 
 	 Serial.println("Closing!!");
     send_mode(5);
     
  	}  
  } 
  else if (digitalRead(8) == HIGH)  
  { 
    myServo.write(90); 
    Serial.println("Opening!!"); 
    send_mode(3);
   } 
}

void send_mode(int mode)
{
    Wire.write(mode); 
    Wire.endTransmission();    // stop transmitting
}
