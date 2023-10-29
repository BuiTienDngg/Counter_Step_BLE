#include<Arduino.h>
#include<ReadWrite.h>
void writeStringToEEPROM(int addr, const String &str) {
  int length = str.length();
  EEPROM.write(addr, length);
  for (int i = 0; i < length; i++) {
    EEPROM.write(addr  + i, str[i]);
  }
  EEPROM.commit();
}
String readStringFromEEPROM(int addr, int length) {
  String str = "";
  for (int i = 0; i < length; i++) {
    str += char(EEPROM.read(addr + i ));
  }
  return str;
}