
# Smart Electronic Badge for Events

ESP32-based electronic badge with e-paper display designed for Codemotion events.
Displays **name, surname, job title, company**, and a **personalized QR code** synchronized directly with the check-in application.

## Key Features

- Ultra-low power e-paper display
- Direct synchronization with check-in application
- USB-C programming and power supply
- Personalized QR code generation
- Fully passive operation during events
- Reusable and sustainable design

## Hardware Components

- **ESP32-C3 Super Mini** (WeAct ESP32-C3)
- **E-paper Display** Waveshare 2.9" black/white/red (GxEPD2_290_C90c)
- **USB-C Port** for power and programming

## Hardware Schematic [deprecated]

![Hardware Schematic](docs/schema_hardware.png)

```
- ESP32 GPIO -> E-paper Display (SPI)
- USB-C -> 5V + Programming
- Button -> GPIO 5 (optional for future features)
```

## Renders and Prototypes

### PCB Renders

![PCB Render Front](docs/render_front.png)
![PCB Render Back](docs/render_back.png)

### Prototype Photos

![Prototype Photo](docs/prototype_photo.png)

## Build & Flash Firmware

### Requirements

- PlatformIO (recommended) or Arduino IDE
- Libraries (automatically managed by PlatformIO):
    - GxEPD2_3C
    - U8g2_for_Adafruit_GFX
    - QRCodeGFX
    - Adafruit GFX Library
    - Adafruit BusIO

### Build Instructions

```bash
# Clone the repository
git clone git@github.com:salvatore-esposito-green/Smart-Badge.git
cd Smart-Badge

# Build and flash with PlatformIO
pio run -t upload && pio device monitor
```

### Firmware Structure

```cpp
// Pin Configuration (ESP32-C3)
#define EPD_CS      7
#define EPD_DC      1
#define EPD_RST     2
#define EPD_BUSY    10
// SCK (4) and MOSI (6) managed by hardware SPI

// Libraries
#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <QRCodeGFX.h>

// Data Structure
struct BadgeData {
  String name;
  String surname;
  String role;
  String company;
  String qrLink;
};

// Global Components
GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(...);
U8G2_FOR_ADAFRUIT_GFX u8g2;
QRCodeGFX qrcode(display);
BadgeData badge;
```

### Main Functions

- **drawContent()**: Renders the complete badge layout on the display
- **refreshDisplay()**: Updates the e-paper display using paged rendering
- **parseSerialData()**: Receives data via serial and updates the badge
- **getValue()**: Parses semicolon-delimited serial input

### Serial Communication Protocol

The badge listens for serial input at **9600 baud** in the following format:

```
Name;Surname;Role;Company;QRLink\n
```

Example:
```
John;Doe;Software Engineer;Acme Corp;https://example.com/profile/johndoe
```

## Roadmap

- [x] Working prototype
- [x] Check-in app synchronization
- [x] Production of first 5 units
- [ ] Over-the-air (OTA) wireless update support
- [ ] Customizable graphic templates
- [ ] Multi-event support

## Production Costs

- **Initial production**: 5 units manufactured via PCBWay (~$20 USD per unit)
- **Estimated volume production** (100-300 units): ~$10 USD per unit

## Contributing

Pull requests and ideas are welcome!

## License

MIT License

## Presentation

For more details about the project, see the [presentation slides](https://www.canva.com/design/DAGpyELCSFQ/qBx6hwTSARdgQ4nmrtGfRQ/edit?utm_content=DAGpyELCSFQ&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton).
