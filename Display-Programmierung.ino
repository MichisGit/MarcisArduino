/***************************************************************************
 * Example sketch for the ina219_1_WE library
 *
 * This sketch shows how to use the ina219_1 module in continuous mode.
 *
 * Further information can be found on:
 * https://wolles-elektronikkiste.de/ina219_1 (German)
 * https://wolles-elektronikkiste.de/en/ina219_1-current-and-power-sensor (English)
 *
 ***************************************************************************/
#include <Wire.h>
// #include <ina219_WE.h>
#include <INA219_WE.h>
#include <LiquidCrystal.h>

#define I2C_ADDRESS_1 0x40
#define I2C_ADDRESS_2 0x45

#define ONLYONE false
#define LCDBright A1
#define MAXLINES 3

unsigned long previousMillis = 0;
const long interval = 800; // 500ms

/* There are several ways to create your ina219_1 object:
 * ina219_1_WE ina219_1 = ina219_1_WE(); -> uses Wire / I2C Address = 0x40
 * ina219_1_WE ina219_1 = ina219_1_WE(I2C_ADDRESS); -> uses Wire / I2C_ADDRESS
 * ina219_1_WE ina219_1 = ina219_1_WE(&Wire); -> you can pass any TwoWire object
 * ina219_1_WE ina219_1 = ina219_1_WE(&Wire, I2C_ADDRESS); -> all together
 */
INA219_WE ina219_1 = INA219_WE(I2C_ADDRESS_1);

INA219_WE ina219_2 = INA219_WE(I2C_ADDRESS_2);

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int x = 0;
float Uges = 0;
float P_V = 0;
float I_V = 0;
float Q_S = 10;
float P_S = 0;

void setup()
{
  // Zeilen variable

  Serial.begin(9600);
  Wire.begin();
  if (!ina219_1.init())
  {
    Serial.println("ina219_1 not connected!");
    while (1)
      ;
  }
  if (!ina219_2.init())
  {
    Serial.println("ina219_2 not connected!");
    // while(1);
  }
  // set up the LCD's number of columns and rows:
  analogWrite(LCDBright, 80);
  lcd.begin(16, 2);

  // Print a message to the LCD.

  lcd.print("hello, Marci!");
  /* Set ADC Mode for Bus and ShuntVoltage
  * Mode *            * Res / Samples *       * Conversion Time *
  BIT_MODE_9        9 Bit Resolution             84 µs
  BIT_MODE_10       10 Bit Resolution            148 µs
  BIT_MODE_11       11 Bit Resolution            276 µs
  BIT_MODE_12       12 Bit Resolution            532 µs  (DEFAULT)
  SAMPLE_MODE_2     Mean Value 2 samples         1.06 ms
  SAMPLE_MODE_4     Mean Value 4 samples         2.13 ms
  SAMPLE_MODE_8     Mean Value 8 samples         4.26 ms
  SAMPLE_MODE_16    Mean Value 16 samples        8.51 ms
  SAMPLE_MODE_32    Mean Value 32 samples        17.02 ms
  SAMPLE_MODE_64    Mean Value 64 samples        34.05 ms
  SAMPLE_MODE_128   Mean Value 128 samples       68.10 ms
  */
   ina219_1.setADCMode(SAMPLE_MODE_32); // choose mode and uncomment for change of default

  /* Set measure mode
  POWER_DOWN - ina219_1 switched off
  TRIGGERED  - measurement on demand
  ADC_OFF    - Analog/Digital Converter switched off
  CONTINUOUS  - Continuous measurements (DEFAULT)
  */
  // ina219_1.setMeasureMode(CONTINUOUS); // choose mode and uncomment for change of default

  /* Set PGain
  * Gain *  * Shunt Voltage Range *   * Max Current (if shunt is 0.1 ohms) *
   PG_40       40 mV                    0.4 A
   PG_80       80 mV                    0.8 A
   PG_160      160 mV                   1.6 A
   PG_320      320 mV                   3.2 A (DEFAULT)
  */
  ina219_1.setPGain(PG_80); // choose gain and uncomment for change of default

  /* Set Bus Voltage Range
   BRNG_16   -> 16 V
   BRNG_32   -> 32 V (DEFAULT)
  */
  ina219_1.setBusRange(BRNG_16); // choose range and uncomment for change of default

  Serial.println("ina219_1 Current Sensor Example Sketch - Continuous");

  /* If the current values delivered by the ina219_1 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by ina219_1
  */
  // ina219_1.setCorrectionFactor(0.98); // insert your correction factor if necessary

  /* If you experience a shunt voltage offset, that means you detect a shunt voltage which is not
     zero, although the current should be zero, you can apply a correction. For this, uncomment the
     following function and apply the offset you have detected.
  */
  // ina219_1.setShuntVoltOffset_mV(0.5); // insert the shunt voltage (millivolts) you detect at zero current
}

