
//pio run -t program

#include <TinyWireS.h>
#include <arduino.h>


#define I2C_SLAVE_ADDRESS 0x4 // the 7-bit address

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

volatile uint8_t i2c_regs[] =
{
    0x0, // channel
    0x0, // low byte
    0x0, // high byte
};

volatile uint8_t what;

const byte reg_size = sizeof(i2c_regs);
volatile byte reg_position;

volatile boolean start_conversion;

int value;
int ADCpin;


void requestEvent()
{
  if(reg_position && !i2c_regs[0])
  {
    TinyWireS.send(i2c_regs[reg_position]);
  }
  reg_position++;

  if (reg_position >= reg_size)
    {
      reg_position = 0;
    }
}

void receiveEvent(uint8_t buffer)
{
    what = TinyWireS.receive();

    if((what >0)&&(what<4))
    {
      i2c_regs[0] = what;
      start_conversion = true;
    }
}


void setup()
{
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);
}

void loop()
{
  TinyWireS_stop_check();

  if(start_conversion)
  {
    start_conversion = false;
  }

  if(i2c_regs[0])
  {
    value = analogRead(i2c_regs[0]);

    cli();
    i2c_regs[0] = 0;
    i2c_regs[1] = lowByte(value);
    i2c_regs[2] = highByte(value);
    reg_position = 1;
    sei();
  }

}
