#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define BTN 3
#define LDR A0

DHT dht(DHTPIN, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

enum State { TEMP, HUM, LIGHT };
State state = TEMP;

int lastRead = HIGH, stableBtn = HIGH;
unsigned long lastChange = 0, lastSample = 0, lastLog = 0;
float t = 0, h = 0;
int light = 0;
bool needDraw = true;

void draw() {
  lcd.clear();
  switch (state) {
    case TEMP:
      lcd.print("Temperature");
      lcd.setCursor(0, 1);
      lcd.print(t, 1); lcd.print(" C");
      break;
    case HUM:
      lcd.print("Humidity");
      lcd.setCursor(0, 1);
      lcd.print(h, 1); lcd.print(" %");
      break;
    case LIGHT:
      lcd.print("Light");
      lcd.setCursor(0, 1);
      lcd.print(light); lcd.print(" %");
      break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BTN, INPUT_PULLUP);
  dht.begin();
  lcd.init();
  lcd.backlight();
  Serial.println("ms,temp,hum,light");
}

void loop() {
  unsigned long now = millis();

  // Debounce
  int r = digitalRead(BTN);
  if (r != lastRead) { lastChange = now; lastRead = r; }
  if (now - lastChange > 50 && r != stableBtn) {
    stableBtn = r;
    if (stableBtn == LOW) {            // press detected
      state = (State)((state + 1) % 3);
      needDraw = true;
    }
  }

  // Sample sensors every 2 s
  if (now - lastSample >= 2000) {
    lastSample = now;
    t = dht.readTemperature();
    h = dht.readHumidity();
    light = map(analogRead(LDR), 0, 1023, 100, 0);
    needDraw = true;
  }

  // Log every 10 s
  if (now - lastLog >= 10000) {
    lastLog = now;
    Serial.print(now); Serial.print(',');
    Serial.print(t);   Serial.print(',');
    Serial.print(h);   Serial.print(',');
    Serial.println(light);
  }

  if (needDraw) { draw(); needDraw = false; }
}