void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you measured something
    previousMillis = currentMillis;

    // Do something here
    measurementFunction();
    LCD_OUTPUT();
    Serial_OUTPUT();
  }
  getTaste();
}
void getTaste()
{
  int analogWert = analogRead(A0);
  // UP
  if (analogWert > 50 && analogWert < 150 && x > 0)
  {
    x--;
    LCD_OUTPUT();
  }
  // DOWN
  if (analogWert > 200 && analogWert < 300 && x < MAXLINES)
  {
    x++;
    LCD_OUTPUT();
  }
}



void LCD_OUTPUT() {
  char buffer[16], floatStr[10];
  lcd.clear();
  lcd.setCursor(0,0);
  if (x=0){
      
    dtostrf(Uges, 4, 2, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "Spannung: %s V", floatStr);
    lcd.print(buffer);
    lcd.setCursor(0,1);
    dtostrf(P_V, 5, 2, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "P am V: %.2f mW",floatStr );
    lcd.print(buffer);
    }
    else if (x=1){
    dtostrf(P_V, 5, 2, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "P am V: %.2f mW",floatStr ); 
    lcd.print(buffer);
    lcd.setCursor(0,1);
    dtostrf(I_V, 5, 2, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "I am V: %.2f mA", floatStr);
    lcd.print(buffer);
    }
    else if (x=2){
    dtostrf(I_V, 5, 2, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "I am V: %.2f mA",floatStr ); 
    lcd.print(buffer);
    lcd.setCursor(0,1);
    dtostrf(Q_S, 8, 1, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "Landung: %.2f mAs",floatStr );
    lcd.print(buffer);
    }
    else if (x=3){
    dtostrf(Q_S, 8, 1, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "Landung: %.2f mAs",floatStr); 
    lcd.print(buffer);
    lcd.setCursor(0,1);
    dtostrf(P_S, 5, 2, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
    sprintf(buffer, "Lade P: %.2f mW",floatStr );
    lcd.print(buffer);
    }
   
  
  }
  void Serial_OUTPUT()
{ 
  char floatStr[10];
  dtostrf(I_V, 5, 2, floatStr);  // (Wert, Breite, Nachkommastellen, Zielpuffer)
  char buffer[17];
  sprintf(buffer, "PamV: %s mW", floatStr);
  Serial.println(buffer);
  Serial.print("Spannung Zelle [V]: ");
  Serial.println(Uges);
  Serial.print("Ausgangsstromstärke I [mA]: ");
  Serial.println(I_V);
  Serial.print("Ausgangsleistung P [mW]: ");
  Serial.println(P_V);
  Serial.print("Ladung Kondensator Q [mAs]:");
  Serial.println(Q_S);
  Serial.print("LadeLeistung P [mW]: ");
  Serial.println(P_S);
}
void measurementFunction()
{
  float busVoltage_V_1 = 0.0; // benutzen
  float current_mA_1 = 0.0;   // benutzen
  float power_mW_1 = 0.0;     // fragen sonst rechnen
  bool ina219_1_overflow = false;

  float busVoltage_V_2 = 0.0; // benutzen
  float current_mA_2 = 0.0;   // benutzen
  float power_mW_2 = 0.0;     // fragen sonst rechnen
  bool ina219_2_overflow = false;

  busVoltage_V_1 = ina219_1.getBusVoltage_V();
  current_mA_1 = ina219_1.getCurrent_mA();
  power_mW_1 = ina219_1.getBusPower();
  ina219_1_overflow = ina219_1.getOverflow();

  if (!ONLYONE)
  {
    busVoltage_V_2 = ina219_2.getBusVoltage_V();
    current_mA_2 = ina219_2.getCurrent_mA();
    power_mW_2 = ina219_2.getBusPower();
    ina219_2_overflow = ina219_2.getOverflow();
  }
  Uges = busVoltage_V_1;
  P_V = power_mW_2;
  I_V = current_mA_2;
  float Qsalt = Q_S;
  Q_S = Qsalt +(current_mA_1 - current_mA_2)/1000 * interval ; // (interval / 1000);
  P_S = power_mW_1 - power_mW_2;
}

