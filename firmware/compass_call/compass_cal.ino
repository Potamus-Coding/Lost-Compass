/*
 * ============================================================
 *  Servo Speed Calibration — ESP32C3 XIAO
 *  Flash this, connect to "CompassCal", open http://192.168.4.1
 *  Tap LAP each time the dial mark completes a full rotation.
 *  Copy the resulting deg/ms values back into the main sketch.
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

#define SERVO_PIN  D10
#define STOP_US    1450   // match your main sketch

const char* AP_SSID = "CompassCal";
const char* AP_PASS = "compass123";

Servo dial;
WebServer server(80);

// ── Handlers ─────────────────────────────────────────────────

void handleStop() {
  dial.writeMicroseconds(STOP_US);
  server.send(200, "text/plain", "OK");
}

void handleSpin() {
  if (server.hasArg("us")) {
    dial.writeMicroseconds(server.arg("us").toInt());
  }
  server.send(200, "text/plain", "OK");
}

// ── Page ─────────────────────────────────────────────────────

const char CAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Speed Cal</title>
<style>
  body{font-family:monospace;background:#1a1a1a;color:#e0d8c0;padding:1.5rem;max-width:340px;margin:0 auto;}
  h2{color:#c9a84c;margin-bottom:.5rem;font-size:1rem;letter-spacing:.1em;}
  p.hint{font-size:.75rem;color:#9a8a60;margin-bottom:1.2rem;line-height:1.5;}
  .row{display:flex;gap:.5rem;margin-bottom:.5rem;}
  button{
    background:#3a2a0a;color:#e0d8c0;border:1px solid #8b6914;
    padding:.55rem .7rem;font-family:monospace;font-size:.82rem;cursor:pointer;flex:1;
  }
  button:active{background:#8b6914;}
  #stopbtn{background:#4a0a0a;border-color:#8b1a1a;width:100%;padding:.7rem;font-size:.95rem;margin:.6rem 0;}
  #lapbtn{width:100%;padding:.85rem;font-size:1rem;background:#0a1a0a;border:2px solid #4a8a4a;margin-bottom:.8rem;}
  #lapbtn:active{background:#1a4a1a;}
  #out{background:#0a0a0a;border:1px solid #3a3020;padding:.8rem;min-height:4rem;font-size:.78rem;line-height:1.7;}
  .lap{color:#c9a84c;}
  .avg{color:#8bcc8b;font-size:.88rem;}
  .warn{color:#cc8b8b;}
</style>
</head>
<body>

<h2>SERVO SPEED CALIBRATION</h2>
<p class="hint">
  Put a mark on the dial edge. Pick a speed below — dial starts spinning.<br>
  Tap LAP every time the mark completes one full rotation.<br>
  3+ laps recommended for accuracy.
</p>

<div class="row">
  <button onclick="spin(1300)">Fast CCW<br>1300us</button>
  <button onclick="spin(1700)">Fast CW<br>1700us</button>
</div>
<div class="row">
  <button onclick="spin(1380)">Med CCW<br>1380us</button>
  <button onclick="spin(1620)">Med CW<br>1620us</button>
</div>
<div class="row">
  <button onclick="spin(1400)">Slow CCW<br>1400us</button>
  <button onclick="spin(1560)">Slow CW<br>1560us</button>
</div>

<button id="stopbtn" onclick="stopDial()">STOP</button>
<button id="lapbtn"  onclick="lap()">LAP &mdash; mark passed</button>

<div id="out">Select a speed to begin.</div>

<script>
  var base = 'http://' + window.location.hostname;
  var laps = [];
  var currentUs = 0;

  function req(url) {
    var x = new XMLHttpRequest();
    x.open('GET', base + url, true);
    x.send();
  }

  function spin(us) {
    currentUs = us;
    laps = [];
    var dir = us > 1450 ? 'CW' : 'CCW';
    document.getElementById('out').innerHTML =
      'Spinning ' + dir + ' at ' + us + 'us<br>' +
      'Tap LAP when the mark passes...';
    req('/spin?us=' + us);
  }

  function stopDial() {
    req('/stop');
    document.getElementById('out').innerHTML += '<br><span class="warn">-- stopped --</span>';
  }

  function lap() {
    if (currentUs === 0) {
      document.getElementById('out').innerHTML = '<span class="warn">Pick a speed first.</span>';
      return;
    }
    var now = Date.now();
    laps.push(now);

    if (laps.length === 1) {
      document.getElementById('out').innerHTML = 'Lap 1 locked. Keep going...';
      return;
    }

    var html = '';
    var times = [];
    for (var i = 1; i < laps.length; i++) {
      var ms  = laps[i] - laps[i - 1];
      var dpm = (360.0 / ms);
      times.push(ms);
      html += '<span class="lap">Lap ' + i + ': ' + ms + 'ms &rarr; ' + dpm.toFixed(4) + ' deg/ms</span><br>';
    }

    var sum = times.reduce(function(a, b) { return a + b; }, 0);
    var avg = sum / times.length;
    var avgDpm = (360.0 / avg).toFixed(4);
    var dir = currentUs > 1450 ? 'CW' : 'CCW';

    html += '<br><span class="avg">AVG (' + dir + ' ' + currentUs + 'us): ' + avgDpm + ' deg/ms</span>';
    document.getElementById('out').innerHTML = html;
  }
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", CAL_HTML);
}

// ── Setup / Loop ──────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  Serial.setTxTimeoutMs(0);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());

  server.on("/",     HTTP_GET, handleRoot);
  server.on("/spin", HTTP_GET, handleSpin);
  server.on("/stop", HTTP_GET, handleStop);
  server.begin();
  Serial.println("Ready — http://192.168.4.1");

  ESP32PWM::allocateTimer(0);
  dial.setPeriodHertz(50);
  dial.attach(SERVO_PIN, 500, 2500);
  dial.writeMicroseconds(STOP_US);
}

void loop() {
  server.handleClient();
  // No delay() — servo holds its PWM, server stays responsive
}
