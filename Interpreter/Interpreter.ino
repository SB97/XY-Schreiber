/*
   https://www.marginallyclever.com/2013/08/how-to-build-an-2-axis-arduino-cnc-gcode-interpreter/
   Der Interpreter soll per Serieller verbindung GCODE empfangen und auswerten.

   DAC: http://henrysbench.capnfatz.com/henrys-bench/arduino-output-devices/arduino-mcp4725-digital-to-analog-converter-tutorial/
*/

#include <Wire.h> // Bus Treiber
#include <Adafruit_MCP4725.h> // DAC Treiber

// Konstanten
#define BAUD (57600) // Baudrate 
#define MAX_BUF (64) // maximale länge der eingehenden Nachrichten

#define X_AUFLOESUNG (4096)
#define X_LAENGE (250) // mm
#define X_TRAVEL_TIME (5000) // ms
#define Y_AUFLOESUNG (3000)
#define Y_LAENGE (200) // mm
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
  long d = (long) ((X_AUFLOESUNG / X_TRAVEL_TIME) * (neuePos * -1));
  delay(d);
  posX = neuePos;
}
/**
   X Achsen bewegung
*/
void driveX2(float xTarget) {
   // float diffX = abs(nX - aX); // entfernung (abs=Absolute value)
  // float steigungX = aX < nX ? 1 : -1; // positive oder negative Richtung
  float neuePos = 0;
  float diff = abs(xTarget - posX);
  if (xTarget > posX) {
    neuePos = (posX + diff);
  } else {
    neuePos = (posX - diff);
  }
  int dacValue = (X_AUFLOESUNG / X_LAENGE) * neuePos;
  dacX.setVoltage(dacValue, false);
  long d = (long) ((X_AUFLOESUNG / X_TRAVEL_TIME) * (neuePos * -1));
  delay(d);
  posX = neuePos;
}

/**
   Y Achsen bewegung
*/
void driveY2(float yTarget) {
   // float diffY = abs(nY - aY); // entfernung (abs=Absolute value)
  // float steigungY = aY < nY ? 1 : -1; // positive oder negative Richtung
  float neuePos = 0;
  float diff = abs(yTarget - posY);
  if (yTarget > posY) {
    neuePos = (posY + diff);
  } else {
    neuePos = (posY - diff);
  }
  int dacValue = (Y_AUFLOESUNG / Y_LAENGE) * neuePos;
  dacY.setVoltage(dacValue, false);
  long d = (long) ((Y_AUFLOESUNG / Y_TRAVEL_TIME) * (neuePos * -1));
  delay(d);
  posY = neuePos;
}

/**
   Y Achsen bewegung
*/
void driveY(float yTarget) {
  float neuePos = posY + yTarget;
  int dacValue = (Y_AUFLOESUNG / Y_LAENGE) * neuePos;
  dacY.setVoltage(dacValue, false);
  long d = (long) ((Y_AUFLOESUNG / Y_TRAVEL_TIME) * (neuePos * -1));
  delay(d);
  posY = neuePos;
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

    /* if (c == '\r\n') {
       Serial.println("habe 'new line' empfangen!");
      } else */if (c == ';') {
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
  Serial.println(suchCode);
  int bufferSize = sizeof(buffer);
  for (int i = 0; i < bufferSize; i++) {
    if (buffer[i] == suchCode) {
      Serial.println("#TRUE#");
      return true;
    }
  }
  Serial.println("#FALSE#");
  return false;
}

/**
    Verarbeitung des empfangenen Komandos
*/
void processCommand() {

  // G-code
  int switchZahl = (int) getFloatFromAssociatedChar('G');
  Serial.println("###");
  Serial.println(switchZahl);
  Serial.println("###");

  // TODO: die vielen getFloatFromAssociatedChar hier abfertigen

  //Serial.println("#WUFF#" + switchZahl );
  // Serial.println("HALLLLLLLOOOOO!!!!!!!!!!!");

  switch (switchZahl) {
    case 28: // homing
      if (commandContainsChar('Z')) {
        drivePen(999.99);
        //drivePen(getFloatFromAssociatedChar('Z'));
      }
      if (commandContainsChar('X')) {
        dacX.setVoltage(0, false);
        posX = 0;
      }
      if (commandContainsChar('Y')) {
        dacY.setVoltage(0, false);
        posY = 0;
      }
      break;
    case 00: // rapid
      if (commandContainsChar('Z')) {
        drivePen(getFloatFromAssociatedChar('Z'));
      }
      if (commandContainsChar('X')) {
        driveX2(getFloatFromAssociatedChar('X'));
      }
      if (commandContainsChar('Y')) {
        driveY2(getFloatFromAssociatedChar('Y'));
      }
      break;
    case 01:
      float x = getFloatFromAssociatedChar('X');
      float y = getFloatFromAssociatedChar('Y');
      // hier bresenham's line algorithm aufrufen
      // und entsprechend fahren
      driveLine((int)posX, (int)x, (int)posY, (int)x);
      break;
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
  Serial.println("Funktion: drivePen");
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

/**
   Bresenham-Algorithmus Implementation für das fahren einer Linie https://de.wikipedia.org/wiki/Bresenham-Algorithmus
   a = alte Kordinate
   n = neue Kordinate
*/
void driveLine(float aX, float nX, float aY, float nY) {
  // Vorbereitung
  // X
  float diffX = abs(nX - aX); // entfernung (abs=Absolute value)
  float steigungX = aX < nX ? 1 : -1; // positive oder negative Richtung
  // Y
  float diffY = abs(nY - aY); // entfernung (abs=Absolute value)
  float steigungY = aY < nY ? 1 : -1; // positive oder negative Richtung
  // Abweichung
  float err = diffX + diffY;
  float err2;

  Serial.println("Funktion: driveLine");

  while (1) {

    driveX(aX);
    Serial.print("X: ");
    Serial.print(aX);
    Serial.println("");
    driveY(aY);
    Serial.print("Y: ");
    Serial.print(aY);
    Serial.println("");

    if (aX == nX && aY == nY) { // Schleifen abbruch
      break;
    }
    err2 = 2 * err;
    if (err2 > diffY) {
      err += diffY;
      aX += steigungX; // berechnung für nächsten Schleifen durchgang ob eine Einheit hoch oder runter gefahren wird
    }
    if (err2 > diffX) {
      err += diffX;
      aY += steigungY; // berechnung für nächsten Schleifen durchgang ob eine Einheit hoch oder runter gefahren wird
    }
  }

  posX = nX;
  posY = nY;


  /*
    void line(int x0, int y0, int x1, int y1) {

    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
    setPixel(x0,y0);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
    }
    }
  */

}
