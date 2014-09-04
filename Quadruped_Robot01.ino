#include <Servo.h>

// All pin definitions


const int PIN_BACKSERVO = 9;
const int PIN_FRONTSERVO = 10;
const int PIN_ECHO = 11;
const int PIN_PING = 12;
const int PIN_STARTBUTTONSTATE = 13;


const int MIN_DISTANCE = 70;              // minimum distance in cm before deciding to walk around obstacles
const int MAX_DISTANCE = 200;          // maximum distance in cm known to the system
const int PING_INTERVALL = 1500;

const int ANGLE_NEUTRAL = 80;
const int FWD_ANGLE1 = 55;
const int FWD_ANGLE2 = 105;
const int SIDE_ANGLE1 = 40;
const int SIDE_ANGLE2 = 120;

const bool DEBUG = true;                      // enable some printing

int ISMOVING = false;
int STEPS_DELAY = 120;                       // delay between steps in ms

int BUTTONSTATE;
int DISTANCE_CONSTRAINT;
long DISTANCE;
long PREVIOUSDISTANCE=0;
int CURRENT_ANGLE;


unsigned long time;
long oldtime = 0;

Servo FRONTSERVO,BACKSERVO;       // our servo objects


// Our walk sequences
int FORWARDS[] = {FWD_ANGLE1,FWD_ANGLE2,FWD_ANGLE2,FWD_ANGLE2,FWD_ANGLE2,FWD_ANGLE1,FWD_ANGLE1,FWD_ANGLE1};
int BACKWARDS[] = {FWD_ANGLE1,FWD_ANGLE1,FWD_ANGLE1,FWD_ANGLE2,FWD_ANGLE2,FWD_ANGLE2,FWD_ANGLE2,FWD_ANGLE1};
int SIDEWAYS[] = {SIDE_ANGLE1,SIDE_ANGLE2,SIDE_ANGLE2,SIDE_ANGLE2,SIDE_ANGLE2,SIDE_ANGLE1,SIDE_ANGLE1,SIDE_ANGLE1};

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_STARTBUTTONSTATE,INPUT);
  pinMode(PIN_PING,OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  FRONTSERVO.attach(PIN_FRONTSERVO);
  BACKSERVO.attach(PIN_BACKSERVO);
}
  
void loop()
{ 
  BUTTONSTATE=digitalRead(PIN_STARTBUTTONSTATE);
  if (BUTTONSTATE == HIGH || ISMOVING==true)
  {  
    ISMOVING = true;    
    
    DISTANCE = getdistance();
    
    if (DISTANCE >= MIN_DISTANCE) 
    {
      walk_fwd();
    } 
    else 
    {
      walk_bck(); 
    }
  } 
  
  else 
  {
    ISMOVING = false;
  }
} 

long getdistance() 
{
  long duration,cm;
  
  // Here we send a ping and measure the time until it bounces back
  digitalWrite(PIN_PING, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_PING, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_PING, LOW);
  
  duration = pulseIn(PIN_ECHO, HIGH);
  
  cm = microsecondsToCentimeters(duration);

  if (DEBUG==true)
  {
    Serial.print("Distance: ");
    Serial.print(cm);
    Serial.print(" cm\n"); 
  }
  return cm;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the DISTANCE of the
  // object we take half of the DISTANCE travelled.
  return microseconds / 29 / 2;
}


void resetservo(Servo& myservo)
{
  myservo.write(ANGLE_NEUTRAL);  
}


void walk_fwd() 
{
  for(int n=0;n<4;n++)
  {
    FRONTSERVO.write(FORWARDS[2*n]);
    BACKSERVO.write(FORWARDS[(2*n)+1]);
    delay(STEPS_DELAY ); 
    Serial.println(STEPS_DELAY );
  }
}

void walk_bck() 
{
  for(int n=0;n<4;n++)
  {
    FRONTSERVO.write(BACKWARDS[2*n]);
    BACKSERVO.write(BACKWARDS[(2*n)+1]);
    delay(STEPS_DELAY ); 
    Serial.println(STEPS_DELAY );
  }
}

void walk_left() 
{ 
  for(int n=0;n<4;n++)
  {
    FRONTSERVO.write(SIDEWAYS[2*n]);
    BACKSERVO.write(SIDEWAYS[(2*n)+1]);
    delay(STEPS_DELAY ); 
    Serial.println(STEPS_DELAY );
  }
}

void walk_right() 
{

  for(int n=0;n<4;n++)
  {
    FRONTSERVO.write(SIDEWAYS[(2*n)+1]);
    BACKSERVO.write(SIDEWAYS[2*n]);
    delay(STEPS_DELAY ); 
    Serial.println(STEPS_DELAY );
  }
}


void idle() 
{
  resetservo(FRONTSERVO);
  resetservo(BACKSERVO);
}
