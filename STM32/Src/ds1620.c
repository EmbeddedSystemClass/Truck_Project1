#include "main.h"
#include "gpio.h"
#include "ds1620.h"

void init1620()
{
	// All pins -> output
//	DS1620_DDR |= (1<< DS1620_PIN_DQ) | (1<< DS1620_PIN_RST) | (1<< DS1620_PIN_CLK);
	writeCommandTo1620( DS1620_CMD_WRITECONF, 0x02 );			// CPU mode; continous conversion
//	writeByteTo1620( DS1620_CMD_STARTCONV );					// Start conversion
}

void shiftOutByte( uint8_t val )
{
	int i;
	// Send uint8_t, LSB first
	for( i = 0; i < 8; i++ )
	{
//		DS1620_PORT &= ~(1<< DS1620_PIN_CLK );					// tick...
		HAL_GPIO_WritePin(GPIOA, DS1620_PIN_CLK, GPIO_PIN_RESET);
		
		// Set bit
		if( val & (1 << i))
		{
//			DS1620_PORT |= (1<< DS1620_PIN_DQ );
			HAL_GPIO_WritePin(GPIOA, DS1620_PIN_DQ, GPIO_PIN_SET);
		}
		else
		{
//			DS1620_PORT &= ~(1<< DS1620_PIN_DQ );
			HAL_GPIO_WritePin(GPIOA, DS1620_PIN_DQ, GPIO_PIN_RESET);
		}
//		DS1620_PORT |= (1<< DS1620_PIN_CLK );					// ...tock
		HAL_GPIO_WritePin(GPIOA, DS1620_PIN_CLK, GPIO_PIN_SET);
	}
}

void writeByteTo1620( uint8_t cmd )
{
//	DS1620_PORT |= (1<< DS1620_PIN_RST );						// start comm - RST high
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_SET);

	shiftOutByte( cmd );
	
//	DS1620_PORT &= ~(1<< DS1620_PIN_RST );						// end comm
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_RESET);
}

void writeCommandTo1620( uint8_t cmd, uint8_t data )
{
//	DS1620_PORT |= (1<< DS1620_PIN_RST );						// start comm - RST high
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_SET);
	
	shiftOutByte( cmd );	// send command
	shiftOutByte( data );	// send 8 bit data
	
//	DS1620_PORT &= ~(1<< DS1620_PIN_RST );						// end comm
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_RESET);
}

void writeTempTo1620( uint8_t reg, int temp )
{
	uint8_t lsb = temp;											// truncate to high uint8_t
	uint8_t msb = temp >> 8;									// shift high -> low uint8_t
	
//	DS1620_PORT |= _BV( DS1620_PIN_RST );						// start comm - RST high
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_SET);
	
	shiftOutByte( reg );	// send register select
	shiftOutByte( lsb );	// send LSB 8 bit data
	shiftOutByte( msb );	// send MSB 8 bit data (only bit 0 is used)
	
//	DS1620_PORT &= ~(1<< DS1620_PIN_RST );						// end comm
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_RESET);

}

//double readTempFrom1620()
//double readTempFrom1620()
int readTempFrom1620()
{
	int i;
	GPIO_PinState state;
	
//	DS1620_PORT |= (1<< DS1620_PIN_RST );						// start comm - RST high
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_SET);

	
	shiftOutByte( DS1620_CMD_READTEMP );						// send register select
	
//	DS1620_DDR &= ~(1<< DS1620_PIN_DQ );						// configure for input
	set_input();
	
	int raw = 0;
	
	for( i=0; i<9; i++ )										// read 9 bits
	{
//		DS1620_PORT &= ~(1<< DS1620_PIN_CLK );					// CLK low
		HAL_GPIO_WritePin(GPIOA, DS1620_PIN_CLK, GPIO_PIN_RESET);

//		_delay_us( 2 );	// 1 µsec delay to allow the DS1620 to set the value before we read it
		osDelay(2);
		state = HAL_GPIO_ReadPin(GPIOA, DS1620_PIN_DQ);
		if(state = GPIO_PIN_SET)
//		if( DS1620_PIN & (1<< DS1620_PIN_DQ ))					// read bit
			raw |= (1 << i);									// add value
//		DS1620_PORT |= (1<< DS1620_PIN_CLK );					// CLK high
		HAL_GPIO_WritePin(GPIOA, DS1620_PIN_CLK, GPIO_PIN_SET);
	}
	
//	DS1620_PORT &= ~(1<< DS1620_PIN_RST );						// end comm
	HAL_GPIO_WritePin(GPIOA, DS1620_PIN_RST, GPIO_PIN_RESET);
	
//	DS1620_DDR |= (1<< DS1620_PIN_DQ );							// DQ back to output mode
	set_output();
//	return (double)(raw/(double)2);								// divide by 2 and return
	return raw;
}
