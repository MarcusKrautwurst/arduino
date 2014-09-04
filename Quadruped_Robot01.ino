#include <Servo.h>

const int PIN_servo_back = 9;
const int PIN_servo_front = 10;
const int PIN_ECHO = 11;
const int PIN_PING = 12;
const int PIN_STARTbuttonstate = 13;

const int min_distance = 70;            // minimum distance in cm before deciding to walk around obstacles
const int max_distance = 200;           // maximum distance in cm known to the system
const int ping_intervall = 1500;        // The intervall when we send a ping to the ultrasonic sensor 

const int angle_default = 80;           // Default angle
const int a1_fwd = 55;                  // First angle for forward walking 
const int a2_fwd = 105;                 // Second angle for forward walking 
const int a1_side = 40;                 // First angle for sideways walking
const int a2_side = 120;                // Second angle for sideways walking

const bool debug = true;                // enable some printing

int isMoving = false;
int steps_delay = 120;                  // delay between steps in ms

int buttonstate;
int distance_constraint;
long distance;
long previous_distance=0;

unsigned long time;
long oldtime = 0;   

Servo servo_front,servo_back;           // our servo objects


// Locomotion sequences

int forward[] = {a1_fwd,a2_fwd,a2_fwd,a2_fwd,a2_fwd,a1_fwd,a1_fwd,a1_fwd};
int backward[] = {a1_fwd,a1_fwd,a1_fwd,a2_fwd,a2_fwd,a2_fwd,a2_fwd,a1_fwd};
int sideway[] = {a1_side,a2_side,a2_side,a2_side,a2_side,a1_side,a1_side,a1_side};


// -------------------- INITALIZATION --------------------

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_STARTbuttonstate,INPUT);
  pinMode(PIN_PING,OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  servo_front.attach(PIN_servo_front);
  servo_back.attach(PIN_servo_back);  

// --------------------- MAIN LOOP ---------------------

void loop()
{ 
  buttonstate=digitalRead(PIN_STARTbuttonstate);
  if (buttonstate == HIGH || isMoving==true)
  {  
    isMoving = true;    
    
    distance = getdistance();
    
    if (distance >= min_distance) 
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
    isMoving = false;
  }
} 

// -------------------- PING FUNCTIONS --------------------

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

  if (debug==true)
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

// -------------------- LOCOMOTION FUNCTIONS --------------------

void resetservo(Servo& myservo)
{
  myservo.write(angle_neutral);  
}


void walk_fwd() 
{
  for(int n=0;n<4;n++)
  {
    servo_front.write(forward[2*n]);
    servo_back.write(forward[(2*n)+1]);
    delay(steps_delay); 
  }
}

void walk_bck() 
{
  for(int n=0;n<4;n++)
  {
    servo_front.write(backward[2*n]);
    servo_back.write(backward[(2*n)+1]);
    delay(steps_delay); 
  }
}

void walk_left() 
{ 
  for(int n=0;n<4;n++)
  {
    servo_front.write(sideway[2*n]);
    servo_back.write(sideway[(2*n)+1]);
    delay(steps_delay); 
  }
}

void walk_right() 
{

  for(int n=0;n<4;n++)
  {
    servo_front.write(sideway[(2*n)+1]);
    servo_back.write(sideway[2*n]);
    delay(steps_delay); 
  }
}


void idle() 
{
  resetservo(servo_front);
  resetservo(servo_back);
}
