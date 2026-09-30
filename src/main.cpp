#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


// Wifi

const char* WIFI_SSID = "SpaDuWach";
const char* WIFI_PASSWORD = "spotty2017";

// Tg

#define BOT_TOKEN "8886193988:AAGWXPPFtD5RCBXr-3_3KygOHM0i9ibT1Zo"
#define CHAT_ID "8019398181"

// Pins

#define PIR_PIN 27
#define LED_PIN 26

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// How long (in milliseconds) the LED stays on after the LAST motion
const unsigned long HOLD_TIME = 4000;

bool motionAlreadyDetected = false;
unsigned long lastMotionTime = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(PIR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("Connecting to Wi-Fi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected!");

    client.setInsecure();

    bool ok = bot.sendMessage(CHAT_ID, "ESP32 PIR system is now ONLINE.", "");
    Serial.println(ok ? "Startup message sent." : "Startup message FAILED.");

    Serial.println("PIR sensor warming up...");
    delay(30000);

    Serial.println("System READY!");
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Wi-Fi lost. Reconnecting...");
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(5000);
        return;
    }

    // Remember the time of the last motion
    if (digitalRead(PIR_PIN) == HIGH)
    {
        lastMotionTime = millis();

        if (!motionAlreadyDetected)
        {
            motionAlreadyDetected = true;
            digitalWrite(LED_PIN, HIGH);
            Serial.println("MOTION DETECTED!");

            bool sent = bot.sendMessage(
                CHAT_ID,
                "🚨 MOTION DETECTED!\nESP32 PIR sensor detected movement.",
                ""
            );

            Serial.println(sent ? "Telegram notification sent." : "Telegram FAILED to send.");
        }
    }

    // Turn OFF only after no motion for HOLD_TIME
    if (motionAlreadyDetected && (millis() - lastMotionTime > HOLD_TIME))
    {
        motionAlreadyDetected = false;
        digitalWrite(LED_PIN, LOW);
        Serial.println("No motion.");
    }

    delay(100);
}