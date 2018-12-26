/**
 * @file main.h
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief Header file of the main.ino file, consists of:
 * TODO
 *
 * Version: 1.0
 */


/* WiFi AP credentials */
const char *ssid = "LED CC";
const char *pass = "ledcontrolcenter";

void get_diag_info();
void new_cli();
void send_file(const char *fn, const char *c_type);

/* File handlers */
void handle_root();
void handle_not_found();
void handle_fcss();
void handle_fjs();
void handle_app_js();


