# PFD Simulator

> A real-time Primary Flight Display (PFD) simulator built in C++ and SFML.
It renders an interactive attitude indicator, airspeed/altitude/vertical speed tapes, and heading indicator that responds to keyboard input.
This project was built to learn a graphics library (SFML), deepen my C++ fundamentals through a hands-on project, and explore my interest in aerospace software.




https://github.com/user-attachments/assets/5f810480-6d4e-4212-a084-7f27c93ea911




## Instruments

|        Instrument          |     Description              |
|----------------------------|------------------------------|
| Attitude Indicator         | Includes a pitch ladder, roll arc, bank pointer. The flight path marker vertical position is from arcsin(vspeed/airspeed); horizontal position is track - heading  |
| Airspeed Tape              | Scrolling tape tracking the speed of the aircraft with stall / normal / caution / overspeed color bands. Has a max of 400 kts. |
| Altitude Tape              | Scrolling tape displaying the altitude, integrated from vertical speed each frame. Increases based on vspeed at ft/sec. Starts at 5,000 ft, has a max of 50000 ft |
| Heading Indicator          | Rotating compass with N, S, E, W labels and degree tick marks.                         |
| Vertical Speed Indicator   | Scrolling tape showing climb and descent rate in ft/min. Increases with pitch.      |
| Controls Overlay           | Press `H` to toggle a menu showing key bindings.                                    |

## Tech Stack

C++17 · SFML 2.6.2 · Visual Studio 2022 · Windows x64

## Build

1. Clone the repository: 
	1. git clone https://github.com/brandontieu626/pfd-display.git
	2. cd pfd-display
2. Open `PFD/PFD.sln` in Visual Studio 2022
3. Select **x64 Debug** or **x64 Release** and build
4. Select run, SFML DLLs are pre-bundled so no separate install of SFML is needed
5. Press **Escape** to exit


## Controls

|          Key                   |          Action                      |
|--------------------------------|--------------------------------------| 
| `UP` / `DOWN`                  |           Pitch up / down            |
| `LEFT` / `RIGHT`               |           Roll left / right          |
| `A` / `D`                      | Turn heading indicator left / right  |
| `W` / `S`                      | Airspeed up / down                   |
| `T` / `G`                      | Add crosswind effect right / left    |
| `H`                            | Toggle menu                          |
| `ESC`                          | Exit                                 |

## Architecture

- **`FlightData`** — A struct containing all plane data that's passed by const-ref to every instrument each frame.
- **`Instrument` base class**
	- Abstract class that all instrument classes inherit from, then implement their own draw() functionality
	- Allows all instruments to be stored in a `vector<unique_ptr<Instrument>>` and drawn in a single loop within main
- **Instrument classes** — Airspeed Tape, Altitude Tape, Attitude Indicator, Heading Indicator, Vertical Speed Indicator (see Instruments table above for details)
- **Render texture pattern** — AttitudeIndicator, AltitudeTape, and AirspeedTape are drawn into offscreen `sf::RenderTexture` canvases. Fixed overlays (readouts, labels) draw directly onto the window on top.
- **Resolution-independent scaling** — Each dimension is derived as fractions of desktop size to make sizing consistent.
- **Flight dynamics** — Vertical speed uses first-order lag (smoothly reaches a target value instead of snapping) through a formula of (`vspeed += (target − vspeed) × k × dt`). Altitude integrates from vspeed each frame.
- **FPM clamping** — Flight path marker calculates total distance to clamp the marker to stay within the AI boundary with combined pitch + crosswind angles.


## Future goals
- **Live flight data mode** - A separate mode that reads in real flight data (from a public flight-tracking API or flight database) 
and drives PFD instruments using that data in real time
	- OpenSky Network - Free, no API Key
	- ADS-B Exchange - Paid RapidAPI subscription
- **CMake build support** - Cross platform build configuration and support so that the project isn't dependent on Visual Studio and Windows




