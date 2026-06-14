#include <WiFi.h>
#include <WebServer.h>
#include "max6675.h"

int thermoDO1 = 19;
int thermoCS1 = 23;
int thermoCLK1 = 5;
// Test

int thermoDO2 = 12;
int thermoCS2 = 13;
int thermoCLK2 = 15;

// Hotspot-SSID und Passwort
const char* ssid = "Toms iphone 14";  // Ersetze dies durch die SSID des iPhone-Hotspots Vodafone-B96C_24GH
const char* password = "Tom_12345";  // Ersetze dies durch das Passwort des Hotspots kegmPz3PRPaX7RM

MAX6675 thermocouple1(thermoCLK1, thermoCS1, thermoDO1);
MAX6675 thermocouple2(thermoCLK2, thermoCS2, thermoDO2);

WebServer server(80);

// Variablen zum Speichern der letzten gemessenen Temperaturen
float lastTemp1 = 0.0;
float lastTemp2 = 0.0;

// Variablen zur Speicherung des Rauchstatus
String smokeStatus = "Kein Rauch";
bool recording = false; // Flag, um die Aufnahme zu steuern
unsigned long startTime = 0; // Zeit, wann die Aufnahme gestartet wurde
String csvData = ""; // Variable zur Speicherung der CSV-Daten

void setup() {
  Serial.begin(9600);

  // Verbindung zum iPhone-Hotspot herstellen
  // Erzwingt, dass der ESP32-S3 moderne WPA3/PMF-Verbindungen akzeptiert
  WiFi.setMinSecurity(WIFI_AUTH_WPA2_PSK);
  WiFi.begin(ssid, password);
  Serial.print("Verbindung zum WLAN wird hergestellt");

  // Warte, bis die Verbindung hergestellt ist
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Versuch fehlgeschlagen.");
  }

  Serial.println("");
  Serial.println("WLAN verbunden!");
  Serial.print("IP Adresse: ");
  Serial.println(WiFi.localIP());

  // Webserver initialisieren
  server.on("/", handleRoot);
  server.on("/temperature", handleTemperature);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.on("/setSmoke", handleSetSmoke);
  server.on("/download", handleDownload);

  server.begin();
  Serial.println("Webserver gestartet");
}

void loop() {
  server.handleClient();  // Webserver-Client bearbeiten
}

