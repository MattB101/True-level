void check_environment(String argument)
{
  for (int i = 0; i < 5; i++)
    objects[i] = false;

  if (argument == "walls" || argument == "all")
  {
    Right_Tilt.write(55);
    Left_Tilt.write(55);
    Front_Tilt.write(100);
    Front_Pan.write(95);
  
    delay(250);
  
    if (filter(1, 50) < 10) objects[0] = true;
    if (filter(2, 50) < 10) objects[1] = true;
    if (filter_long(50) < 10) objects[2] = true;
  }

  if (argument == "edges" || argument == "all" && objects[0] == false && objects[1] == false)
  {
    Right_Tilt.write(105);
    Left_Tilt.write(5);
    delay(250);
    
    for (int i = 0; i < 30; i++)
    {
  
      if (objects[3] == false) Left_Tilt.write(15 + i);
      if (objects[4] == false) Right_Tilt.write(110 - i);
      delay(15);
  
      if (filter(1, 50) > 15 && objects[3] == false)
      {
        Serial.println("L Edge@");
        Serial.println(5 + i - 1);
        objects[3] = true;
        Left_Tilt.write(5 + i - 1);
        delay(15);
      }
      if (filter(2, 50) > 15 && objects[4] == false)
      {
        Serial.println("R Edge@");
        Serial.println(105 - i + 1);
        objects[4] = true;
        Right_Tilt.write(105 - i + 1);
        delay(15);
      } 
    }
  }
}

void get_edge_distance()
{
  objects[3] = false;
  objects[4] = false;
  objects[5] = false;
  
  Right_Tilt.write(105);
  Left_Tilt.write(5);
  Front_Tilt.write(50);
  delay(250);
    
  for (int i = 0; i < 30; i++)
  {

    if (objects[3] == false) Left_Tilt.write(5 + i);
    if (objects[4] == false) Right_Tilt.write(105 - i);
    if (objects[5] == false) Front_Tilt.write(50 + i);
    
    delay(15);

    left_dist = filter(1, 50);
    right_dist= filter(2, 50);
    front_dist = filter_long(50);
    
    if (left_dist> 15 && objects[3] == false)
    {
      //Serial.println("L Edge@");
      //Serial.println(5 + i - 1);
      objects[3] = true;
      Left_Tilt.write(5 + i - 1);
    }
    if (right_dist > 15 && objects[4] == false)
    {
      //Serial.println("R Edge@");
      //Serial.println(105 - i + 1);
      objects[4] = true;
      Right_Tilt.write(105 - i + 1);
    } 
    if (front_dist > 20 && objects[5] == false)
    {
      objects[4] = true;
      Front_Tilt.write(50 + i - 1);
    }
  }
}

void align_edges()
{
  int thresh = 2.5;
  
  get_edge_distance();
  left_dist= left_dist_old;
  right_dist = right_dist_old;

  drive_reverse(5);
  get_edge_distance();
  
  if (left_dist-left_dist_old > .5)
    left(2, 1);
  else if (right_dist-right_dist_old > .5)
    right(2,1);

  get_edge_distance();
    
  if ((right_dist > left_dist) && abs(right_dist - left_dist) >= thresh)
  {
    left(3, 1);
    reverse(3, 1);
    right(4, 1);
  }
  else if ((left_dist > right_dist) && abs(left_dist - right_dist) >= thresh)
  {
    right(3, 1);
    reverse(3, 1);
    left(4, 1);
  }
  else
  {
    exit(0);
  }
}

void sense_wall(int mode)
{
  cm = filter(analogPin0, 50);
  if (cm < 5)
  {
    wall = 1;
  }
  else
    wall = 0;

  if (mode == 0 || wall == 0)
  {
    Right_Tilt.write(100);
  }
  else if (mode == 1 || wall == 1)
  {
    Right_Tilt.write(50);
  }
}

void find_wall()
{
  while (wall == 1)
  {
    Direction = !(Direction);

    if (Direction == false)
      right(count, 1);
    else
      left(count, 1);

    sense_wall(1);
    count = count + 1;
  }

  sense_wall(0);

  drive_reverse(5);

  if (Direction == false) right(5 + count, 1);
  else left(5 + count, 1);

  count = 0;
}

