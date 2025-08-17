#define Log(msg) Serial.print("["); Serial.print(__PRETTY_FUNCTION__); Serial.print(":"); Serial.print(__LINE__); Serial.print("] "); Serial.print(msg)
#define Logf(fmt, ...) logf(__PRETTY_FUNCTION__,__LINE__, fmt, ##__VA_ARGS__)
#define Logln(msg) Serial.print("["); Serial.print(__PRETTY_FUNCTION__); Serial.print(":"); Serial.print(__LINE__); Serial.print("] "); Serial.println(msg)
#define Logfln(fmt, ...) logfln(__PRETTY_FUNCTION__,__LINE__, fmt, ##__VA_ARGS__)

#include <Arduino.h>
#include <stdarg.h>

void logf(const char* func, int line, const char* fmt, ...) {
    Serial.print("[");
    Serial.print(func);
    Serial.print(":");
    Serial.print(line);
    Serial.print("] ");

    char buffer[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    Serial.println(buffer);
}

void logfln(const char* func, int line, const char* fmt, ...) {
    char buffer[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    Serial.print("[");
    Serial.print(func);
    Serial.print(":");
    Serial.print(line);
    Serial.print("] ");
    Serial.println(buffer);
}