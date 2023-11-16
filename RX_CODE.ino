// UPDATED 11-14-2023 3:01pm - CC

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_DPS310.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED_1(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED_2(U8G2_R0, U8X8_PIN_NONE);
RF24 nrf24l01(9, 10);
Adafruit_DPS310 dps310;
TwoWire i2cbus;

const int BUTTON_1_PIN = 7;
const int BUTTON_2_PIN = 8;
uint8_t DRAW_STATE_1 = 1;
uint8_t DRAW_STATE_2 = 1;
float SPL_DRONE_1;
float SPL_DRONE_2;
float SPL_DRONE_3;
float SPL_DRONE_AVG;
float SPL_GROUND;
float ALT_DRONE;
float ALT_GROUND;
float DELTA_ALT;
float SPL_PREDICT;

void OLED_1_page_1() {
  OLED_1.setCursor(0, 10);
  OLED_1.print("Drone Altitude: ");
  OLED_1.setCursor(0, 20);
  OLED_1.print(ALT_DRONE);
  OLED_1.print(" m");
  OLED_1.setCursor(0, 30);
  OLED_1.print("Ground Altitude: ");
  OLED_1.setCursor(0, 40);
  OLED_1.print(ALT_GROUND);
  OLED_1.print(" m");
  OLED_1.setCursor(0, 50);
  OLED_1.print("Delta Altitude: ");
  OLED_1.setCursor(0, 60);
  OLED_1.print(DELTA_ALT);
  OLED_1.print(" m");
}

void OLED_1_page_2() {
  OLED_1.setCursor(0, 17);
  OLED_1.print("Predicted SPL: ");
  OLED_1.setCursor(0, 27);
  OLED_1.print(SPL_PREDICT);
  OLED_1.print(" dB");
  OLED_1.setCursor(0, 37);
  OLED_1.print("Ground SPL: ");
  OLED_1.setCursor(0, 47);
  OLED_1.print(SPL_GROUND);
  OLED_1.print(" dB");
}


void OLED_2_page_1() {
  OLED_2.setCursor(0, 10);
  OLED_2.print("SPL 1: ");
  OLED_2.setCursor(0, 20);
  OLED_2.print(SPL_DRONE_1);
  OLED_2.print(" dB");
  OLED_2.setCursor(0, 30);
  OLED_2.print("SPL 2");
  OLED_2.setCursor(0, 40);
  OLED_2.print(SPL_DRONE_2);
  OLED_2.print(" dB");
  OLED_2.setCursor(0, 50);
  OLED_2.print("SPL 3: ");
  OLED_2.setCursor(0, 60);
  OLED_2.print(SPL_DRONE_3);
  OLED_2.print(" dB");
}

void OLED_2_page_2() {
  OLED_2.setCursor(0, 17);
  OLED_2.print("Average Drone SPL: ");
  OLED_2.setCursor(0, 27);
  OLED_2.print(SPL_DRONE_AVG);
  OLED_2.print(" dB");
  OLED_1.setCursor(0, 37);
  OLED_1.print("Ground SPL: ");
  OLED_1.setCursor(0, 47);
  OLED_1.print(SPL_GROUND);
  OLED_1.print(" dB");
}

float GetAlt() {
  sensors_event_t temp_event, pressure_event;
  dps310.getEvents(&temp_event, &pressure_event);
  ALT_GROUND = dps310.readAltitude(1013.25);
  return ALT_GROUND;
}

float ReceiveString() {
  char RECEIVED_CHAR[32]; 
  nrf24l01.read(&RECEIVED_CHAR, sizeof(RECEIVED_CHAR));
  String DATA_RECEIVE = String(RECEIVED_CHAR);
  String ALT_STRING = DATA_RECEIVE.substring(11, 15);
  String SPL_1_STRING = DATA_RECEIVE.substring(24, 28);
  String SPL_2_STRING = DATA_RECEIVE.substring(37, 41);
  String SPL_3_STRING = DATA_RECEIVE.substring(50, 54);
  String SPL_DRONE_AVG_STRING = DATA_RECEIVE.substring(65, 69);
  ALT_DRONE = ALT_STRING.toFloat();
  SPL_DRONE_1 = SPL_1_STRING.toFloat();
  SPL_DRONE_2 = SPL_2_STRING.toFloat();
  SPL_DRONE_3 = SPL_3_STRING.toFloat();
  SPL_DRONE_AVG = SPL_DRONE_AVG_STRING.toFloat();
  return SPL_DRONE_1, SPL_DRONE_2, SPL_DRONE_3, SPL_DRONE_AVG, ALT_DRONE;
}

float GetSPL(int SPL_ADDRESS) {
  i2cbus.beginTransmission(SPL_ADDRESS);
  i2cbus.write(0x0A);
  i2cbus.endTransmission(false);
  i2cbus.requestFrom(SPL_ADDRESS, 1);
  SPL_GROUND = i2cbus.read();
  return SPL_GROUND;
}

void UpdateOLED_1(void) {
  switch(DRAW_STATE_1) {
    case 1: OLED_1_page_1(); break;
    case 2: OLED_1_page_2(); break;
  }
}
void UpdateOLED_2(void) {
  switch(DRAW_STATE_2) {
    case 1: OLED_2_page_1(); break;
    case 2: OLED_2_page_2(); break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  OLED_1.setI2CAddress(0x3C * 2);
  OLED_2.setI2CAddress(0x3D * 2);
  // if (!nrf24l01.begin()) {
  //   Serial.println("NRF24L01 connection failed!");
  //   for(;;); 
  // }
  // if (!dps310.begin_I2C(0x77, &Wire)) {
  //   Serial.println("DPS310 connection failed!");
  //   for(;;);
  // }
  // if(!OLED_1.begin()) { 
  //   Serial.println("OLED_1 connection failed");
  //   for(;;); 
  // }
  // if(!OLED_2.begin()) { 
  //   Serial.println("OLED_2 connection failed");
  //   for(;;); 
  // }
  OLED_1.begin();
  OLED_1.setFont(u8g2_font_6x10_tf);  
  OLED_2.begin();
  OLED_2.setFont(u8g2_font_6x10_tf);
  dps310.begin_I2C(0x77, &Wire);
  dps310.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps310.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  nrf24l01.begin();
  nrf24l01.openReadingPipe(1, 00001);
  nrf24l01.setPALevel(RF24_PA_MIN);
  nrf24l01.startListening();
}

void loop() {
  uint8_t BUTTON_1_STATE = 0; 
  uint8_t BUTTON_2_STATE = 0;
  uint8_t BUTTON_1_LAST_STATE = 0;
  uint8_t BUTTON_2_LAST_STATE = 0;
  BUTTON_1_STATE = digitalRead(BUTTON_1_PIN);
  BUTTON_2_STATE = digitalRead(BUTTON_2_PIN);
  ReceiveString();
  GetAlt();
  GetSPL(0x46);
  DELTA_ALT = ALT_DRONE - ALT_GROUND;
  SPL_PREDICT = SPL_DRONE_AVG * 0.4; // placeholder equation
  OLED_1.clearBuffer();
  UpdateOLED_1();
  OLED_1.sendBuffer();
  OLED_2.clearBuffer();
  UpdateOLED_2();
  OLED_2.sendBuffer();
  if(BUTTON_1_STATE != BUTTON_1_LAST_STATE){
    DRAW_STATE_1++;
    delay(100);
    if(DRAW_STATE_1 >= 3) {
      DRAW_STATE_1 = 1;
    }
  }  
  if(BUTTON_2_STATE != BUTTON_2_LAST_STATE){
    DRAW_STATE_2++;
    delay(100);
    if(DRAW_STATE_2 >= 3) {
      DRAW_STATE_2 = 1;
    }
  }
}
