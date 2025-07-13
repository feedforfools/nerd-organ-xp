#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

class Logger
{
    public:
        static void log(const String& message) {
            Serial.println(message);
        }
};


#endif