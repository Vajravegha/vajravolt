// MPU6050 sensor module can be soldered to the I2C mount. 
// AD0 pin needs to be connected to Vcc (3.3v) to change I2C address to 0x69 instead of default address 0x68. This is to avoid address clash with DS3231 RTC on-board

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;


char a, b, c;
unsigned long t1 = 0; // timer
void setup() {
  delay(5000);
  Serial.begin(115200);
  Wire.begin(5, 4);  // this is essential
  if (!mpu.begin(0x69)) {  //AD0 pin needs to be connected to Vcc (3.3v) to change I2C address to 0x69 instead of default address 0x68. This is to avoid address clash with DS3231 RTC on-board
    Serial.println("Failed to find MPU6050 chip");
  }
  else
  {
    Serial.println("MPU6050 Found!");
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("");
  delay(100);

  
}

void loop()
{

  if ((millis() - t1) > 1000)
  {
    t1 = millis();
    sensors_event_t acc, gyr, temp;
    mpu.getEvent(&acc, &gyr, &temp);
    Serial.print("Acceleration X: ");
    Serial.print(acc.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(acc.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(acc.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(gyr.gyro.x);
    Serial.print(", Y: ");
    Serial.print(gyr.gyro.y);
    Serial.print(", Z: ");
    Serial.print(gyr.gyro.z);
    Serial.println(" rad/s");

    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degC");

  }
  
}
