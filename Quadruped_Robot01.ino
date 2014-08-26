#include <Servo.h>

// All pin definitions

const int PIN_LED_GREEN = 3;
const int PIN_LED_RED = 4;
const int PIN_BACKSERVO = 9;
const int PIN_FRONTSERVO = 10;
const int PIN_ECHO = 11;
const int PIN_PING = 12;

const int PIN_STARTBUTTONSTATE = 13;
const int MIN_DISTANCE = 15;           // minimum distance in cm before switching to BACKWARDSs
const int MAX_DISTANCE = 200;          // maximum distance in cm known to the system

const int ANGLE_NEUTRAL = 80;
const int FWD_ANGLE1 = 55;
const int FWD_ANGLE2 = 105;
const int SIDE_ANGLE1 = 40;
const int SIDE_ANGLE2 = 120;

const bool DEBUG = false;              // enable some printing

int BUTTONSTATE;
int ISMOVING = false;
int STEPS_DELAY = 120;                  // delay between steps in ms
int DISTANCE_CONSTRAINT;
long DISTANCE;

Servo FRONTSERVO,BACKSERVO;             // our servo objects


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
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  FRONTSERVO.attach(PIN_FRONTSERVO);
  BACKSERVO.attach(PIN_BACKSERVO);
}
  
void loop()
{ 
  BUTTONSTATE=digitalRead(PIN_STARTBUTTONSTATE);
  if (BUTTONSTATE == HIGH || STARTEDMOVING==true)
  {  
    STARTEDMOVING = true;
    
    DISTANCE = getdistance();
    DISTANCE_CONSTRAINT = constrain(DISTANCE, MAX_DISTANCE, MIN_DISTANCE);
    STEPS_DELAY  = map(DISTANCE_CONSTRAINT, MAX_DISTANCE,MIN_DISTANCE,400,120); // this is our speed mofifier based on DISTANCE
      
    if (DISTANCE >= MIN_DISTANCE) // if we are 10 cm close to something, start walking BACKWARDSs
    {
      digitalWrite(PIN_LED_GREEN, HIGH);
      digitalWrite(PIN_LED_RED, LOW);
      walk_fwd();
    } 
    else 
    {
      digitalWrite(PIN_LED_GREEN, LOW);
      digitalWrite(PIN_LED_RED, HIGH);
      walk_left(); 
    }
  }
  else 
  {
    STARTEDMOVING = false;
  }    
} 

long getdistance() 
{
  long duration, cm;
  
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
    Serial.println();
    Serial.print(cm);
    Serial.print(" cm\t");  
    return cm;
  }
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
  FRONTSERVO.write(SIDEWAYS[2*n]);
  BACKSERVO.write(SIDEWAYS[(2*n)+1]);

  for(int n=0;n<4;n++)
  {
    BACKSERVO.write(FORWARDS[(2*n)+1]);
  }
}


void idle() 
{
  resetservo(FRONTSERVO);
  resetservo(BACKSERVO);
}
