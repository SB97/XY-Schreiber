/*
   https://www.marginallyclever.com/2013/08/how-to-build-an-2-axis-arduino-cnc-gcode-interpreter/
   Der Interpreter soll per Serieller verbindung GCODE empfangen und auswerten.
*/

// Konstanten
#define BAUD (57600) // Baudrate 
#define MAX_BUF (64) // maximale länge der eingehenden Nachrichten

char buffer[MAX_BUF]; // zwischenablage
int bufferAuslastung; // belegung der zwischenablage

void setup() {
  // Serielle verbindung starten
  Serial.begin(BAUD);
  Serial.print(F("Start!"));
  ready();
}

/*
   Vorbereitung auf neuen Serielen Input
*/
void ready() {
  bufferAuslastung = 0; // Nachrichtenspeicher leeren
  Serial.print(F("> ")); // output um zu sehen wann der Arduino bereit für einen Befehl ist
}

/*
   GCODE empfangen und weiterleiten
*/
void loop() {
  // empfangen
  if ( Serial.available() ) {
    char c = Serial.read(); // empfang speichern
    Serial.print("habe '" + String(c) + "' empfangen!");

    // speichern
    if ( bufferAuslastung < MAX_BUF ) {
      buffer[bufferAuslastung++] = c; // ????????????????????????
    }

    // Nachrichten ende
    if ( c == '\n' ) {
      Serial.print("Befehl ende!");
      buffer[bufferAuslastung] = 0; // ?????????????????????????
      //processCommand(); // Nachricht verarbeiten
      ready();
    }
  }
}

/*
    Funktion um nach bestimmtem char in Buffer suchen und Zahl dahinter zurückzugeben
*/
float processCommand(char suchCode) {
  int bufferSize = sizeof(buffer);
  float errorRueckgabe = -999.99;

  for (int i = 0; i < bufferSize; i++) {
    if (buffer[i] == suchCode) {
      return atof(i + 1); // Zahlen nach suchCode als Float zurück geben
    }
  }
  return errorRueckgabe;
}

/*
    Verarbeitung des empfangenen Komandos
*/
