#include <Arduino.h>
#include <string.h>
void writeStringToEEPROM(int addr, const String &str);
String readStringFromEEPROM(int addr, int length) ;
std::string intToString(int num);