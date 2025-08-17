#ifndef SDS011READER_H
#define SDS011READER_H

#include <Arduino.h>
#include "Log.h"

constexpr uint8_t DATA_HEAD = 0xAA;

class SensorData {
public:
  float pm25;
  float pm10;

  SensorData(float pm25, float pm10): pm25(pm25), pm10(pm10){}
};

class Sds011Reader {
public:
  HardwareSerial& serial;

  Sds011Reader(HardwareSerial& serial);

  bool readUntilDataHead();
  int convertHighLowByteToDecimal(uint8_t high, uint8_t low);
  SensorData getLastSensorData();

private:
  SensorData readSensorData();
private:
  void waitForData();
};

#endif