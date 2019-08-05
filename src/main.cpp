#include <Arduino.h>
#include <vector>
#include <FS.h>


struct Sample
{
  int value;
  int timestamp;
};

struct Sensor
{
  char name[30];
  char family[16];
  int type;
  std::vector<Sample> samples;

  void load(File& file);
  void save(File& file) const;
};


struct Config {
  std::vector<Sensor> sensors;
  
  
  void load(File& file);
  void save(File& file) const;
};


void Config::save(File& file) const 
{
  auto n_sensors = sensors.size();
  file.write((uint8_t *)&n_sensors, sizeof(n_sensors));
  for (const auto& sensor: sensors) {
    sensor.save(file);
  }
}

void Config::load(File& file) 
{
  std::size_t n_sensors;
  file.read((uint8_t *)&n_sensors, sizeof(n_sensors));
  
  sensors.clear();
  sensors.resize(n_sensors);
  for (auto& sensor: sensors) {    
    sensor.load(file);
  }
}


void Sensor::save(File& file) const 
{
  file.write((uint8_t*)name, sizeof(name));
  file.write((uint8_t*)family, sizeof(family));
  file.write((uint8_t*)&type, sizeof(type));
  
  auto n_samples = samples.size();
  file.write((uint8_t *)&n_samples, sizeof(n_samples));
  for (const auto& sample: samples) {
    file.write((uint8_t*)&sample, sizeof(sample));
  }
}

void Sensor::load(File& file) 
{  
  file.read((uint8_t*)name, sizeof(name));
  file.read((uint8_t*)family, sizeof(family));
  file.read((uint8_t*)&type, sizeof(type));

  std::size_t n_samples;
  file.read((uint8_t *)&n_samples, sizeof(n_samples));

  samples.clear();
  samples.resize(n_samples);
  for (auto& sample: samples) {    
    file.read((uint8_t*)&sample, sizeof(sample));
  }
}

auto constexpr kConfigFile = "config5.bin";

void writeSampleConfig()
{
  if (!SPIFFS.begin()) return;
  File file = SPIFFS.open(kConfigFile, "w");
  Config c;
  c.sensors.resize(2);
  strcpy(c.sensors[0].name, "Temp quarto 1");
  strcpy(c.sensors[0].family, "tempereratura");
  
  c.sensors[0].samples.push_back({10, 111111});
  c.sensors[0].samples.push_back({11, 222222});

  strcpy(c.sensors[1].name, "Humidade quarto 1");
  strcpy(c.sensors[1].family, "humidade");

  c.save(file);
  file.close();
  Serial.println("CREATED");
  SPIFFS.end();
}


void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin()) return;

  // if (!SPIFFS.exists(kConfigFile)) {
    // Serial.println("Creating configuration file");
    // SPIFFS.end();
    // writeSampleConfig();
    // return;
  // }
  Serial.println("READING");
  File file = SPIFFS.open(kConfigFile, "r");
  Config c;
  c.load(file);
  file.close();

  Serial.println(c.sensors.size());
  Serial.println(c.sensors.back().name);  
  Serial.println(c.sensors[0].samples.size());
  Serial.println(c.sensors[0].samples[0].value);
  Serial.println(c.sensors[0].samples[0].timestamp);
  
  SPIFFS.end();
}

void loop() {
  // put your main code here, to run repeatedly:
}