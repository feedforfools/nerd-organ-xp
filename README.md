# Nerd Organ XP

Nerd Organ XP is a flexible and modular MIDI controller and event processor firmware designed for the Teensy 4.1 platform. It serves as a central hub for routing and processing musical events from various hardware and software sources.

## Core Features

*   **Advanced MIDI Routing:** Dynamically route MIDI events between physical hardware ports and USB MIDI devices.
*   **Modular Architecture:** The system is built around a clear distinction between event `Sources` (inputs), `Sinks` (outputs), and `Processors` (modifiers), making it highly extensible.
*   **Hardware Keybed Support:** Includes a robust scanner for Fatar-style keybeds, supporting dual contacts for both high-trigger and velocity-sensitive notes.
*   **USB MIDI Host:** Connect and interact with multiple USB MIDI devices, which are dynamically recognized and integrated into the routing system.
*   **Real-time Control:** Use physical switches to map controls and change the behavior of event processors on-the-fly (e.g., enable/disable processors, change MIDI channels, toggle modes).
*   **Event Processing:** Intercept and modify musical events with `NoteProcessor` modules, allowing for transposition, channel re-mapping, filtering, and more.

## Hardware Requirements

This project is designed for a custom hardware setup based on:

*   **Microcontroller:** Teensy 4.1
*   **Keyboard:** 61-key Fatar keybed
*   **I/O:** Hardware MIDI IN/OUT ports, USB Host port for MIDI devices, and physical selector switches.

All hardware-specific settings and pin configurations can be found in `src/config/config.h`.

## Building the Firmware

This project is configured for [PlatformIO](https://platformio.org/).

1.  Install [Visual Studio Code](https://code.visualstudio.com/).
2.  Install the [PlatformIO IDE extension](https://platformio.org/platformio-ide) from the VS Code marketplace.
3.  Clone this repository to your local machine.
4.  Open the cloned project folder in VS Code.
5.  Use the PlatformIO toolbar at the bottom of the VS Code window to build and upload the firmware to your Teensy 4.1.

## Future Enhancements & Roadmap

This project is under active development. Key areas for future improvement include:

*   **Known Issue:** A very rare and random bug causes a single MIDI note to be generated without any trigger. This occurs infrequently after long periods of operation and requires further investigation.
*   **Expanded USB Device Support:** Add native support for non-class-compliant USB MIDI devices to improve compatibility with a wider range of hardware, particularly the Roland Boutique series.
*   **Ecosystem Expansion with `nerd-pico`:** This firmware is one part of a larger modular ecosystem. It is designed to integrate with the **[nerd-pico](https://github.com/feedforfools/nerd-pico)**, a separate controller based on a Teensy 4.0.
    *   The `nerd-pico` will serve as a dedicated controller for pitch bend and modulation wheels.
    *   Future versions may include additional buttons, knobs, and an LCD screen for real-time status visualization and on-the-fly configuration of this project's routing and processor settings.

## Contributing

Contributions are welcome! If you have an idea for a new feature, a bug fix, or an improvement, please feel free to open an issue to discuss it or submit a pull request.