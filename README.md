# TUMO

Fab Lab Barcelona for the government of Piauí. Summer immersion program on
making, electronics and prototyping.

This repo contains code examples to be run on the [**Grove Beginner Kit for
Arduino**](https://wiki.seeedstudio.com/Grove-Beginner-Kit-For-Arduino/). The two projects — a sea **buoy** and an automated **planter** — are
built almost entirely from the sensors and actuators already on the board, plus
two external modules for the planter.


## Hardware

The Grove Beginner Kit is a single board with a Seeeduino Lotus (Arduino
UNO / ATmega328P compatible) and 10 modules wired to fixed pins. Modules can be
snapped off and reconnected with Grove cables, and every module also has a Grove
socket for attaching external sensors.

| Module | Pin | Type |
| --- | --- | --- |
| LED (red) | D4 | digital out |
| Buzzer | D5 | digital out |
| OLED display 0.96" (SSD1315) | I2C `0x3C` | out |
| Button | D6 | digital in |
| Rotary potentiometer | A0 | analog in |
| Light sensor | A6 | analog in |
| Sound sensor (mic) | A2 | analog in |
| 3-axis accelerometer (LIS3DHTR) | I2C `0x19` | in |
| Temp + humidity (DHT11) | D3 | digital in |
| Barometer (SPA06-003) | I2C `0x77` | in |

### External modules

- **buoy** — none required.
- **planter** — soil-moisture sensor (analog, e.g. A1) and a relay
  module (digital, e.g. D2) driving the pump or valve.
  - [Soil Sensor](https://www.amazon.es/ARCELI-higr%C3%B3metro-detecci%C3%B3n-Humedad-Arduino/dp/B07CQT5RC8)
  - [Grove Relay Module](https://wiki.seeedstudio.com/Grove-Relay/)

## Repo layout

```
buoy/buoy.ino       sketch for the buoy
planter/planter.ino sketch for the planter
```

---

## buoy.ino

Code for a buoy recording sea behaviour: temperature, wave motion, daylight and
pressure.

- input: accelerometer (wave motion)
- input: barometer (pressure + temperature)
- input: light sensor (day / night, turbidity)
- output: red LED (status / alarm)

### Options

Pick one as a starting point; each builds on the previous.

1. **Serial logger (simplest).** Every second, read light, pressure,
   temperature and the acceleration magnitude and print one CSV line to the
   Serial Monitor. Blink the LED once per reading so you can see it is alive.
   Students capture the data by copy-pasting the Serial Monitor, or by saving
   it with the IDE's Serial Plotter / a `screen` session.
2. **Wave counter.** Compute `a = sqrt(x² + y² + z²) - 1g`, high-pass it
   (subtract a slow running average), and count zero-crossings or peaks over a
   30–60 s window to estimate waves per minute and rough wave height. Flash the
   LED on each detected wave.



---

## planter.ino

Code for an automated planter that measures soil humidity and controls watering
through a relay.

- input: soil moisture — resistive sensor (external, analog)
- input: air temperature + relative humidity — DHT11 (on kit)
- output: OLED display (on kit)
- output: relay driving pump / valve (external, digital)

### Options

1. **Threshold watering (simplest).** Read the soil sensor, map it to 0–100 %,
   show it on the OLED, and switch the relay ON while moisture is below a fixed
   percentage, OFF above it. One `if`. Print the same values over Serial.
2. **Hysteresis + pulse watering.** Use two thresholds (e.g. water below 30 %,
   stop above 45 %) so the relay does not chatter around a single value. When
   watering, run the pump in short pulses (e.g. 5 s on / 30 s off) to let water
   soak in before re-measuring — avoids over-watering a slow-draining pot.

### Calibration

The resistive probe needs a two-point calibration before the % mapping means
anything: note the raw analog value in dry air and again with the probe in a
glass of water, then `map()` between those two. Do this once per sensor. A
resistive probe reads **high in dry soil, low in wet soil**.

### Safety

Keep the relay, pump wiring and mains/12 V supply away from water and from the
board. The relay module isolates the Arduino side; still, never run the pump dry
for long.
