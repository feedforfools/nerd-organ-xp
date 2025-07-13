// Possible key states
typedef enum {
   KEY_IS_UP,
   KEY_IS_DOWN,
   KEY_IS_GOING_UP,    // We increment the timer in this state
   KEY_IS_GOING_DOWN,  // We increment the timer in this state
} state_t;

// Possible events
typedef enum {
   KEY_TOUCHED,        // Key is touched (break contact in short, still not to bottom)
   KEY_PRESSED,        // Key reached bottom (make contact in short, fully pressed)
   KEY_RELEASED,       // Key is released (make contact not short, break still short, still not to top)
   KEY_TOP,            // Key reached top (no more shorts, no more touched)
} event_t;

typedef struct {
   int           midi_note:8;
   bool          played;
   state_t       state:4; // Bit fields
   unsigned long int  t:32; // It's difficult to go beyond 4billion cycles
} key_fatar_t;

typedef struct {
   byte pin;
   byte bit;
} teensy_port_reg;

typedef struct {
   byte breaks;         // byte of shorts in break contacts (top) in current bank
   byte makes;          // byte of shorts in make contacts (bottom) in current bank
} bank_t;
