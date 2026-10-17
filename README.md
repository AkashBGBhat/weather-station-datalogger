# 🌦️ Weather Station & Data Logger

An Arduino UNO weather station that reads temperature, humidity, and ambient
light, shows live readings on an I2C LCD, and streams CSV-formatted logs over
Serial — all without a single blocking `delay()`.

**▶️ Try it live on Wokwi (no hardware required):** https://wokwi.com/projects/478064888727475201

## Features

- **Three sensors:** DHT22 (temperature & humidity) + photoresistor (light level)
- **Push-button UI:** one debounced button cycles the LCD between three screens — Temperature → Humidity → Light
- **Non-blocking loop:** everything is scheduled with `millis()` timers, so the button stays responsive at all times
- **CSV data logging:** a full data row prints to Serial every 10 seconds, ready to copy into a spreadsheet

## How It Works

The `loop()` runs three independent timers concurrently:

| Task | Interval | What it does |
|---|---|---|
| Sensor sampling | every 2 s | Reads DHT22 temp/humidity and maps the LDR's ADC value to a 0–100% light level |
| CSV logging | every 10 s | Prints `ms,temp,hum,light` over Serial |
| LCD display | on change | Redraws only when there's new data or the user pressed the button |

A **debouncer** filters button noise: a state change must hold stable for
50 ms before it's accepted. A **state machine** (`enum State { TEMP, HUM, LIGHT }`)
tracks which screen is shown — each button press advances the state, wrapping
around with `% 3`.

The light reading uses `map(analogRead(LDR), 0, 1023, 100, 0)` — note the
reversed range, so a *higher* voltage means *more* light... inverted to read
intuitively as a brightness percentage.

## Hardware

| Part | Role |
|---|---|
| Arduino UNO | Microcontroller board |
| DHT22 | Temperature & humidity (digital, pin 2) |
| Photoresistor (LDR) module | Light level (analog, A0) |
| 16×2 I2C LCD | Live readings display |
| Push button | Screen selector (pin 3, INPUT_PULLUP) |

## Wiring

| Part | Arduino UNO Pin |
|---|---|
| DHT22 VCC / GND / SDA | 5V / GND / D2 |
| LDR VCC / GND / AO | 5V / GND / A0 |
| LCD VCC / GND / SDA / SCL | 5V / GND / A4 / A5 |
| Button pin 1 / pin 2 | D3 / GND |

The exact layout is defined in [`diagram.json`](diagram.json) — open the Wokwi
link above to see the wired circuit.

## Files

- `sketch.ino` — the Arduino program (sensors, debouncer, state machine, LCD, CSV logger)
- `diagram.json` — Wokwi circuit definition (board, parts, wiring)

## Sample Serial Output

```
ms,temp,hum,light
10023,24.30,50.10,58
20021,24.40,50.00,58
30018,25.10,49.80,44
```

Grab the Serial Monitor output, paste into a `.csv` file, and you have a
spreadsheet-ready weather log — the timestamp column is milliseconds since
boot (`millis()`).

## How to Run

**Option 1 — Wokwi (easiest):** click the link above, press ▶️ Play, and open
the Serial Monitor at 9600 baud. Click the green button to cycle LCD screens;
adjust the DHT22 sliders and shine more/less light on the LDR to see both the
LCD and CSV stream react live.

**Option 2 — Real hardware:** wire the circuit as above, flash the sketch with
the Arduino IDE, open Serial Monitor at 9600 baud.

## Concepts Demonstrated

- `millis()`-based non-blocking scheduling (vs. blocking `delay()`)
- Button debouncing with a state-change timer
- State machines (`enum` + modulo wrap-around)
- I2C LCD communication (LiquidCrystal_I2C)
- Analog-to-digital conversion and `map()` scaling
- Structured data logging (CSV format)
- Separating concerns: sampling, logging, and display as independent tasks

## Planned Improvements

- [ ] Save logs to SD card instead of Serial only
- [ ] Add RTC for human-readable timestamps
- [ ] Min/max/average summaries per hour
- [ ] Graph readings live in Serial Plotter
- [ ] ESP32 + Wi-Fi version posting to a web dashboard
- [ ] EEPROM storage of last readings across power cycles

## Built With

- Arduino UNO (C++ / Arduino framework)
- [Wokwi](https://wokwi.com) — browser-based simulation
- Adafruit DHT + LiquidCrystal_I2C libraries

## Author

**Akash Bhat** — [github.com/AkashBGBhat](https://github.com/AkashBGBhat)
