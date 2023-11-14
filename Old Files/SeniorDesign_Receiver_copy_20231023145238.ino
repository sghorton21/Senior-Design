//[2:59 PM] Cortes, Christopher Eugene

// Senior Design Code

// I2C sensor data acquisition on Daisy Seed (STM32H7-based system)

// Christopher Cortes

// 10-18-2023

 

// Include libraries

  #include <Adafruit_DPS310.h>

 // #include <U8g2lib.h>

  #include <Wire.h>

// #include <DaisyDuino.h> // Not really necessary but just in case

 

// // Define objects

//   Adafruit_DPS310 dps_1;

  Adafruit_DPS310 dps_ref;

  // U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED_1(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

 

// Define variables

  int db;

  float alt_ref, altitude_drone, delta_alt, spl_ground;

 

  float seaLevelhPa=1013.25;

  sensors_event_t temp_event, pressure_event;

  int SPL_ADDRESS = 0X48;

  byte DB_REGISTER = 0X0A;

 

// Setup function

  void setup() {

    // SPL Init

    Serial.begin(115200);

    Wire.begin();

 

    // U8g2 init

    // OLED_1.begin();

    // OLED_1.setFont(u8g2_font_fub11_tr);

 

    // // DPS310 init

    // dps_1.begin_I2C(0x77, &Wire);

    // dps_1.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);

    // dps_1.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

 

    dps_ref.begin_I2C(0x77, &Wire);

    dps_ref.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);

    dps_ref.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  }

 

void loop() {

  // // DPS310

  //   dps_1.getEvents(&temp_event, &pressure_event);

    dps_ref.getEvents(&temp_event, &pressure_event); // communicate with DPS310

    alt_ref = dps_ref.readAltitude(seaLevelhPa);

    // altitude_drone =  1.5*dps_ref.readAltitude(seaLevelhPa);

    // delta_alt = altitude_drone - alt_ref; // store altitude

 

  //SPL

    Wire.beginTransmission(SPL_ADDRESS);

    Wire.write(DB_REGISTER);

    Wire.endTransmission(false);

    Wire.requestFrom(SPL_ADDRESS, 1);

    byte db = Wire.read();

 

 

  // Calculate

  // spl_ground = db*(sq(alt_ref / altitude_drone ));

 

  // Print to OLED

  //     OLED_1.firstPage();

  //     do {

      //OLED_1.clearDisplay();

      // OLED_1.setCursor(0, 20);

      // OLED_1.print("Altitude = ");

      // OLED_1.setCursor(0,40);

      // OLED_1.print(alt_ref);

      // OLED_1.print(" m");

      // OLED_1.setCursor(0,60);

      // OLED_1.print("SPL = ");

      // OLED_1.print(spl_ground);

      // OLED_1.print(" dB");

      // }

      // while (OLED_1.nextPage());

 

  // Print to Serial Monitor

    // Serial.print("SPL drone = ");

    // Serial.println(db);

    // Serial.print("Altitude drone = ");

    // Serial.println(altitude_drone);

    Serial.print("Altitude ground = ");

    Serial.println(alt_ref);

    // Serial.print("Difference = ");

    // Serial.println(altitude_drone - alt_ref);

    // Serial.print("SPL ground = ");

    // Serial.println(spl_ground);

}
