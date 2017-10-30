#include "MCP23X17.h"

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

static uint8_t settings = 0x00;
static uint8_t address = IO_EXPANDER_ADDR;

static void SetRegisterValue(uint8_t regAddr, uint8_t value);
static uint8_t ReadRegisterValue(uint8_t regAddr);


static struct rt_i2c_bus_device * i2c_device;

volatile static uint8_t GPIOA_val = 0x00;
volatile static uint8_t GPIOB_val = 0x00;


//---------------------------------------------------------
void MCP23017_Init(void)
{
	// Ensure that we are set to use BANK1 configuration
	SetRegisterValue(IOCON, 0x00);
	// Set IO Direction Port A
	SetRegisterValue(IODIRA, 0x00); // OUTPUT
	// Set IO Direction Port B
	SetRegisterValue(IODIRB, 0xFF); // INPUT
    
    SetRegisterValue(GPINTENB, 0xFF);
    
    MCP23017_SetIOPortA(GPIOA_val);
    GPIOB_val = MCP23017_ReadPortB();
}

//---------------------------------------------------------
void  MCP23017_SetDeviceAddress(uint8_t addr)
{
	address = addr;
}

//---------------------------------------------------------
uint8_t  MCP23017_ReadPortA(void)
{
	return ReadRegisterValue(MCP_GPIOA);
}

//---------------------------------------------------------
uint8_t  MCP23017_ReadPortB(void)
{
	return ReadRegisterValue(MCP_GPIOB);
}

//---------------------------------------------------------
uint8_t  MCP23017_ReadIntCapA(void)
{
	return ReadRegisterValue(INTCAPA);
}

//---------------------------------------------------------
uint8_t  MCP23017_ReadIntCapB(void)
{
	return ReadRegisterValue(INTCAPB);
}

//---------------------------------------------------------
void  MCP23017_SetIOPortA(uint8_t value)
{
	SetRegisterValue(MCP_GPIOA, value);	
}

//---------------------------------------------------------
void  MCP23017_SetIOPortB(uint8_t value)
{
	SetRegisterValue(MCP_GPIOB, value);	
}


uint8_t MCP23017_CheckDevicePresent(void)
{
   uint8_t devicePresent = 0;
   uint8_t regValue = ReadRegisterValue(INTCONA);
   SetRegisterValue(INTCONA, 0xAA);
   devicePresent = (ReadRegisterValue(INTCONA) == 0xAA);
   SetRegisterValue(INTCONA, regValue);
   
   return devicePresent;      
}


////---------------------------------------------------------
//static BYTE ReadRegisterValue(BYTE regAddr)
//{
//   BYTE value = 0x00;
//   BYTE reg[20];
//   BYTE i;

//   // [S]
//   I2C_Start();
//   // [OP][R]
//   address |= 0x01;
//   I2C_PutByte(address);
//   I2C_GetACK();
//   
//   I2C_PutByte(regAddr);
//   I2C_GetACK();

//   // [S]
//   I2C_Start();
//   // [OP][R]
//   address |= 0x01;
//   I2C_PutByte(address);
//   I2C_GetACK();
//   
//   
//   // [Dout]
//   //for(i=0; i<20; i++)
//   //   reg[i] = I2C_GetByte();
//   value = I2C_GetByte();   
//      
//   return value;
//   //return reg[0];
//}


////---------------------------------------------------------
//// Write operation will have the following format
//// [S][OP][W][ADDR][Din][Din][Din]...[P]
//static void SetRegisterValue(BYTE regAddr, BYTE value)
//{
//   // [S]               
//   I2C_Start();
//   
//   // [OP][W][ADDR]
//   // Zero out the last bit to inform device we are
//   // going to do write operation
//   address &= 0xFE;
//   I2C_PutByte(address);   
//   I2C_GetACK();           // Wait for acknowledgement   
//   
//   // [Din]   
//   I2C_PutByte(regAddr);  I2C_GetACK();
//   I2C_PutByte(value);    I2C_GetACK();
//   
//   // [P]      
//   I2C_Stop();
//}

