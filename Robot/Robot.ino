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
int state = 1;
boolean start = false;
boolean reset = false;
boolean mag = false;
unsigned int INT_MAG = 0;
unsigned int LATE_MAG_READING = 0;
boolean MAG_CHECK = false;
byte MAG_STATE = 0;

/* Sensors */
//SharpIR Front_Long(GP2YA41SK0F, A3);
const int analogPin0 = 0;                   //Center sensor
const int analogPin1 = 1;                   //Left sensor
const int analogPin2 = 2;                   //Right sesnor
const int analogPin7 = 7;                   //mag sensor
int last_pos = 0;

/* Array of Local Objects */
/* Rebuild this array by calling sense_environment() */
/*
   False = No object
   True = Object present

   [0] left wall
   [1] right wall
   [2] front wall
   [3] left edge9
   [4] right edge
   [5] front edge
*/
boolean objects[6];

/* Timing for Movement functions */
int test1 = 50;
int test2 = 100;
int test3 = 50;
int test4 = 150;

/* State Variables for Action functions */
float cm, cm_right, cm_left, left_dist, right_dist, front_dist, front_dist_short, left_dist_old, right_dist_old, front_dist_old = 0;
int wall, edge, obstacle, line, steps, right_speed, left_speed, repeat = 0;
int count = 1;
boolean toggle = true;

void setup()
{
  repeat = 0;
  
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
  delay(1000);
}
unsigned short tick = 0;
void loop()
{
  if (xbee.available())
  {
    UI(xbee.read());
  }

  if (!mag && start)
  {
    if (!MAG_CHECK) calibrate_mag();
    MAG_STATE = read_mag();
  }

  if (start && mag)
  {
    switch (state)
    {
      /* Base */
      case 0:
        check_environment("all", 50);
        Speed(4);
        if (objects[2] == false && objects[0] == true && objects[1] == true) state = 1;
        if (objects[2] == false && objects[0] == false && objects[1] == false) state = 4;
        break;
      /* Paddleboard */
      case 1:
        //xbee.println("Roach: I am in Paddleboards. I must go straight or die!");
        scissor("lift", 350, false);
        Speed(4);
        state = 2;
        break;
      case 2:
        if (!in_between()) state = 3;
        break;
      case 3:
        Speed(3);
        follow_line(15, 6);
        scissor("lower", 350, false);
        state = 4;
        steps = 0;
        break;
      /* Wall Lift */
      case 4:
        //xbee.println("Roach: I am in Wall lift. I must use my strength to lift this black wall!");
        Speed(5);
        if (repeat == 0) follow_line(10, 1);
        else if (repeat == 1) follow_line(10, 2);
        if (repeat == 2) state = 5;
        break;
      case 5:
        Speed(3);
        drive_forward(5);
        check_environment("walls", 50);
        if (front_dist < 7 || front_dist_short < 5) state = 6;
        break;
      case 6:
        tracks("lift", 1);
        check_environment("walls", 50);
        while (objects[0] == false || objects[1] == false)
        {
          drive_forward(2);
          check_environment("walls", 50);
        }
        drive_forward(32);
        right(2, 1);
        tracks("stop", 1);
        state = 7;
        break;
      //xbee.println("Roach: I am in U turn. This is easy I must sniff the line!");
      /* U-Turn */
      case 7:
        Speed(3);
        if (steps < 62)
        {
          follow_line(25, 2);
          steps++;
          if (steps == 35) scissor("lift", 1850, false);
        }
        else state = 8;
        break;
      case 8:
        //xbee.println("Roach: I am in Rail runner. O boy...I can sense the food!");
        check_environment("floor", 2);
        if (left_dist > 6 && right_dist > 6) state = 9; //was 8
        else
        {
          forward(2, 1);
          tracks("pulse", 1);
        }
        break;
      case 9:
        scissor("lower", 1850, false);
        state = 10;
        break;
      case 10:
        tracks("pulse", 1);
        check_environment("floor", 2);
        if (left_dist < 6 && right_dist < 6) 
        {
          tracks("pulse", 4);
          //scissor("lift", 1890, false); //1800 maybe
          forward(10, 1);
          steps = 0;
          state = 11;
          repeat = 0;
        }
        break;
      case 11:
      Speed(3);
        if (repeat == 0) follow_line(10, 1);
        else if (repeat == 1) follow_line(10, 2);
        if (repeat == 2) state = 12;
        //follow_line(10, 1);
        //steps++;
        //if (steps == ) state = 12;
        break;
      case 12:
        Speed(3);
        scissor("lift", 4500, false);
        forward(10,1);
        right(1,1);
        forward(15,1);
        right(1,1);
        forward(15,1);
        right(1,1);
        tracks("lift", 4);
        forward(25,1);
        //tracks("stop", 1);
        scissor("lower", 1000, false);
        tracks("stop", 1);
        scissor("lower", 4500, false);
        state = 13;
        break;
      case 13: //calibration state
        xbee.println("I did it!! Yummy food I will live your 10+ years now!");
        exit(0);
        break;
      default:
        state = 0;
        break;
    }
  }
  tick++;
}
