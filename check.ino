#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;
uint32_t tsLastReport = 0;
void onBeatDetected() {
  Serial.println("♥ Beat!");
}


////////////////////////////////

#include <SPI.h>
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int minVal = 265;
int maxVal = 402;

double x;
double y;
double z;



void setup()
{
  Serial.begin(115200);

  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;);
  }
  else
  {
    Serial.println("SUCCESS");
  }

  // Configure sensor to use 7.6mA for LED drive
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback routine
  pox.setOnBeatDetectedCallback(onBeatDetected);


  ////////////////////////////////

  Wire.begin();

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void loop()
{
  // Read from the sensor
  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    ////////////////////////////////

    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU_addr, 14, true);
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
    int xAng = map(AcX, minVal, maxVal, -90, 90);
    int yAng = map(AcY, minVal, maxVal, -90, 90);
    int zAng = map(AcZ, minVal, maxVal, -90, 90);

    x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
    y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
    z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

//            Serial.print("AngleX= ");
//            Serial.println(x);
//    
//            Serial.print("AngleY= ");
//            Serial.println(y);
//    
//            Serial.print("AngleZ= ");
//            Serial.println(z);

    if (x > 260 && x < 280)
      Serial.println("i need water");
    if (y > 50 && y < 80)
      Serial.println("need medicine");
    if (x > 280 && x < 320)
      Serial.println("RESTROOM");
    if (x > 60 && x < 90)
      Serial.println("I FEEL HUNGRY");
    if (x > 100 && x < 130 && y > 90 && y < 110)
      Serial.print("FEELING STRESSED");
    if (x > 200 && x < 224)
      Serial.println("EMERGENCY THEIF");
    if (z == 225 & y == 225)
      Serial.println("malfunction");
    Serial.println();

    tsLastReport = millis();
  }
}
