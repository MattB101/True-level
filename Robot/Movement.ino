void tracks(String  action, int repeat)
{
  if (action == "lift" || action == "backward")
  {
    digitalWrite(digitalPin44, HIGH);
    digitalWrite(digitalPin43, LOW);
    digitalWrite(digitalPin45, HIGH);
  }
  else if (action == "drive" || action == "forward")
  {
    digitalWrite(digitalPin44, HIGH);
    digitalWrite(digitalPin43, HIGH);
    digitalWrite(digitalPin45, LOW);
  }
  else if (action == "stop")
  {
    digitalWrite(digitalPin44, LOW);
  }
  else if (action == "pulse")
  {
    tracks("drive", 1);
    delay(500 * repeat);
    tracks("stop", 1);
  }
  else if (action == "pulse_r")
  {
    tracks("lift", 1);
    delay(500 * repeat);
    tracks("stop", 1);
  }
}

void scissor(String action, int height, boolean hold)
{
  if (action == "lift" || action == "up")
  {
    for (int i = 0; i < height; i++)
      myStepper->onestep(FORWARD, DOUBLE);

    if (hold == false)
      myStepper->release();
  }
  else if (action == "lower" || action == "down")
  {
    for (int i = 0; i < height; i++)
      myStepper->onestep(BACKWARD, DOUBLE);

    if (hold == false)
      myStepper->release();
  }
}

void Speed(int ratioNum)
{
  switch (ratioNum)
  {
    case 1:
      left_speed = 250;
      right_speed = 245;
      break;
    case 2:
      left_speed = 200;
      right_speed = 196;
      break;
    case 3:
      left_speed = 150;
      right_speed = 146;
      break;
    case 4:
      left_speed = 100;
      right_speed = 96;
      break;
    default:
      break;
  }
}

void right(int repeat , int denom)
{
  for (int i = 0; i < repeat; i = i + 1)
  {
    M3->run(FORWARD);
    M4->run(BACKWARD);

    M4->setSpeed(right_speed);
    M3->setSpeed(left_speed);

    delay(test1 / denom);

    M3->setSpeed(0);
    M4->setSpeed(0);

    delay(test3);
    M3->run(RELEASE);
    M4->run(RELEASE);
  }
}

void drive_right(int repeat)
{
  M3->run(FORWARD);
  M4->run(BACKWARD);

  M4->setSpeed(right_speed);
  M3->setSpeed(left_speed);

  delay(test1 * repeat);

  M4->setSpeed(0);
  M3->setSpeed(0);

  delay(test3);
  M3->run(RELEASE);
  M4->run(RELEASE);
}

void left(int repeat , int denom)
{
  for (int i = 0; i < repeat; i = i + 1)
  {
    //fix wiring
    M3->run(BACKWARD);
    M4->run(FORWARD);

    M4->setSpeed(right_speed);
    M3->setSpeed(left_speed);

    delay(test1 / denom);

    M3->setSpeed(0);
    M4->setSpeed(0);

    delay(test3);
    M3->run(RELEASE);
    M4->run(RELEASE);
  }
}

void drive_left(int repeat)
{
  M3->run(BACKWARD);
  M4->run(FORWARD);

  M4->setSpeed(right_speed);
  M3->setSpeed(left_speed);

  delay(test1 * repeat);

  M4->setSpeed(0);
  M3->setSpeed(0);

  delay(test3);
  M3->run(RELEASE);
  M4->run(RELEASE);
}

void reverse(int repeat, int denom)
{
  for (int i = 0; i < repeat; i = i + 1)
  {
    //fix wiring
    M3->run(BACKWARD);
    M4->run(BACKWARD);

    M4->setSpeed(right_speed);
    M3->setSpeed(left_speed);

    delay(test1 / denom);

    M3->setSpeed(0);
    M4->setSpeed(0);

    delay(test3);
    M3->run(RELEASE);
    M4->run(RELEASE);
  }
}

void drive_reverse(int repeat)
{
  M3->run(BACKWARD);
  M4->run(BACKWARD);

  M4->setSpeed(right_speed);
  M3->setSpeed(left_speed);

  delay(test1 * repeat);

  M4->setSpeed(0);
  M3->setSpeed(0);

  delay(test3);
  M3->run(RELEASE);
  M4->run(RELEASE);
}

void forward(int repeat, int denom)
{
  for (int i = 0; i < repeat; i = i + 1)
  {
    //fix wiring
    M3->run(FORWARD);
    M4->run(FORWARD);

    M4->setSpeed(right_speed);
    M3->setSpeed(left_speed);

    delay(test1 / denom);

    M4->setSpeed(0);
    M3->setSpeed(0);

    delay(test3);
    M3->run(RELEASE);
    M4->run(RELEASE);
  }
}

void drive_forward(int repeat)
{
  M3->run(FORWARD);
  M4->run(FORWARD);

  M4->setSpeed(right_speed);
  M3->setSpeed(left_speed);

  delay(test1 * repeat);

  M4->setSpeed(0);
  M3->setSpeed(0);

  delay(test3);
  M3->run(RELEASE);
  M4->run(RELEASE);
}
