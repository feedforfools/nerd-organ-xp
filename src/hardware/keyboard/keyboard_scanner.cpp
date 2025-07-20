#include "keyboard_scanner.h"

KeyboardScanner::KeyboardScanner()
{
    // Initialize multiplexer pins
    mplxEnable[LEFT] = { .pin = 6, .bit = 10 };
    mplxEnable[RIGHT] = { .pin = 9, .bit = 11 };
    mplxAddress[0] = { .pin = 10, .bit = 0 };
    mplxAddress[1] = { .pin = 11, .bit = 2 };
    mplxAddress[2] = { .pin = 12, .bit = 1 };

    // Initialize grid matrix input pins
    teensy_port_reg gridMatrix[NUM_BANKS] = 
    {
        { .pin = 14, .bit = 18 },
        { .pin = 15, .bit = 19 },
        { .pin = 16, .bit = 23 },
        { .pin = 17, .bit = 22 },
        { .pin = 18, .bit = 17 },
        { .pin = 19, .bit = 16 },
        { .pin = 20, .bit = 26 },
        { .pin = 21, .bit = 27 }
    };
    memcpy(gridMatrixInput, gridMatrix, sizeof(gridMatrixInput));
}

void KeyboardScanner::init()
{
    initializePins();
    initializeKeys();

    memset(banks, 0x00, sizeof(banks));
    memset(prev_banks, 0x00, sizeof(prev_banks));
}

void KeyboardScanner::initializePins()
{
    // Setup multiplexer pins
    for (int i = 0; i < NUM_KEYZONES; i++)
    {
        pinMode(mplxEnable[i].pin, OUTPUT);
    }

    for (int i = 0; i < NUM_BIT_MPLX; i++)
    {
        pinMode(mplxAddress[i].pin, OUTPUT);
    }

    // Setup grid matrix input pins
    for (int i = 0; i < NUM_BANKS; i++)
    {
        pinMode(gridMatrixInput[i].pin, INPUT_PULLDOWN);
    }
}

void KeyboardScanner::initializeKeys()
{
    for (int i = 0; i < NUM_KEYS; i++)
    {
        keys[i].played = false;
        keys[i].state = KEY_IS_UP;
        keys[i].t = 0;
    }

    memset(debounceCounters, 0, sizeof(debounceCounters));
}

void KeyboardScanner::registerListener(IKeyEventListener *listener)
{
    if (listener) 
    {   
        listeners.push_back(listener);
    }
}

void KeyboardScanner::scan()
{
    // Store previous bank states
    memcpy(prev_banks, banks, sizeof(banks));

    // Scan banks
    for (int i = 0; i < NUM_BANKS; ++i)
    {
        byte left, right;

        const uint32_t MUX_ADDRESS_MASK = (1 << mplxAddress[0].bit) | (1 << mplxAddress[1].bit) | (1 << mplxAddress[2].bit);
        // Scan left keyzone
        GPIO7_DR_SET = (1 << mplxEnable[LEFT].bit) | (i & MUX_ADDRESS_MASK);
        delayMicroseconds(10);
        left = compactDataRegister((GPIO6_DR >> 16), gridMatrixInput, 8);
        GPIO7_DR_CLEAR = (1 << mplxEnable[LEFT].bit);
        delayMicroseconds(10);

        // Scan right keyzone
        GPIO7_DR_SET = (1 << mplxEnable[RIGHT].bit) | (i & MUX_ADDRESS_MASK);
        delayMicroseconds(10);
        right = compactDataRegister((GPIO6_DR >> 16), gridMatrixInput, 8);
        const uint32_t MUX_ENABLE_AND_ADDRESS_MASK = (1 << mplxEnable[LEFT].bit) | (1 << mplxEnable[RIGHT].bit) | MUX_ADDRESS_MASK;
        GPIO7_DR_CLEAR = MUX_ENABLE_AND_ADDRESS_MASK;
        delayMicroseconds(10);

        banks[i].breaks = (left & 0x0F) | (right & 0x0F) << 4;
        banks[i].makes = (left & 0xF0) >> 4 | (right & 0xF0);
    }

    processScanResults();
}

