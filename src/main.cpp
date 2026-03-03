#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <QRCodeGFX.h>

// --- PIN CONFIGURATION (WeAct ESP32-C3) ---
#define EPD_CS      7
#define EPD_DC      1
#define EPD_RST     2
#define EPD_BUSY    10
// SCK (4) and MOSI (6) are managed by hardware SPI default

// --- DATA STRUCTURE ---
struct BadgeData {
  String name = "Codemotion";
  String surname = "Milan 2025";
  String role = "Attendee";
  String company = "Community";
  String qrLink = "https://codemotion.com";
};

// --- GLOBAL OBJECTS ---
// Nota: Assicurati che il modello del display (GxEPD2_290_C90c) corrisponda esattamente al tuo hardware
GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));
U8G2_FOR_ADAFRUIT_GFX u8g2;
QRCodeGFX qrcode(display);

BadgeData badge;
bool needRefresh = true;

// --- FUNCTION PROTOTYPES (Obbligatori in .cpp) ---
String getValue(String data, char separator, int index);
void parseSerialData();
void drawContent();
void refreshDisplay();

// --- HELPER FUNCTIONS ---

// Helper to extract data from the semicolon-separated string
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void parseSerialData() {
  if (Serial.available() > 0) {
    String rx = Serial.readStringUntil('\n');
    rx.trim();
    if (rx.length() < 5) return; // Ignore noise or empty inputs

    // Expected format: Name;Surname;Role;Company;QRLink
    badge.name    = getValue(rx, ';', 0);
    badge.surname = getValue(rx, ';', 1);
    badge.role    = getValue(rx, ';', 2);
    badge.company = getValue(rx, ';', 3);
    badge.qrLink  = getValue(rx, ';', 4);
    
    // If data is missing, use placeholders to avoid graphical crashes
    if(badge.qrLink == "") badge.qrLink = "https://google.com";
    
    Serial.println("Dati ricevuti. Aggiornamento in corso...");
    needRefresh = true;
  }
}

// --- GRAPHIC LOGIC ---

void drawContent() {
  // Font and Color Setup
  u8g2.setBackgroundColor(GxEPD_WHITE);
  u8g2.setForegroundColor(GxEPD_BLACK);
  
  int x_margin = 5;
  int y_cursor = 0;

  // 1. NAME
  u8g2.setFont(u8g2_font_crox5tb_tf); // Medium Bold Font
  y_cursor += u8g2.getFontAscent() + 10; 
  u8g2.setCursor(x_margin, y_cursor);
  u8g2.print(badge.name);
  
  // 2. SURNAME
  y_cursor += u8g2.getFontAscent() + 10; // Automatic spacing based on font
  u8g2.setCursor(x_margin, y_cursor);
  u8g2.print(badge.surname);

  // 3. ROLE (Smaller font)
  u8g2.setFont(u8g2_font_luBIS10_tf);
  y_cursor += u8g2.getFontAscent() + 35; // A bit more space
  u8g2.setCursor(x_margin, y_cursor);
  u8g2.print(badge.role);

  // 4. COMPANY (In Red)
  u8g2.setForegroundColor(GxEPD_RED);
  u8g2.setFont(u8g2_font_helvB18_tf);
  y_cursor += u8g2.getFontAscent() + 5;
  u8g2.setCursor(x_margin, y_cursor);
  u8g2.print(badge.company);

  // 5. QR CODE
  // generate the qr code
  qrcode.generateData(badge.qrLink);
  // Calculate position to place it on the right, vertically centered or at the bottom
  qrcode.setScale(3); // QR Size
  int qrSize = qrcode.getSideLength();
  int qrX = display.width() - qrSize; // Right-aligned with margin
  
  // Note: qrcode.draw uses graphic primitives, so it respects display rotation
  qrcode.draw(qrX, 0);
}

void refreshDisplay() {
  display.setRotation(3);
  
  // Paged loop mandatory for E-ink on ESP32 (RAM saving)
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE); // Clear buffer
    drawContent();
  } while (display.nextPage());
  
  display.hibernate(); // Turn off display controller to save power
  needRefresh = false;
  Serial.println("Display Aggiornato.");
}

// --- SETUP & LOOP ---

void setup() {
  Serial.begin(9600);
  delay(1000); 

  // Init Display
  display.init(115200, true, 50, false);
  u8g2.begin(display); // Connect u8g2 to GxEPD2

  // Draw for the first time with default data
  refreshDisplay();
}

void loop() {
  // Listen to serial always
  parseSerialData();

  // Update only if needed
  if (needRefresh) {
    refreshDisplay();
  }
  
  // Small delay to avoid unnecessary CPU saturation
  delay(100); 
}