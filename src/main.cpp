#include <Motor.h>
#include <Uart.h>
#include <QTRSensors.h>

#define NUM_SENSORS 5            // Số lượng cảm biến line
#define NUM_SAMPLES_PER_SENSOR 5 // average 5 analog samples per sensor reading
#define EMITTER_PIN QTRNoEmitterPin

QTRSensors qtra;
uint16_t sensorValues[NUM_SENSORS];
uint16_t meanV = 500;
uint16_t position;
// Motor Driver Properties
uint8_t max_speed = 90;
uint8_t base_speed = 90;
uint8_t turn_speed = 85;
uint8_t min_speed = 0;
// PID Properties
const double KP = 0.1425;
const double KI = 0.0001015;
const double KD = 3.8355;
int error;
double lastError = 0;
double sumError = 0;
const int GOAL = 2000;
//=================================
const int trig = 11; // chân trig của HC-SR04
const int echo = 10; // chân echo của HC-SR04

// // chân cảm biến hồng ngoại
// const int ir_left = 12;
// const int ir_right = 13;

uint16_t distance;    // biến lưu khoảng cách
uint8_t speed1 = 125; // tốc độ động cơ (đi thẳng)
uint8_t speed2 = 125; // độ nhạy rẽ trái, phải
uint8_t speed3 = (uint8_t)(speed1 - (float)speed2 / 255 * speed1);

bool active = false; // trạng thái motor
// Mode = 0 : Chế độ điều khiển
// Mode = 1 : Chế độ auto
int8_t Mode = 0;

void Tinh_khoangcach();
void calibrateLineSensor();
void Mode_control();
void Mode_follow_line();

void setup()
{
    Serial.begin(9600);

    qtra.setTypeAnalog();
    qtra.setSamplesPerSensor(NUM_SAMPLES_PER_SENSOR);
    qtra.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4}, NUM_SENSORS);
    delay(3000);
    calibrateLineSensor();

    // pinMode(ir_left, INPUT_PULLUP);
    // pinMode(ir_right, INPUT_PULLUP);
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void loop()
{
    receive_uart();
    // if (data.length() == 0)
    // {
    //     active = false;
    // }
    if (data.length() > 2)
    {
        // tách chuỗi data nhận được ("type,value")
        String type = data.substring(0, data.indexOf(','));
        uint8_t value = data.substring(data.indexOf(',') + 1).toInt();
        if (type.equals("speed1"))
        {
            speed1 = constrain(value, 0, 255);
            speed3 = (uint8_t)(speed1 - (float)speed2 / 255 * speed1);
        }
        else if (type.equals("speed2"))
        {
            speed2 = constrain(value, 0, 255);
            speed3 = (uint8_t)(speed1 - (float)speed2 / 255 * speed1);
        }
        else if (type.equals("mode"))
        {
            if (value == 0 || value == 1)
            {
                Mode = value;
            }
        }
        else if (type.equals("inf"))
        {
            String info =(String) speed1 +","+ speed2 + "," + KP + "," + KI + "," + KD + "," + max_speed + "," + min_speed + "," + base_speed + "," + turn_speed;
            Serial.println(info);
        }
    }

    if (Mode == 0)
    {
        Mode_control();
    }
    else if (Mode == 1)
    {
        Mode_follow_line();
    }

    data = ""; // reset lại chuỗi lưu trữ dữ liệu
}
// Chế độ điều khiển
void Mode_control()
{
    if (data.equals("F")) // Tiến thẳng phía trước
    {
        Motor1_Tien(speed1);
        Motor2_Tien(speed1);
    }
    else if (data.equals("FR")) // Tiến sang phải
    {
        Motor1_Tien(speed1);
        Motor2_Tien(speed3);
    }
    else if (data.equals("FL")) // Tiến sang trái
    {
        Motor1_Tien(speed3);
        Motor2_Tien(speed1);
    }
    else if (data.equals("B")) // Lùi thẳng
    {                          // lui
        Motor1_Lui(speed1);
        Motor2_Lui(speed1);
    }
    else if (data.equals("BR")) // Lùi sang phải
    {
        Motor1_Lui(speed1);
        Motor2_Lui(speed3);
    }
    else if (data.equals("BL")) // Lùi sang trái
    {
        Motor1_Lui(speed3);
        Motor2_Lui(speed1);
    }
    else if (data.equals("L")) // Vòng trái
    {
        Motor2_Tien(speed2);
        // Motor2_Tien(255 - speed2);
    }
    else if (data.equals("R")) // Vòng phải
    {
        // Motor1_Tien(255 - speed2);
        Motor1_Tien(speed2);
    }

    delay(59);
    Motor1_Dung();
    Motor2_Dung();
}

