#include "Sds011Reader.h"
#include "Log.h"

Sds011Reader::Sds011Reader(HardwareSerial &serial)
    : serial(serial) {}

bool Sds011Reader::readUntilDataHead()
{
  Logln("Reading until data head...");
  int value;

  while (true)
  {
    value = serial.read();
    if (value == DATA_HEAD)
    {
      Logln("Read data head successfully.");
      return true;
    }
    if (value == -1)
    {
      Logfln("Read value: %d", value);
      return false;
    }
    delay(1);
  }
}

void Sds011Reader::waitForData()
{
  while (serial.available() < 4)
  {
    delay(1);
  }
}

int Sds011Reader::convertHighLowByteToDecimal(uint8_t high, uint8_t low)
{
  return (high << 8) | low;
}

SensorData Sds011Reader::getLastSensorData()
{
  bool hasReadHead = false;
  SensorData currentData = SensorData(-1, -1);

  while (true)
  {
    if (!hasReadHead)
    {
      hasReadHead = readUntilDataHead();
    }

    if (hasReadHead)
    {
      currentData = readSensorData();
    }

    if (!readUntilDataHead())
    {
      break;
    }
    hasReadHead = true;
  }

  return currentData;
}

SensorData Sds011Reader::readSensorData()
{
  int pm25Low = serial.read();
  int pm25High = serial.read();
  int pm10Low = serial.read();
  int pm10High = serial.read();
  float pm25 = convertHighLowByteToDecimal(pm25High, pm25Low) / 10.0;
  float pm10 = convertHighLowByteToDecimal(pm10High, pm10Low) / 10.0;

  Logfln("Pm2.5: %f, Pm10: %f", pm25, pm10);
  return SensorData(pm25, pm10);
}