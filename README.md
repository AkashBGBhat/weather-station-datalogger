# weather-station-datalogger
Arduino Uno weather station with DHT22 and LDR sensors, a debounced button, a state-machine UI on an I2C LCD, and CSV data logging. Simulated in Wokwi.
https://wokwi.com/projects/478064888727475201

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

The code below is for the diagram.json of the Weather station 

    {
      "version": 1,
      "author": "Weather Station",
      "editor": "wokwi",
      "parts": [
        { "type": "wokwi-arduino-uno", "id": "uno", "top": 0, "left": 0, "attrs": {} },
        { "type": "wokwi-dht22", "id": "dht1", "top": -140, "left": 330, "attrs": { "temperature": "24", "humidity": "50" } },
        { "type": "wokwi-photoresistor-sensor", "id": "ldr1", "top": -140, "left": -200, "attrs": {} },
        { "type": "wokwi-lcd1602", "id": "lcd1", "top": -300, "left": 0, "attrs": { "pins": "i2c" } },
        { "type": "wokwi-pushbutton", "id": "btn1", "top": 60, "left": 320, "attrs": { "color": "green" } }
      ],
      "connections": [
        ["uno:5V", "dht1:VCC", "red", []],
        ["uno:GND.1", "dht1:GND", "black", []],
        ["uno:2", "dht1:SDA", "green", []],

    ["uno:5V", "ldr1:VCC", "red", []],
    ["uno:GND.1", "ldr1:GND", "black", []],
    ["uno:A0", "ldr1:AO", "orange", []],

    ["uno:5V", "lcd1:VCC", "red", []],
    ["uno:GND.1", "lcd1:GND", "black", []],
    ["uno:A4", "lcd1:SDA", "blue", []],
    ["uno:A5", "lcd1:SCL", "purple", []],

    ["uno:3", "btn1:1.l", "green", []],
    ["uno:GND.2", "btn1:2.l", "black", []]
      ],
      "dependencies": {}
    }
