#define FLAG_SHOW_METHOD_REFS 0

#if FLAG_SHOW_METHOD_REFS
    #define  Log(msg) Serial.print("["); Serial.print(__PRETTY_FUNCTION__); Serial.print(":"); Serial.print(__LINE__); Serial.print("] "); Serial.print(msg)
#else
    #define Log(msg) Serial.print(msg)
#endif

#if FLAG_SHOW_METHOD_REFS
    #define  Logf(fmt, ...) logf(__PRETTY_FUNCTION__,__LINE__, fmt, ##__VA_ARGS__)
#else
    #define Logf(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#endif

#if FLAG_SHOW_METHOD_REFS
    #define   Logln(msg) Serial.print("["); Serial.print(__PRETTY_FUNCTION__); Serial.print(":"); Serial.print(__LINE__); Serial.print("] "); Serial.println(msg)
#else
    #define Logln(msg) Serial.println(msg)
#endif

#if FLAG_SHOW_METHOD_REFS
    #define  Logfln(fmt, ...) logfln(__PRETTY_FUNCTION__,__LINE__, fmt, ##__VA_ARGS__)
#else
    #define Logfln(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__); Serial.println("")
#endif

#define LogPlain(msg) Serial.print(msg)
#define LogPlainLn(msg) Serial.println(msg)

#include <Arduino.h>
#include <stdarg.h>

void logf(const char* tag, int level, const char* fmt, ...);
void logfln(const char* tag, int level, const char* fmt, ...);