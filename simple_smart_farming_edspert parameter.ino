//aridoutofrange -- Ramadhirra Azzahra Putri

//LCD Library
#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd_1(32);
Adafruit_LiquidCrystal lcd_2(33);
Adafruit_LiquidCrystal lcd_3(34);

//Defining Pins
#define pinTemp 3
#define pinSoil 1
#define pinLDR 2
#define pinMotor 5
#define pinFSR 0
#define ledRed 4
#define ledGreen 2
#define ledYellow 7

//Gas Sensor
#define pinGas 0
#define resLoad 1
#define slope - 0.263
#define resO 20
#define gradient 0.42

//Lux Sensor
#define MAX_ADC_READING 1023
#define ADC_REF_VOLTAGE 5.0
#define REF_RESISTANCE 10000
#define LUX_CALC_SCALAR 125235178.3654270
#define LUX_CALC_EXPONENT - 1.604568157

int ldrRawData;
int buzzer = 6;
int fsrReading;
int fsrVoltage;

float fsrForce;
float resistorVoltage, ldrVoltage;
float ldrResistance;
float ldrLux;

void setup() {
  pinMode(pinTemp, INPUT);
  pinMode(pinSoil, INPUT);
  pinMode(pinMotor, OUTPUT);
  pinMode(pinLDR, INPUT);
  pinMode(pinFSR, INPUT);
  pinMode(pinGas, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  Serial.begin(9600);

  lcd_1.begin(16, 2);
  lcd_2.begin(16, 2);
  lcd_3.begin(16, 2);

}

void loop() {
  // Read Temperature Sensor
  int temp = analogRead(pinTemp); // Analog Value
  float voltage = temp * (5000 / 1024.0);
  float temperature = (voltage - 500) / 10; //Converted Value (in Celsius)

  // Read Soil Moisture Sensor
  int soil = analogRead(pinSoil); // Analog Value
  int moisture_data_new = map(soil, 0, 876, 0, 100); //Converted Value (in Percentage)

  // Read UV sensor 
  ldrRawData = analogRead(pinLDR); // Analog Value
  resistorVoltage = (float) ldrRawData / MAX_ADC_READING * ADC_REF_VOLTAGE;
  ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
  ldrResistance = ldrVoltage / resistorVoltage * REF_RESISTANCE;
  ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT); //Converted Value (in Lux)

  //Read Force Sensor
  fsrReading = analogRead(pinFSR); //Analog Value
  fsrVoltage = map(fsrReading, 0, 1023, 0, 5000);
  fsrForce = fsrVoltage / 876.2; //Converted Value (in Newton)

  //Read Gas Sensor
  float VRL; //Voltage drop 
  float sensorRes; //Sensor resistance 
  float ratio; //Ratio
  int gas = analogRead(pinGas); //Analog Value
  VRL = analogRead(pinGas) * (5.0 / 1023.0);
  sensorRes = ((5.0 * resLoad) / VRL) - resLoad; //Formula to get the value of sensor resistance
  ratio = sensorRes / resO; // Formula to find the ratio
  float ppm = pow(10, ((log10(ratio) - gradient) / slope)); //Formula to calculate ppm

  //Print to LCD
  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp: ");
  lcd_1.print(temperature);
  lcd_1.print("\262C");

  lcd_1.setCursor(0, 1);
  lcd_1.print("Soil Dew: ");
  lcd_1.print(moisture_data_new);
  lcd_1.print(" %");

  lcd_2.setCursor(0, 0);
  lcd_2.print("UV: ");
  lcd_2.print(ldrLux);
  lcd_2.print(" LUX");

  lcd_2.setCursor(0, 1);
  lcd_2.print("Force: ");
  lcd_2.print(fsrForce);
  lcd_2.print(" N");

  lcd_3.setCursor(0, 0);
  lcd_3.print("CO2: ");
  lcd_3.print(ppm);
  lcd_3.print(" PPM");

  //Conditional Statement
  if (temperature > 30 && moisture_data_new < 40 && ldrLux > 20){
    analogWrite(pinMotor, 50);
    lcd_3.setCursor(0, 1);
    lcd_3.print("              ");
    lcd_3.setCursor(0, 1);
    lcd_3.print("Ideal");
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledYellow, LOW);
  } 
  else if (temperature >35 && moisture_data_new < 20) {
    analogWrite(pinMotor, 50);
    lcd_3.setCursor(0, 1);
    lcd_3.print("              ");
    lcd_3.setCursor(0, 1);
    lcd_3.print("Acceptable");
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, HIGH);
  }
  else {
    lcd_3.setCursor(0, 1);
    lcd_3.print("              ");
    lcd_3.setCursor(0, 1);
    lcd_3.print("Not Ideal!");
    tone(buzzer, 500, 700);
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, LOW);
    delay(50);
  }

  //Print to Serial Monitor
  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.print(" C \t");

  Serial.print("Soil Moisture : ");
  Serial.print(moisture_data_new);
  Serial.print(" % \t");

  Serial.print("UV Meter: ");
  Serial.print(ldrLux);
  Serial.print(" LUX \n");

  Serial.print("Force : ");
  Serial.print(fsrForce);
  Serial.print(" N \t");

  Serial.print("CO2 : ");
  Serial.print(ppm);
  Serial.print(" PPM \t");
}