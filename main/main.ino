/**
 * @file main.ino
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief TODO
 * 
 * Version: 1.0
 */


#include "main.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "definitions.h" 		// TODO HEEADERS

#include "index_html.h"


ESP8266WebServer wserver(80);
/* FILE *index;					 Pointer to main page */

// WiFiServer.hasClient()
// WiFiServer.available(byte *status);

/**
 * @brief Prints diagnostic info
 */
void get_diag_info()
{
	Serial.printf("CPU frequency: %u Mhz\n", ESP.getCpuFreqMhz());
	Serial.printf("Size of the current sketch: %u\n", ESP.getSketchSize());
	Serial.printf("Free sketch size: %u\n", ESP.getFreeSketchSpace());
	Serial.printf("Flash chip size: %u\n", ESP.getFlashChipSize());
	Serial.printf("Flash real chip size: %u\n", ESP.getFlashChipRealSize());
}

/**
 * @brief New client has connected, output serial message
 */
void new_cli()
{
	WiFiClient cli = wserver.client();
	if (!cli) { 
		return;
	}

	Serial.print("New client has connected at: ");
	Serial.println(cli.remoteIP());		
	Serial.printf("Total connected: %d\n, "WiFi.softAPgetStationNum());
}

/**
 * @brief Handles request of the root dir
 */
void handle_root()
{
	// TODO
	server.send(200, "text/html", "<h1>Main Page</h1>");

	digitalWrite(LED_1, HIGH);
	digitalWrite(LED_2, HIGH);
	digitalWrite(LED_3, HIGH);
	digitalWrite(LED_4, HIGH);
	digitalWrite(LED_5, HIGH);
	digitalWrite(LED_6, HIGH);
	digitalWrite(LED_7, HIGH);
	digitalWrite(LED_8, HIGH);
	digitalWrite(LED_9, HIGH);
}

/**
 * @brief Handles not found documents
 */
void handle_not_found()
{
	String msg = "<h1>Not Found</h1>";
	// TODO string
	for (uint8_t i = 0; i < wserver.args(); i++) {
		msg += " " + wserver.argName(i) + ": " + wserver.arg(i) + "<br>";
		// TODO rather PRINT or NOTHING
	}
	server.send(404, "text/html", msg);
}


/* Setup Code */
void setup() {
	delay(1000);
	Serial.begin(115200);		/* Configuring serial monitor */
	Serial.println();
	delay(10);
	get_diag_info();

	/* SPIFFS configuration */
/*	Serial.println("Starting SPIFFS ..."); 
	if (!(SPIFFS.begin())) {			 Mounts the SPIFFS file system */
/*		Serial.println("Failed to start SPIFFS, restarting ...");	
		ESP.restart();
	}
	
	Serial.println("Opening main index file ...");
	index = SPIFFS.open("index.html", "r");
	if (!index) {
		Serial.println("Failed to open index file, restarting ...");	
		ESP.restart();	
	} */

	/* Set pins to output */
	pinMode(PIN_D0, OUTPUT);
	pinMode(PIN_D1, OUTPUT);
	pinMode(PIN_D2, OUTPUT);
	pinMode(PIN_D3, OUTPUT);
	pinMode(PIN_D4, OUTPUT);
	pinMode(PIN_D5, OUTPUT);
	pinMode(PIN_D6, OUTPUT);
	pinMode(PIN_D7, OUTPUT);
	pinMode(PIN_D8, OUTPUT);

	/* Configuring AP mode */
	Serial.print("Starting soft-AP mode ...");
	if (!(WiFi.softAP(ssid, pass))) {	
		Serial.println("Failed, restarting...");	
		ESP.restart();
	}
	Serial.println("Success");	
	WiFi.printDiag(Serial);		/* Get diagnostic info */

	Serial.print("Soft-AP IP Address: ");
	Serial.println(WiFi.softAPIP());		
	Serial.println("-----------------");

	/* Configuring web server */
	wserver.on("/", handle_root);
	wserver.onNotFound(handle_not_found);
	wserver.begin();
	Serial.println("Web Server started!");

}


/* Main Loop */
void loop() {
	wserver.handleClient();
	/* Check if a client has connected */
	new_cli();

}
