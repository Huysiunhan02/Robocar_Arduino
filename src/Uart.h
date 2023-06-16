#include <Arduino.h>

#define MAX_BUFFER_SIZE 128

String data = "";        // chuỗi lưu trữ dữ liệu đọc được từ UART
boolean stringComplete = false; // biến cờ hiệu cho biết đã đọc đủ dữ liệu chưa
// char test[5];
// Đọc dữ liệu từ cổng uart
// Đọc dữ liệu nhận được (byte) và chuyển thành một chuỗi String
// - Tốc độ cao hơn so với dùng Serial.readString();
void receive_uart() 
{
    static char buffer[MAX_BUFFER_SIZE]; // mảng đệm để lưu trữ dữ liệu đọc được từ UART
    static int bufferIndex = 0;          // vị trí hiện tại trong mảng đệm
    int uartData;                        // biến lưu trữ dữ liệu đọc được từ UART
    // Serial.readBytesUntil('\n',test,5);
    while (Serial.available() > 0 && stringComplete == false)
    {                             // kiểm tra có dữ liệu mới từ UART và chưa đọc đủ dữ liệu chưa
        uartData = Serial.read(); // đọc một byte từ UART
        if (uartData == '#')
        {                                 // nếu đọc được ký tự '#' thì kết thúc chuỗi
            buffer[bufferIndex] = '\0';   // thêm ký tự kết thúc chuỗi
            data = String(buffer); // gán chuỗi đọc được vào biến data
            bufferIndex = 0;              // reset lại vị trí trong mảng đệm
            stringComplete = true;        // đặt cờ hiệu đã đọc đủ dữ liệu
        }
        else if (bufferIndex < MAX_BUFFER_SIZE - 1)
        { // nếu chưa đọc đủ một chuỗi thì lưu vào mảng đệm
            buffer[bufferIndex++] = (char)uartData;
        }
    }

    if (stringComplete == true)
    {                                // nếu đã đọc đủ dữ liệu
        Serial.println(data); // in dữ liệu đọc được ra Serial Monitor để kiểm tra
        stringComplete = false;      // reset lại cờ hiệu đã đọc đủ dữ liệu
    }
}