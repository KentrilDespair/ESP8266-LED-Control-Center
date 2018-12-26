/**
 * @file main.ino
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief TODO
 * 
 * Version: 1.0
 */


#include "main.h"
#include "definitions.h"     // TODO HEEADERS ONLY USED - VISIBLE
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* TODO Global Variables */
/* WiFi AP credentials */
static const char *ssid = "LED CC";
static const char *pass = "ledcontrolcenter";
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

	// TODO if EXISTS GZIP!! & server accpets!

	File f = SPIFFS.open(f_name, "r");    /* FS to open a file */
	if (!f) {
		Serial.printf("File: \"%s\" could not be opened.\n", f_name.begin());
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
 * @brief Handles framework CSS
 */
void handle_fcss()
{
	send_file("/framework7.min.css", "text/css");
}

/**
 * @brief Handles the framework7.min.js file
 */
void handle_fjs()
{
	send_file("/framework7.min.js", "application/javascript");
}

/**
 * @brief Handles the app.js file
 */
void handle_app_js()
{
	send_file("/my_app.js", "application/javascript");
}

/**
 * @brief Handles not found documents
 */
void handle_not_found()
{
	String msg = "<h1>Not Found</h1>";
	msg += wserver.uri();
	msg += "Arguments: ";
	msg += wserver.args();			// IS NUM
	msg += "\n";

	for (uint8_t i = 0; i < wserver.args(); i++) {
		msg += " " + wserver.argName(i)	+ ": " + wserver.arg(i) + "\n";
	}

	wserver.send(404, "text/html", msg);

	String req = wserver.uri();		/* Request */

	if (req.indexOf("/ind") != -1)	/* TODO SERVER ON */
	{
		if (wserver.args() < 2)	{ return; }
		seq_individual((wserver.arg(0)).toInt, (wserver.arg(1)).toInt);
		change_seq(SEQ_INDIV);	/* No function call will be done */
	}
	else if (req.indexOf("/one"))
	{
		if (wserver.args() < 1)	{ return; }
		cur_speed = (wserver.arg(0)).toInt();
		change_seq(SEQ_ONE);
	
	}
	else if (req.indexOf("/row"))
	{
		if (wserver.args() < 2)	{ return; }
		cur_speed = (wserver.arg(0)).toInt();
		cur_pos = (wserver.arg(1)).toInt();
		change_seq(SEQ_ROW);
	
	}
	else if (req.indexOf("/col"))
	{
		if (wserver.args() < 2)	{ return; }	
		cur_speed = (wserver.arg(0)).toInt();
		cur_pos = (wserver.arg(1)).toInt();
		change_seq(SEQ_COL);
	}
	else if (req.indexOf("/cir"))
	{
		if (wserver.args() < 3)	{ return; }
		cur_speed = (wserver.arg(0)).toInt();
		cur_len = (wserver.arg(1)).toInt();
		cur_direct = (wserver.arg(2)).toInt();
		change_seq(SEQ_CIRC);
	}
	else if (req.indexOf("/swp"))
	{
		if (wserver.args() < 1)	{ return; }
		cur_speed = (wserver.arg(0)).toInt();
		change_seq(SEQ_SWAP);
	}
	else if (req.indexOf("/arw"))	
	{
		if (wserver.args() < 2)	{ return; }
		cur_speed = (wserver.arg(0)).toInt();
		cur_direct = (wserver.arg(1)).toInt();
		change_seq(SEQ_ARROW);
	}
	else	/* Just a normal not found */
		return;

	if (cur_speed < 100 || cur_speed > 3000) {
		speed = 1000;			/* Back to default */
	}
	if (cur_pos != POS_TOP && cur_pos != POS_LEFT && 
		cur_pos != POS_RIGHT && cur_pos != POS_BOTTOM) {
		cur_pos = POS_TOP; 
	}

	if (cur_len > 9) {
		cur_len = 1;
	}

	if (cur_direct != CLKW && cur_direct != ACLKW) {
		cur_direct = CLKW;
	}

	end_timer = millis() + cur_speed;
	cur_seq_continue();

	// seq_one_by_one(1000);
	// seq_row(1000, POS_TOP);
	// seq_col(1000, POS_TOP);
	// seq_circle(1000, 1, CLKW);
	// seq_swap(1000);
	// seq_arrow(1000, POS_RIGHT);
}

// /ind?led=x&state=1
// /one?speed=xxxx
// /row?speed=xxxx&pos=1
// /col?speed=xxxx&pos=1
// /cic?speed=xxxx&len=x&dir=1
// /swp?speed=xxxx
// /arw?speed=xxxx&dir=1

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

	/* Reset LEDs */
	led_reset();
	cur_len = 1;		/* Max. number of LEDs ON */
  
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

	if (millis() >= end_timer)	
	{
		cur_seq_continue();
		end_timer += cur_speed;
	}
}
