
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include <string.h>
#include "C:\Users\admin\Documents\PlatformIO\Projects\BLE\lib\ReadWrite.h"
BLEServer* pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool testConnected = true;
std:: string receiveData;
uint32_t value = 0;
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define EEPROM_SIZE 100
String Default =  "00000000";

bool isEquals(std::string a, String b){
  int count = 0;
  Serial.println("bắt đầu so sánh 2 ID");
    for(int i = 0 ; i < 8; i++){
      if(a[i] == b[i]){
        count ++;
      }
    }
    if(count == 8) return true;
    else return false;
}
bool isFirstDevice(){
  // Serial.println("check first device");
  // Serial.print(Default); Serial.print("         ");
  String tmp = readStringFromEEPROM(0,8);
  // Serial.println(tmp);
  if(tmp == Default) return true;
  else return false;
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      testConnected = true; // khi này app vừa pair với thiết bị 
      }
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      receiveData = pCharacteristic->getValue();
      if (receiveData.length() > 0) {
        Serial.print("New value: ");
        for (int i = 0; i < receiveData.length(); i++){
            Serial.print(receiveData[i]);
            Serial.println();
        }
      }
      delay(1000);
    }
};
void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  for(int i = 0;i < 8; i++){
    EEPROM.write(i,0);
  }
  Serial.println("Starting BLE work!");

  BLEDevice::init("Long name works now");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  writeStringToEEPROM(0,Default);
  Serial.print("EEPROM hiện tại là ");
  Serial.println(readStringFromEEPROM(0,8));
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}
void loop() {
  if(testConnected){ // khi kết nối với app, mở ra 1 connect tạm thời để check ID
      if(isFirstDevice()){
              // EEPROM.writeString(0,receiveData.c_str());// ghi ID vào eeprom
              Serial.println("lần đầu pair");
              if(receiveData.length() > 0){
                writeStringToEEPROM(0,receiveData.c_str());
                testConnected = false;
              } 
      }else Serial.println("Đợi APP gửi ID");
  }else{
    Serial.println("Eeprom = " + readStringFromEEPROM(0,8));
  }

      
      // if(receiveData.length() > 0){ // nhận được ID từ app
      //     else if(isEquals(receiveData,EEPROM.readString(0))){ // nếu ID nhận được = ID trong EEPROOM
      //         Serial.println("ID đã nhận bằng ID trong eeprom, cho phép kết nối");
      //     }else Serial.println("ID không trùng khớp ");
      // }else Serial.println("đợi APP gửi ");
  // }

  // put your main code here, to run repeatedly:
  // if (deviceConnected && !isFirstDevice()) { // kết nối và kiểm tra ID gửi về từ APP
  //       Serial.println(EEPROM.readString(0));
  //       delay(1000);
  // }
  
    //pCharacteristic->setValue((uint8_t*)&value, 4);
        // pCharacteristic->notify();
        // value++;
        // delay(1000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
}