
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include <string.h>
#include "ReadWritelib.h"
BLEServer* pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool tmpConnected = false;
bool trueConnected = false;
BLEServer* pConnectedServer = NULL; 
std:: string receiveData;
std:: string receiveID = "";
uint32_t value = 0;
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define EEPROM_SIZE 100
const int TIME_TO_DISCONNECT =  2000;
String Default =  "00000000";
String ID = "12345678";
int button = 2;
bool isFirstDevice(){
  String tmp = readStringFromEEPROM(0,8);
  if(tmp == Default) return true;
  else return false;
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      tmpConnected = true; // khi này app vừa pair với thiết bị 
      }
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      receiveID = pCharacteristic->getValue();
      if(trueConnected == false){
          if (receiveID.length() > 0) {
            Serial.print("New value: ");
            for (int i = 0; i < receiveID.length(); i++){
              Serial.print(receiveID[i]);
              Serial.println();
             }
        }
      }else{
          if (receiveData.length() > 0) {
              Serial.print("New value: ");
              for (int i = 0; i < receiveData.length(); i++){
                Serial.print(receiveData[i]);
                Serial.println();
              }
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
  pinMode(button,INPUT_PULLDOWN);
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
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  writeStringToEEPROM(0,ID);
  Serial.print("EEPROM hiện tại là ");
  Serial.println(readStringFromEEPROM(0,8));
  delay(2000);
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}
void loop() {
  if(tmpConnected  && !trueConnected ){ // khi kết nối với app, mở ra 1 connect tạm thời để check ID
      if(isFirstDevice()){// tức là ID trong EEPROM là 00000000
              Serial.println("lần đầu pair");
              if(receiveID.length() > 0){
                writeStringToEEPROM(0,receiveID.c_str());
                EEPROM.commit();
                tmpConnected = false;
                trueConnected = true;
              } 
      }else{ // ID trong EEPRom khac 00000000 vi du 01234567
              Serial.println("Đã mất trinh ");
              delay(1000);
              if(receiveID.length() > 0){
                String tmp = receiveID.c_str();
                if( tmp == readStringFromEEPROM(0,8)){ // so sánh chuỗi nhận được với trong eeprom
                Serial.print(tmp);Serial.print("   ");
                Serial.println(readStringFromEEPROM(0,8));
                delay(1000);
                trueConnected = true;
                tmpConnected = false;
                Serial.println("cho phép pair");
                pCharacteristic->setValue("CONNECT OK");

                delay(1000);
                }else{
                  Serial.println("Không cho phép pair");
                  pCharacteristic->setValue("CONNECT FAIL");
                  delay(TIME_TO_DISCONNECT);
                  pConnectedServer->disconnect(pConnectedServer->getConnectedCount());
                  pConnectedServer = NULL;
                  // ngắt kết nối 
                  delay(1000);
                } 
              }
      } Serial.println("Đợi APP gửi ID");
  }else{ //  đã kết nối tới app, bắt đầu gửi bước chân 
    if(digitalRead(button) == 1){
      delay(50);
      while(digitalRead(button) == 1);
      value++;
      std::string V = intToString(value);
      pCharacteristic->setValue(V);
    }
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