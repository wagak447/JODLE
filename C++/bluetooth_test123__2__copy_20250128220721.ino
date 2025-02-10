#include <SoftwareSerial.h>
SoftwareSerial bt_serial(11, 12);
int info;
bool get_info() {
  if (bt_serial.available()) {
    info = bt_serial.parseInt();
    return true;
  }
  else {
    return false;
  }
}
void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  bt_serial.begin(9600);
  Serial.begin(9600);
  //digitalWrite(13, 1);
  info = 0;
}

void loop() {
   
 
  // Ожидаем и выводим ответ от Bluetooth модуля.
  if (bt_serial.available()) {
    info = bt_serial.parseInt();  // Читаем символ из Bluetooth модуля.
    Serial.print(info);  // Выводим символ в монитор порта.
  }
}
