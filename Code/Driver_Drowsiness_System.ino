#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "LOGESH";
const char* password = "123456789";

ESP8266WebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int LCD_SDA = D2;
const int LCD_SCL = D1;

const int ENA = D7;
const int IN1 = D0;
const int IN2 = D4;

const int MQ3_PIN = A0;
const int EYE_PIN = D3;

const int TRIG_PIN = D5;
const int ECHO_PIN = D6;

const int RED_LED = D8;
const int BUZZER = D2;   // change to a truly free pin if LCD SDA is used

int alcoholThreshold1 = 500;
int alcoholThreshold2 = 650;

const unsigned long eyeCloseLimit = 5000UL;
const unsigned long stepInterval = 1000UL;

const int speedSteps[] = {100, 85, 70, 55, 40, 25, 10, 5, 0};
const int numSteps = sizeof(speedSteps) / sizeof(speedSteps[0]);

const float stopDistanceCm = 10.0;

int alcoholRaw = 0;
int eyeValue = LOW;
float distanceCm = -1.0;

bool eyeClosed = false;
unsigned long eyeClosedStart = 0;
unsigned long lastStepTime = 0;
int eyeStepIndex = 0;

String alcoholStatus = "SAFE";
String eyeStatus = "OPEN";
String vehicleStatus = "RUNNING";
String motorStatus = "RUNNING";
String ledRedStatus = "OFF";

int motorSpeedPercent = 100;
unsigned long lastSensorTime = 0;
unsigned long lastLcdTime = 0;

// Manual override flag
bool manualStop = false;

void setMotorSpeed(int percent) {
  percent = constrain(percent, 0, 100);
  int duty = map(percent, 0, 100, 0, 1023);
  analogWrite(ENA, duty);
}

float measureDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (duration == 0) return -1.0;

  return duration * 0.0343 / 2.0;
}

void readSensors() {
  alcoholRaw = analogRead(MQ3_PIN);

  if (alcoholRaw > alcoholThreshold2) alcoholStatus = "DRUNK";
  else if (alcoholRaw > alcoholThreshold1) alcoholStatus = "DETECTED";
  else alcoholStatus = "SAFE";

  eyeValue = digitalRead(EYE_PIN);

  if (eyeValue == HIGH) {
    if (!eyeClosed) {
      eyeClosed = true;
      eyeClosedStart = millis();
      lastStepTime = millis();
      eyeStepIndex = 0;
    }
    eyeStatus = "CLOSED";
  } else {
    eyeClosed = false;
    eyeClosedStart = 0;
    lastStepTime = 0;
    eyeStepIndex = 0;
    eyeStatus = "OPEN";
  }

  distanceCm = measureDistanceCm();
}

int getDistanceSpeed(float d) {
  if (d < 0) return 100;
  if (d <= stopDistanceCm) return 0;
  if (d <= 15) return 5;
  if (d <= 20) return 10;
  if (d <= 25) return 25;
  if (d <= 30) return 40;
  if (d <= 35) return 55;
  if (d <= 40) return 70;
  if (d <= 50) return 85;
  return 100;
}

void updateEyeSpeed() {
  if (!eyeClosed) return;

  unsigned long elapsed = millis() - eyeClosedStart;

  if (elapsed < eyeCloseLimit) {
    return;
  }

  if (millis() - lastStepTime >= stepInterval) {
    lastStepTime = millis();
    if (eyeStepIndex < numSteps - 1) eyeStepIndex++;
  }
}

void controlSystem() {
  // If manual STOP is pressed, force motor off
  if (manualStop) {
    motorSpeedPercent = 0;
    vehicleStatus = "STOPPED";
    motorStatus = "STOPPED";
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    setMotorSpeed(0);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    ledRedStatus = "ON";
    return;
  }

  bool drunkStop = (alcoholRaw > alcoholThreshold2);

  int alcoholSpeed = 100;
  if (alcoholRaw > alcoholThreshold2) alcoholSpeed = 0;
  else if (alcoholRaw > alcoholThreshold1) alcoholSpeed = 60;

  int distanceSpeed = getDistanceSpeed(distanceCm);

  updateEyeSpeed();

  int eyeSpeed = 100;
  if (eyeClosed && millis() - eyeClosedStart >= eyeCloseLimit) {
    eyeSpeed = speedSteps[eyeStepIndex];
  }

  motorSpeedPercent = min(alcoholSpeed, min(distanceSpeed, eyeSpeed));

  if (drunkStop) motorSpeedPercent = 0;

  if (motorSpeedPercent == 0) {
    vehicleStatus = "STOPPED";
    motorStatus = "STOPPED";
  } else {
    vehicleStatus = "RUNNING";
    motorStatus = "RUNNING";
  }

  if (motorSpeedPercent == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }

  setMotorSpeed(motorSpeedPercent);

  bool red = (motorSpeedPercent == 0);
  digitalWrite(RED_LED, red ? HIGH : LOW);
  digitalWrite(BUZZER, red ? HIGH : LOW);
  ledRedStatus = red ? "ON" : "OFF";
}

