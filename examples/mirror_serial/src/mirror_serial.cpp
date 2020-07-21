/**
 * 
 * Demonstrates how content bothe reads and writes to Serial1 can be output
 * on Serial
 * 
 */

#include <Arduino.h>
#include <StreamSpy.h>

StreamSpy SerialMirror(Serial1);

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);

  SerialMirror.onWrite([](const uint8_t *buffer, size_t size) {
    Serial.write(buffer, size);
  });
  SerialMirror.onRead([](const uint8_t *buffer, size_t size) {
    Serial.write(buffer, size);    
  });

  SerialMirror.println("");
  SerialMirror.println("StreamSpy");
  SerialMirror.println("");


}

void loop()
{
  if(0 == millis() % 1000) {
    SerialMirror.println(millis());
  }
  while (SerialMirror.available()) {
    char buffer[128];
    SerialMirror.readBytes(buffer, 128);
  }
}
