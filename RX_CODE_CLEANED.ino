// UPDATED 11-10-2023 2:07pm - CC

#include <SPI.h>                  //SPI
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_DPS310.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED_1(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);
RF24 radio(9, 10); // CE, CSN (MAKE SURE THESE ARE THE RIGHT PINS ON ARDUINO)
Adafruit_DPS310 dps;
TwoWire splRx;

const byte address[6] = "00001";
const int DB_REGISTER = 0x0A;
const int BUTTON_1_PIN = 2; // CHANGE THIS NUMBER TO THE RIGHT PIN NUMBER
// const int BUTTON_2_PIN = 3;
float SPL_DRONE, SPL_GROUND, ALT_DRONE, ALT_GROUND, DELTA_ALT, SPL_PREDICT;
int BUTTON_1_STATE = 0, BUTTON_2_STATE = 0, BUTTON_1_LAST_STATE = 0, BUTTON_2_LAST_STATE = 0;

void OLED_1_page_0() {
  OLED_1.print("Page 0");
  // OLED_1.setCursor(0, 0);
  // OLED_1.print("Drone Altitude: ");
  // OLED_1.setCursor(0, 10);  // Print "SPL Value: " on OLED
  // OLED_1.print(ALT_DRONE);       // Print SPL receiving value on OLED
  // OLED_1.print(" m");          // Print SPL units (dB) on OLED
  // OLED_1.setCursor(0, 20);      // Set "SPL Value: " at this position on OLED
  // OLED_1.print("Ground Altitude: ");  // Print "SPL Value: " on OLED
  // OLED_1.setCursor(0, 30);
  // OLED_1.print(ALT_GROUND);       // Print SPL receiving value on OLED
  // OLED_1.print(" m");          // Print SPL units (dB) on OLED
  // OLED_1.setCursor(0, 40);
  // OLED_1.print("Delta Altitude: ");
  // OLED_1.setCursor(0, 50);
  // OLED_1.print(DELTA_ALT);
  // OLED_1.print(" m");
}

void OLED_1_page_1() {
  OLED_1.print("Page 1");
}

void OLED_1_page_2() {
  OLED_1.print("Page 2");
}

float GetAlt() {
  sensors_event_t temp_event, pressure_event;
  dps.getEvents(&temp_event, &pressure_event);
  ALT_GROUND = dps.readAltitude(1013.25);
  return ALT_GROUND;
}

float GetString() {
  char combinedMessage[32]; 
  radio.read(&combinedMessage, sizeof(combinedMessage));
  String combinedStr = String(combinedMessage);
  int separatorIndex = combinedStr.indexOf('|'); //Defines the index of | in the receiving string as an integer value
  String splStr = combinedStr.substring(4, separatorIndex - 3); //Create a new string from the receiving string that contains the SPL value
  String altitudeStr = combinedStr.substring(separatorIndex + 12, separatorIndex + 20); 
  SPL_DRONE = splStr.toFloat();
  ALT_DRONE = altitudeStr.toFloat();
  return SPL_DRONE, ALT_DRONE;
}

float GetSPL(int SPL_ADDRESS) {
  splRx.beginTransmission(SPL_ADDRESS);
  splRx.write(DB_REGISTER);
  splRx.endTransmission(false);
  splRx.requestFrom(SPL_ADDRESS, 1);
  SPL_GROUND = splRx.read();
  return SPL_GROUND;
}

uint8_t draw_state_1 = 0;
uint8_t draw_state_2 = 0;
void UpdateOLED_1(void) {
  switch(draw_state_1 >> 3) {
    case 0: OLED_1_page_0(); break;
    case 1: OLED_1_page_1(); break;
    case 2: OLED_1_page_2(); break;
  }
}

void setup() {
  if(!OLED_1.begin()) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  Serial.begin(115200);
  delay(2000);
  OLED_1.setFont(u8g2_font_6x10_tf);
  dps.begin_I2C(0x77, &Wire);
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  GetString();
  GetAlt();
  GetSPL(0x46);
  DELTA_ALT = ALT_DRONE - ALT_GROUND;
  SPL_PREDICT = SPL_DRONE * 0.4; // placeholder equation
  OLED_1.clearBuffer();
  UpdateOLED_1();
  OLED_1.sendBuffer();
  if(BUTTON_1_STATE != BUTTON_1_LAST_STATE) {
    if(BUTTON_1_STATE == HIGH) {
      draw_state_1++;
      if(draw_state_1 >= 3) {
        draw_state_1 = 0;
      }
    }
  }
}
