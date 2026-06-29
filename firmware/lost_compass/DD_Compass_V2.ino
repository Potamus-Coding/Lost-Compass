/*
 * ============================================================
 *  Magnetic Traverse Compass — ESP32C3 XIAO (Seeed Studio)
 *  Anno Domini MDCCCXXXIV  (theme: 1834 field instrument)
 * ============================================================
 *  Routes:
 *    GET /          → HTML page
 *    GET /heading   → JSON {"heading":247.3,"mode":"normal","target":0.0}
 *    GET /set       → params: mode=normal|lazyspin|erraticspin|fixed
 *                             deg=0..359
 *
 *  Connect to WiFi "CompassControl" then open http://192.168.4.1
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "driver/gpio.h"
#include <Wire.h>
#include <qmc5883p.h>

#define SERVO_PIN D10
#define LID_PIN D2
#define BAT_PIN D1



// ===================================================================
// Servo initiation & calibration
// ===================================================================
// - These need validation below
#define STOP_US 1450
#define FAST_CCW 1300
#define FAST_CW 1700
const float DEG_PER_MS_FAST_CW = 0.3f;   // 510° / 2000ms
const float DEG_PER_MS_FAST_CCW = 0.2441f;  // 430° / 2000ms
#define MED_CCW 1380
#define MED_CW 1620
const float DEG_PER_MS_MED_CW = 0.1808f;   // MED_CW  = 1620 (170µs above stop) → 295° / 2000ms
const float DEG_PER_MS_MED_CCW = 0.1293f;  // MED_CCW = 1380 ( 70µs below stop) → 230° / 2000ms
#define SLOW_CCW 1400
#define SLOW_CW 1560
const float DEG_PER_MS_SLOW_CW = 0.0844f;   // SLOW_CW  = 1560 (110µs above stop) → 145° / 2000ms
const float DEG_PER_MS_SLOW_CCW = 0.0927f;  // SLOW_CCW = 1400 ( 50µs below stop) → 120° / 2000ms

float cwBias  = 1.00f;   // tune up if CW moves further than expected
float ccwBias = 1.00f;   // tune up if CCW moves further than expected

#define SENSOR_ROTATION  -90.0f   // mounting correction; flip sign if N↔S inverted


float snappedHeading = 0.0f;

float snapToCardinal(float h) {
  return fmod(round(h / 45.0f) * 45.0f + 360.0f, 360.0f);
}

float angDiff(float a, float b) {
  float d = fmod(fabs(a - b), 360.0f);
  return d > 180.0f ? 360.0f - d : d;
}

// Estimated dial position in degrees — updated by every movement function
float dialBearing = 0.0f;
float currentHeading = 0.0f;
float targetHeading = 0.0f;
unsigned long lastHeadingUpdate = 0;
float headingOffset = 0.0f;

String currentMode = "fixed";

// One-liner helper: adds a timed move to the dead-reckoning estimate
inline void trackMove(bool cw, float degPerMs, int ms) {
  float scale = cw ? cwBias : ccwBias;
  dialBearing = fmod(dialBearing + (cw ? 1.0f : -1.0f) * degPerMs * scale * ms + 3600.0f, 360.0f);
}

Servo dial;

// ===================================================================
// Compass initiation & calibration
// ===================================================================
QMC5883P mag;
// INSERT "SOFT-IRON" CALIBRATION VALUES HERE
/* Valeus pre assembly
const float SCALE_AVG = 0.528f;
const float SCALE_X = 0.573f;
const float SCALE_Y = 0.483f;
*/
const float SCALE_AVG = 0.528f;
const float SCALE_X   = 0.533f;
const float SCALE_Y   = 0.524f;
const float Declination = 2.87;

// ===================================================================
// AP initiation & calibration
// ===================================================================
const char* AP_SSID = "CompassControl";
const char* AP_PASS = "compass123";

//Optional - Add logic to connect to own wifi as needed

// ===================================================================
// Battery
// ===================================================================
const float BAT_DIVIDER = 2.0f;
// TODO: adjust segment thresholds (volts) for your battery chemistry.
//   These are typical single-cell LiPo values.
//   Index 0 = 1 bar, index 4 = 5 bars.
const float BAT_THRESH[5] = { 3.0f, 3.3f, 3.5f, 3.7f, 4.0f };

