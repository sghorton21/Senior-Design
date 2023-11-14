// Senior Design Code
// I2C sensor data acquisition on Daisy Seed (STM32H7-based system)
// Christopher Cortes
// Last edit: 10-30-2023 9:48am - CC

#include <Adafruit_DPS310.h>
#include <U8g2lib.h>
#include <Wire.h>

Adafruit_DPS310 altRx;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED_1(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int DB;
float ALT_REF, ALT_DRONE, DELTA_ALT, SPL_DRONE, SPL_GROUND;
float seaLevelhPa=1013.25;
sensors_event_t temp_event, pressure_event;
int SPL_ADDRESS = 0x48;
int DB_REGISTER = 0x0A;

void setup() {
  // SPL Init
  Serial.begin(115200);
  altRx.begin_I2C(0x77, &Wire);
  altRx.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  altRx.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  }

void loop() {
  getAltRef();
  getSpl();
  SPL_GROUND = ALT_REF * SPL_DRONE;
  Serial.print("Altitude ground = ");
  Serial.println(ALT_REF);
}

// Read ALT_REF
float getAltRef() {
    altRx.getEvents(&temp_event, &pressure_event); // communicate with DPS310
    ALT_REF = altRx.readAltitude(seaLevelhPa);
    return ALT_REF;
}

int getSpl() {
    Wire.beginTransmission(SPL_ADDRESS);
    Wire.write(DB_REGISTER); 
    Wire.endTransmission(false); 
    Wire.requestFrom(SPL_ADDRESS, 1); 
    byte DB = Wire.read();
    SPL_DRONE = float(DB);
}

void printOled() {
      OLED_1.firstPage();
      do {
      OLED_1.clearDisplay();
      OLED_1.setCursor(0, 20);
      OLED_1.print("Altitude = ");
      OLED_1.setCursor(0,40);
      OLED_1.print(ALT_REF);
      OLED_1.print(" m");
      OLED_1.setCursor(0,60);
      OLED_1.print("SPL = ");
      OLED_1.print(SPL_GROUND);
      OLED_1.print(" DB");
      } 
      while (OLED_1.nextPage());
}
