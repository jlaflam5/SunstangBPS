/*!
LTC2758: Dual Serial 18-Bit SoftSpan IOUT DAC

@verbatim

The LTC2758 is a dual, current-output, serial-input precision multiplying DAC
with selectable output ranges. Ranges can either be programmed in software for
maximum flexibility (each of the DACs can be programmed to any one of six output
ranges) or hardwired through pin-strapping. Two unipolar ranges are available
(0V to 5V and 0V to 10V), and four bipolar ranges (±2.5V, ±5V, ±10V and –2.5V
to 7.5V). These ranges are obtained when an external precision 5V reference is
used. The output ranges for other reference voltages are easy to calculate by
observing that each range is a multiple of the external reference voltage. The
ranges can then be expressed: 0 to 1×, 0 to 2×, ±0.5×, ±1×, ±2×, and –0.5× to 1.5×.

@endverbatim

http://www.linear.com/product/LTC2758

http://www.linear.com/product/LTC2758#demoboards

REVISION HISTORY
$Revision: 3488 $
$Date: 2015-05-26 11:01:05 -0700 (Tue, 26 May 2015) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

//! @defgroup LTC2758 LTC2758: Dual Serial 18-Bit SoftSpan IOUT DAC

/*! @file
    @ingroup LTC2758
    Library for LTC2758 Dual Serial 18-Bit SoftSpan IOUT DAC
*/

#include <Arduino.h>
#include <stdint.h>
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <SPI.h>

// Transmits 32 bit input stream: 4-bit command + 4-bit don't-care + 18-bit data + 6 don't care
void LTC2758_write(uint8_t cs, uint8_t dac_command, uint8_t dac_address, uint32_t data)
{
  output_low(cs);
  spi_write(dac_command|dac_address);
  spi_write((uint8_t)((data >> 10) & 0xFF));  // D17:D10
  spi_write((uint8_t)((data >> 2) & 0xFF));     // D9:D2
  spi_write((uint8_t)((data << 6) & 0xFF));     // D1:D0
  output_high(cs);
}

// Calculate the LTC2758 DAC output voltage given the DAC code and and the minimum / maximum
// outputs for a given softspan range.
float LTC2758_code_to_voltage(uint32_t dac_code, float min_output, float max_output)
{
  float dac_voltage;
  dac_voltage = (((float) dac_code / 262143.0) * (max_output - min_output)) + min_output;            // Calculate the dac_voltage
  return (dac_voltage);
}
// Calculate a LTC2758 DAC code given the desired output voltage and the minimum / maximum
// outputs for a given softspan range.
uint32_t LTC2758_voltage_to_code(float dac_voltage, float min_output, float max_output)
{
  uint32_t dac_code;
  float float_code;
  float_code = 262143.0 * (dac_voltage - min_output) / (max_output - min_output);                    // Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);     // Round
  if (float_code < 0.0) float_code = 0.0;
  if (float_code > 262143.0) float_code = 262143.0;
  dac_code = (uint32_t) (float_code);                                                               // Convert to unsigned integer
  return (dac_code);
}
