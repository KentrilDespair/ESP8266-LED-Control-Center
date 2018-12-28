/**
 * @file my_app.js
 * @date 23.12.2018
 * @author Martin Smutny (xsmutn13)
 * @brief Part of IMP Project - ESP8266 LED Control Center.
 *  Main app script, consists of:
 *    - App initialization using Framework7 predefined type
 *    - Definiton of 'sequence send' function, which sends 
 *        GET requests using AJAX (Framework7 approach)
 *    - Definitions of functions for each sequence, mainly
 *        using event handlers
 * Version: 1.2
 */
// new framework7 app
var app = new Framework7({
  root: '#app',         // App root element
  name: 'My App',       // App Name
  id: 'com.myapp.test', // App id
  panel: {              // Enable swipe panel
    swipe: 'left',
  },
});
// init the app
var mainView = app.views.create('.view-main');

/**
 * @brief Sends GET requests with parameters using 
 *  Framework 7 AJAX interface
 * @param seq Sequence ID
 */
function send_seq(seq, param1, param2 = 0, param3 = 0) {
  switch(seq)
  {
    case 0: app.request.put("/ind?led=" + param1 + "&state=" + param2); break;
    case 1: app.request.put("/one?speed=" + param1); break;
    case 2: app.request.put("/row?speed=" + param1 + "&pos=" + param2); break;
    case 3: app.request.put("/col?speed=" + param1 + "&pos=" + param2); break;
    case 4: app.request.put("/cir?speed=" + param1 + "&len=" + param2 + "&dir=" + param3); break;
    case 5: app.request.put("/swap?speed=" + param1); break;
    case 6: app.request.put("/arw?speed=" + param1 + "&pos=" + param2);
  }
}
// Getting active toggles, only one sequence active at a time
var active_elems = document.getElementsByClassName("toggle");
var tgls = [];
for (var i = 0; i < active_elems.length; i++) {
  tgls.push(app.toggle.get(active_elems[i]));
}

/* -------------------------- SEQ Individual */
var click_arr = [0, 0, 0,  0, 0, 0,  0, 0, 0];

var chbx = document.getElementsByTagName("input");
// after refresh - fill the array
for (var i = 1; i < 10; i++) {  // skip the active toggle
  if (chbx[i].checked) {
    click_arr[i-1] = 1; 
  }
}

chbx[1].onclick = function() {    // 1st input is the Active toggle
  if (chbx[1].checked) { 
    click_arr[0] = 1; 
  } else {
    click_arr[0] = 0;
  }
  if (tgls[0].checked) {    // when active send
    send_seq(0, 1, click_arr[0]);
  }
}
chbx[2].onclick = function() { // LED 2
  if (chbx[2].checked) { 
    click_arr[1] = 1; 
  } else {
    click_arr[1] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 2, click_arr[1]);
  }
}
chbx[3].onclick = function() { // LED 3
  if (chbx[3].checked) { 
    click_arr[2] = 1; 
  } else {
    click_arr[2] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 3, click_arr[2]);
  }
}
chbx[4].onclick = function() { // LED 4
  if (chbx[4].checked) { 
    click_arr[3] = 1; 
  } else {
    click_arr[3] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 4, click_arr[3]);
  }
}
chbx[5].onclick = function() { // LED 5
  if (chbx[5].checked) { 
    click_arr[4] = 1; 
  } else {
    click_arr[4] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 5, click_arr[4]);
  }
}
chbx[6].onclick = function() { // LED 6
  if (chbx[6].checked) { 
    click_arr[5] = 1; 
  } else {
    click_arr[5] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 6, click_arr[5]);
  }
}
chbx[7].onclick = function() { // LED 7
  if (chbx[7].checked) { 
    click_arr[6] = 1; 
  } else {
    click_arr[6] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 7, click_arr[6]);
  }
}
chbx[8].onclick = function() { // LED 8
  if (chbx[8].checked) { 
    click_arr[7] = 1; 
  } else {
    click_arr[7] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 8, click_arr[7]);
  }
}
chbx[9].onclick = function() { // LED 9
  if (chbx[9].checked) { 
    click_arr[8] = 1; 
  } else {
    click_arr[8] = 0;
  }
  if (tgls[0].checked) {
    send_seq(0, 9, click_arr[8]);
  }
}
// when actiave changed
tgls[0].on('change', function() { 
  if (!(tgls[0].checked)) { return; }
  for (var i = 0; i < tgls.length; i++) {
    if (i != 0 && tgls[i].checked) {
        tgls[i].toggle(); // only one at a time
    }
  }
  for (var i = 0; i < click_arr.length; i++) {
    send_seq(0, i+1, click_arr[i]);
  }});

/* -------------------------- SEQ One By One */
tgls[1].on('change', function() {
  if (!(tgls[1].checked)) { return; }
  for (var i = 0; i < tgls.length; i++) {
    if (i != 1 && tgls[i].checked) {
        tgls[i].toggle();
    }
  }
  send_seq(1, slider_arr[0].getValue());
});

var sliders = document.getElementsByClassName("range-slider");
var slider_arr = [];
for (var i = 0; i < sliders.length; i++) {
  slider_arr.push(app.range.get(sliders[i]));
}

