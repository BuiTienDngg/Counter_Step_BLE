#include<Arduino.h>
#include<ReadWritelib.h>
#include<EEPROM.h>
void writeStringToEEPROM(int addr, const String &str) {
  int length = str.length();
  EEPROM.write(addr, length);
  for (int i = 0; i < length; i++) {
    EEPROM.write(addr  + i, str[i]);
  }
  delay(200);
  EEPROM.commit();
}
String readStringFromEEPROM(int addr, int length) {
  String str = "";
  for (int i = 0; i < length; i++) {
    str += char(EEPROM.read(addr + i ));
  }
  return str;
}
std::string intToString(int num) {
    std::string result_String;
    bool isNegative = false;

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    do {
        result_String = char(num % 10 + '0') + result_String;
        num /= 10;
    } while (num > 0);

    if (isNegative) {
        result_String = "-" + result_String;
    }

    return result_String;
}