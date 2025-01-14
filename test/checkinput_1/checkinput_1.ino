const int pin[4] = {32,33,34,35};
void setup(){
  Serial.begin(115200);
  for(int i = 0;i<=100;i++){
    Serial.print("DEBUG");
    }
}
void loop(){
  Serial.println();
  int raw[4];
  for(int i = 0; i<4;i++){
    raw[i] = analogRead(pin[i]);
    Serial.println("Pin %d raw = %d",pin[i],raw[i]);
  } 
  delay(3000);
}