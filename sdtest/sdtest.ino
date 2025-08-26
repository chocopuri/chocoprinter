#include "wifi_config_store.hpp"
// #include "web_server.hpp"
// #include "mdns_service.hpp"

// #include <WebServer.h>

// mdns_service mdns{ "picow" };

// WebServer server(80);

json_store config_file{ "config.json" };

void aasss(bool expect_true, const char* message = "error") {
  if (expect_true)
    return;
  Serial.println(message);
}

void setup()
{
  Serial.begin(115200);

  delay(2000);
  
  aasss(sd_card_begin());
  aasss(config_file.begin());

  const auto configs = get_all_wifi_config(config_file);
  for (auto&& config : configs)
  {
    Serial.println(config.ssid.c_str());
    Serial.println(config.pass.c_str());
  }

  // begin_wifi("localnet-5G", "kokokoko");
  // mdns.begin();

  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  
  // server.on("/", HTTP_GET, [](){
  //   server.send(200, "application/json", R"({ "hoge": "hoge" })");
  // });
  
  // server.on("/", HTTP_POST, [](){
  //   String postBody = server.arg("plain"); // 生のPOSTデータ
  //   Serial.println("Received POST body:");
  //   Serial.println(postBody);
  //   server.send(200, "application/json", R"({ "status": "success" })");
  // });

  // server.begin();

  // config_file.dump_file();
}

void loop() {
  // server.handleClient();
  // mdns.update();
}