void inBetweenObjects()
{
  float thresh = 2.5; //We can make this small if needed...
  if (flag == 0)
  {
    for (int i = 1; i < 350; i++)
    {
      myStepper->onestep(FORWARD, DOUBLE);
      flag = 1;
    }
    myStepper->release();
  }

  cm_right = filter(analogPin2, 50);
  cm_left = filter(analogPin1, 50);

  if ((cm_right > cm_left) && abs(cm_right - cm_left) >= thresh)
  {
    left(3, 1);
    reverse(3, 1);
    right(4, 1);
  }
  else if ((cm_left > cm_right) && abs(cm_left - cm_right) >= thresh)
  {
    right(3, 1);
    reverse(3, 1);
    left(4, 1);
  }
  else if (cm_right > 15 && cm_left > 15)
  {
    if (flag1 == 0)
    {
      for (int i = 1; i < 350; i++)
      {
        myStepper->onestep(BACKWARD, DOUBLE);
      }
      myStepper->release();
      flag1 = 1;
    }
    state = 2;
  }
  else
  {
    drive_forward(20);
  }
}

void sense_edge(int mode)
{
  cm = filter(analogPin0, 50);

  if (cm >= 10)
  {
    edge = 1;
    Serial.print("edge@");
    Serial.println(cm);
  }
  else
    edge = 0;

  if (mode == 0 || edge == 0) Right_Tilt.write(50);
  else if (mode == 1 || edge == 1) Right_Tilt.write(100);
}

void find_edge()
{
  while (edge == 1)
  {
    Direction = !(Direction);
    if (Direction == false)
      right(count, 1);
    else
      left(count, 1);

    sense_edge(1);
    count = count + 1;
  }

  sense_edge(0);

  drive_reverse(5);

  if (Direction == false) right(5 + count, 1);
  else left(5 + count, 1);

  count = 0;
}

float IR_Distance(int sensorNum)
{
  float dist = 0;
  float voltages = analogRead(sensorNum) * .004828125;
  dist = (5 * pow(voltages, -1));
  delay(1);
  return dist;
}

float filter_long(int window)
{
  float dist = 0;

  for (int i = 0; i < window; i++)
  {
    dist = dist + Front_Long.getDistance();
  }
  dist = dist / window;
  return dist;
}

float filter(int sensorNum, int window)
{
  float dist = 0;

  for (int i = 0; i < window; i++)
  {
    dist = dist + IR_Distance(sensorNum);
  }
  dist = dist / window;
  return dist;
}

void detectMag()
{
  float valRead = analogRead(analogPin7) * .004828125;
  Serial.println(valRead);
  if (valRead <= 2.00) {
    Serial.println("Magnetic Field Found");
  }
  else {
    Serial.println("No Field Found");
  }
}

void follow_line()
{
  Front_Tilt.write(25);
  find_line();
  forward(1, 1);
}

void acquire_line()
{
  int line_index = 0;
  Front_Tilt.write(25);
  delay(250);
  left(15, 2);
  while (line == 0 && line_index < 30)
  {
    right(1, 2);
    sense_line();
    line_index++;
  }
}

void find_line()
{
  int thresh = 2;
  
  while (line == 0)
  {
    Direction = !(Direction);
    if (Direction == false)
    {
      for (int i = 0; i < count; i++)
      {
        right(1, 2);
        sense_line();
        if (line == 1) break;
      }
      if (count > thresh&& line == 0)
      {
        right(5, 2);
        count = count + 5;
      }
    }
    else
    {
      for (int i = 0; i < count; i++)
      {
        left(1, 2);
        sense_line();
        if (line == 1) break;
      }
      if (count > thresh&& line == 0)
      {
        left(5, 2);
        count = count + 5;
      }
    } 
    count = count + 1;
  }

  sense_line();
  count = 0;
}

void sense_line()
{
  cm = filter(analogPin0, 50);

  if (cm > 7.5)
  {
    line = 1;
    //Serial.print("line@");
    //Serial.println(cm);
  }
  else {
    line = 0;
    //Serial.print("no@");
    //Serial.println(cm);
  }
}
