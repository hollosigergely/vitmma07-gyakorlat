#include "WifiAp.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>


#define AP_SSID	"VITMMA07_3"
#define AP_PASS "Password"


void setup(void){
	Serial.begin(115200);
	delay(5000);

	Serial.println("Enable AP mode (SSID: " AP_SSID ")");

	WiFi.softAP(AP_SSID, AP_PASS);
  	IPAddress myIP = WiFi.softAPIP();
  	Serial.print("MAC address: ");
	Serial.println(WiFi.softAPmacAddress());
	Serial.print("AP IP address: ");
  	Serial.println(myIP);
	
}

void loop(void){
}
