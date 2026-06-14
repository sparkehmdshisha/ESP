# ESP32 Temperatur- und Rauchüberwachungssystem (Dashboard)

Dieses Projekt bietet ein umfassendes, auf einem ESP32 basierendes System zur Überwachung von Temperaturen mithilfe von MAX6675-Thermoelementen. Das System ist darauf ausgelegt, zwei Temperaturquellen simultan zu messen und diese Daten zusammen mit einem benutzerdefinierten „Rauchstatus“ auf einem eingebetteten Web-Dashboard in Echtzeit darzustellen.

Zudem ermöglicht das System die lokale Aufzeichnung von Messwerten über die Zeit und bietet eine direkte Download-Möglichkeit als CSV-Datei zur weiteren Datenanalyse.

---

## 🌟 Hauptfunktionen

- **Dual-Temperaturmessung**: Echtzeit-Auslesung von zwei MAX6675-Sensoren.
- **Integriertes Web-Dashboard (Dark Mode)**: Ein ansprechendes, mobilfreundliches HTML/CSS-Frontend, direkt vom ESP32 gehostet.
- **Echtzeit-Diagramm**: Visuelle Darstellung der Temperaturverläufe beider Sensoren mit Hilfe von `Chart.js`.
- **Rauchstatus-Management**: Manuelle Zuweisung eines aktuellen Status ("Kein Rauch", "Leichter Rauch", "Optimaler Rauch"), welcher in die Datensätze integriert wird.
- **CSV-Datenaufzeichnung**: Start- und Stoppfunktion für die Datenaufzeichnung mit der Möglichkeit, die Ergebnisse als formatierte `.csv`-Datei herunterzuladen.

---

## ⚙️ Hardware-Anforderungen

Für den Aufbau dieses Projekts werden folgende Komponenten benötigt:

- 1x ESP32-Entwicklungsboard (z. B. ESP32-S3 oder ESP32-WROOM)
- 2x MAX6675 Thermoelement-Verstärkermodule (SPI-Schnittstelle)
- 2x K-Typ Thermoelemente (passend für die zu erwartenden Temperaturbereiche)
- Jumper-Kabel und ggf. ein Breadboard für den Versuchsaufbau

---

## 🔌 Pin-Belegung / Verdrahtung (Wiring)

Die Kommunikation mit den MAX6675-Modulen erfolgt über die SPI-Schnittstelle. Die Pins sind im Code wie folgt definiert:

| ESP32 Pin | MAX6675 Sensor 1 | MAX6675 Sensor 2 |
| :--- | :--- | :--- |
| **GPIO 18** | `SCK` (Clock / CLK1) | |
| **GPIO 15** | `CS` (Chip Select / CS1)| |
| **GPIO 2** | `MISO` (Data Out / DO1) | |
| **GPIO 19** | | `SCK` (Clock / CLK2) |
| **GPIO 21** | | `CS` (Chip Select / CS2)|
| **GPIO 22** | | `MISO` (Data Out / DO2) |

*(Hinweis: `VCC` der Module an 3.3V oder 5V des ESP32 anschließen, `GND` an GND des ESP32.)*

---

## 🛠️ Software & Bibliotheken

Um diesen Code erfolgreich kompilieren und hochladen zu können, müssen folgende Bibliotheken in der Arduino IDE (oder in PlatformIO) installiert sein:

1. **MAX6675 Library**: Zum Auslesen der Sensorwerte.
2. **WiFi.h**: (Standardmäßig im ESP32-Core enthalten)
3. **WebServer.h**: (Standardmäßig im ESP32-Core enthalten)

### WLAN-Konfiguration

Im Code (`sketch_oct8b.ino`) müssen die Zugangsdaten für das Netzwerk (z. B. ein Smartphone-Hotspot oder lokaler Router) angepasst werden:

```cpp
const char* ssid = "DEIN_WLAN_NAME";
const char* password = "DEIN_PASSWORT";
```

Das Projekt verwendet `WiFi.setMinSecurity(WIFI_AUTH_WPA2_PSK);`, um moderne WPA3/PMF-Verbindungen (wie bei iOS-Hotspots) zu unterstützen.

---

## 🚀 Inbetriebnahme und Nutzung

1. **Upload:** Den Code über die Arduino IDE auf den ESP32 flashen.
2. **Serieller Monitor:** Den Seriellen Monitor öffnen (Baudrate `9600`).
3. **IP-Adresse:** Warten, bis der ESP32 sich mit dem WLAN verbunden hat. Die zugewiesene lokale IP-Adresse wird im Seriellen Monitor angezeigt.
4. **Dashboard aufrufen:** Diese IP-Adresse in den Browser eines beliebigen Geräts im selben Netzwerk (PC, Smartphone, Tablet) eingeben.

### Das Dashboard
- **Live-Werte & Diagramm:** Oben im Dashboard werden die aktuellen Temperaturen beider Sensoren angezeigt. Das Diagramm aktualisiert sich automatisch alle 2 Sekunden.
- **Rauchstatus-Kontrolle:** Mit den drei Schaltflächen ("Kein Rauch", "Leichter Rauch", "Optimaler Rauch") kann der Zustand im System markiert werden. Die Hintergrundfarbe im Diagramm passt sich subtil an den gewählten Status an.
- **Aufnahmesteuerung:**
  - `▶ Aufnahme Start`: Beginnt mit dem Speichern der Zeit, Temperaturen und des Rauchstatus im Speicher des ESP32.
  - `■ Aufnahme Stop`: Beendet die Aufzeichnung.
  - `↓ CSV Download`: Lädt die gespeicherten Daten als `.csv`-Datei zur Analyse (z.B. in Excel) herunter.

---

## 📄 Struktur der CSV-Datei

Die generierte Datei hat das folgende Format (Trennzeichen: Komma):

```csv
Zeit (s),Temperatur 1 (°C),Temperatur 2 (°C),Rauchstatus
0,150.25,250.50,Optimaler Rauch
2,152.00,251.75,Optimaler Rauch
```

---

## 📝 Lizenz und Hinweise

Dieses Projekt ist für den allgemeinen und industriellen Einsatz im Bereich Temperaturprotokollierung ausgelegt. Da Daten im RAM des ESP32 (als `String`) gespeichert werden, sollte darauf geachtet werden, dass extrem lange Aufnahmen (> mehrere Stunden) möglicherweise zu Speicherengpässen (OOM) auf dem Mikrocontroller führen können. Für Langzeitaufzeichnungen wird das Speichern auf einer SD-Karte oder das Übermitteln der Daten an eine externe Datenbank (z. B. InfluxDB/MQTT) empfohlen.