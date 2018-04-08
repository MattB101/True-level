#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Encoder.h>
#include <utility/Adafruit_MS_PWMServoDriver.h>
#include <SharpIR.h>

/* Drive Motor Variables */
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myStepper = AFMS.getStepper(200, 1);
Adafruit_DCMotor *M3 = AFMS.getMotor(3);
Adafruit_DCMotor *M4 = AFMS.getMotor(4);

/* H Bridge */
const int digitalPin44 = 44;                //Enable for H-bridge
const int digitalPin43 = 43;                //MC1
const int digitalPin45 = 45;                //MC2

/* Motor Encoders */
Encoder leftEnc(18, 19);           //Motor2
Encoder rightEnc(2, 3);            //Motor1

/* Servo Variables */
Servo Right_Tilt;                                //Center servo pin Digital 24
Servo Left_Tilt;                               //Right servo pin Digital 23
Servo Front_Tilt;                               //Left servo pin Digital 22
Servo Front_Pan;                               //Center 2nd servo pin Digital 21

/* Radio */
SoftwareSerial xbee(10, 11);

/* State Machine Variables */
int state = 5;
boolean start = true;

/* Sensors */
SharpIR Front_Long(GP2YA41SK0F, A3);
const int analogPin0 = 0;                   //Center sensor
const int analogPin1 = 1;                   //Left sensor
const int analogPin2 = 2;                   //Right sensor
const int analogPin7 = 7;                   //mag sensor

/* Variable Heap */
boolean objects[6]; // 0, left wall, 1 right wall, 2 front wall, 3 left edge, 4 right edge, 5, front edge

//TESTING
int test1 = 50;
int test2 = 100;
int test3 = 50;
int test4 = 150;

//Random Variables
char val;                                  // Data received from the serial port
int radius = 16;
float left_dist, right_dist, front_dist, left_dist_old, right_dist_old, front_dist_old;
int angle = 45;
int done, right_speed, left_speed = 0;
boolean Direction = true;
int count, inc, pause = 1;
int global_count, flag, flag1 = 0;
int index, lastIndex, wall, edge, obstacle, line = 0;
float distance = 0;
long duration;
float cm, cm_right, cm_left;
int stack_ptr = 0;
float global_memory[15];
float slope = 0;
int steps = 0;

void setup()
{
  /* Telemetry */
  Serial.begin(9600);
  xbee.begin(9600);

  /* Servos */
  Right_Tilt.attach(24);
  Left_Tilt.attach(23);
  Front_Tilt.attach(22);
  Front_Pan.attach(42);

  /* Servo Initial Position */
  Right_Tilt.write(55);
  delay(250);
  Left_Tilt.write(55);
  delay(250);
  Front_Tilt.write(25);
  delay(250);
  Front_Pan.write(95);
  delay(250);

  /* Motor Shield */
  AFMS.begin();

  /* Stepper */
  myStepper->setSpeed(1);

  /* Track H-Bridge */
  pinMode(digitalPin43, OUTPUT);
  pinMode(digitalPin44, OUTPUT);
  pinMode(digitalPin45, OUTPUT);

  //Settling Time
  delay(250);
}
// 0, left wall, 1 right wall, 2 front wall, 3 left edge, 4 right edge, 5, front edge
void loop()
{
  if (start)
  {
    switch (state)
    {
      case 0:
        check_environment("all");
        if (objects[2] == false && objects[0] == true && objects[1] == true)
          state = 1;
        if (objects[2] == false && objects[0] == false && objects[1] == false)
          state = 3;
        /*
          if((objects[2] == true && objects[0] == false && objects[1] == false}
          state    = 2;
        */
        break;
      case 1://Paddleboard state
        Speed(4);
        inBetweenObjects();
        break;
      case 2://Wall-lift state
        Speed(4);
        Front_Tilt.write(25);
        acquire_line(); 
        drive_forward(10);
        tracks("lift");
        check_environment("walls");
        
        while (objects[0] == false && objects[1] == false)
        { 
          forward(2, 1);
          check_environment("walls");
        }
        
        drive_forward(25);
        tracks("stop");
        state = 3;
        break;
      case 3://U-turn state
        acquire_line();
        state = 4;
        break;
      case 4://Bars bb almost there
        follow_line();
        steps++;
        if (steps > 225)
        {
          //sense_environment()
          exit(0);
        }
        break;
      case 5://pull ups yo...
        align_edges();
        break;
      case 9://Reset state...
        exit(0);
        break;
      default:
        //Ideal state???
        break;
    }
  }
}
