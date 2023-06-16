#include <Arduino.h>

// chân điều khiển l298n
#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4
#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0
//Motor phải quay ngược
//int speed : pwm điều khiển tốc độ quay
void Motor1_Lui(int speed)
{
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
	digitalWrite(IN1, HIGH);// chân này không có PWM
	analogWrite(IN2, 255 - speed);
}
//Motor phải quay thuận
//int speed : pwm điều khiển tốc độ quay
void Motor1_Tien(int speed)
{
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
	digitalWrite(IN1, LOW);// chân này không có PWM
	analogWrite(IN2, speed);
}
//Motor trái quay ngược
//int speed : pwm điều khiển tốc độ quay
void Motor2_Lui(int speed)
{
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
	analogWrite(IN3, speed);
	digitalWrite(IN4, LOW);// chân này không có PWM
}

//Motor trái quay thuận
//int speed : pwm điều khiển tốc độ quay
void Motor2_Tien(int speed)
{
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED
	analogWrite(IN3, 255 - speed);
	digitalWrite(IN4, HIGH);// chân này không có PWM
}

//Dừng Motor phải
void Motor1_Dung()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}

//Dừng motor trái
void Motor2_Dung()
{
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}