slider_arr[0].on('changed', function() {  // Slider is changed
  if (tgls[1].checked) {  // sequence is active
    send_seq(1, slider_arr[0].getValue());
  }
});
/* -------------------------- SEQ Row By Row */
var row_rds = document.getElementsByName("row_dir");  // getting radios
tgls[2].on('change', function() {
  if (!(tgls[2].checked)) { return; }
  for (var i = 0; i < tgls.length; i++) {
    if (i != 2 && tgls[i].checked) {
        tgls[i].toggle();
    }
  }
  send_seq(2, slider_arr[1].getValue(), row_rds[0].checked ? 1 : 7);
});
slider_arr[1].on('changed', function() {  // Slider is changed
  if (tgls[2].checked) {                  // sequence is active
    send_seq(2, slider_arr[1].getValue(), row_rds[0].checked ? 1 : 7);
  }
});
row_rds[0].onchange = function() {        // radio button is changed
  if (tgls[2].checked) {
    send_seq(2, slider_arr[1].getValue(), 1);
  }
}
row_rds[1].onchange = function() {
  if (tgls[2].checked) {
    send_seq(2, slider_arr[1].getValue(), 7);
  }
}
/* -------------------------- SEQ Col By Col */
var col_rds = document.getElementsByName("col_dir");
tgls[3].on('change', function() {
  if (!(tgls[3].checked)) { return; }
  for (var i = 0; i < tgls.length; i++) {
    if (i != 3 && tgls[i].checked) {
        tgls[i].toggle();
    }
  }
  send_seq(3, slider_arr[2].getValue(), col_rds[0].checked ? 3 : 5);
});
slider_arr[2].on('changed', function() {
  if (tgls[3].checked) {
    send_seq(3, slider_arr[2].getValue(), col_rds[0].checked ? 3 : 5);
  }
});
col_rds[0].onchange = function() {
  if (tgls[3].checked) {
    send_seq(3, slider_arr[2].getValue(), 3);
  }
}
col_rds[1].onchange = function() {
  if (tgls[3].checked) {
    send_seq(3, slider_arr[2].getValue(), 5);
  }
}
/* -------------------------- SEQ Circle (Rotation) */
var cir_rds = document.getElementsByName("cir_dir");
var stepper_leds = app.stepper.get((document.getElementsByClassName("stepper"))[0]);
tgls[4].on('change', function() {
  if (!(tgls[4].checked)) { return; }
  for (var i = 0; i < tgls.length; i++) {
    if (i != 4 && tgls[i].checked) {
        tgls[i].toggle();
    }
  }
  send_seq(4, slider_arr[3].getValue(), stepper_leds.getValue(), cir_rds[0].checked ? 97 : 98);
});
slider_arr[3].on('changed', function() {
  if (tgls[4].checked) {
    send_seq(4, slider_arr[3].getValue(), stepper_leds.getValue(), cir_rds[0].checked ? 97 : 98);
  }
});
cir_rds[0].onchange = function() {
  if (tgls[4].checked) {
    send_seq(4, slider_arr[3].getValue(), stepper_leds.getValue(), 97);
  }
}
cir_rds[1].onchange = function() {
  if (tgls[4].checked) {
    send_seq(4, slider_arr[3].getValue(), stepper_leds.getValue(), 98);
  }
}
stepper_leds.on('change', function() {
  if (tgls[4].checked) {
    send_seq(4, slider_arr[3].getValue(), stepper_leds.getValue(), cir_rds[0].checked ? 97 : 98);
  } 
});
/* -------------------------- SEQ Swap */
tgls[5].on('change', function() {
  if (!(tgls[5].checked)) { return; }
  for (var i = 0; i < tgls.length; i++) {
    if (i != 5 && tgls[i].checked) {
        tgls[i].toggle();
    }
  }
  send_seq(5, slider_arr[4].getValue());
});
slider_arr[4].on('changed', function() {
  if (tgls[5].checked) {
    send_seq(5, slider_arr[4].getValue());
  }
});
/* -------------------------- SEQ Arrow */
var arw_rds = document.getElementsByName("arw_dir");
tgls[6].on('change', function() {
  if (!(tgls[6].checked)) { return; }
  for (var i = 0; i < tgls.length; i++) {
    if (i != 6 && tgls[i].checked) {
        tgls[i].toggle();
    }
  }
  var pos = 1;
  if (arw_rds[1].checked) { pos = 7; }
  else if (arw_rds[2].checked) { pos = 3; }
  else if (arw_rds[3].checked) { pos = 5; }
  send_seq(6, slider_arr[5].getValue(), pos);
});
slider_arr[5].on('changed', function() {
  if (tgls[6].checked) {
	var pos = 1;
	if (arw_rds[1].checked) { pos = 7; }
	else if (arw_rds[2].checked) { pos = 3; }
	else if (arw_rds[3].checked) { pos = 5; }
    send_seq(6, slider_arr[5].getValue(), pos);
  }
});
arw_rds[0].onchange = function() {
  if (tgls[6].checked) {
    send_seq(6, slider_arr[5].getValue(), 1);
  }
}
arw_rds[1].onchange = function() {
  if (tgls[6].checked) {
    send_seq(6, slider_arr[5].getValue(), 7);
  }
}
arw_rds[2].onchange = function() {
  if (tgls[6].checked) {
    send_seq(6, slider_arr[5].getValue(), 3);
  }
}
arw_rds[3].onchange = function() {
  if (tgls[6].checked) {
    send_seq(6, slider_arr[5].getValue(), 5);
  }
}
