const int numSensors = 6;   // Total number of sensors
const int pinIRd[numSensors] = {2, 3, 4, 5, 6, 7};  // Digital pins for IR sensors
const int pinIRa[numSensors] = {A0, A1, A2, A3, A4, A5};  // Analog pins for IR sensors
const int pinLED = 9;
int IRvalueA[numSensors] = {0};   // Array to store analog readings
int IRvalueD[numSensors] = {0};   // Array to store digital readings

void setup()
{
  Serial.begin(9600);
  pinMode(pinLED, OUTPUT);
  for (int i = 0; i < numSensors; i++) {
    pinMode(pinIRd[i], INPUT);
    pinMode(pinIRa[i], INPUT);
  }
}

void loop()
{
  // Read values for all sensors at once
  for (int i = 0; i < numSensors; i++) {
    IRvalueA[i] = analogRead(pinIRa[i]);
    IRvalueD[i] = digitalRead(pinIRd[i]);
  }

  // Print header for the table
  Serial.println("Sensor\tAnalog\tDigital");

  // Print values for all sensors in a table format
  for (int i = 0; i < numSensors; i++) {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(IRvalueA[i]);
    Serial.print("\t");
    Serial.println(IRvalueD[i]);
  }

  delay(1000);
}



