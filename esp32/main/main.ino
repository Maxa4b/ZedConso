#include <Wire.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>

struct Releve {
  float shuntVoltage;
  float busVoltage;
  float current_mA;
  float power_mW;
  float loadVoltage;
  String formattedTime;
};

std::vector<Releve> donnees;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_INA219 ina219;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000);

const char* ssid = "DESKTOP-R574T7I 3179";
const char* password = "azertyui";
const char* mqtt_server = "192.168.137.1"; // IP du PC qui héberge le broker

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("✅ Connecté !");
    } else {
      Serial.print("❌ Erreur : ");
      Serial.print(client.state());
      Serial.println(" -> nouvelle tentative dans 1s");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 4, -1);
  pinMode(6, OUTPUT); 


  // OLED. 1.9 / 3.2 / 10mohm U = RI I = U / R
  Serial.println("Init écran...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("❌ Erreur OLED");
    while (true);
  }

  // INA219
  Serial.println("Init capteur...");
  if (!ina219.begin()) {
    Serial.println("❌ Erreur INA219");
    while (true);
  }

  // WiFi
  Serial.println("Connexion WiFi...");
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Échec WiFi !");
    while (true);
  }
  Serial.println("\n✅ WiFi connecté");

  timeClient.begin();
  timeClient.update();

  client.setServer(mqtt_server, 1883);
  while (!client.connected())
  {      Serial.println("Connecting to MQTT...");
        if (client.connect("ESP32Client"))
            Serial.println("connected");
        else
        {   Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
  }

  Serial.println("✅ Setup terminé !");
}

void loop() {
  client.loop();
  timeClient.update();

  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float loadvoltage = busvoltage + (shuntvoltage / 1000.0);

  donnees.push_back({shuntvoltage, busvoltage, current_mA, power_mW, loadvoltage, timeClient.getFormattedTime()});

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.print(current_mA >= 1000.0 ? current_mA / 1000.0 : current_mA, 3);
  display.println(current_mA >= 1000.0 ? " A" : " mA");
  display.display();

  // Envoi si bouton FPGA appuyé
  if (Serial1.available() && Serial1.read() == 'A') {
    Serial.println("📤 Bouton reçu : envoi des données MQTT...");

    for (const auto& r : donnees) {
      String message = r.formattedTime + "," +
                       String(r.shuntVoltage, 3) + "," +
                       String(r.busVoltage, 3) + "," +
                       String(r.current_mA, 3) + "," +
                       String(r.power_mW, 3) + "," +
                       String(r.loadVoltage, 3);
      client.publish("esp32/releves", message.c_str());
      delay(10);
    }

    client.publish("esp32/releves", "END");

    Serial.println("✅ Envoi terminé !");
    donnees.clear();
  }

  delay(200);
}
