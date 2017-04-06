#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "Config.h"

void Config::mount()
{
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount file system");
  }
}

void Config::read(const char* fileName) {
  strcpy(apiEndpoint, "http://app.smogly.pl/api/v1/metering/");
  strcpy(token, "9539f7d1");

  if (!SPIFFS.exists(fileName)) {
    Serial.println("Cannot find config file");
    return;
  }

  File configFile = SPIFFS.open(fileName, "r");
  if (!configFile) {
    Serial.println("Cannot open config file");
    return;
  }

  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  configFile.close();
  if (!json.success()) {
    Serial.println("Failed to load json config");
    return;
  }

  strncpy(apiEndpoint, json["apiEndpoint"], PARAM_LEN);
  strncpy(token, json["token"], PARAM_LEN);
}

void Config::save(const char* fileName)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["apiEndpoint"] = apiEndpoint;
  json["token"] = token;
  File configFile = SPIFFS.open(fileName, "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }
  json.printTo(configFile);
  configFile.close();
  SPIFFS.end();
}

void Config::reset()
{
  Serial.println("*DEBUG MODE ON: formatting SPIFFS");
  SPIFFS.format();
}
