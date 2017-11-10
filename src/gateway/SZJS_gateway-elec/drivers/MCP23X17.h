#ifndef __MCP23X17_H__
#define __MCP23X17_H__

#include "board.h"


#define IODIRA         0x00
#define IODIRB         0x01
#define MCP_GPIOA      0x12
#define MCP_GPIOB      0x13
#define IOCON		   0x0A
#define GPINTENA       0x04
#define GPINTENB       0x05
#define INTCAPA        0x10
#define INTCAPB        0x11

#define IO_EXPANDER_ADDR   0x40

// IOCON Register Masks
#define IOCON_UNUSED   0x01
#define IOCON_INTPOL   0x02
#define IOCON_ODR      0x04
#define IOCON_HAEN     0x08
#define IOCON_DISSLW   0x10
#define IOCON_SEQOP    0x20
#define IOCON_MIRROR   0x30
#define IOCON_BANK1    0x80

#define INTCONA        0x04

#define OUTPUT_PORTS      0x00
#define INPUT_PORTS       0xFF


extern void  MCP23017_SetIOPortA(uint8_t value);
extern void  MCP23017_SetIOPortB(uint8_t value);
extern uint8_t  MCP23017_ReadIntCapA(void);
extern uint8_t  MCP23017_ReadIntCapB(void);
extern uint8_t  MCP23017_ReadPortA(void);
extern uint8_t  MCP23017_ReadPortB(void);

extern int MCP_23X17_output_bit(uint8_t bit, uint8_t value);
extern int MCP_23X17_output(uint8_t value);
extern uint8_t MCP_23X17_input_bit(uint8_t bit);
extern uint8_t MCP_23X17_input(void);
extern uint8_t MCP_23X17_input_INT(void);


extern int MCP23X17_hw_init(const char * i2c_bus_device_name);



#endif // __MCP23X17_H__
