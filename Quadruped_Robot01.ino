#include <Servo.h>

const int PIN_SERVO_BACK = 9;
const int PIN_SERVO_FRONT = 10;
const int PIN_SERVO_HEAD = 11;
const int PIN_PING = 12;
const int PIN_ECHO = 13;
const int PIN_STARTBUTTON = 8;

const int min_distance = 70;            // minimum distance in cm before deciding to walk around obstacles
const int max_distance = 200;           // maximum distance in cm known to the system
const int ping_interval = 5000;         // The interval when we send a ping to the ultrasonic sensor 

const int angle_default = 80;           // Default angle
const int a1_fwd = 55;                  // First angle for forward walking 
const int a2_fwd = 105;                 // Second angle for forward walking 
const int a1_side = 55;                 // First angle for sideways walking
const int a2_side = 120;                // Second angle for sideways walkin
const int a_headscan = 45;              // The angle he head will turn to scan

const bool debug = true;                // enable some printing

int steps_delay = 350;                  // delay between steps in ms
int scan_delay = 500;                   // delay between scanning directions
int isMoving = false;

int turn_distance = 60;                // The amount of centimeters when we scan for a new direction

int buttonstate;
int distance_constraint;
long distance;
char next_direction = '\0';             // This variable will hold our next direction suggested by the ultrasonic sensor

long previousMillis = 0;

Servo servo_front,servo_back,servo_head;  // our servo objects


// Locomotion sequences

int forward[] = {a1_fwd,a2_fwd,a2_fwd,a2_fwd,a2_fwd,a1_fwd,a1_fwd,a1_fwd};
int backward[] = {a1_fwd,a1_fwd,a1_fwd,a2_fwd,a2_fwd,a2_fwd,a2_fwd,a1_fwd};
int sideway[] = {a1_side,a2_side,a2_side,a2_side,a2_side,a1_side,a1_side,a1_side};


// -------------------- INITALIZATION --------------------

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_STARTBUTTON,INPUT);
  pinMode(PIN_PING,OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  servo_head.attach(PIN_SERVO_HEAD);
  servo_front.attach(PIN_SERVO_FRONT);
  servo_back.attach(PIN_SERVO_BACK);    
  
  resetservo(servo_head);
  resetservo(servo_back);
  resetservo(servo_front);
}
// --------------------- MAIN LOOP ---------------------

void loop()
{ 
  buttonstate=digitalRead(PIN_STARTBUTTON);
  if (buttonstate == HIGH || isMoving==true)
  {  
    isMoving = true; 

    // This will make sure we only ping according to the ping interval variable

    distance = getdistance();
    delay(50);
    
    if (distance < turn_distance)
    {
      unsigned long currentMillis = millis();
      if(currentMillis - previousMillis > ping_interval){
        previousMillis = currentMillis; 
        // before we scan for directions, we should reset the servos
        resetservo(servo_front);
        resetservo(servo_back);
        next_direction = get_next_direction();
      }
      
      // Here we decide which way to go next    
      if (next_direction != '\0'){
        if (next_direction == 'l'){
          walk_left();
        } else {
          walk_right();
        }
      } else {
        walk_fwd();
      }
    } else {
      walk_fwd();
    }
  } 
} 

// -------------------- PING FUNCTIONS --------------------

const char get_next_direction()
{
  // This function will scan in an angle forward in both directions and will return us the longer distance
  unsigned int leftside = 0;
  unsigned int rightside = 0;
  
  servo_head.write(angle_default+a_headscan);
  delay(300);
  leftside = getdistance();
  delay(scan_delay);
  
  servo_head.write(angle_default-a_headscan);
  delay(300);
  rightside = getdistance();
  delay(scan_delay);

  resetservo(servo_head);
  
  if (leftside > rightside)
  {
    return 'l';
  }
  else
  {
    return 'r';
  }  

}
  


int getdistance() 
{
  // Get the distance to the next obstacle
  long duration,cm;

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
  //return microseconds * 0.689;
}

// -------------------- LOCOMOTION FUNCTIONS --------------------

void resetservo(Servo& myservo)
{
  myservo.write(angle_default);  
}


void walk_fwd() 
{
  Serial.println("Walking forwards");
  for(int n=0;n<4;n++)
  {
    servo_front.write(forward[2*n]);
    servo_back.write(forward[(2*n)+1]);
    delay(steps_delay); 
  }
}

void walk_bck() 
{
  Serial.println("Walking backwards");
  for(int n=0;n<4;n++)
  {
    servo_front.write(backward[2*n]);
    servo_back.write(backward[(2*n)+1]);
    delay(steps_delay); 
  }
}

void walk_left() 
{ 
  Serial.println("Walking left");
  for(int n=0;n<4;n++)
  {
    servo_front.write(sideway[2*n]);
    servo_back.write(sideway[(2*n)+1]);
    delay(steps_delay); 
  }
}

void walk_right() 
{
  Serial.println("Walking right");
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
