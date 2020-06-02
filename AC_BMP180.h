/*
	AC_BMP180.h
	Bosch BMP180 pressure sensor library for the Arduino microcontroller
	made by Alessandro Carotenuto
	Uses floating-point equations from the Weather Station Data Logger project
	http://wmrx00.sourceforge.net/
	http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf
	Forked from BMP085 library by M.Grusin
	Ver 1.0.19042020
	
	Our example code uses the "beerware" license. You can do anything
	you like with this code. No really, anything. If you find it useful,
	buy me a (root) beer someday.
*/

#ifndef AC_bmp180_h  
#define AC_bmp180_h    
#include "Arduino.h"   
 
class AC_BMP180
{     
	public:  
		AC_BMP180();							//base type
		
		void begin();
			// call pressure.begin() to initialize BMP180 before use
		
		void startTemperature();
			// Read the uncompensated temperature value
		
		float GetTemperature();
			// ut value from startTemperature
			
		void startPressure();
			// Read the uncompensated pressure value
		
		long GetPressure();
			// Calculate pressure given up
			// calibration values must be known
			// b5 is also required so GetTemperature(...) must be called first.
			// Value returned will be pressure in units of Pa.
			
	private:
		int ReadInt(unsigned char address);
		// read an signed int (16 bits) from a BMP180 register
		// address: BMP180 register address
		// value: external signed int for returned value (16 bits)
		// returns 1 for success, 0 for fail, with result in value
		// Read 2 bytes from the BMP085
		// First byte will be from 'address'
		// Second byte will be from 'address'+1
		
		char Read(unsigned char address);
		// Read 1 byte from the BMP180 at 'address'
		
		
		// Calibration values
		int ac1,ac2,ac3,b1,b2,mb,mc,md;
		long b5;
		unsigned int ac4,ac5,ac6;
		// Other values
		unsigned int ut;
		unsigned long up;
		const unsigned char OSS = 0;  // Oversampling Setting
			
		
};	

#define BMP180_ADDRESS 0x77  // I2C address of BMP085
		
#endif