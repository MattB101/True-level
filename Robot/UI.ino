void UI(byte cmd_typed)
{
  switch (cmd_typed)
  {
    case 49:// 1 PB
      state = 0;
      break;
    case 50:// 2 WL
      state = 4;
      break;
    case 51:// 3 UT
      state = 7;
      break;
    case 52:// 4 RR
      state = 8;
      break;
    case 53:// 5 RL
      state = 11;
      break;
    case 114:// r ready
      start = true;
      break;
    case 120: //x reset
      state = 0;
      start = false;
      mag = false;
      MAG_CHECK = false;
      INT_MAG = 0;
      LATE_MAG_READING = 0;
      MAG_STATE = 0;
      steps = 0;
      repeat = 0;
      count = 1;
      toggle = true;
      tracks("stop", 1);
      //if (state == 2) scissor("lower", 350, false);
      //if (state == 7) scissor("lower", 1200, false);
      break;
    default:
      state = 0;
      //do we want to do something in here ?!?!
      break;
  }
}
