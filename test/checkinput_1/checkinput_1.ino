#include <Wire.h>
int raw[4];
const int pin[4] = {32,33,34,35};
void setup(){
  Serial.begin(115200);
  Serial.println("DEBUG");
  for(int i = 0; i< 4;i++){
    pinMode(pin[i],INPUT);
  }
}
void loop(){
  Serial.println();
  for(int i = 0; i<4;i++){
    raw[i] = analogRead(pin[i]);
    //Serial.println("Pin %d raw = %d",pin[i],raw[i]);
    //Serial.println("===PIN===");
    //Serial.println(pin[i]);
    //Serial.println("===RAW===");
    //Serial.println(raw[i]);
    Serial.printf("Pin %d raw = %d\n", pin[i], raw[i]);
    Serial.println();

  } 
  //cleanup for better debug readability
  delay(3000);
}