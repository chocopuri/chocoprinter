#include "wifi.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

static WiFiMulti wifiMulti;

void wifi_begin(const std::vector<AccessPoint>& aps)
{
    for (const auto& ap : aps)
    {
        wifiMulti.addAP(ap.ssid, ap.pass);
    }

    Serial.println("Connecting WiFi...");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
}

void wifi_update()
{
    // 再接続チェック
    if (wifiMulti.run() != WL_CONNECTED)
    {
        Serial.println("WiFi lost, reconnecting...");
    }
}