float readBatV() {
  // 12-bit ADC, 3.3 V reference, scaled by divider
  int raw = analogRead(BAT_PIN);
  return (raw / 4095.0f) * 3.3f * BAT_DIVIDER;
}

int batBars(float v) {
  int bars = 0;
  for (int i = 0; i < 5; i++)
    if (v >= BAT_THRESH[i]) bars = i + 1;
  return bars;
}


WebServer server(80);

#include "html_page.h"
#include "config_page.h"


// ── Handlers ──────────────────────────────────────────────────────────────────

void handleRoot() {
  server.sendHeader("Connection", "close");
  server.send_P(200, "text/html", HTML_PAGE);
}

void handleHeading() {
  server.sendHeader("Connection", "close");
  server.sendHeader("Cache-Control", "no-cache, no-store");
  float bv = readBatV();
  int bb = batBars(bv);
  String json = "{\"heading\":" + String(currentHeading, 1) + ",\"mode\":\"" + currentMode + "\"" + ",\"target\":" + String(targetHeading, 1) + ",\"bv\":" + String(bv, 2) + ",\"bb\":" + String(bb) + ",\"offset\":" + String(headingOffset, 1) + "}";
  server.send(200, "application/json", json);
}

void handleSet() {
  server.sendHeader("Connection", "close");
  if (server.hasArg("mode")) {
    String m = server.arg("mode");
    if (m == "normal" || m == "lazyspin" || m == "erraticspin" || m == "fixed") {
      currentMode = m;
      Serial.println("Mode: " + currentMode);
    }
  }
  if (server.hasArg("deg")) {
    float d = server.arg("deg").toFloat();
    if (d >= 0.0f && d <= 359.9f) {
      targetHeading = d;
      currentMode = "fixed";
      Serial.println("Fixed bearing: " + String(targetHeading, 1));
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleConfig() {
  server.sendHeader("Connection", "close");
  server.send_P(200, "text/html", CONFIG_PAGE);
}

void handleSetOffset() {
  server.sendHeader("Connection", "close");
  if (server.hasArg("reset")) {
    headingOffset = 0.0f;
  } else if (server.hasArg("delta")) {
    headingOffset += server.arg("delta").toFloat();
  }
  server.send(200, "text/plain", String(headingOffset, 1));
}

void handleSetBias() {
  server.sendHeader("Connection", "close");
  if (server.hasArg("cw"))  cwBias  = constrain(cwBias  + server.arg("cw").toFloat(),  0.5f, 2.0f);
  if (server.hasArg("ccw")) ccwBias = constrain(ccwBias + server.arg("ccw").toFloat(), 0.5f, 2.0f);
  String r = String(cwBias, 2) + "," + String(ccwBias, 2);
  server.send(200, "text/plain", r);
}

void handleZeroDial() {
  server.sendHeader("Connection", "close");
  dialBearing = 0.0f;
  server.send(200, "text/plain", "OK");
}

void Lazy() {
  int action = random(4);

  if (action == 0) {
    // Long drift: one wide slow arc, no interruption
    bool cw = random(2);
    int ms = random(500, 1400);
    dial.writeMicroseconds(cw ? SLOW_CW : SLOW_CCW);
    delay(ms);
    trackMove(cw, cw ? DEG_PER_MS_SLOW_CW : DEG_PER_MS_SLOW_CCW, ms);

  } else if (action == 1) {
    // Sweep and settle: arc out, hang at the far end, drift partway back
    bool cw = random(2);
    int ms1 = random(400, 900);  // outward sweep
    int ms2 = random(200, 500);  // partial return — shorter, doesn't fully retrace
    dial.writeMicroseconds(cw ? SLOW_CW : SLOW_CCW);
    delay(ms1);
    trackMove(cw, cw ? DEG_PER_MS_SLOW_CW : DEG_PER_MS_SLOW_CCW, ms1);
    dial.writeMicroseconds(STOP_US);
    delay(random(100, 400));  // hangs at the apex of the arc
    dial.writeMicroseconds(cw ? SLOW_CCW : SLOW_CW);
    delay(ms2);
    trackMove(!cw, !cw ? DEG_PER_MS_SLOW_CW : DEG_PER_MS_SLOW_CCW, ms2);

  } else if (action == 2) {
    // Build and fade: medium speed entry that bleeds into a slow trailing drift
    bool cw = random(2);
    int ms1 = random(150, 350);  // faster leading edge
    int ms2 = random(400, 800);  // slow trailing arc, same direction
    dial.writeMicroseconds(cw ? MED_CW : MED_CCW);
    delay(ms1);
    trackMove(cw, cw ? DEG_PER_MS_MED_CW : DEG_PER_MS_MED_CCW, ms1);
    dial.writeMicroseconds(cw ? SLOW_CW : SLOW_CCW);  // same direction, bleeds slower
    delay(ms2);
    trackMove(cw, cw ? DEG_PER_MS_SLOW_CW : DEG_PER_MS_SLOW_CCW, ms2);

  } else {
    // Dreamy pause: needle stops and lingers
    dial.writeMicroseconds(STOP_US);
    delay(random(300, 800));
  }

  dial.writeMicroseconds(STOP_US);
  delay(random(80, 200));  // longer rest than erratic — adds to the languid feel
}

void Normal() {
  if (random(4) == 0) {
    bool cw = random(2);
    int ms = random(25, 65);
    dial.writeMicroseconds(cw ? SLOW_CW : SLOW_CCW);
    delay(ms);
    dial.writeMicroseconds(STOP_US);
    trackMove(cw, cw ? DEG_PER_MS_SLOW_CW : DEG_PER_MS_SLOW_CCW, ms);
  }

  /* -- OLD FOR REFERENCE--
  // returnToHeading() has already slewed the dial to live north.
  // A small random tremor makes the needle look alive — like a real compass settling.
  // returnToHeading() corrects any accumulated drift on the next loop iteration.
  if (random(4) == 0) {
    bool cw = random(2);
    int ms = random(25, 65);
    dial.writeMicroseconds(cw ? SLOW_CW : SLOW_CCW);
    delay(ms);
    dial.writeMicroseconds(STOP_US);
    trackMove(cw, cw ? DEG_PER_MS_SLOW_CW : DEG_PER_MS_SLOW_CCW, ms);
  }
  dialBearing = currentHeading;  // ← add this
  */
}

void Fixed() {
  // Needle is parked at targetHeading by returnToHeading().
  // A navigation bearing should look locked — no tremor.
  dial.writeMicroseconds(STOP_US);
}

void Erratic() {
  int action = random(4);

  if (action == 0) {
    // Jolt
    bool cw = random(2);
    int ms = random(60, 200);  // capture before use
    dial.writeMicroseconds(cw ? FAST_CW : FAST_CCW);
    delay(ms);
    trackMove(cw, cw ? DEG_PER_MS_FAST_CW : DEG_PER_MS_FAST_CCW, ms);

  } else if (action == 1) {
    // Snap-reverse
    bool cw = random(2);
    int ms1 = random(80, 180);
    int ms2 = random(100, 250);
    dial.writeMicroseconds(cw ? FAST_CW : FAST_CCW);
    delay(ms1);
    trackMove(cw, cw ? DEG_PER_MS_FAST_CW : DEG_PER_MS_FAST_CCW, ms1);
    dial.writeMicroseconds(cw ? FAST_CCW : FAST_CW);
    delay(ms2);
    trackMove(!cw, !cw ? DEG_PER_MS_FAST_CW : DEG_PER_MS_FAST_CCW, ms2);  // reversed direction

  } else if (action == 2) {
    // Tremble
    int count = random(4, 10);
    for (int i = 0; i < count; i++) {
      bool cw = (i % 2 == 0);
      dial.writeMicroseconds(cw ? MED_CW : MED_CCW);
      delay(40);
      trackMove(cw, cw ? DEG_PER_MS_MED_CW : DEG_PER_MS_MED_CCW, 40);
    }

  } else {
    // Freeze — no movement, nothing to track
    dial.writeMicroseconds(STOP_US);
    delay(random(100, 500));
  }

  dial.writeMicroseconds(STOP_US);
  delay(random(20, 80));
}

void returnToHeading(float targetDeg) {
  // Shortest path: CW or CCW?
  float diff = fmod(targetDeg - dialBearing + 360.0f, 360.0f);
  bool cw = (diff <= 180.0f);
  if (!cw) diff = 360.0f - diff;  // always positive arc

  float rate = cw ? DEG_PER_MS_MED_CW : DEG_PER_MS_MED_CCW;
  int ms = (int)(diff / rate);
  if (ms > 10) {  // skip sub-10ms nudges
    dial.writeMicroseconds(cw ? MED_CW : MED_CCW);
    delay(ms);
    dial.writeMicroseconds(STOP_US);
  }
  dialBearing = targetDeg;  // dead reckoning says we're here now
}


void enterSleep() {
  //Shore delay needed?
  // TODO - Save whichever angle the dial is currently showing
  dial.detach();  // cut PWM signal — servo draws near-zero current
  Serial.flush();
  Serial.end();
  WiFi.setSleep(true);

  // Wake when lid opens (LID_PIN goes LOW)
  gpio_wakeup_enable((gpio_num_t)LID_PIN, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  delay(50);  // debounce before sleeping
  esp_light_sleep_start();

  // ── Execution resumes here on wake ──────────────────────────
  WiFi.disconnect(true);  // full teardown of any previous state
  WiFi.mode(WIFI_OFF);    // force a clean mode reset
  delay(100);
  WiFi.mode(WIFI_AP);
  delay(100);
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(200);
  server.begin();
  Serial.begin(115200);
  dial.attach(SERVO_PIN);
}


// ── Setup ─────────────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  Serial.setTxTimeoutMs(0);  // ← don't block if no Serial monitor connected
  delay(500);

  Serial.println("\n=== Magnetic Traverse Compass ===");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/heading", HTTP_GET, handleHeading);
  server.on("/set", HTTP_GET, handleSet);
  server.on("/config", HTTP_GET, handleConfig);
  server.on("/setoffset", HTTP_GET, handleSetOffset);
  server.on("/setbias", HTTP_GET, handleSetBias);

  server.onNotFound([]() {
    server.sendHeader("Connection", "close");
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("Server ready — http://192.168.4.1");

  randomSeed(esp_random());
  ESP32PWM::allocateTimer(0);
  dial.setPeriodHertz(50);
  dial.attach(SERVO_PIN, 500, 2500);

  Wire.begin(6, 7);  // Adapt SDA, SCL
  mag.begin();
  // INSERT "HARD-IRON" CALIBRATION COMMAND HERE
  //mag.setHardIronOffsets(0.079f, 0.403f); Old pre assembly vallues
  mag.setHardIronOffsets(0.231f, -0.624f);

  pinMode(LID_PIN, INPUT_PULLDOWN);
}

// ── Loop ──────────────────────────────────────────────────────────────────────

void loop() {
  //Handle any webserver traffic
  server.handleClient();

  //Code snippet to read the compass
  if (millis() - lastHeadingUpdate >= 1000) {
    lastHeadingUpdate = millis();
    float xyz[3];
    bool ok = mag.readXYZ(xyz);
    if (ok) {
      xyz[0] *= SCALE_AVG / SCALE_X;
      xyz[1] *= SCALE_AVG / SCALE_Y;
      currentHeading = fmod(mag.getHeadingDeg(Declination) + headingOffset + SENSOR_ROTATION + 3600.0f, 360.0f);
      float candidate = snapToCardinal(currentHeading);
      if (angDiff(candidate, snappedHeading) > 1.0f)
        snappedHeading = candidate;
      Serial.printf("Heading: %3.0f°\n", currentHeading);
    } else {
      Serial.println("mag read FAILED");  // ← tells you immediately if I2C is the problem
    }
    Serial.println(currentMode);
  }

  //Define normal operation
  if (currentMode == "normal") {
    //Serial.println("Mode = Normal");
    returnToHeading(snappedHeading);
    Normal();

  } else if (currentMode == "lazyspin") {
    //Serial.println("Mode = Lazy Spin");
    Lazy();
  }

  else if (currentMode == "erraticspin") {
    //Serial.println("Mode = Erratic Spin");
    Erratic();
  }

  else if (currentMode == "fixed") {
    //Serial.println("Mode = Fixed");
    returnToHeading(targetHeading);  //Potential previous state logic here
    Fixed();
  }




  //code snippet to check for sleep
  if (digitalRead(LID_PIN) == HIGH) {
    enterSleep();
    return;
  }
}