void KeyboardScanner::processScanResults()
{
    for (int bankIndex = 0; bankIndex < NUM_BANKS; ++bankIndex)
    {
        byte currentBreaks = banks[bankIndex].breaks;
        byte previousBreaks = prev_banks[bankIndex].breaks;
        byte currentMakes = banks[bankIndex].makes;
        byte previousMakes = prev_banks[bankIndex].makes;

        // TODO: Debounce also the inverse contact changes (from pressed to released)
        for (int keyInBankIndex = 0; keyInBankIndex < NUM_KEYS_PER_BANK; ++keyInBankIndex)
        {
            uint8_t keyIndex = bankIndex + keyInBankIndex * NUM_KEYS_PER_BANK;
            if (keyIndex >= NUM_KEYS) continue;

            // Check top contacts (breaks)
            bool currentBreakState = bitRead(currentBreaks, keyInBankIndex);
            bool previousBreakState = bitRead(previousBreaks, keyInBankIndex);
            // Debouncing key touch events
            if (debounceCounters[keyIndex][TOP] > 0)
            {
                if (currentBreakState)
                {
                    ++debounceCounters[keyIndex][TOP]; // Continue debouncing
                    if (debounceCounters[keyIndex][TOP] >= NUM_DEBOUNCE_CYCLES)
                    {
                        // Debounce complete => trigger key event
                        triggerKeyEvent(keyIndex, KEY_TOUCHED);
                        debounceCounters[keyIndex][TOP] = 0; // Reset debounce counter
                    }
                }
                else 
                {
                    debounceCounters[keyIndex][TOP] = 0;
                }
            }
            else
            {
                if (currentBreakState && !previousBreakState)
                {
                    ++debounceCounters[keyIndex][TOP]; // Start debouncing
                } 
                else if (!currentBreakState && previousBreakState)
                {
                    triggerKeyEvent(keyIndex, KEY_TOP); // Key released (no debounce here)
                }
            }

            // Check bottom contacts (makes)
            bool currentMakeState = bitRead(currentMakes, keyInBankIndex);
            bool previousMakeState = bitRead(previousMakes, keyInBankIndex);
            // Debouncing key touch events
            if (debounceCounters[keyIndex][BOTTOM] > 0)
            {
                if (currentMakeState)
                {
                    ++debounceCounters[keyIndex][BOTTOM]; // Continue debouncing
                    if (debounceCounters[keyIndex][BOTTOM] >= NUM_DEBOUNCE_CYCLES)
                    {
                        // Debounce complete => trigger key event
                        triggerKeyEvent(keyIndex, KEY_PRESSED);
                        debounceCounters[keyIndex][BOTTOM] = 0; // Reset debounce counter
                    }
                }
                else
                {
                    debounceCounters[keyIndex][BOTTOM] = 0;
                }
            }
            else
            {
                if (currentMakeState && !previousMakeState)
                {
                    ++debounceCounters[keyIndex][BOTTOM]; // Start debouncing
                }
                else if (!currentMakeState && previousMakeState)
                {
                    triggerKeyEvent(keyIndex, KEY_RELEASED); // Key released (no debounce here)
                }
            }
        }
    }
}

void KeyboardScanner::triggerKeyEvent(uint8_t keyIndex, event_t event)
{
    key_fatar_t *key = &keys[keyIndex];
    uint32_t t = 0;

    switch (event)
    {
        case KEY_TOUCHED:
            key->state = KEY_IS_GOING_DOWN;
            key->t = 0;
            break;

        case KEY_PRESSED:
            key->state = KEY_IS_DOWN;
            t = key->t;
            key->played = true;
            break;
        
        case KEY_RELEASED:
            key->state = KEY_IS_GOING_UP;
            key->t = 0;
            break;
        
        case KEY_TOP:
            t = key->t;
            key->state = KEY_IS_UP;
            key->played = false;
            key->t = 0;
            break;
        
        default:
            break;
    }

    // Notify all listeners about the key event
    KeyEvent keyEvent = { keyIndex, event, t };
    for (auto listener : listeners)
    {
        listener->onKeyEvent(keyEvent);
    }
}

byte KeyboardScanner::compactDataRegister(word dataRegister, teensy_port_reg arrangement[], int size)
{
    byte out = 0b0;
    for (int i = 0; i < size; ++i)
    {
        word temp = dataRegister & (1 << int(arrangement[i].bit - 16));
        if (arrangement[i].bit - 16 - i > 0)
        {
            out |= temp >> int(arrangement[i].bit - 16 - i);
        } else
        {
            out |= temp << int(abs(arrangement[i].bit - 16 - i));
        }
    }
    return out;
}

void KeyboardScanner::updateTimers()
{
    for (int i = 0; i < NUM_KEYS; ++i)
    {
        state_t keyState = keys[i].state;
        if (keyState == KEY_IS_GOING_UP || keyState == KEY_IS_GOING_DOWN)
        {
            ++keys[i].t;
        }
    }
}