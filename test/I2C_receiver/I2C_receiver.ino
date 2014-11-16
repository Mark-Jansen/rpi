#include <Wire.h>
// A4 = SDA
// A5 = SCL

void setup()
{
    Wire.begin(4);
    Wire.onReceive(receiveEvent);
    Serial.begin(9600);
    Serial.println("start");
}

void loop()
{
    delay(100);
}

void receiveEvent(int howMany)
{
    Serial.print(">");
    int cnt = 0;
    while(Wire.available()) {
        char c = Wire.read();
        Serial.print(c);
        cnt++;
    }
    Serial.print("< (");
    Serial.print(cnt);
    Serial.println(")");
}
