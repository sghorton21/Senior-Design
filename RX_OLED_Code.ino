#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <U8g2lib.h>

RF24 radio(9, 10); // CE, CSN (MAKE SURE THESE ARE THE RIGHT PINS ON ARDUINO)


const byte address[6] = "00001";
char receivedStr[32];
U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED_1(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  OLED_1.begin();
  OLED_1.setFont(u8g2_font_fub11_tr);
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char receivedMessage[32]; 
    radio.read(&receivedMessage, sizeof(receivedMessage));
    // Serial.println(text);

  String receivedStr(receivedMessage);

  if(receivedStr.startsWith("SPL Value: ")) {

    String splValueStr = receivedStr.substring(11);
    float splValue = splValueStr.toFloat();

    OLED_1.firstPage();
    do {
    OLED_1.clearDisplay();
    OLED_1.setCursor(0, 20);
    OLED_1.print("SPL Value ");

    OLED_1.setCursor(0,40);
    OLED_1.print(splValue);
    OLED_1.print(" dB");


    OLED_1.sendBuffer();
    }
    while (OLED_1.nextPage());


    // Serial.println("Received SPL Value");
    // Serial.print(splValue);
    // Serial.println(" dB");
    
    delay(500);
    }
  }
}
