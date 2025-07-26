#include "keybed_source.h"
#include <cstring>
#include "utils/logger.h"

KeybedSource::KeybedSource()
{
    std::memset(velocityNoteOnSent, 0, sizeof(velocityNoteOnSent));
}

void KeybedSource::onKeyEvent(const KeyEvent& event)
{
    int noteNumber = MIDI_STARTING_NOTE + event.keyIndex;

    MusicalEvent musicalEvent;
    musicalEvent.sourcePortId = getPortId();
    musicalEvent.channel = 0;
    musicalEvent.data1 = (uint8_t)noteNumber;

    switch (event.eventType)
    {
        case KEY_TOUCHED:
            musicalEvent.type = MusicalEventType::NOTE_ON_HIGH_TRIGGER;
            musicalEvent.data2 = HIGH_TRIG_VELOCITY;
            fireEvent(musicalEvent);
            break;

        case KEY_PRESSED:
            if (!velocityNoteOnSent[event.keyIndex])
            {
                musicalEvent.type = MusicalEventType::NOTE_ON;
                musicalEvent.data2 = (uint8_t)calculateVelocity(event.deltaTime);
                fireEvent(musicalEvent);
                velocityNoteOnSent[event.keyIndex] = true;
            }
            break;

        case KEY_TOP:
            musicalEvent.type = MusicalEventType::NOTE_OFF;
            musicalEvent.data2 = 0;
            fireEvent(musicalEvent);
            velocityNoteOnSent[event.keyIndex] = false;
            break;
        
        case KEY_RELEASED:
            // TODO: setup release velocity if needed
            break;
    }
}

// int KeybedSource::calculateVelocity(uint32_t t)
// {
//     // TODO: separate velocity calculation in another class with different algorithms
//     // TODO: add support for different velocity curves configurable via system settings
//     if(t < 11) return 127;
//     if(t < 28) return int(-1.588 * t + 142.882);
//     if(t < 41) return int(-1.5385 * t + 141.5385);
//     if(t < 67) return int(-0.7692 * t + 110.769);
//     if(t < 111) return int(-0.4545 * t + 90);
//     if(t < 181) return int(-0.2857 * t + 71.4286);
//     if(t < 330) return int(-0.1267 * t + 42.8);
//     if(t > 329) return 1;
//     return 1;
// }

int KeybedSource::calculateVelocity(uint32_t t)
{
    const float CALIBRATED_TIME_FASTEST = 2.0f;
    const float CALIBRATED_TIME_SLOWEST = 250.0f;
    const float VELOCITY_EXPONENT = 6.3f;

    float clamped_t = static_cast<float>(t);
    if (clamped_t < CALIBRATED_TIME_FASTEST) clamped_t = CALIBRATED_TIME_FASTEST;
    if (clamped_t > CALIBRATED_TIME_SLOWEST) clamped_t = CALIBRATED_TIME_SLOWEST;
    float normalized_velocity = (CALIBRATED_TIME_SLOWEST - clamped_t) / (CALIBRATED_TIME_SLOWEST - CALIBRATED_TIME_FASTEST);
    float curved_velocity = pow(normalized_velocity, VELOCITY_EXPONENT);
    int velocity = 1 + static_cast<int>(curved_velocity * 126.0f);

    return velocity;
}
