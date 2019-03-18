/*
   https://www.marginallyclever.com/2013/08/how-to-build-an-2-axis-arduino-cnc-gcode-interpreter/
   Der Interpreter soll per Serieller verbindung GCODE empfangen und auswerten.
*/

// Konstanten
#define BAUD (57600) // Baudrate 
#define MAX_BUF (64) // maximale länge der eingehenden Nachrichten

char buffer[MAX_BUF]; // zwischenablage
int bufferAuslastung; // belegung der zwischenablage

// Plotter
#define PEN_PIN (2)
float penZ = 0;

void setup() {
  // Serielle verbindung starten
  Serial.begin(BAUD);
  Serial.println(("Start!"));
  ready();

  // Pen
  pinMode(PEN_PIN, OUTPUT);
}

/*
   Vorbereitung auf neuen Serielen Input
*/
void ready() {
  bufferAuslastung = 0; // Nachrichtenspeicher leeren
  Serial.println("> "); // output um zu sehen wann der Arduino bereit für einen Befehl ist
}

/*
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
    }else {
      Serial.println("habe '" + String(c) + "' empfangen!");
    }

//Serial.println("###" + String(c) + "###");
    // speichern
    if ( bufferAuslastung < MAX_BUF ) {
      buffer[bufferAuslastung++] = c; // ????????????????????????
    }

    // Nachrichten ende
    if ( c == ';' ) {

    // TEST
    Serial.println("Befehls String #");
    for(int i=0; i<=bufferAuslastung; i++) {
      Serial.print(buffer[i]);
      }
    Serial.println("#");
      
      Serial.print("Befehl ende!");
      buffer[bufferAuslastung] = 0; // ?????????????????????????
      processCommand(); // Nachricht verarbeiten
      ready();
    }
  }
}

/*
    Funktion um nach bestimmtem char in Buffer suchen und Zahl dahinter zurückzugeben
*/
float parseCode(char suchCode) {
  int bufferSize = sizeof(buffer);
  float errorRueckgabe = -999.99;

  for (int i = 0; i < bufferSize; i++) {
    if (buffer[i] == suchCode) {
      return atof(i + 1); // Zahlen nach suchCode als Float zurück geben
    }
  }
  return errorRueckgabe;
}

boolean commandContainsChar(char suchCode) {
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
int switchZahl = parseCode('G');
switch (switchZahl) {
case 28:
  if (commandContainsChar('Z')) {
    Serial.println("MIAUUUUUUUUUUU");
    drivePen(-999.99);
  } else if (commandContainsChar('X')) {
    // X 0 fahren
  } else if (commandContainsChar('Y')) {
    // Y 0 fahren
  }
case -999:
  break;
  //case 0: ;
default: break;
}
}

void drivePen(float newZ) {
  Serial.println("Funktion: drivePen");
  if (newZ != penZ) {
    if (newZ > penZ) {
      // Stift hoch fahren
      digitalWrite(PEN_PIN, HIGH);
      penZ = newZ;
    } else {
      // Stift runter fahren
      digitalWrite(PEN_PIN, LOW);
      penZ = newZ;
    }
  }
}
