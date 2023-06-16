# Arduino program to control Robocar by Bluetooth

This program makes the robocar operate in two modes:
> Control mode: move up, down, left, right.

> Follow line mode: My robot use 5 sensor.

Code built on PlatformIO.

## Hardware devices
- Arduino Uno R3
- Module L298 control motor
- QTR Sensor (*5)
- HCSR04-Distance
- Module Bluetooth HC06

## Instruct
### Library
Downd this library: 
[QTRSensor](https://github.com/pololu/qtr-sensors-arduino.git)

### Setup properties

Number of sensors

```c
#define NUM_SENSORS 5 
#define NUM_SAMPLES_PER_SENSOR 5
```

PID Properties

```c
// Motor Driver Properties
uint8_t max_speed = 90;
uint8_t base_speed = 90;
uint8_t turn_speed = 85;
uint8_t min_speed = 0;
// PID Properties
const double KP = 0.1425;
const double KI = 0.0001015;
const double KD = 3.8355;

const int GOAL = 2000;
```

#### Note: 
- Each speed will need to set different PID parameters. You need to change the PID parameter to suit your robot.
- "GOAL" will also depend on the number of QTR sensors.
