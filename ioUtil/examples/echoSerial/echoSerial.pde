// echo back input on serial port.

#include <ioUtil.h>  // SerialLineBuffer

SerialLineBuffer cmd;

void setup()
{
  Serial.begin(9600);
  Serial.println("Enter stuff, terminated by 0 \r \n or ;");
}

void loop()
{
  if (cmd.isComplete())
    {
      Serial.print(cmd.get());
    }
  delay(200);
}
