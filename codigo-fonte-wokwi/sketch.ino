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
const char *TOPIC_SUBSCRIBE_LED = "fiap/iot/led/state";
const char *TOPIC_PUBLISH_STATUS = "fiap/iot/led/status";

WiFiClient espClient;
PubSubClient client(espClient);

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

// Para enviar status só uma vez
bool lastLedOn = false;
int lastColor = -1;

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
  if (ledOn != lastLedOn || currentColor != lastColor) {
    String msg;
    if (ledOn) {
      msg = statuses[currentColor].message;
    } else {
      msg = "Desligado";
    }
    client.publish(TOPIC_PUBLISH_STATUS, msg.c_str());
    Serial.println("Status publicado: " + msg);

    lastLedOn = ledOn;
    lastColor = currentColor;
  }
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

// Callback MQTT para receber JSON { power, color }
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.println("Erro ao parsear JSON MQTT");
    return;
  }

  const char* power = doc["power"];  // "on" ou "off"
  int color = doc["color"] | -1;     // pode ser -1 se não enviar

  bool newLedOn = (String(power) == "ON");

  bool changed = false;
  if (ledOn != newLedOn) {
    ledOn = newLedOn;
    changed = true;
  }

  if (color >= 0 && color < totalStatuses && currentColor != color) {
    currentColor = color;
    changed = true;
  }

  if (changed) {
    updateLed();
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT... ");
    if (client.connect(ID_MQTT)) {
      Serial.println("conectado!");
      client.subscribe(TOPIC_SUBSCRIBE_LED);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando em 5s");
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

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  client.setServer(BROKER_MQTT, BROKER_PORT);
  client.setCallback(mqttCallback);

  updateLed();
  lastBuzzTime = millis();
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Leitura do botão com debounce e controle do LED local
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && !buttonPressed) {
    if (millis() - lastDebounceTime > debounceDelay) {
      buttonPressed = true;
      buttonPressTime = millis();
      longPressHandled = false;
      lastDebounceTime = millis();
    }
  }

  if (buttonPressed && buttonState == LOW) {
    if (!longPressHandled && millis() - buttonPressTime > 1000) {
      ledOn = !ledOn;
      updateLed();
      longPressHandled = true;
    }
  }

  if (buttonState == HIGH && buttonPressed) {
    if (millis() - lastDebounceTime > debounceDelay) {
      buttonPressed = false;
      lastDebounceTime = millis();
      if (!longPressHandled && ledOn) {
        currentColor = (currentColor + 1) % totalStatuses;
        updateLed();
      }
    }
  }

  // Controle do buzzer sem delay
  unsigned long now = millis();
  if (ledOn) {
    if (!buzzerOn && (now - lastBuzzTime >= buzzInterval)) {
      startBuzz();
      lastBuzzTime = now;
    }
    if (buzzerOn && (now - buzzerStartTime >= buzzDuration)) {
      stopBuzz();
    }
  } else {
    stopBuzz();
  }
}