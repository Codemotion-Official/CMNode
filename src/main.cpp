#define ENABLE_GxEPD2_GFX 0
#define QRCODE_VERSION 3

// --- PIN DEFINITIONS ---
const int LED_PIN_DEBUG = 8;

const int DISPLAY_SCK_PIN = 4;
const int DISPLAY_MOSI_PIN = 6;

const int DISPLAY_DC_PIN = 1;
const int DISPLAY_CS_PIN = 7;
const int DISPLAY_BUSY_PIN = 3;
const int DISPLAY_RES_PIN = 2;

const int BUTTON_PIN = 5;

// --- INCLUDES ---
#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <QRCodeGFX.h>

#define SpaceOfLine  4

// --- DATA STRUCTURE ---
struct BadgeData {
  String name;
  String surname;
  String jobTitle;
  String company;
  String qrLink;
};

// --- FORWARD DECLARATIONS ---
void drawBadge(const BadgeData& data);
void drawQr(String link);
void setupBLE();

// --- GLOBAL OBJECTS ---
GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(/*CS=5*/ DISPLAY_CS_PIN, /*DC=*/ DISPLAY_DC_PIN, /*RST=*/ DISPLAY_RES_PIN, /*BUSY=*/ DISPLAY_BUSY_PIN));
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

QRCodeGFX qrcode(display);

BadgeData currentData; 
bool dataHasChanged = false;

void setup() {
  currentData.name = "Codemotion Conference";
  currentData.surname = "Milan 2025";
  currentData.jobTitle = "Ready to connect...";
  currentData.company = "Codemotion";
  currentData.qrLink = "https://codemotion.com";

  pinMode(LED_PIN_DEBUG, OUTPUT);
  digitalWrite(LED_PIN_DEBUG, LOW);

  Serial.begin(9600);
  
  // --- Display ---
  display.init(115200, true, 50, false);
  u8g2_for_adafruit_gfx.begin(display);
}

void drawBadge(const BadgeData& data)
{
  display.setRotation(3);

  u8g2_for_adafruit_gfx.setFont(u8g2_font_crox5tb_tf);
  uint16_t fontHeight = u8g2_for_adafruit_gfx.u8g2.font_info.max_char_height;

  uint16_t x = 5; 
  uint16_t y = fontHeight;

  display.setFullWindow();
  display.firstPage();
  do
  {
    u8g2_for_adafruit_gfx.setBackgroundColor(GxEPD_WHITE);
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_BLACK);

    u8g2_for_adafruit_gfx.setCursor(x, y);
    u8g2_for_adafruit_gfx.print(data.name);

    fontHeight = u8g2_for_adafruit_gfx.u8g2.font_info.max_char_height;
    y += fontHeight; 

    u8g2_for_adafruit_gfx.setCursor(x, y);  
    u8g2_for_adafruit_gfx.print(data.surname);

    fontHeight = u8g2_for_adafruit_gfx.u8g2.font_info.max_char_height;
    y += fontHeight + (4 * SpaceOfLine);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_luBIS10_tf);

    u8g2_for_adafruit_gfx.setCursor(x, y);
    u8g2_for_adafruit_gfx.print(data.jobTitle);

    fontHeight = u8g2_for_adafruit_gfx.u8g2.font_info.max_char_height;
    y += fontHeight + (2 * SpaceOfLine);
    u8g2_for_adafruit_gfx.setForegroundColor(GxEPD_RED);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB18_tf);

    u8g2_for_adafruit_gfx.setCursor(x, y);
    u8g2_for_adafruit_gfx.print(data.company);

    drawQr(data.qrLink);
  }
  while (display.nextPage());
}

void drawQr(String link)
{
    bool success = qrcode.generateData(link);

    if (success) {
      qrcode.setScale(3);

      int16_t sideLength = qrcode.getSideLength();

      int16_t x = (display.width() - sideLength);
      int16_t y = 0;

      qrcode.draw(x, y);
    }
}

void parseAndLoadBadgeData(String data) {
  Serial.println("Ricevuta nuova stringa: " + data);
  
  int lastIndex = 0;
  int currentIndex = data.indexOf(';');
  
  if (currentIndex == -1) {
    Serial.println("Errore: Formato dati non valido. Assicurati di usare ';' come separatore.");
    return;
  }

  // 1. Name
  currentData.name = data.substring(lastIndex, currentIndex);
  lastIndex = currentIndex + 1;
  
  // 2. Surname
  currentIndex = data.indexOf(';', lastIndex);
  if (currentIndex == -1) return;
  currentData.surname = data.substring(lastIndex, currentIndex);
  lastIndex = currentIndex + 1;
  
  // 3. Job Title
  currentIndex = data.indexOf(';', lastIndex);
  if (currentIndex == -1) return;
  currentData.jobTitle = data.substring(lastIndex, currentIndex);
  lastIndex = currentIndex + 1;
  
  // 4. Company
  currentIndex = data.indexOf(';', lastIndex);
  if (currentIndex == -1) return;
  currentData.company = data.substring(lastIndex, currentIndex);
  lastIndex = currentIndex + 1;
  
  // 5. Url QR Code
  currentData.qrLink = data.substring(lastIndex);

  Serial.println("--- Dati Badge Aggiornati ---");
  Serial.println("Nome: " + currentData.name);
  Serial.println("Cognome: " + currentData.surname);
  Serial.println("Posizione: " + currentData.jobTitle);
  Serial.println("Azienda: " + currentData.company);
  Serial.println("QR Link: " + currentData.qrLink);
  Serial.println("-----------------------------");

  dataHasChanged = true;
}

void checkSerialInput() {
  if (Serial.available() > 0) {
    String incomingData = Serial.readStringUntil('\n');
    incomingData.trim();
    
    if (incomingData.length() > 0) {
      parseAndLoadBadgeData(incomingData);
    }
  }
}

void loop() {
  checkSerialInput();

  if (dataHasChanged) {
    Serial.println("Dati cambiati, aggiorno il display...");

    dataHasChanged = false;
    drawBadge(currentData);
    display.hibernate();

    Serial.println("Display aggiornato.");
  }

  delay(1000); 
}