/**
   @file main.ino
   @date 23.12.2018
   @author Martin Smutny (xsmutn13)
   @brief TODO

   Version: 1.3	(+ Temp sensor)
*/

/* #define SPIFFS_FORMAT */  	/* TODO Uncomment to format */
/* #define TEMP_SENSOR */		/* TODO Uncomment to enable TEMP sensor */
/* #define SEND_CHUNKED */


#include "main.h"
#include "definitions.h"     // TODO HEEADERS ONLY USED - VISIBLE
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* Global Variables */
static uint32_t del_timer;		/* Simulates delay, or speed */

#ifdef SEND_CHUNKED
String str_buff;          /* Globally accessible buffer*/
#define BUFF_SIZE 4096
#endif

#ifdef TEMP_SENSOR
static uint32_t temp_timer;		/* For temp readings */
#endif

/* WiFi AP credentials */
static const char *ssid = "LED CC";
static const char *pass = "ledcontrolcenter";
ESP8266WebServer wserver(80);       /* Web server */


/**
   @brief Prints diagnostic info
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
   @brief New client has connected, output serial message
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
   @brief Sends a certain file with certain content_type
 	If GZIP file exists, then is preffered.
   @param fn Path with a name of the file
   @param c_type Content type of the file
*/
void send_file(const char *fn, const char *c_type)
{
  new_cli();
  String f_name(fn);

  /* Supports gzip encoding */
    if ((wserver.header("Accept-Encoding")).startsWith("gzip")) {
      if (SPIFFS.exists(f_name + ".gz")) {
        f_name += ".gz"; 
      }
    }
    
  Serial.printf("-> \"%s\" requested, supports encoding: %s\n", f_name.begin(),
                (wserver.header("Accept-Encoding")).c_str());

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
   @brief
*/
#ifdef SEND_CHUNKED
void send_file_chunked(const char *fn, const char *c_type)
{
  new_cli();
  String f_name(fn);
  bool gzip = false;

  /* Supports gzip encoding
    if ((wserver.header("Accept-Encoding")).indexOf("gzip") != -1) {
    f_name += ".gz";
    gzip = true;
    } */

  Serial.printf("CHUNKED \"%s\" requested, supports encoding: %s\n", fn,
                (wserver.header("Accept-Encoding")).c_str());

  File f = SPIFFS.open(f_name, "r");    /* FS to open a file */
  if (!f) {
    Serial.printf("File: \"%s\" could not be opened.\n", f_name.begin());
    return;
  }

  wserver.setContentLength(f.size()); /* Chunked */
  uint32_t B_pos;           /* xth byte in file */
  bool first_part = true;       /* First part needs to be sent */

  while (42)
  {
    B_pos = 0;
    str_buff = "";  /* Clears the string */

    if (f.size() - f.position() >= BUFF_SIZE - 1)
    {
      while (B_pos < BUFF_SIZE) {
        str_buff += char(f.read());
        B_pos++;
      }
    }
    else            /* Last chunk */
    {
      while (f.available()) {
        str_buff += char(f.read());
      }
      break;
    }

    if (first_part)
    {
      if (gzip) {
        wserver.sendHeader("Content-Encoding", "gzip");
      }
      wserver.send(200, c_type, str_buff);
      first_part = false;
      continue;
    }
    wserver.sendContent(str_buff);
  }

  f.close();
}
#endif

/**
   @brief Handles request of the root dir
*/
void handle_root()
{
#ifndef SEND_CHUNKED
  send_file("/index.min.html", "text/html");
#else
  send_file_chunked("/index.html", "text/html");
#endif
}

/**
   @brief Handles framework CSS
*/
void handle_fcss()
{
#ifndef SEND_CHUNKED
  send_file("/framework7.min.css", "text/css");
#else
  send_file_chunked("/framework7.min.css", "text/css");
#endif
}

/**
   @brief Handles the framework7.min.js file
*/
void handle_fjs()
{
#ifndef SEND_CHUNKED
  send_file("/framework7.min.js", "application/javascript");
#else
  send_file_chunked("/framework7.min.js", "application/javascript");
#endif
}

/**
   @brief Handles the app.js file
*/
void handle_app_js()
{
#ifndef SEND_CHUNKED
  send_file("/my_app.min.js", "application/javascript");
#else
  send_file_chunked("/my_app.js", "application/javascript");
#endif
}

/**
   @brief Performs check on current speed when writing into it
 	1 slowest - 100 fastest
*/
void set_cur_speed(uint8_t speed)
{
  if (speed < 1 || speed > 100) {
    cur_speed = 1000;				/* Back to default - ~1 sec */
    return;
  }
  cur_speed = 2000 - (speed * 19);		/* interval 100 to 1981 */
  del_timer = millis() + cur_speed;	/* Sets next "delay" */
}

/**
   @brief Performs check on position when writing into it
*/
void set_cur_pos(uint8_t pos)
{
  switch (pos)
  {
    case POS_TOP: case POS_LEFT: case POS_RIGHT: case POS_BOTTOM:
      cur_pos = pos;
      return;
    default:
      cur_pos = POS_TOP;
  }
}

/**
   @brief Performs check on direction when writing into it
*/
void set_cur_dir(uint8_t dir)
{
  switch (dir)
  {
    case DIR_CLKW: case DIR_ACLKW:
      cur_direct = dir;
      return;
    default:
      cur_direct = DIR_CLKW;
  }
}

/**
   @brief Performs check on length of leds when setting it
*/
void set_cur_len(uint8_t len)
{
  if (cur_len > 8) {
    cur_len = 1;
    return;
  }
  cur_len = len;
}

/**
   @brief Handles sequence "Individual"
*/
bool handle_seq_ind()
{
  if (wserver.args() < 2)	{
    return false;
  }
  change_seq(SEQ_INDIV);			/* No function call will be done */
  seq_individual((wserver.arg(0)).toInt(), (wserver.arg(1)).toInt());
  return true;
}

/**
   @brief Handles sequence "One by One"
*/
bool handle_seq_one()
{
  if (wserver.args() < 1)	{
    return false;
  }
  set_cur_speed((wserver.arg(0)).toInt());
  change_seq(SEQ_ONE);
  return true;
}

/**
   @brief Handles sequence "Row by Row"
*/
bool handle_seq_row()
{
  if (wserver.args() < 2)	{
    return false;
  }
  set_cur_speed((wserver.arg(0)).toInt());
  set_cur_pos((wserver.arg(1)).toInt());
  change_seq(SEQ_ROW);
  return true;
}

/**
   @brief Handles sequence "Column by Column"
*/
bool handle_seq_col()
{
  if (wserver.args() < 2)	{
    return false;
  }
  set_cur_speed((wserver.arg(0)).toInt());
  set_cur_pos((wserver.arg(1)).toInt());
  change_seq(SEQ_COL);
  return true;
}

/**
   @brief Handles sequence "Circle (Rotation)"
*/
bool handle_seq_cir()
{
  if (wserver.args() < 3)	{
    return false;
  }
  set_cur_speed((wserver.arg(0)).toInt());
  set_cur_len((wserver.arg(1)).toInt());
  set_cur_dir((wserver.arg(2)).toInt());
  change_seq(SEQ_CIRC);
  return true;
}

/**
   @brief Handles sequence "Swap"
*/
bool handle_seq_swp()
{
  if (wserver.args() < 1)	{
    return false;
  }
  set_cur_speed((wserver.arg(0)).toInt());
  change_seq(SEQ_SWAP);
  return true;
}

/**
   @brief Handles sequence "Arrow"
*/
bool handle_seq_arw()
{
  if (wserver.args() < 2)	{
    return false;
  }
  set_cur_speed((wserver.arg(0)).toInt());
  set_cur_pos((wserver.arg(1)).toInt());
  change_seq(SEQ_ARROW);
  return true;
}

/**
   @brief Handles not found documents
*/
void handle_not_found()
{
  String req = wserver.uri();		/* request */
  bool is_req = false;			/* URL recognized */

  switch (req[1])
  {
    case 'a':
      if (req.indexOf("/arw") != -1) {
        is_req = handle_seq_arw();
      }
      break;
    case 'c':
      if (req.indexOf("/cir") != -1) {
        is_req = handle_seq_cir();
      }
      else if (req.indexOf("/col") != -1) {
        is_req = handle_seq_col();
      }
      break;
    case 'i':
      if (req.indexOf("/ind") != -1) {
        is_req = handle_seq_ind();
      }
      break;
    case 'o':
      if (req.indexOf("/one") != -1) {
        is_req = handle_seq_one();
      }
      break;
    case 'r':
      if (req.indexOf("/row") != -1) {
        is_req = handle_seq_row();
      }
      break;
    case 's':
      if (req.indexOf("/swp") != -1) {
        is_req = handle_seq_swp();
      }
  }

  if (is_req) {					/* Sequence request recognized */
    return;
  }

  String msg = "<h1>Not Found: <br>";
  msg += req;
  msg += "<br>Arguments: ";
  msg += wserver.args();
  msg += "<br>";

  for (uint8_t i = 0; i < wserver.args(); i++) {
    msg += wserver.argName(i) + ": " + wserver.arg(i) + "<br>";
  }
  msg += "</h1>";
  wserver.send(404, "text/html", msg);
}

#ifdef TEMP_SENSOR
/** @see https://learn.adafruit.com/tmp36-temperature-sensor/
		using-a-temp-sensor */
void get_temp()
{
  uint16_t temp_reading;				/* 0 - 1023 range */
  temp_reading = analogRead(TMP_PIN);
  Serial.printf("Raw temp: %u\n", temp_reading);

  /* Voltage at pin in milliVolts = (reading from ADC) * (3300/1024) */
  float voltage = temp_reading * 3.3;
  voltage /= 1024.0;

  /* now print out the temperature */
  float temps = (voltage - 0.5) * 100 ;
  /* converting from 10 mv per degree with 500 mV offset
     to degrees ((voltage - 500mV) times 100) */
  Serial.printf("Temps: %f C\n", temps);
}
#endif

/* Setup Code */
void setup()
{
  delay(1000);
  Serial.begin(115200);		/* Configuring serial monitor */
  Serial.println();
  delay(10);
  get_diag_info();

#ifdef SPIFFS_FORMAT
  Serial.printf("FORMATTING: %s\n", SPIFFS.format() ? "true" : "false");
#endif

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
  del_timer = UINT_MAX;
#ifdef TEMP_SENSOR
  temp_timer = millis() + 1000;
#endif
#ifdef SEND_CHUNKED
  str_buff.reserve(BUFF_SIZE);
#endif

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
  /* Collect received headers */
  const char *hdr_keys[] = {"Accept-Encoding"};
  wserver.collectHeaders(hdr_keys, sizeof(hdr_keys) / sizeof(char*));
  wserver.begin();
  Serial.println("Web Server started!");
}

/* Main Loop */
void loop()
{
  wserver.handleClient();

  if (millis() >= del_timer)	/* After x milliseconds, calls sequence */
  {
    cur_seq_continue();
    del_timer += cur_speed;   /* Next "delay" */
  }

#ifdef TEMP_SENSOR
  if (millis() >= temp_timer)
  {
    temp_timer += 1000;
    get_temp();
  }
#endif
}
