#include "Log.h"

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