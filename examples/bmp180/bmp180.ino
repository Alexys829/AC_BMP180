#include <AC_BMP180.h>

AC_BMP180 baromether;


void setup() {
 
  Serial.begin(9600);
  baromether.begin();
}

void loop() {
  baromether.startTemperature();
  float t = baromether.GetTemperature();
  Serial.println(t);
  baromether.startPressure();
  float p = baromether.GetPressure();
  Serial.println(p);
  delay(5000);
}
