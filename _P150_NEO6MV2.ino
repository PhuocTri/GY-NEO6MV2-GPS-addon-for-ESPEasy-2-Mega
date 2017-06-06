/*
  Created by Strontvlieg
  This plugin reads the LON, LAT and Speed value from the GY-NEO6MV2 Sensor
*/

#define PLUGIN_150
#define PLUGIN_ID_150         150
#define PLUGIN_NAME_150       "GPS Module GY-NEO6MV2 [TESTING]"
#define PLUGIN_VALUENAME1_150 "Lon"
#define PLUGIN_VALUENAME2_150 "Lat"
#define PLUGIN_VALUENAME3_150 "Speed"
#define PLUGIN_READ_TIMEOUT   3000

boolean Plugin_150_init = false;

#include <SoftwareSerial.h>
SoftwareSerial *Plugin_150_SoftSerial;

#include <TinyGPS++.h>
TinyGPSPlus GPS;

boolean Plugin_150(byte function, struct EventStruct *event, String& string) {

  bool success = false;

  switch (function) {
    case PLUGIN_DEVICE_ADD: {
        Device[++deviceCount].Number = PLUGIN_ID_150;
        Device[deviceCount].Type = DEVICE_TYPE_DUAL;
        Device[deviceCount].VType = SENSOR_TYPE_TRIPLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 3;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }


    case PLUGIN_GET_DEVICENAME: {
        string = F(PLUGIN_NAME_150);
        break;
      }


    case PLUGIN_GET_DEVICEVALUENAMES: {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_150));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_150));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_150));
        break;
      }


    case PLUGIN_INIT: {
        Plugin_150_SoftSerial = new SoftwareSerial(Settings.TaskDevicePin1[event->TaskIndex], Settings.TaskDevicePin2[event->TaskIndex]);
        Plugin_150_SoftSerial->begin(9600);
        addLog(LOG_LEVEL_INFO, F("NEO6: Init OK "));

        timerSensor[event->TaskIndex] = millis() + 15000;

        Plugin_150_init = true;
        success = true;
        break;
      }


    case PLUGIN_WRITE: {
        ;
      }


    case PLUGIN_READ: {
        if (Plugin_150_init) {
          long start = millis();

          while (((millis() - start) < PLUGIN_READ_TIMEOUT)) {
            if (Plugin_150_SoftSerial->available() > 0) {
              
              int rawData = Plugin_150_SoftSerial->read();

              if (GPS.encode(rawData)) {
                float Lon = 0.0;
                float Lat = 0.0;
                float Speed = 0.0;

                Lon = GPS.location.lng();
                Lat = GPS.location.lat();
                Speed = GPS.speed.kmph();
                int Fix = GPS.location.age();

                success = true;

                UserVar[event->BaseVarIndex] = Lon;
                UserVar[event->BaseVarIndex + 1] = Lat;
                UserVar[event->BaseVarIndex + 2] = Speed;

                String log = F("NEO6: ");
                log += F("lon:");
                log += Lon;
                log += F(" lat:");
                log += Lat;
                log += F(" km/h:");
                log += Speed;
                log += F(" fix:");
                log += Fix;
                addLog(LOG_LEVEL_INFO, log);
                break;

              }
            } else {
              delay(10);
            }
          }
        }
        break;
      }
  }
  return success;
}
