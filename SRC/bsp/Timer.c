#include "2440addr.h"
#include "option.h"

//Function Declaration
void Timer_Init(void);
void Timer_Start(int hz);

void Timer_Init(void)
{
	/* 	Timer0 Init 
	* Prescaler value : 255, dead zone length = 0
	* Divider value : 1/16, no DMA mode
	* New frequency : (PCLK/(Prescaler value+1))*Divider value 
	= (50Mhz/(256))*(1/16)	= 12.207Khz(12207Hz)
	*/
	rTCFG0 = (0<<8)|(0xff); //256
	rTCFG1 = (0<<20)|(3); // no DMA, (1/16)
	
	/* TCON¼³Á¤ :Dead zone disable,  auto reload on, output inverter off
	*  manual update no operation, timer0 stop, TCNTB0=0, TCMPB0 =0
	*/
	rTCON  = (0<<4)|(1<<3)|(1<<2)|(0<<1)|(0);

	rTCNTB0 = 0;
	rTCMPB0 = 0;
}

void Timer_Start(int hz)
{
	rSRCPND = 0xffffffff;
	rINTPND = 0xffffffff;
	
	
	rTCNTB0 = 12207/hz;

	rTCON |= (1<<1)|(0);
	rTCON &= ~(1<<1);
	
	rTCON |= 1;
	
	/* Interrupt Unmasking */
	rINTMSK &= ~(1<<10);
}