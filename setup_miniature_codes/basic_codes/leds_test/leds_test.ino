// Define the pin numbers for the motor and 6 solenoids
const int motor = 4;
const int solenoid_1 = 16;
const int solenoid_2_3 = 17;
const int solenoid_4 = 5;
const int solenoid_5 = 18;
const int solenoid_6 = 19;

void setup() {
  // Set each pin as an OUTPUT
  pinMode(motor, OUTPUT);
  pinMode(solenoid_1, OUTPUT);
  pinMode(solenoid_2_3, OUTPUT);
  pinMode(solenoid_4, OUTPUT);
  pinMode(solenoid_5, OUTPUT);
  pinMode(solenoid_6, OUTPUT);

  digitalWrite(motor, LOW);
  digitalWrite(solenoid_1, LOW);
  digitalWrite(solenoid_2_3, LOW);
  digitalWrite(solenoid_4, LOW);
  digitalWrite(solenoid_5, LOW);
  digitalWrite(solenoid_6, LOW);
  delay(3000); // Wait for 1 second

}

void loop() {

  // case 0,0,0 
  digitalWrite(motor, LOW);
  digitalWrite(solenoid_1, LOW);
  digitalWrite(solenoid_2_3, LOW);
  digitalWrite(solenoid_4, LOW);
  digitalWrite(solenoid_5, LOW);
  digitalWrite(solenoid_6, LOW);
  delay(3000); // Wait for 1 second
  
  // Case 1,0,0
  digitalWrite(motor, HIGH);
  digitalWrite(solenoid_1, HIGH);
  digitalWrite(solenoid_2_3, LOW);
  digitalWrite(solenoid_4, HIGH);
  digitalWrite(solenoid_5, LOW);
  digitalWrite(solenoid_6, LOW);
  delay(3000); // Wait for 1 second

  // case 1,1,0
  digitalWrite(motor, HIGH);
  digitalWrite(solenoid_1, LOW);
  digitalWrite(solenoid_2_3, HIGH);
  digitalWrite(solenoid_4, HIGH);
  digitalWrite(solenoid_5, LOW);
  digitalWrite(solenoid_6, LOW);
  delay(3000); // Wait for 1 second

  // case : 1,0,1
  digitalWrite(motor, HIGH);
  digitalWrite(solenoid_1, HIGH);
  digitalWrite(solenoid_2_3, LOW);
  digitalWrite(solenoid_4, LOW);
  digitalWrite(solenoid_5, HIGH);
  digitalWrite(solenoid_6, HIGH);
  delay(3000); // Wait for 1 second

  // case 1,1,1
  digitalWrite(motor, HIGH);
  digitalWrite(solenoid_1, LOW);
  digitalWrite(solenoid_2_3, HIGH);
  digitalWrite(solenoid_4, LOW);
  digitalWrite(solenoid_5, HIGH);
  digitalWrite(solenoid_6, HIGH);
  delay(3000); // Wait for 1 second
}
