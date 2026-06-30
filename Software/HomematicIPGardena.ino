/*
 * Homematic IP Bewässerungssteuerung
 * (c) Markus Mönig, markus@moenig.online
 *
 * Hardware:
 * - ATTiny85
 * - DRV8833
 * - HmIP-PCBS-BAT
 * - Gardena 9V Latch-Ventil
 *
 * Pinbelegung:
 * PB0 -> DRV8833 SLP/EEP
 * PB1 -> DRV8833 IN2
 * PB2 -> DRV8833 IN1
 * PB3 -> HmIP-PCBS-BAT Ausgang
 */

// --------------------------------------------------
// Pin Definitionen
// --------------------------------------------------

const uint8_t PIN_DRV_SLEEP = 0;   // PB0
const uint8_t PIN_DRV_IN2   = 1;   // PB1
const uint8_t PIN_DRV_IN1   = 2;   // PB2
const uint8_t PIN_TRIGGER   = 3;   // PB3

// --------------------------------------------------
// Pulszeiten
// --------------------------------------------------

const uint16_t OPEN_TIME_MS  = 63;
const uint16_t CLOSE_TIME_MS = 250;

// --------------------------------------------------
// Status
// --------------------------------------------------

bool lastTriggerState;

// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
    // DRV8833 Pins
    pinMode(PIN_DRV_SLEEP, OUTPUT);
    pinMode(PIN_DRV_IN1, OUTPUT);
    pinMode(PIN_DRV_IN2, OUTPUT);

    // Homematic Eingang
    pinMode(PIN_TRIGGER, INPUT_PULLUP);

    // DRV8833 initial schlafen
    digitalWrite(PIN_DRV_SLEEP, LOW);

    // Ausgänge sicher aus
    digitalWrite(PIN_DRV_IN1, LOW);
    digitalWrite(PIN_DRV_IN2, LOW);

    // Initialzustand lesen
    lastTriggerState = digitalRead(PIN_TRIGGER);
}

// --------------------------------------------------
// Hauptschleife
// --------------------------------------------------

void loop()
{
    bool currentState = digitalRead(PIN_TRIGGER);

    // Zustandsänderung erkannt
    if(currentState != lastTriggerState)
    {
        delay(10);

        currentState = digitalRead(PIN_TRIGGER);

        if(currentState != lastTriggerState)
        {
            // Homematic aktiv (LOW wegen Open Drain)
            if(currentState == LOW)
            {
                openValve();
            }
            else
            {
                closeValve();
            }

            lastTriggerState = currentState;
        }
    }

    delay(50);
}

// --------------------------------------------------
// Ventil öffnen
// --------------------------------------------------

void openValve()
{
    // DRV8833 aktivieren
    digitalWrite(PIN_DRV_SLEEP, HIGH);

    delay(5);

    // Polarität OPEN
    digitalWrite(PIN_DRV_IN1, HIGH);
    digitalWrite(PIN_DRV_IN2, LOW);

    delay(OPEN_TIME_MS);

    // Ausgänge deaktivieren
    digitalWrite(PIN_DRV_IN1, LOW);
    digitalWrite(PIN_DRV_IN2, LOW);

    // DRV8833 schlafen
    digitalWrite(PIN_DRV_SLEEP, LOW);
}

// --------------------------------------------------
// Ventil schließen
// --------------------------------------------------

void closeValve()
{
    // DRV8833 aktivieren
    digitalWrite(PIN_DRV_SLEEP, HIGH);

    delay(5);

    // Polarität CLOSE
    digitalWrite(PIN_DRV_IN1, LOW);
    digitalWrite(PIN_DRV_IN2, HIGH);

    delay(CLOSE_TIME_MS);

    // Ausgänge deaktivieren
    digitalWrite(PIN_DRV_IN1, LOW);
    digitalWrite(PIN_DRV_IN2, LOW);

    // DRV8833 schlafen
    digitalWrite(PIN_DRV_SLEEP, LOW);
}