// Funktion zum Rendern der Webseite
void handleRoot() {
  String html = "<html>"
                "<head>"
                "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<style>"
                "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #121212; color: #e0e0e0; margin: 0; padding: 20px; }"
                "h1 { color: #ffffff; text-align: center; margin-bottom: 30px; font-weight: 300; letter-spacing: 1px; }"
                ".container { max-width: 650px; margin: auto; padding: 30px; border-radius: 12px; background-color: #1e1e1e; box-shadow: 0 8px 30px rgba(0, 0, 0, 0.5); border: 1px solid #333; }"
                ".data-panel { display: flex; justify-content: space-around; background-color: #2a2a2a; padding: 15px; border-radius: 8px; margin-bottom: 20px; }"
                ".data-item { text-align: center; }"
                ".data-label { font-size: 14px; color: #888; margin-bottom: 5px; }"
                ".data-value { font-size: 24px; font-weight: bold; color: #4CAF50; }"
                "button { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555; padding: 12px 18px; border-radius: 6px; cursor: pointer; margin: 5px; font-size: 14px; transition: all 0.3s ease; }"
                "button:hover { background-color: #555; border-color: #777; transform: translateY(-2px); }"
                "button.primary { background-color: #2196F3; border-color: #2196F3; }"
                "button.primary:hover { background-color: #1976D2; }"
                "button.danger { background-color: #f44336; border-color: #f44336; }"
                "button.danger:hover { background-color: #d32f2f; }"
                "button.success { background-color: #4CAF50; border-color: #4CAF50; }"
                "button.success:hover { background-color: #388E3C; }"
                ".status-container { background-color: #2a2a2a; padding: 15px; border-radius: 8px; margin-bottom: 20px; text-align: center; }"
                ".status { margin: 0; font-size: 18px; color: #ccc; }"
                "#smokeStatus { font-weight: bold; color: #ffeb3b; }"
                ".controls-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 10px; margin-bottom: 20px; }"
                "#timer { font-size: 20px; color: #f44336; text-align: center; margin: 15px 0; font-weight: bold; min-height: 24px; }"
                "#myChart { width: 100%; height: 400px; background-color: #1e1e1e; border-radius: 8px; padding: 10px; box-sizing: border-box; }"
                "</style>"
                "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
                "<script>"
                "Chart.defaults.color = '#aaa';"
                "Chart.defaults.borderColor = '#333';"
                "let recordingTime = 0;"
                "let timerInterval = null;"
                "let chart = null;"
                "let temperatureData1 = [];"
                "let temperatureData2 = [];"
                "let labels = [];"
                "let smokeStatus = '" + smokeStatus + "';"
                "let backgroundColor1 = 'rgba(75, 192, 192, 0.2)';"
                "let backgroundColor2 = 'rgba(153, 102, 255, 0.2)';"

                "function initChart() {"
                "  let ctx = document.getElementById('myChart').getContext('2d');"
                "  chart = new Chart(ctx, {"
                "    type: 'line',"
                "    data: {"
                "      labels: labels,"
                "      datasets: [{"
                "        label: 'Temp 1 (°C)',"
                "        data: temperatureData1,"
                "        backgroundColor: backgroundColor1,"
                "        borderColor: 'rgba(75, 192, 192, 1)',"
                "        borderWidth: 2,"
                "        pointRadius: 3,"
                "        pointBackgroundColor: 'rgba(75, 192, 192, 1)',"
                "        fill: true,"
                "        tension: 0.4"
                "      }, {"
                "        label: 'Temp 2 (°C)',"
                "        data: temperatureData2,"
                "        backgroundColor: backgroundColor2,"
                "        borderColor: 'rgba(153, 102, 255, 1)',"
                "        borderWidth: 2,"
                "        pointRadius: 3,"
                "        pointBackgroundColor: 'rgba(153, 102, 255, 1)',"
                "        fill: true,"
                "        tension: 0.4"
                "      }]"
                "    },"
                "    options: {"
                "      responsive: true,"
                "      maintainAspectRatio: false,"
                "      plugins: {"
                "        legend: { labels: { color: '#e0e0e0', font: { size: 14 } } },"
                "        tooltip: { mode: 'index', intersect: false, backgroundColor: 'rgba(0,0,0,0.8)', titleColor: '#fff', bodyColor: '#ccc' }"
                "      },"
                "      scales: {"
                "        x: {"
                "          title: { display: true, text: 'Zeit (s)', color: '#888' },"
                "          grid: { color: '#333' },"
                "          ticks: { color: '#aaa' }"
                "        },"
                "        y: {"
                "          title: { display: true, text: 'Temperatur (°C)', color: '#888' },"
                "          grid: { color: '#333' },"
                "          ticks: { color: '#aaa' }"
                "        }"
                "      }"
                "    }"
                "  });"
                "}"

                "function updateTemperature() {"
                "  var xhttp = new XMLHttpRequest();"
                "  xhttp.onreadystatechange = function() {"
                "    if (this.readyState == 4 && this.status == 200) {"
                "      var temp = JSON.parse(this.responseText);"
                "      labels.push(recordingTime + 's');"
                "      temperatureData1.push(temp.temp1);"
                "      temperatureData2.push(temp.temp2);"
                "      chart.update();"
                "      document.getElementById('temp1').innerHTML = temp.temp1 + ' °C';"
                "      document.getElementById('temp2').innerHTML = temp.temp2 + ' °C';"
                "    }"
                "  };"
                "  xhttp.open('GET', '/temperature', true);"
                "  xhttp.send();"
                "}"

                "function setSmoke(status) {"
                "  var xhttp = new XMLHttpRequest();"
                "  xhttp.open('GET', '/setSmoke?status=' + status, true);"
                "  xhttp.send();"
                "  document.getElementById('smokeStatus').innerHTML = status;"
                "  smokeStatus = status;"
                "  updateBackgroundColor(status);"
                "}"

                "function updateBackgroundColor(status) {"
                "  if (status === 'Kein Rauch') {"
                "    backgroundColor1 = 'rgba(75, 192, 192, 0.2)';"
                "    backgroundColor2 = 'rgba(153, 102, 255, 0.2)';"
                "  } else if (status === 'Leichter Rauch') {"
                "    backgroundColor1 = 'rgba(255, 206, 86, 0.2)';"
                "    backgroundColor2 = 'rgba(255, 159, 64, 0.2)';"
                "  } else if (status === 'Optimaler Rauch') {"
                "    backgroundColor1 = 'rgba(255, 99, 132, 0.2)';"
                "    backgroundColor2 = 'rgba(54, 162, 235, 0.2)';"
                "  }"
                "  chart.data.datasets[0].backgroundColor = backgroundColor1;"
                "  chart.data.datasets[1].backgroundColor = backgroundColor2;"
                "  chart.update();"
                "}"

                "setInterval(updateTemperature, 2000);"
                "function startRecording() {"
                "  var xhttp = new XMLHttpRequest();"
                "  xhttp.open('GET', '/start', true);"
                "  xhttp.send();"
                "  recordingTime = 0;"
                "  timerInterval = setInterval(function() { recordingTime++; document.getElementById('timer').innerHTML = 'Aufnahmezeit: ' + recordingTime + 's'; }, 1000);"
                "}"
                "function stopRecording() {"
                "  var xhttp = new XMLHttpRequest();"
                "  xhttp.open('GET', '/stop', true);"
                "  xhttp.send();"
                "  clearInterval(timerInterval);"
                "  document.getElementById('timer').innerHTML = '';"
                "}"
                "function downloadCSV() {"
                "  window.location.href = '/download';"
                "}"
                "window.onload = function() { initChart(); };"
                "</script>"
                "</head>"
                "<body>"
                "<div class='container'>"
                "<h1>Sensor Dashboard</h1>"

                "<div class='data-panel'>"
                "  <div class='data-item'>"
                "    <div class='data-label'>Thermoelement 1</div>"
                "    <div class='data-value' id='temp1'>-- °C</div>"
                "  </div>"
                "  <div class='data-item'>"
                "    <div class='data-label'>Thermoelement 2</div>"
                "    <div class='data-value' id='temp2' style='color: #9c27b0;'>-- °C</div>"
                "  </div>"
                "</div>"

                "<div class='status-container'>"
                "  <h2 class='status'>Aktueller Rauchstatus: <span id='smokeStatus'>" + smokeStatus + "</span></h2>"
                "</div>"

                "<div class='controls-grid'>"
                "  <button onclick=\"setSmoke('Kein Rauch')\">Kein Rauch</button>"
                "  <button onclick=\"setSmoke('Leichter Rauch')\">Leichter Rauch</button>"
                "  <button onclick=\"setSmoke('Optimaler Rauch')\">Optimaler Rauch</button>"
                "</div>"

                "<div class='controls-grid' style='margin-top: 15px;'>"
                "  <button class='success' onclick=\"startRecording()\">▶ Aufnahme Start</button>"
                "  <button class='danger' onclick=\"stopRecording()\">■ Aufnahme Stop</button>"
                "  <button class='primary' onclick=\"downloadCSV()\">↓ CSV Download</button>"
                "</div>"

                "<div id='timer'></div>"

                "<div style='position: relative; height: 400px; width: 100%;'>"
                "  <canvas id='myChart'></canvas>"
                "</div>"

                "</div>"
                "</body>"
                "</html>";
  server.send(200, "text/html", html);
}

