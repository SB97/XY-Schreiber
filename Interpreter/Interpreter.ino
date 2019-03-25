/*
   https://www.marginallyclever.com/2013/08/how-to-build-an-2-axis-arduino-cnc-gcode-interpreter/
   Der Interpreter soll per Serieller verbindung GCODE empfangen und auswerten.

   DAC: http://henrysbench.capnfatz.com/henrys-bench/arduino-output-devices/arduino-mcp4725-digital-to-analog-converter-tutorial/
*/

#include <Wire.h>
#include <Adafruit_MCP4725.h>

// Konstanten
#define BAUD (57600) // Baudrate 
#define MAX_BUF (64) // maximale länge der eingehenden Nachrichten

#define X_AUFLOESUNG (4096)
#define X_LAENGE (200) // mm
#define X_TRAVEL_TIME (5000) // ms
#define Y_AUFLOESUNG (4096/2)
#define Y_LAENGE (150) // mm
#define Y_TRAVEL_TIME (4000) // ms

char buffer[MAX_BUF]; // zwischenablage
int bufferAuslastung; // belegung der zwischenablage

// DAC
Adafruit_MCP4725 dacX; // X Konstruktor
Adafruit_MCP4725 dacY; // Y Konstruktor

// Plotter
#define PEN_PIN (2) // Pin für Pen up/down steuerung
float penZ = 0; // aktuelle Z "höhe" für vergleich von neuem Wert aus GCODE
float posX = 0; // start X Position -> aktuelle Position
float posY = 0; // start Y Position -> aktuelle Position

/**
   X Achsen bewegung
*/
void driveX(float xTarget) {
  float neuePos = posX + xTarget;
  int dacValue = (X_AUFLOESUNG / X_LAENGE) * neuePos;
  dacX.setVoltage(dacValue, false);
  long d = (long) ((X_AUFLOESUNG / X_TRAVEL_TIME) * (neuePos * -1);
  delay(d);
}

/**
   Y Achsen bewegung
*/
void driveY(float yTarget) {
  float neuePos = posY + yTarget;
  int dacValue = (Y_AUFLOESUNG / Y_LAENGE) * neuePos;
  dacY.setVoltage(dacValue, false);
  long d = (long) ((Y_AUFLOESUNG / Y_TRAVEL_TIME) * neuePos * -1);
  delay(d);
}

void setup() {
  // Serielle verbindung starten
  Serial.begin(BAUD);
  Serial.println(("Start!"));
  ready();

  // Pen
  pinMode(PEN_PIN, OUTPUT); // Pen/Z
  dacX.begin(0x62); // X Bus Adresse
  dacY.begin(0x63); // Y Bus Adresse
}

/**
   Vorbereitung auf neuen Serielen Input
*/
void ready() {
  bufferAuslastung = 0; // Nachrichtenspeicher leeren
  Serial.println("> "); // output um zu sehen wann der Arduino bereit für einen Befehl ist
}

/**
   GCODE empfangen und weiterleiten
*/
void loop() {
  // empfangen
  if ( Serial.available()) {

    char c = Serial.read(); // empfang speichern

    if (c == '\r\n') {
      Serial.println("habe 'new line' empfangen!");
    } else if (c == ';') {
      Serial.println("habe '#;#' empfangen!");
    } else {
      Serial.println("habe '" + String(c) + "' empfangen!");
    }

    // speichern
    if ( bufferAuslastung < MAX_BUF ) {
      buffer[bufferAuslastung++] = c; // ????????????????????????
    }

    // Nachrichten ende
    if ( c == ';' ) {

      // debugging
      Serial.println("Befehls String #");
      for (int i = 0; i <= bufferAuslastung; i++) {
        Serial.print(buffer[i]);
      }
      Serial.println("");
      Serial.println("#");

      // Serial.print("Befehl ende!");
      buffer[bufferAuslastung] = 0; // ?????????????????????????

      processCommand(); // Nachricht verarbeiten
      ready();
    }
  }
}

/**
    Funktion um nach bestimmtem char in Buffer suchen und Zahl dahinter zurückzugeben
*/
float getFloatFromAssociatedChar(char suchCode) {
  float errorRueckgabe = -999.99;
  char *ptr = buffer; // start at the beginning of buffer
  while ((long)ptr > 1 && (*ptr) && (long)ptr < (long)buffer + bufferAuslastung) { // walk to the end
    if (*ptr == suchCode) { // if you find code on your walk,
      return atof(ptr + 1); // convert the digits that follow into a float and return it
    }
    ptr = strchr(ptr, ' ') + 1; // take a step from here to the letter after the next space
  }
  return errorRueckgabe; // end reached, nothing found, return default val.
  // keine Ahnung wie das Funktioniert... aber es funktioniert... https://github.com/MarginallyClever/GcodeCNCDemo/blob/master/GcodeCNCDemo2Axis/GcodeCNCDemo2Axis.ino
}

boolean commandContainsChar(char suchCode) {
  Serial.println("Funktion: commandContainsChar");
  int bufferSize = sizeof(buffer);
  for (int i = 0; i < bufferSize; i++) {
    if (buffer[i] == suchCode) {
      return true;
    }
  }
  return false;
}

/*
    Verarbeitung des empfangenen Komandos
*/
void processCommand() {

  // G-code
  int switchZahl = (int) getFloatFromAssociatedChar('G');

  Serial.println("#" + switchZahl);

  switch (switchZahl) {
    case 28:
      if (commandContainsChar('Z')) {
        drivePen(999.99);
        //drivePen(getFloatFromAssociatedChar('Z'));
      }
      /*else if (commandContainsChar('X')) {
        // X 0 fahren
        } else if (commandContainsChar('Y')) {
        // Y 0 fahren
        }
      */
      if (commandContainsChar('X')) {
        driveX(0.0);
      }
      if (commandContainsChar('Y')) {
        driveY(0.0);
      }
    case -999:
      Serial.println("# switch FEHLER!");
      break;
    default:
      Serial.println("# switch DEFAULT!");
      break;
  }
}

/**
   Fährt den Stift hoch oder runter.
*/
void drivePen(float newZ) {
  //Serial.println("Funktion: drivePen");
  if (newZ != penZ) {
    if (newZ > penZ) {
      // Stift hoch fahren
      digitalWrite(PEN_PIN, HIGH);
    } else {
      // Stift runter fahren
      digitalWrite(PEN_PIN, LOW);
    }
    penZ = newZ;
  }
}

void setPosition(float x, float y) {
  posX = x;
  posY = y;
}
