// http://henrysbench.capnfatz.com/henrys-bench/arduino-output-devices/arduino-mcp4725-digital-to-analog-converter-tutorial/

#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dacX; // constructor X
Adafruit_MCP4725 dacY; // constructor Y

/*
 * const int xLaenge = 25;
 * const int yLaenge = 30;
 * 
 * const int x_cm = 0;
 * const y_cm = 0;
 * 
 * int xDac = 0;
 * int yDac = 0;
 */

void setup(void) {
  Serial.begin(9600);
  // The I2C Address: Run the I2C Scanner if you're not sure 
  dacX.begin(0x62); // I2C Adress X
  dacY.begin(0x63); // I2C Adress Y

  /*
   * 
   */
  
}

void loop(void) {
  
    uint32_t dac_value;
    
    for (dac_value = 0; dac_value < 4096; dac_value = dac_value + 15)
    {
      dacX.setVoltage(dac_value, false);
      dacY.setVoltage(dac_value, false);
      delay(100);

      Serial.print("DAC Value: ");
      Serial.print(dac_value);
      Serial.print("\n");
    }    

      dacX.setVoltage(0, false);
      dacY.setVoltage(0, false);
    delay(9999);
}
