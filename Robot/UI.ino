void UI(byte cmd_typed)
{
  switch (cmd_typed)
  {
    case 49:// 1
      state = 0;
      break;
    case 50:// 2
      state = 4;
      break;// 3
    case 51:
      state = 7;
      break;
    case 52:// 4
      state = 8;
      break;
    case 53:// 5
      state = 11;
      break;
    case 114:// r
      start = true;
      break;
    case 120: //x
      start = false;
      mag = false;
      steps = 0;
      repeat = 0;
      count = 1;
      toggle = true;
      tracks("stop", 1);
      if (state == 2) scissor("lower", 350, false);
      if (state == 7) scissor("lower", 1200, false);
      break;
    default:
      break;
  }
}
