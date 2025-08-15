#include "Sds011Reader.h"

Sds011Reader::Sds011Reader(HardwareSerial& serial)
  : serial(serial){}

void Sds011Reader::readUntilDataHead() {
  while (serial.read() != DATA_HEAD) {
    delay(1);
  }
}

void Sds011Reader::waitForData() {
  while (serial.available() < 4) {
    delay(1);
  }
}

int Sds011Reader::convertHighLowByteToDecimal(uint8_t high, uint8_t low) {
  return (high << 8) | low;
}
 
SensorData Sds011Reader::getSensorData() {
  waitForData();
  readUntilDataHead();
  int pm25Low = serial.read();
  int pm25High = serial.read();
  int pm10Low = serial.read();
  int pm10High = serial.read();
  float pm25 = convertHighLowByteToDecimal(pm25High, pm25Low) / 10.0;
  float pm10 = convertHighLowByteToDecimal(pm10High, pm10Low) / 10.0;

  return SensorData(pm25, pm10);
}