// Chế độ dò line
void Mode_follow_line()
{
    // Get line position
    position = qtra.readLineBlack(sensorValues);
    // min = qtra.calibrationOn.minimum[2];
    // max = qtra.calibrationOn.maximum[2];
    // Serial.println(position);
    // delay(500);
    if (position == 500)
    { // phải
        Motor1_Tien(turn_speed);
        Motor2_Tien(min_speed);
        // delay(100);
        return;
    }
    else if (position == 3500)
    { // trái
        Motor1_Tien(min_speed);
        Motor2_Tien(turn_speed);
        // delay(100);
        return;
    }
    if (sensorValues[0] > 50 && sensorValues[1] > 50 && sensorValues[2] > 50 && sensorValues[3] > 50 && sensorValues[4] > 50)
    {
        error = lastError;
    }
    else
    {
        error = GOAL - position;
    }

    // Compute motor adjustment
    int adjustment = KP * error + KD * (error - lastError) + KI * (sumError);

    // Store error for next increment
    lastError = error;
    sumError += error;
    // Adjust motors
    Motor1_Tien(constrain(base_speed + adjustment, min_speed, max_speed));
    Motor2_Tien(constrain(base_speed - adjustment, min_speed, max_speed));
}
// Tính toán cảm biến Line
void calibrateLineSensor()
{
    digitalWrite(13, HIGH); // turn on Arduino's LED to indicate we are in calibration mode

    // analogRead() takes about 0.1 ms on an AVR.
    // 0.1 ms per sensor * 5 samples per sensor read * 5 sensors
    // * 10 reads per calibrate() call = ~25 ms per calibrate() call.
    // Call calibrate() 100 times to make calibration take about 2 seconds.
    for (int i = 0; i < 100; i++) // make the calibration take about 5 seconds
    {
        if (i < 50)
        {
            Motor1_Tien(55);
            Motor2_Lui(55);
        }
        else if (i == 50)
        {
            Motor1_Tien(0);
            Motor2_Lui(0);
            delay(1000);
        }
        else
        {
            Motor1_Lui(55);
            Motor2_Tien(55);
        }
        qtra.calibrate(); // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
        delay(20);
    }
    Motor1_Tien(0);
    Motor2_Tien(0);
    delay(500);
    digitalWrite(13, LOW); // turn off Arduino's LED to indicate we are through with calibration
}

// Tính khoảng cách vật cản (Cảm biến siêu âm)
void Tinh_khoangcach()
{
    unsigned long duration; // biến đo thời gian
    digitalWrite(trig, 0);
    delayMicroseconds(2);
    digitalWrite(trig, 1);
    delayMicroseconds(5); // xung có độ dài 5 microSeconds
    digitalWrite(trig, 0);

    /* Tính toán thời gian */
    duration = pulseIn(echo, HIGH); // Đo độ rộng xung HIGH ở chân echo.

    if (duration > 23500) // Ngoại lệ
    {
        return;
    }

    distance = int(duration / 2 / 29.412); // Tính khoảng cách đến vật.
    delayMicroseconds(45);
    // Serial.print(distance);
}