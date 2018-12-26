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


ESP8266WebServer wserver(80);
String str_buff;					/* Globally accessible buffer */
char buff[BUFF_SIZE+1];				/* Auxiliary buffer */

// WiFiServer.hasClient()
// WiFiServer.available(byte *status);

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

	Serial.print("New client has connected at: ");
	Serial.println(cli.remoteIP());		
	Serial.printf("Total connected: %d\n", WiFi.softAPgetStationNum());
}

/**
 * @brief Handles request of the root dir
 */
void handle_root()
{
  	Serial.println("-> \"index.html\" requested");
	File f = SPIFFS.open("/index.html.gz", "r");
	if (!f) {
		Serial.println("File: \"index.html\" could not be opened.");
		return;
	}

	size_t sent = wserver.streamFile(f, "text/html");	/* Has less size */
	(void)sent;
	f.close();
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

	wserver.setContentLength(f.size()); /* Chunked */
	uint32_t B_pos;					    /* xth byte in file */
	bool first_part = true;				/* First part needs to be sent */
	
	while(f.available()) 
	{
		B_pos = 0;
		str_buff = "";  /* Clears the string, stupid, I know */   

		if (f.size() - f.position() < BUFF_SIZE-1)	{		/* Last chunk */
			Serial.println("Going to send last chunk");
			break;
		}
		while (B_pos < BUFF_SIZE) {
			str_buff += char(f.read());	
			B_pos++;
		}
   
		if (first_part)	
		{
      Serial.println("Sending first chunk");
      wserver.sendHeader("Content-Encoding", "gzip");
			wserver.send(200, "text/css", str_buff);
			first_part = false;
			continue;
		}
		wserver.sendContent(str_buff);
	}
 

	while(f.available()) {
    	str_buff += char(f.read());  
	}
	wserver.sendContent(str_buff);
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

	// 38000 B of free heap
  wserver.setContentLength(f.size()); /* Chunked */
  uint32_t B_pos;           /* xth byte in file */
  bool first_part = true;       /* First part needs to be sent */
  
  /*
  while(f.available()) 
  {
    B_pos = 0;
    str_buff = "";  Clears the string    
    Serial.printf("Pos of f: %u\n", f.position());
    if (f.size() - f.position() < BUFF_SIZE-1) {   Last chunk 
      Serial.println("Going to send last chunk");
      break;
    }
    while (B_pos < BUFF_SIZE) {
      str_buff += char(f.read()); 
      B_pos++;
    }

    Serial.printf("String len: %u\n", str_buff.length());
    if (first_part) 
    {
      Serial.println("Sending first chunk");
      wserver.sendHeader("Content-Encoding", "gzip");
      wserver.send(200, "application/javascript", str_buff);
      first_part = false;
      continue;
    }
    wserver.sendContent(str_buff);
  }

  while(f.available()) {
      str_buff += char(f.read());
  }
  */
  
	while(readBytes(&buff, BUFF_SIZE))
	{
		str_buff = buff;			/* Copy to string */
  		Serial.printf("String len: %u\n", str_buff.length());

		if (first_part)
		{
			Serial.println("Sending first chunk");
			wserver.sendHeader("Content-Encoding", "gzip");
			wserver.send(200, "application/javascript", str_buff);
			first_part = false;
			continue;
		}

  		wserver.sendContent(str_buff);
	}

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
	memset(buff, 0, sizeof(buff));

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
	/* Check if a client has connected */
	new_cli();

}
