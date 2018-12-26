/**
 * @file main.ino
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief TODO
 * 
 * Version: 1.0
 */


#include "main.h"
#include "definitions.h"     // TODO HEEADERS
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* TODO Global Variables */
ESP8266WebServer wserver(80);       /* Web server */


/**
 * @brief Prints diagnostic info
 */
void get_diag_info()
{
	Serial.printf("CPU frequency: %u Mhz\n", ESP.getCpuFreqMHz());
	Serial.printf("Size of the current sketch: %u\n", ESP.getSketchSize());
	Serial.printf("Free sketch size: %u\n", ESP.getFreeSketchSpace());
	Serial.printf("Flash chip size: %u\n", ESP.getFlashChipSize());
	Serial.printf("Flash real chip size: %u\n", ESP.getFlashChipRealSize());
	Serial.printf("Free Heap size: %u\n", ESP.getFreeHeap());
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

	Serial.print("Request from client: ");
	Serial.println(cli.remoteIP());
	Serial.printf("Total connected: %d\n", WiFi.softAPgetStationNum());
}

/**
 * @brief Sends a certain file with certain content_type
 *	If GZIP file exists, then is preffered.
 * @param fn Path with a name of the file
 * @param c_type Content type of the file
 */
void send_file(const char *fn, const char *c_type)
{
	new_cli();
	String f_name(fn);
	f_name += ".gz";							/* GZIP compressed file */
	Serial.printf("-> \"%s\" requested\n", fn);

	// TODO if EXISTS!! & server accpets!

	File f = SPIFFS.open(f_name, "r");    /* FS to open a file */
	if (!f) {
		Serial.printf("File: \"%s\" could not be opened.\n", f_name);
		return;
	}

	size_t len = wserver.streamFile(f, c_type);		/* Has less size */
	(void)len;
	f.close();
}

/**
 * @brief Handles request of the root dir
 */
void handle_root()
{
	send_file("/index.html", "text/html");
}

/**
 * @brief Handles framework CSS TODO ONE COMMON BREAK UP FUNCTION
 */
void handle_fcss()
{
	Serial.println("-> \"framework7.min.css\" requested");  
	File f = SPIFFS.open("/framework7.min.css.gz", "r");
	if (!f) {
		Serial.println("File: \"framework7.min.css\" could not be opened.");
		return;
	}
  size_t sent = wserver.streamFile(f, "text/css");
  (void)sent;

	f.close();
}

/**
 * @brief Handles the framework7.min.js file
 */
void handle_fjs()
{
	Serial.println("-> \"framework7.min.js\" requested");
	File f = SPIFFS.open("/framework7.min.js.gz", "r");
	if (!f) {
		Serial.println("File: \"framework7.min.js\" could not be opened.");
		return;
	}

  size_t sent = wserver.streamFile(f, "application/javascript");
  (void)sent;

	f.close();
}

/**
 * @brief Handles the app.js file
 */
void handle_app_js()
{
	Serial.println("-> \"my_app.js\" requested");  
	File f = SPIFFS.open("/my_app.js", "r");
	if (!f) {
		Serial.println("File: \"my_app.js\" could not be opened.");
		return;
	}
	size_t sent = wserver.streamFile(f, "application/javascript");
	(void)sent;
	f.close();
}

/**
 * @brief Handles not found documents
 */
void handle_not_found()
{
	String msg = "<h1>Not Found</h1>";
	msg += wserver.uri();
	wserver.send(404, "text/html", msg);
}


/* Setup Code */
void setup()
{
	delay(1000);
	Serial.begin(115200);		/* Configuring serial monitor */
	Serial.println();
	delay(10);
	get_diag_info();

/*  Serial.printf("FORMATTING: %s\n", SPIFFS.format() ? "true" : "false"); */

	/* SPIFFS configuration */
	Serial.println("Starting SPIFFS ..."); 
	if (!(SPIFFS.begin())) {			/* Mounts the SPIFFS file system */
		Serial.println("Failed to start SPIFFS, restarting ...");	
		ESP.restart();
	}

	/* Set pins to output */
	Serial.println("Setting pins ...");
	pinMode(PIN_D0, OUTPUT);
	pinMode(PIN_D1, OUTPUT);
	pinMode(PIN_D2, OUTPUT);
	pinMode(PIN_D3, OUTPUT);
	pinMode(PIN_D4, OUTPUT);
	pinMode(PIN_D5, OUTPUT);
	pinMode(PIN_D6, OUTPUT);
	pinMode(PIN_D7, OUTPUT);
	pinMode(PIN_D8, OUTPUT);

	/* TODO reset LEDs */

	/* Configuring AP mode */
	Serial.print("Starting soft-AP mode ...");
	if (!(WiFi.softAP(ssid, pass))) {	
		Serial.println("Failed, restarting...");	
		ESP.restart();
	}
	Serial.println("Success");	
	/* WiFi.printDiag(Serial); */		/* Get diagnostic info */

	Serial.print("Soft-AP IP Address: ");
	Serial.println(WiFi.softAPIP());		
	Serial.println("-----------------");

	str_buff.reserve(BUFF_SIZE);
  Serial.printf("LEN: %u\n", str_buff.length());

	/* Configuring web server */
	wserver.on("/", handle_root);
	wserver.on("/framework7.min.css", handle_fcss);
	wserver.on("/framework7.min.js", handle_fjs);
	wserver.on("/my_app.js", handle_app_js);
	wserver.onNotFound(handle_not_found);
	wserver.begin();
	Serial.println("Web Server started!");
}


/* Main Loop */
void loop() {
	wserver.handleClient();

}