//---------------------------------------------------------
static uint8_t ReadRegisterValue(uint8_t regAddr)
{
  
    struct rt_i2c_msg msg[2];
    rt_uint8_t send_buffer[1];
    rt_uint8_t rec_buffer[1];

    RT_ASSERT(i2c_device != RT_NULL);

    send_buffer[0] = regAddr;
    //send_buffer[1] = value;

    msg[0].addr = address>>1;
    msg[0].flags = RT_I2C_WR;
    msg[0].len = 1;
    msg[0].buf = send_buffer;

    msg[1].addr = address>>1;
    msg[1].flags = RT_I2C_RD ;
    msg[1].len = 1;
    msg[1].buf = rec_buffer;

    rt_i2c_transfer(i2c_device, &msg[0], 2); 

    return rec_buffer[0];

//    struct rt_i2c_msg msg;
//    rt_uint8_t send_buffer[1];
//    rt_uint8_t rec_buffer[22+6];

//    RT_ASSERT(i2c_device != RT_NULL);

////    send_buffer[0] = regAddr;
////    //send_buffer[1] = value;

////    msg.addr = address>>1;
////    msg.flags = RT_I2C_WR;
////    msg.len = 1;
////    msg.buf = send_buffer;
////    rt_i2c_transfer(i2c_device, &msg, 1); 

//    msg.addr = address>>1;
//    msg.flags = RT_I2C_RD;
//    msg.len = 22;
//    //msg.len = 1;
//    msg.buf = rec_buffer;
//    rt_i2c_transfer(i2c_device, &msg, 1); 

//    return rec_buffer[regAddr];
}


//---------------------------------------------------------
// Write operation will have the following format
// [S][OP][W][ADDR][Din][Din][Din]...[P]
static void SetRegisterValue(uint8_t regAddr, uint8_t value)
{
    struct rt_i2c_msg msg;
    rt_uint8_t send_buffer[2];
    
    
    RT_ASSERT(i2c_device != RT_NULL);

    send_buffer[0] = regAddr;
    send_buffer[1] = value;

    msg.addr = address>>1;
    msg.flags = RT_I2C_WR;
    msg.len = 2;
    msg.buf = send_buffer;

    rt_i2c_transfer(i2c_device, &msg, 2);    
}

int MCP23X17_hw_init(const char * i2c_bus_device_name)
{

    i2c_device = rt_i2c_bus_device_find(i2c_bus_device_name);
    if(i2c_device == RT_NULL)
    {
        rt_kprintf("i2c bus device %s not found!\r\n", i2c_bus_device_name);
        return -RT_ENOSYS;
    }
    
    // Reset MCP23017
    rt_pin_write(PIN_MCP23017_RST      , PIN_LOW);
    rt_thread_delay(RT_TICK_PER_SECOND/10);
    rt_pin_write(PIN_MCP23017_RST      , PIN_HIGH);
    rt_thread_delay(RT_TICK_PER_SECOND/10);
    
    MCP23017_Init();
    
    //rt_pin_read(PIN_MCP23017_INTB);
    //MCP23017_SetIOPortA(0x00);
    //MCP23017_ReadPortB();
    
    #if 0
    rt_thread_delay(RT_TICK_PER_SECOND*2);
    MCP_23X17_output(0xFF);
    rt_thread_delay(RT_TICK_PER_SECOND*2);
    MCP_23X17_output(0x00);
    
    #endif 
    
    
    return 0;
}

int MCP_23X17_output_bit(uint8_t bit, uint8_t value)
{
    //uint8_t temp_bit = 1<<bit;
    
    if (value)
    {
        GPIOA_val |= (1<<bit);
    }
    else
    {
        GPIOA_val &= ~(1<<bit);
    }
    
    MCP23017_SetIOPortA(GPIOA_val);
    
    return 0;
}

int MCP_23X17_output(uint8_t value)
{
    GPIOA_val = value;
    MCP23017_SetIOPortA(GPIOA_val);
    return 0;
}

uint8_t MCP_23X17_input_bit(uint8_t bit)
{
    GPIOB_val = MCP23017_ReadPortB();
    
    if (GPIOB_val & (1<<bit))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t MCP_23X17_input(void)
{
    uint8_t res = 0x00;
    res = MCP23017_ReadPortB();
    return ~res;
}

uint8_t MCP_23X17_input_INT(void)
{
    return MCP23017_ReadIntCapB();
}

