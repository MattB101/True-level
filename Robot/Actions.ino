void check_environment(String argument, int samples)
{
  for (int i = 0; i < 5; i++)
    objects[i] = false;

  if (argument == "walls" || argument == "all")
  {
    Right_Tilt.write(55);
    Left_Tilt.write(55);
    Front_Tilt.write(110);
    Front_Pan.write(95);
  
    delay(abs(100 - last_pos) * 30);
    
    front_dist = filter_long(samples);
    Serial.println(front_dist);
    left_dist = filter(1, 50);
    right_dist = filter(2, 50);
    
    if (left_dist < 10) objects[0] = true;
    if (right_dist < 10) objects[1] = true;
    if (front_dist < 10) objects[2] = true;
    
    last_pos = 110;
  }

  if (argument == "edges" || argument == "all" && objects[0] == false && objects[1] == false)
  {
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

   if (argument == "floor")
   {
      Right_Tilt.write(110);
      Left_Tilt.write(5);
      delay(250);
      left_dist = filter(1, samples);
      right_dist= filter(2, samples);
   }
}

void align_edges()
{
  float thresh = 2.5;
  
  check_environment("edges",50);
  left_dist_old = left_dist;
  right_dist_old = right_dist;

  drive_reverse(5);
  check_environment("edges",50);
  
  if (left_dist - left_dist_old > .5) right(2, 1);
  else if (right_dist - right_dist_old > .5) left(2,1);

  check_environment("edges",50);
    
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

boolean in_between()
{
  float thresh = 2.5; 
  
  check_environment("walls",50);

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
  else if (right_dist > 15 && left_dist > 15)  
    return false;
  else
    drive_forward(20);
    
  return true;
}

float IR_Distance(int sensorNum)
{
  float dist = 0;
  float voltages = analogRead(sensorNum) * .004828125;
  dist = (5 * pow(voltages, -1));
  delay(2);
  return dist;
}

/*
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
*/

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

boolean detectMag()
{
  float valRead = analogRead(analogPin7) * .004828125;
  
  for (int i = 0; i < 50; i++)  
    valRead = valRead + analogRead(analogPin7) * .004828125;
  
  valRead = valRead / 50;
  Serial.println(valRead);
  
  if (valRead >= 2.58 || valRead <= 2.38) 
  {
    Serial.println("Magnetic Field Found");
    return true;
  }
  else 
  {
    return false;
    Serial.println("No Field Found");
  }
}

boolean follow_line(int stop_condition, int steps)
{  
  boolean ret = false;
  Front_Tilt.write(25);
  delay(abs(25 - last_pos) * 30);
  ret = find_line(stop_condition);
  
  if (ret == false)
  {
    reverse(6, 1);
    repeat++; 
  }
  else
  {
    if (repeat == 1) repeat = 2;
    else if (repeat == 2) repeat = 0; 
    forward(steps, 1);
  }
  last_pos = 25;
  return ret;
}

boolean find_line(int stop_condition)
{
  count = 0;
  int thresh = 2;
  toggle = true;
  
  while (line == 0)
  {
    toggle = !(toggle);
    if (toggle == false)
    {
      for (int i = 0; i < count + 1; i++)
      {
        right(1, 2);
        sense_line();
        if (line == 1) break;
      }
      if (count > thresh && line == 0)
      {
        if (count >= stop_condition)
        {
          left(count/2, 2);
          return false;          
        }
        right(5, 2);
        count = count + 5;
      }
    }
    else
    {
      for (int i = 0; i < count + 1; i++)
      {
        left(1, 2);
        sense_line();
        if (line == 1) break;
      }
      if (count > thresh && line == 0)
      {
        if (count >= stop_condition)
        {
          right((count/2) + 1, 2);
          return false; 
        }
        left(5, 2);
        count = count + 5;
      }
    } 
    count++;
  }

  sense_line();
  return true;
}

float IR_Distance_Long()
{
  float dist = 0;
  float voltages = analogRead(A3) * .004828125;
  dist = (13 * pow(voltages, -1));
  delay(1);
  return dist;
}

float filter_long(int window)
{
  float dist = 0;

  for (int i = 0; i < window; i++)
  {
    dist = dist + IR_Distance_Long();
  }
  dist = dist / window;
  return dist;
}

void sense_line()
{
  cm = filter(analogPin0, 50);

  if (cm > 7.5) line = 1; 
  else line = 0;   
}

void sense_wall(int mode)
{
  cm = filter(analogPin0, 50);
  
  if (cm < 5) wall = 1;
  else wall = 0;

  if (mode == 0 || wall == 0) Right_Tilt.write(100);
  else if (mode == 1 || wall == 1) Right_Tilt.write(50);
  
}

void find_wall()
{
  while (wall == 1)
  {
    toggle = !(toggle);

    if (toggle == false)
      right(count, 1);
    else
      left(count, 1);

    sense_wall(1);
    count++;
  }

  sense_wall(0);

  drive_reverse(5);

  if (toggle == false) right(5 + count, 1);
  else left(5 + count, 1);

  count = 0;
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
    toggle = !(toggle);
    if (toggle == false)
      right(count, 1);
    else
      left(count, 1);

    sense_edge(1);
    count++;
  }

  sense_edge(0);

  drive_reverse(5);

  if (toggle == false) right(5 + count, 1);
  else left(5 + count, 1);

  count = 0;
}
