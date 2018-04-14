#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

void setup()
{
  Serial.begin(9600);
  Serial.println("Antennae sensing and ready to search for food master!");
  Serial.print("HOT TREATS: 1 = PB, 2 = WL, 3 = UT, 4 = RR, 5 = RL, r = ready, x = reset");
  Serial.println();
  Serial.println("Roach must feed!!");
  Serial.println("--------------------------");
  Serial.println("Command me master!");
  mySerial.begin(9600);
}

void loop() // run over and over
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
  {
    byte readVal = Serial.read();
    if (readVal != 0x0d && readVal != 0x0a && readVal != 0x00)
    {
      mySerial.write(readVal);
    }
  }
}

