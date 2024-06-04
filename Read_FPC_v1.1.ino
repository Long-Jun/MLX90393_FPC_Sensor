#include <Wire.h>

long x = 0;
long y = 0;
long z = 0;

// 定義MLX90393的I2C地址
#define ADDR1 0x0C
#define ADDR2 0x0D
#define ADDR3 0x0E

void setup() {
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  delay(100);

  //初始化I2C通信
  Wire.begin();
  //初始化串口及鮑率設定
  Serial.begin(9600);

  //分別設定三個MLX90393的初始化參數
  setupSensor(ADDR1);
  delay(100);
  setupSensor(ADDR2);
  delay(100);
  setupSensor(ADDR3);



  delay(15);
}

void loop() {
  float t, x, y, z;

  // 讀取三個傳感器的數據並顯示
  readAndPrintSensorData(ADDR1, &t, &x, &y, &z);
  Serial.print(z);
  Serial.print(",");
  readAndPrintSensorData(ADDR2, &t, &x, &y, &z);
  Serial.print(z);
  Serial.print(",");
  readAndPrintSensorData(ADDR3, &t, &x, &y, &z);
  Serial.println(z);

}

void setupSensor(byte address) {
  unsigned int data[9];

  //EXIT Mode
  Wire.beginTransmission(address);  // Start I2C Transmission
  Wire.write(0x80);                // Select Write register command
  Wire.endTransmission();          // Stop I2C Transmission
  delay(10);

  //RESET
  Wire.beginTransmission(address);  // Start I2C Transmission
  Wire.write(0xF0);                // Select Write register command
  Wire.endTransmission();          // Stop I2C Transmission
  delay(10);

  //CONF1
  Wire.beginTransmission(address);  // Start I2C Transmission
  Wire.write(0x60);                // Select Write register command
  Wire.write(0x00);                // Set AH = 0x00, BIST disabled
  Wire.write(0x1C);                // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
  Wire.write(0x00);                // Select address register, (0x00 << 2)
  Wire.endTransmission();          // Stop I2C Transmission
  Wire.requestFrom((uint8_t)address, (uint8_t)1);  // Request 9 bytes of data
  if (Wire.available() == 1)
    Serial.println(Wire.read(), HEX);
  delay(10);

  //CONF2
  Wire.beginTransmission(address);
  Wire.write(0x60);
  Wire.write(0x67);
  Wire.write(0xC0);
  Wire.write(0x01 << 2);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)address, (uint8_t)1);
  if (Wire.available() == 1)
    Serial.println(Wire.read(), HEX);
  delay(10);

  //CONF3
  Wire.beginTransmission(address);  // Start I2C Transmission
  Wire.write(0x60);                // Select Write register command
  Wire.write(0x02);                // Set AH = 0x02
  Wire.write(0xB4);                // Set AL = 0xB4, RES for magnetic measurement = 0
  Wire.write(0x02 << 2);              // Select address register, (0x02 << 2)
  Wire.endTransmission();          // Stop I2C Transmission
  Wire.requestFrom((uint8_t)address, (uint8_t)1);  // Request 9 bytes of data
  if (Wire.available() == 1)
    Serial.println(Wire.read(), HEX);
  delay(10);

  //OFFSET Z SET
  Wire.beginTransmission(address);  // Start I2C Transmission
  Wire.write(0x60);                // Select Write register command
  Wire.write(0x00);                // Set AH = 0x00, BIST disabled
  Wire.write(0x00);                // Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
  Wire.write(0x18);                // Select address register, (0x00 << 2)
  Wire.endTransmission();          // Stop I2C Transmission
  Wire.requestFrom((uint8_t)address, (uint8_t)1);  // Request 9 bytes of data
  if (Wire.available() == 1)
    Serial.println(Wire.read(), HEX);
  delay(10);

  Serial.println("fucking");

  Wire.beginTransmission(address);  // Start I2C Transmission
  Wire.write(0x50);                // Select Write register command
  Wire.write(0x08);                // Select Write register command
  Wire.endTransmission();          // Stop I2C Transmission
  Wire.requestFrom((uint8_t)address, (uint8_t)3);  // Request 9 bytes of data
  if (Wire.available() == 3) {
    for (int i = 0; i < 3; i++)
      Serial.print(Wire.read(), HEX);
    Serial.println();
  }

  Wire.beginTransmission(address);  // Start I2C Transmission
  Wire.write(0x50);                // Select Write register command
  Wire.write(0x18);                // Select Write register command
  Wire.endTransmission();          // Stop I2C Transmission
  Wire.requestFrom((uint8_t)address, (uint8_t)3);  // Request 9 bytes of data
  if (Wire.available() == 3) {
    for (int i = 0; i < 3; i++)
      Serial.print(Wire.read(), HEX);
    Serial.println();
  }
}

void readAndPrintSensorData(byte address, float *t, float *x, float *y, float *z) {
  unsigned int data[9];

  // 發送測量命令
  Wire.beginTransmission(address);
  Wire.write(0x3F);  // Start single measurement mode, ZYX enabled
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)address, (uint8_t)1);  // Request 1 byte of data

  delay(10);

  // 發送讀取命令
  Wire.beginTransmission(address);
  Wire.write(0x4F);  // Send read measurement command, ZYX enabled
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)address, (uint8_t)9);  // Request 9 bytes of data

  if (Wire.available() == 9) {
    for (int i = 0; i < 9; i++) {
      data[i] = Wire.read();
    }
  }

  // 轉換數據
  *x = data[3] << 8 | data[4];
  *y = data[5] << 8 | data[6];
  *z = data[7] << 8 | data[8];
  unsigned int Temper = data[1] * 256 + data[2];
  *t = (Temper - 46244) / 45.2 + 25;

  //  // 輸出數據到串口監視器
  //  Serial.print("Address: 0x");
  //  Serial.print(address, HEX);
  //  Serial.print(" | Temperature: ");
  //  Serial.print(Temper);
  //  Serial.print(" C | X: ");
  //  Serial.print(xMag);
  //  Serial.print(" uT, Y: ");
  //  Serial.print(yMag);
  //  Serial.print(" uT, Z: ");
  //  Serial.println(zMag);
}