String getHtml() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Driver Protection System</title>
<style>
body{font-family:Arial;background:#eef2ff;margin:0;padding:16px;color:#111}
.container{max-width:900px;margin:auto}
.header{background:#111827;color:#fff;padding:16px;border-radius:14px;text-align:center;font-size:24px;font-weight:bold}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:12px;margin-top:14px}
.card{background:#fff;border-radius:14px;padding:14px;box-shadow:0 4px 14px rgba(0,0,0,.08)}
.label{font-size:14px;color:#555}
.value{font-size:24px;font-weight:bold;margin-top:6px}
.warn{border-left:8px solid #f59e0b}
.controls{margin-top:16px;display:flex;gap:12px;justify-content:center}
.btn{padding:12px 24px;border:none;border-radius:10px;font-size:18px;font-weight:bold;cursor:pointer;color:#fff}
.btn-run{background:#22c55e}
.btn-stop{background:#ef4444}
.btn:disabled{opacity:0.4;cursor:not-allowed}
@media(max-width:640px){.grid{grid-template-columns:1fr}}
</style>
</head>
<body>
<div class="container">
<div class="header">Driver Protection System</div>
<div class="grid">
<div class="card"><div class="label">Alcohol Status</div><div class="value" id="alcoholStatus">--</div></div>
<div class="card"><div class="label">Alcohol Value</div><div class="value" id="alcoholRaw">--</div></div>
<div class="card"><div class="label">Eye Status</div><div class="value" id="eyeStatus">--</div></div>
<div class="card"><div class="label">Distance</div><div class="value" id="distanceCm">--</div></div>
<div class="card"><div class="label">Motor Speed</div><div class="value" id="motorSpeedPercent">--</div></div>
<div class="card"><div class="label">Motor Status</div><div class="value" id="motorStatus">--</div></div>
<div class="card"><div class="label">Red LED</div><div class="value" id="redLED">--</div></div>
<div class="card warn" style="grid-column:1/-1"><div class="label">Vehicle Status</div><div class="value" id="vehicleStatus">--</div></div>
</div>

<div class="controls">
<button class="btn btn-run" id="runBtn" onclick="sendCommand('run')">RUN</button>
<button class="btn btn-stop" id="stopBtn" onclick="sendCommand('stop')">STOP</button>
</div>
</div>
<script>
async function updateData(){
  const r = await fetch('/data');
  const d = await r.json();
  document.getElementById('alcoholStatus').innerText = d.alcoholStatus;
  document.getElementById('alcoholRaw').innerText = d.alcoholRaw;
  document.getElementById('eyeStatus').innerText = d.eyeStatus;
  document.getElementById('distanceCm').innerText = d.distanceText;
  document.getElementById('motorSpeedPercent').innerText = d.motorSpeedPercent + ' %';
  document.getElementById('motorStatus').innerText = d.motorStatus;
  document.getElementById('vehicleStatus').innerText = d.vehicleStatus;
  document.getElementById('redLED').innerText = d.redLED;
}
async function sendCommand(cmd){
  await fetch('/cmd?act=' + cmd, { cache: 'no-store' });
  updateData();
}
setInterval(updateData,1000);
updateData();
</script>
</body>
</html>
)rawliteral";
}

void handleRoot() {
  server.send(200, "text/html", getHtml());
}

void handleData() {
  String distanceText = (distanceCm < 0) ? "NO ECHO" : String(distanceCm / 100.0, 2) + " m";

  String json = "{";
  json += "\"alcoholRaw\":" + String(alcoholRaw) + ",";
  json += "\"alcoholStatus\":\"" + alcoholStatus + "\",";
  json += "\"eyeStatus\":\"" + eyeStatus + "\",";
  json += "\"distanceText\":\"" + distanceText + "\",";
  json += "\"motorSpeedPercent\":" + String(motorSpeedPercent) + ",";
  json += "\"motorStatus\":\"" + motorStatus + "\",";
  json += "\"vehicleStatus\":\"" + vehicleStatus + "\",";
  json += "\"redLED\":\"" + ledRedStatus + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

// Handle STOP / RUN commands from web page
void handleCommand() {
  if (server.hasArg("act")) {
    String act = server.arg("act");
    if (act == "stop") {
      manualStop = true;
    } else if (act == "run") {
      manualStop = false;
    }
  }
  server.send(200, "text/plain", "OK");
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("E:");
  lcd.print(eyeStatus);
  lcd.print(" D:");
  if (distanceCm < 0) lcd.print("NO");
  else lcd.print(distanceCm / 100.0, 2);

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(motorSpeedPercent);
  lcd.print("% ");
  lcd.print(alcoholStatus);
}

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(EYE_PIN, INPUT);
  pinMode(MQ3_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  analogWriteRange(1023);
  analogWriteFreq(1000);

  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Starting...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 50) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.print("WiFi FAILED");
    Serial.println("WiFi connection FAILED");
    while (true) {
      delay(1000);
    }
  }

  Serial.print("WiFi connected, IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/cmd", handleCommand);  // STOP / RUN button handler
  server.begin();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);
  lcd.clear();
}

void loop() {
  server.handleClient();

  if (millis() - lastSensorTime >= 200) {
    lastSensorTime = millis();
    readSensors();
    controlSystem();

    Serial.print("Alcohol=");
    Serial.print(alcoholRaw);
    Serial.print(" Eye=");
    Serial.print(eyeStatus);
    Serial.print(" Dist=");
    if (distanceCm < 0) Serial.print("NO ECHO");
    else Serial.print(distanceCm / 100.0, 2);
    Serial.print(" m Motor=");
    Serial.println(motorSpeedPercent);
  }

  if (millis() - lastLcdTime >= 500) {
    lastLcdTime = millis();
    updateLCD();
  }
}