// UPDATED 11-11-2023 7:58pm - CC

#include <SPI.h>                  //SPI
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_DPS310.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED_1(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED_2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
RF24 NRF24L01(9, 10); // CE, CSN (MAKE SURE THESE ARE THE RIGHT PINS ON ARDUINO)
Adafruit_DPS310 DPS310;
TwoWire splRx;

const byte address[6] = "00001";
const int DB_REGISTER = 0x0A;
const int BUTTON_1_PIN = 8; // CHANGE THIS NUMBER TO THE RIGHT PIN NUMBER
// const int BUTTON_2_PIN = 3;
float SPL_DRONE, SPL_GROUND, ALT_DRONE, ALT_GROUND, DELTA_ALT, SPL_PREDICT;
int BUTTON_1_STATE = 0, BUTTON_2_STATE = 0, BUTTON_1_LAST_STATE = 0, BUTTON_2_LAST_STATE = 0;

void OLED_1_page_0() {
  OLED_1.setCursor(0, 10);
  OLED_1.print("Drone Altitude: ");
  OLED_1.setCursor(0, 20);  // Print "SPL Value: " on OLED
  OLED_1.print(ALT_DRONE);       // Print SPL receiving value on OLED
  OLED_1.print(" m");          // Print SPL units (dB) on OLED
  OLED_1.setCursor(0, 30);      // Set "SPL Value: " at this position on OLED
  OLED_1.print("Ground Altitude: ");  // Print "SPL Value: " on OLED
  OLED_1.setCursor(0, 40);
  OLED_1.print(ALT_GROUND);       // Print SPL receiving value on OLED
  OLED_1.print(" m");          // Print SPL units (dB) on OLED
  OLED_1.setCursor(0, 50);
  OLED_1.print("Delta Altitude: ");
  OLED_1.setCursor(0, 60);
  OLED_1.print(DELTA_ALT);
  OLED_1.print(" m");
}

void OLED_1_page_1() {
  OLED_1.setCursor(0, 10);
  OLED_1.print("Average Drone SPL: ");
  OLED_1.setCursor(0, 20);  // Print "SPL Value: " on OLED
  OLED_1.print(SPL_DRONE);       // Print SPL receiving value on OLED
  OLED_1.print(" dB");          // Print SPL units (dB) on OLED
  OLED_1.setCursor(0, 30);      // Set "SPL Value: " at this position on OLED
  OLED_1.print("Actual Ground SPL: ");  // Print "SPL Value: " on OLED
  OLED_1.setCursor(0, 40);
  OLED_1.print(SPL_GROUND);       // Print SPL receiving value on OLED
  OLED_1.print(" dB");          // Print SPL units (dB) on OLED
  OLED_1.setCursor(0, 50);
  OLED_1.print("Predicted Ground SPL: ");
  OLED_1.setCursor(0, 60);
  OLED_1.print(SPL_PREDICT);
  OLED_1.print(" dB");
}

void OLED_1_page_2() {
  OLED_1.setCursor(0,10);
  OLED_1.print("Page 2");
}

// void OLED_2_page_0() {
//   OLED_2.setCursor(0, 10);
//   OLED_2.print("Drone Altitude: ");
//   OLED_2.setCursor(0, 20);  // Print "SPL Value: " on OLED
//   OLED_2.print(ALT_DRONE);       // Print SPL receiving value on OLED
//   OLED_2.print(" m");          // Print SPL units (dB) on OLED
//   OLED_2.setCursor(0, 30);      // Set "SPL Value: " at this position on OLED
//   OLED_2.print("Ground Altitude: ");  // Print "SPL Value: " on OLED
//   OLED_2.setCursor(0, 40);
//   OLED_2.print(ALT_GROUND);       // Print SPL receiving value on OLED
//   OLED_2.print(" m");          // Print SPL units (dB) on OLED
//   OLED_2.setCursor(0, 50);
//   OLED_2.print("Delta Altitude: ");
//   OLED_2.setCursor(0, 60);
//   OLED_2.print(DELTA_ALT);
//   OLED_2.print(" m");
// }

// void OLED_2_page_1() {
//   OLED_2.setCursor(0, 10);
//   OLED_2.print("Average Drone SPL: ");
//   OLED_2.setCursor(0, 20);  // Print "SPL Value: " on OLED
//   OLED_2.print(SPL_DRONE);       // Print SPL receiving value on OLED
//   OLED_2.print(" dB");          // Print SPL units (dB) on OLED
//   OLED_2.setCursor(0, 30);      // Set "SPL Value: " at this position on OLED
//   OLED_2.print("Actual Ground SPL: ");  // Print "SPL Value: " on OLED
//   OLED_2.setCursor(0, 40);
//   OLED_2.print(SPL_GROUND);       // Print SPL receiving value on OLED
//   OLED_2.print(" dB");          // Print SPL units (dB) on OLED
//   OLED_2.setCursor(0, 50);
//   OLED_2.print("Predicted Ground SPL: ");
//   OLED_2.setCursor(0, 60);
//   OLED_2.print(SPL_PREDICT);
//   OLED_2.print(" dB");
// }

// void OLED_2_page_2() {
//   OLED_2.setCursor(0,10);
//   OLED_2.print("Page 2");
// }

float GetAlt() {
  sensors_event_t temp_event, pressure_event;
  DPS310.getEvents(&temp_event, &pressure_event);
  ALT_GROUND = DPS310.readAltitude(1013.25);
  return ALT_GROUND;
}

// float GetString() {
//   char combinedMessage[32]; 
//   NRF24L01.read(&combinedMessage, sizeof(combinedMessage));
//   String combinedStr = String(combinedMessage);
//   int separatorIndex = combinedStr.indexOf('|'); //Defines the index of | in the receiving string as an integer value
//   String splStr = combinedStr.substring(4, separatorIndex - 3); //Create a new string from the receiving string that contains the SPL value
//   String altitudeStr = combinedStr.substring(separatorIndex + 12, separatorIndex + 20); 
//   SPL_DRONE = splStr.toFloat();
//   ALT_DRONE = altitudeStr.toFloat();
//   return SPL_DRONE, ALT_DRONE;
// }

float GetSPL(int SPL_ADDRESS) {
  splRx.beginTransmission(SPL_ADDRESS);
  splRx.write(DB_REGISTER);
  splRx.endTransmission(false);
  splRx.requestFrom(SPL_ADDRESS, 1);
  SPL_GROUND = splRx.read();
  return SPL_GROUND;
}

uint8_t draw_state_1 = 0;
void UpdateOLED_1(void) {
  switch(draw_state_1) {
    case 0: OLED_1_page_0(); break;
    case 1: OLED_1_page_1(); break;
    case 2: OLED_1_page_2(); break;
  }
}
// uint8_t draw_state_2 = 0;
// void UpdateOLED_2(void) {
//   switch(draw_state_2) {
//     case 0: OLED_2_page_0(); break;
//     case 1: OLED_2_page_1(); break;
//     case 2: OLED_2_page_2(); break;
//   }
// }

void setup() {
  pinMode(BUTTON_1_PIN, INPUT);
  // pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  // OLED_1.setI2CAddress(0x3C);
  // OLED_2.setI2CAddress(0x3D);
  if(!OLED_1.begin()) { 
    Serial.println("OLED_1 connection failed");
    for(;;); 
  }
  // if(!OLED_2.begin()) { 
  //   Serial.println("OLED_2 connection failed");
  //   for(;;); 
  // }
  // if (!DPS310.begin_I2C(0x77, &Wire)) {
  //   Serial.println("DPS310 connection failed!");
  //   for(;;);
  // }
  // if (!NRF24L01.begin()) {
  //   Serial.println("NRF24L01 connection failed!");
  //   for(;;); 
  // }
  OLED_1.begin();
  OLED_1.setFont(u8g2_font_6x10_tf);  
  // OLED_2.begin();
  // OLED_2.setFont(u8g2_font_6x10_tf);
  DPS310.begin_I2C(0x77, &Wire);
  DPS310.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  DPS310.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  // NRF24L01.begin();
  // NRF24L01.openReadingPipe(1, address);
  // NRF24L01.setPALevel(RF24_PA_MIN);
  // NRF24L01.startListening();
}

void loop() {
  BUTTON_1_STATE = digitalRead(BUTTON_1_PIN);
  // BUTTON_2_STATE = digitalRead(BUTTON_2_PIN);
  // GetString();
  GetAlt();
  GetSPL(0x46);
  DELTA_ALT = ALT_DRONE - ALT_GROUND;
  SPL_PREDICT = SPL_DRONE * 0.4; // placeholder equation
  OLED_1.clearBuffer();
  // OLED_2.clearBuffer();
  UpdateOLED_1();
  // UpdateOLED_2();
  OLED_1.sendBuffer();
  // OLED_2.sendBuffer();
  if(BUTTON_1_STATE != BUTTON_1_LAST_STATE){
      draw_state_1++;
      delay(100);
      if(draw_state_1 >= 3) {
        draw_state_1 = 0;
      }
  }  
  // if(BUTTON_2_STATE != BUTTON_2_LAST_STATE){
  //     draw_state_2++;
  //     delay(100);
  //     if(draw_state_2 >= 3) {
  //       draw_state_2 = 0;
  //     }
  // }
}
