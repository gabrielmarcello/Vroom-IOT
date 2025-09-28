#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// WiFi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

// MQTT
const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "esp32_mqtt";
const char *TOPIC_PUBLISH_STATUS = "fiap/iot/led/status";

// ID do dispositivo
const int id_tag = 1;

// Pinos
const int pinRed = 21;
const int pinGreen = 22;
const int pinBlue = 23;
const int buttonPin = 26;
const int buzzerPin = 13;

// Estados
int currentColor = 0;
bool ledOn = false;

// Controle botão
unsigned long buttonPressTime = 0;
bool buttonPressed = false;
bool longPressHandled = false;
const unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;

// Controle buzzer
unsigned long lastBuzzTime = 0;
const unsigned long buzzInterval = 1000;
const unsigned long buzzDuration = 250;
bool buzzerOn = false;
unsigned long buzzerStartTime = 0;

// Estados do LED e mensagem
struct LedStatus {
  uint8_t r, g, b;
  const char *message;
};

LedStatus statuses[] = {
  {255, 0, 0,     "Problema Mecânico"},
  {0, 255, 0,     "Documentação Pendente"},
  {0, 0, 255,     "Problema Elétrico"},
  {255, 255, 0,   "Problema Estético"},
  {255, 50, 0,    "Problema Segurança"},
  {150, 0, 0,     "Problemas Múltiplos"},
  {130, 0, 255,   "Motocicleta Pronta para Uso"}
};
const int totalStatuses = sizeof(statuses) / sizeof(statuses[0]);

WiFiClient espClient;
PubSubClient client(espClient);

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(pinRed, r);
  analogWrite(pinGreen, g);
  analogWrite(pinBlue, b);
}

void startBuzz() {
  tone(buzzerPin, 2500);
  buzzerOn = true;
  buzzerStartTime = millis();
}

void stopBuzz() {
  noTone(buzzerPin);
  buzzerOn = false;
}

void publishStatus() {
  StaticJsonDocument<200> doc;
  doc["id_tag"] = id_tag;
  doc["led_on"] = ledOn;

  if (ledOn) {
    doc["problema"] = statuses[currentColor].message;
    doc["color"] = currentColor;
  } else {
    doc["problema"] = "Desligado";
    doc["color"] = -1;
  }

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(TOPIC_PUBLISH_STATUS, buffer);
  Serial.print("Enviado MQTT: ");
  Serial.println(buffer); // LOG do envio
}

void updateLed() {
  if (!ledOn) {
    setColor(0, 0, 0);
    stopBuzz();
    publishStatus();
    return;
  }
  LedStatus current = statuses[currentColor];
  setColor(current.r, current.g, current.b);
  publishStatus();
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Tentando conectar MQTT...");
    if (client.connect(ID_MQTT)) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(", tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Conectando WiFi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado! IP: " + WiFi.localIP().toString());

  client.setServer(BROKER_MQTT, BROKER_PORT);

  updateLed();
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  int buttonState = digitalRead(buttonPin);

  // Debounce
  if (buttonState == LOW && !buttonPressed) {
    if (millis() - lastDebounceTime > debounceDelay) {
      buttonPressed = true;
      buttonPressTime = millis();
      longPressHandled = false;
      lastDebounceTime = millis();
    }
  }

  // Long press alterna LED
  if (buttonPressed && buttonState == LOW) {
    if (!longPressHandled && millis() - buttonPressTime > 1000) {
      ledOn = !ledOn;
      Serial.println("Long Press: LED alternado para " + String(ledOn ? "ON" : "OFF"));
      updateLed();
      longPressHandled = true;
    }
  }

  // Short press muda cor
  if (buttonState == HIGH && buttonPressed) {
    if (millis() - lastDebounceTime > debounceDelay) {
      buttonPressed = false;
      lastDebounceTime = millis();
      if (!longPressHandled) {
        if (!ledOn) ledOn = true;  // liga LED se estiver desligado
        else currentColor = (currentColor + 1) % totalStatuses; // muda cor
        Serial.println("Short Press: LED atualizado, cor atual = " + String(currentColor));
        updateLed();
      }
    }
  }

  // Controle buzzer
  unsigned long now = millis();
  if (ledOn) {
    if (!buzzerOn && (now - lastBuzzTime >= buzzInterval)) {
      startBuzz();
      lastBuzzTime = now;
    }
    if (buzzerOn && (now - buzzerStartTime >= buzzDuration)) stopBuzz();
  } else stopBuzz();
}
