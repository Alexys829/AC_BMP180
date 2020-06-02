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

#include <AC_BMP180.h>
#include <Wire.h>

AC_BMP180::AC_BMP180()
// Base library type
{
}

void AC_BMP180::begin()
// Initialize library for subsequent pressure measurements
{
	// Start up the Arduino's "wire" (I2C) library:
	
	Wire.begin();

	// The BMP180 includes factory calibration data stored on the device.
	// Each device has different numbers, these must be retrieved and
	// used in the calculations when taking pressure measurements.

	// Retrieve calibration data from device:
	
	ac1 = ReadInt(0xAA);
	ac2 = ReadInt(0xAC);
	ac3 = ReadInt(0xAE);
	ac4 = ReadInt(0xB0);
	ac5 = ReadInt(0xB2);
	ac6 = ReadInt(0xB4);
	b1 = ReadInt(0xB6);
	b2 = ReadInt(0xB8);
	mb = ReadInt(0xBA);
	mc = ReadInt(0xBC);
	md = ReadInt(0xBE);
		
	// Success!
}

int AC_BMP180::ReadInt(unsigned char address)
{
	unsigned char msb, lsb;

	Wire.beginTransmission(BMP180_ADDRESS);
	Wire.write(address);
	Wire.endTransmission();

	Wire.requestFrom(BMP180_ADDRESS, 2);
	while(Wire.available()<2);
	msb = Wire.read();
	lsb = Wire.read();

	return (int) msb<<8 | lsb;
}

void AC_BMP180::startTemperature()
{

	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
	Wire.beginTransmission(BMP180_ADDRESS);
	Wire.write(0xF4);
	Wire.write(0x2E);
	Wire.endTransmission();

	// Wait at least 4.5ms
	delay(5);

	// Read two bytes from registers 0xF6 and 0xF7
	ut = ReadInt(0xF6);
}

float AC_BMP180::GetTemperature()
{
	long x1, x2;

	x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
	x2 = ((long)mc << 11)/(x1 + md);
	b5 = x1 + x2;

	float temp = ((b5 + 8)>>4);
	temp = temp /10;

	return temp;
}

void AC_BMP180::startPressure()
{
	unsigned char msb, lsb, xlsb;

	// Write 0x34+(OSS<<6) into register 0xF4
	// Request a pressure reading w/ oversampling setting
	Wire.beginTransmission(BMP180_ADDRESS);
	Wire.write(0xF4);
	Wire.write(0x34 + (OSS<<6));
	Wire.endTransmission();

	// Wait for conversion, delay time dependent on OSS
	delay(2 + (3<<OSS));

	 // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
	msb = Read(0xF6);
	lsb = Read(0xF7);
	xlsb = Read(0xF8);

	up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

}

long AC_BMP180::GetPressure()
{
	long x1, x2, x3, b3, b6, p;
	unsigned long b4, b7;

	b6 = b5 - 4000;
	// Calculate B3
	x1 = (b2 * (b6 * b6)>>12)>>11;
	x2 = (ac2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

	// Calculate B4
	x1 = (ac3 * b6)>>13;
	x2 = (b1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

	b7 = ((unsigned long)(up - b3) * (50000>>OSS));
	if (b7 < 0x80000000)
		p = (b7<<1)/b4;
	else
		p = (b7/b4)<<1;

	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	p += (x1 + x2 + 3791)>>4;

	long temp = p;
	return temp;
}

char AC_BMP180::Read(unsigned char address)
{
	unsigned char data;

	Wire.beginTransmission(BMP180_ADDRESS);
	Wire.write(address);
	Wire.endTransmission();

	Wire.requestFrom(BMP180_ADDRESS, 1);
	while(!Wire.available());

	return Wire.read();
}