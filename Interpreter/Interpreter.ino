/*
 * https://www.marginallyclever.com/2013/08/how-to-build-an-2-axis-arduino-cnc-gcode-interpreter/
 * Der Interpreter soll per Serieller verbindung GCODE empfangen und auswerten.
 */

// Konstanten
#define BAUD (57600) // Baudrate 
#define MAX_BUF (64) // maximale länge der eingehenden Nachrichten

char buffer[MAX_BUF]; // zwischenablage
int bufferAuslastung; // belegung der zwischenablage

void setup() {
  // Serielle verbindung starten
  Serial.begin(BAUD);

}

void loop() {
  // put your main code here, to run repeatedly:

}
