#include <Wire.h>
int raw[4];
const int pin[4] = {32,33,34,35};
void setup(){
  Serial.begin(115200);
  for(int i = 0;i<=100;i++){
    Serial.print("DEBUG");
    }
}
void loop(){
  Serial.println();
  for(int i = 0; i<4;i++){
    raw[i] = analogRead(pin[i]);
    //Serial.println("Pin %d raw = %d",pin[i],raw[i]);
    Serial.println("PIN = ");
    Serial.print(pin[i]);
    Serial.print("   RAW = ");
    Serial.print(raw[i]);
  } 
  delay(3000);
}