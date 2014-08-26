#include <Servo.h>


const int greenLedPin = 3;
const int redLedPin = 4;
const int echoPin = 11;
const int pingPin = 12;
const int buttonPin = 13;
const int minDistance = 15; // minimum distance in cm before switching to backwards
int button;
int startedMoving = false;
int mydelay=120;
long distance;
int cstr_dist;

Servo frontservo,backservo;

int neutralAngle = 80;
int a1 = 55;
int a2 = 105;

int forward[] = {a1,a2,a2,a2,a2,a1,a1,a1};
int backward[] = {a1,a1,a1,a2,a2,a2,a2,a1};
int left[] = {a1,a2};


void setup()
{
  Serial.begin(115200);
  pinMode(buttonPin,INPUT);
  pinMode(pingPin,OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  frontservo.attach(10);
  backservo.attach(9);
}
  
void loop()
{ 
  button=digitalRead(buttonPin); // pushbutton
  if (button == HIGH || startedMoving==true)
  {  
    startedMoving = true;
    
    distance = getdistance();
    cstr_dist = constrain(distance, 200, minDistance);
    mydelay = map(cstr_dist, 200,minDistance,400,120); // this is our speed mofifier based on distance
      
    if (distance >= minDistance) // if we are 10 cm close to something, start walking backwards
    {
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(redLedPin, LOW);
      walkfwd();
    } 
    else 
    {
      digitalWrite(greenLedPin, LOW);
      digitalWrite(redLedPin, HIGH);
      turnleft(); 
    }
  }
  else 
  {
    startedMoving = false;
  }    
} 

long getdistance() 
{
  long duration, cm;
  
  // The pinging
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);
//  Serial.println();
//  Serial.print(cm);
//  Serial.print(" cm\t");  
  return cm;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}


void resetservo(Servo& myservo)
{
  myservo.write(neutralAngle);  
}


void walkfwd() 
{
  int a1 = 55;
  int a2 = 105;
  
  for(int n=0;n<4;n++)
  {
    frontservo.write(forward[2*n]);
    backservo.write(forward[(2*n)+1]);
    delay(mydelay); 
    Serial.println(mydelay);
  }
}

void walkbwd() 
{
  int a1 = 55;
  int a2 = 105;
  
  for(int n=0;n<4;n++)
  {
    frontservo.write(backward[2*n]);
    backservo.write(backward[(2*n)+1]);
    delay(mydelay); 
    Serial.println(mydelay);
  }
}

void turnleft() 
{
  int a1 = 40;
  int a2 = 120;
  
  for(int n=0;n<4;n++)
  {
    frontservo.write(forward[2*n]);
    backservo.write(forward[(2*n)+1]);
    delay(mydelay); 
    Serial.println(mydelay);
  }
}

void turnright() 
{
  int a1 = 40;
  int a2 = 120;
  frontservo.write(a2);
  for(int n=0;n<4;n++)
  {
    backservo.write(forward[(2*n)+1]);
  }
}


void idle() 
{
  resetservo(frontservo);
  resetservo(backservo);
}