// Temperaturwerte als JSON senden
void handleTemperature() {
  float temp1 = thermocouple1.readCelsius();
  float temp2 = thermocouple2.readCelsius();

  if (recording) {
    unsigned long currentTime = (millis() - startTime) / 1000;
    csvData += String(currentTime) + "," + String(temp1) + "," + String(temp2) + "," + smokeStatus + "\n";
  }

  String json = "{\"temp1\": " + String(temp1) + ", \"temp2\": " + String(temp2) + "}";
  server.send(200, "application/json", json);
}

// Start der Aufnahme
void handleStart() {
  recording = true;
  startTime = millis();
  csvData = "Zeit (s),Temperatur 1 (°C),Temperatur 2 (°C),Rauchstatus\n";  // CSV-Kopfzeile
  server.send(200, "text/plain", "Aufnahme gestartet");
}

// Stoppen der Aufnahme
void handleStop() {
  recording = false;
  server.send(200, "text/plain", "Aufnahme gestoppt");
}

// Rauchstatus setzen
void handleSetSmoke() {
  if (server.hasArg("status")) {
    smokeStatus = server.arg("status");
    server.send(200, "text/plain", "Rauchstatus aktualisiert");
  } else {
    server.send(400, "text/plain", "Fehler: Kein Rauchstatus angegeben");
  }
}

// CSV-Datei zum Download anbieten
void handleDownload() {
  server.send(200, "text/csv", csvData);
}