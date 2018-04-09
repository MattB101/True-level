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
int state = 11;
boolean start = true;

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
float cm, cm_right, cm_left, left_dist, right_dist, front_dist, left_dist_old, right_dist_old, front_dist_old = 0;
int wall, edge, obstacle, line, steps, right_speed, left_speed = 0;
int count = 1;
boolean toggle = true;

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

void loop()
{
  if (start)
  {
    switch (state)
    {
      /* Base */
      case 0:
        check_environment("all", 50);
        Speed(3);
        if (objects[2] == false && objects[0] == true && objects[1] == true) state = 1;
        if (objects[2] == false && objects[0] == false && objects[1] == false) state = 4;
        break;
      /* Paddleboard */
      case 1:
        scissor("lift", 350, false);
        Speed(3);
        state = 2;
        break;
      case 2:
        if (!in_between()) state = 3;
        break;
      case 3:
        follow_line(0, 5);
        drive_forward(6);
        scissor("lower", 350, false);
        state = 4;
        steps = 0;
        break;
      /* Wall Lift */
      case 4:
        //follow_line(0, 2);
        //steps++;
        follow_line(0, 2);
        check_environment("walls", 350);
        if (!isinf(front_dist) && front_dist < 25) state = 5;
        break;
      case 5:
        Speed(3);
        drive_forward(5);
        check_environment("walls", 50);
        if (front_dist < 7) state = 6;
        break;
      case 6:
        tracks("lift", 1);
        check_environment("walls", 50);
        while (objects[0] == false || objects[1] == false)
        {
          drive_forward(2);
          check_environment("walls", 50);
        }
        drive_forward(30);
        tracks("stop", 1);
        state = 7;
        break;
      /* U-Turn */
      case 7:
        if (steps < 112)
        {
          follow_line(0, 1);
          steps++;
          if (steps == 40) scissor("lift", 1200, false);
          //if (steps == 112) tracks("drive");
        }
        else
        {
          check_environment("floor", 2);
          if (left_dist > 15 && right_dist > 15) state = 8;
          else 
          {
            forward(2, 1);
            tracks("pulse", 1);
          }
        }
        break;
      case 8:
        scissor("lower", 1850, false);
        state = 9;
        break;
      case 9:
        tracks("pulse", 1);
        check_environment("floor", 2);
        if (left_dist < 10 && right_dist < 10)
        {
          tracks("pulse", 2);
          scissor("lift", 1400, false);
          delay(500);
          tracks("stop", 1);
          steps = 0;
          state = 10;
        }
        break;
      case 10:
        follow_line(0, 1);
        steps++;
        if (steps == 110) exit(0);
        break;
      case 11:
        /*check_environment("floor", 2);
        Serial.println("left");
        Serial.println(left_dist);
        Serial.println("right");
        Serial.println(right_dist);
        tracks("pulse", 1);
        delay(500);
        detectMag();
        delay(1000);
        */
      default:
        break;
    }
  }
}
