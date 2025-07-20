#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

#ifdef DEBUG
class Logger
{
    public:
        static void begin(const int baudRate = 9600)
        {
            Serial.begin(baudRate);
            while (!Serial && millis() < 4000); // Wait for Serial to be ready
        }
        
        static void log(const String& message)
        {
            Serial.println(message);
        }

        static void flush()
        {
            Serial.flush();
        }
};
#else
class Logger
{
    public:
        static void begin(const int baudRate = 9600) {}
        static void log(const String& message) 
        {
            (void)message;
        }
        static void flush() {}
};
#endif

#endif