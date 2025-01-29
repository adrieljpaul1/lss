#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TCA_ADDR 0x70  // Default I2C address of TCA9548A
#define LCD_ADDR 0x27  // Default I2C address of LCD (check your LCD’s address)

// Function to select a TCA9548A channel
void tca_select(uint8_t channel) {
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

// Create an array of LCD objects (assuming all use the same address)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Modify if using a 20x4 LCD

void setup() {
    Wire.begin();
    Serial.begin(115200);

    for (uint8_t i = 0; i < 7; i++) {  // Loop through each LCD
        tca_select(i);
        lcd.init();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("LCD ");
        lcd.print(i);
    }
}

void loop() {
    for (uint8_t i = 0; i < 7; i++) {
        tca_select(i);
        lcd.setCursor(0, 1);
        lcd.print("Hello!");
        delay(1000);
    }
}
