/*
 * =====================================================================
 * NAME         : stop-watch.c
 *
 * Descriptions : Main routine for S3C2440
 *
 * IDE          : CodeWarrior 2.0 or Later
 *
 * Modification
 *
 * =====================================================================
 */
#include "2440addr.h"

#include "includes.h"
#include "bicycle.h"

#include "common.h"

/* ********************************************************************* */
/* Global definitions */


/* ********************************************************************* */
/* File local definitions */

#define  TASK_STK_SIZE 		512                 /* Size of each task's stacks (# of WORDs) */
#define  NO_TASKS           10                  /* Number of identical tasks */
#define  MAZE_SIZE  		19


OS_STK   TaskStk[NO_TASKS][TASK_STK_SIZE];      /* Tasks stacks */
OS_STK   TaskStartStk[TASK_STK_SIZE];
char     TaskData[NO_TASKS];                    /* Parameters to pass to each task */


/***************************************
 * 
 * Title: Timer_Interrupt
 * 
 ***************************************/

void Timer0_ISR()
{
	/* TODO : Pending Register Clear */
	rSRCPND |= (1<<10);
	rINTPND |= (1<<10);

//	Uart_Send_String("T");
	OSTimeTick();

}

void Inval_ISR()
{
	Uart_Printf("Invalid Interrupt");
}

void (*irqtbl[32])(void)={
	Inval_ISR,	//	EINT0
	Inval_ISR,	//	EINT1
	Inval_ISR,	//	EINT2
	Inval_ISR,	//	EINT3
	Inval_ISR,	//	EINT4_7
	Inval_ISR,	//	EINT8_23
	Inval_ISR,	//	pISR_CAM
	Inval_ISR,	//	BAT_FLT	      
	Inval_ISR,	//	TICK	      
	Inval_ISR,	//	WDT_AC97T_AC97
	Timer0_ISR,	//	TIMER0	      
	Inval_ISR,	//	TIMER1	      
	Inval_ISR,	//	TIMER2	      
	Inval_ISR,	//	TIMER3	      
	Inval_ISR,	//	TIMER4	      
	Inval_ISR,	//      UART2	      
    Inval_ISR,      //      LCD	      
	Inval_ISR,      //      DMA0	      
	Inval_ISR,	//	DMA1	 														     
	Inval_ISR,	//	DMA2	 														     
	Inval_ISR,	//	DMA3	 														     
	Inval_ISR,	//	SDI	 														     
	Inval_ISR,	//	SPI0	 														     
	Inval_ISR,	//	UART1	 														     // Added for 2440.
	Inval_ISR,	//	NFCON	 														     
	Inval_ISR,	//	USBD	 														     
	Inval_ISR,	//	USBH	 														     
	Inval_ISR,	//	IIC	 														     
	Inval_ISR,	//	UART0	 																     
	Inval_ISR,	//	SPI1	 																     
	Inval_ISR,	//	RTC	 																     
	Inval_ISR	//	ADC	 																     
};

void C_IRQHandler(void)
{
	irqtbl[rINTOFFSET]();
}

void Task (void *data)
{
}

void TaskStart (void *data)
{
        U8 i;
        char key;

        data = data;                            /* Prevent compiler warning */

		Timer_Start(OS_TICKS_PER_SEC);
		
        OSStatInit();                           /* Initialize uC/OS-II's statistics */
		
#if 0
        for (i = 0; i < NO_TASKS; i++)
        {                                       /* Create NO_TASKS identical tasks */
                TaskData[i] = 1 + i;          /* Each task will display its own letter */
                OSTaskCreate(Task, (void *)&TaskData[i], (void *)&TaskStk[i][TASK_STK_SIZE - 1], i + 1);
                Lcd_Printf(300,30+20*i,BLACK,WHITE,1,1,"Task %d Shape: %c",i+1, i+1);
        }
#endif

        Uart_Printf("<-POWER OFF TO QUIT->\n");

		Lcd_Printf(5*8,16*16,WHITE,GREEN,1,1,"Task Count: ",OSTaskCtr);  
		Lcd_Printf(25*8,16*16,WHITE,GREEN,1,1,"CPU Usage: ",OSCPUUsage);  
		Lcd_Printf(40*8,16*16,WHITE,GREEN,1,1,"Context SW: ",OSCtxSwCtr);  

        while(1)
        {
 				Lcd_Printf(16*8,16*16,WHITE,GREEN,1,1,"%-3d",OSTaskCtr);  
 				Lcd_Printf(35*8,16*16,WHITE,GREEN,1,1,"%-3d",OSCPUUsage);  
 				Lcd_Printf(51*8,16*16,WHITE,GREEN,1,1,"%-3d",OSCtxSwCtr);  
							
                OSCtxSwCtr = 0;
                
                if(Uart_Get_Pressed()==0x1b)
                {
					while(1)
					{
						if(Uart_Get_Pressed()==0x1b)	break;
					}
                }

                OSTimeDlyHMSM(0, 0, 1, 0);      /* Wait one second */

        }
}

void Main()
{
	hardware_init();
	clear_screen(GREEN);

	Uart_Send_String("Stop Watch in uC/OS-II. Designed by Hyunwoo Lee\n");
	Lcd_Printf(10*8,0,WHITE,GREEN,1,1,"Stop Watch. Designed by Hyunwoo Lee");  

	Timer_Init();

    OSInit();                               /* Initialize uC/OS-II                                      */
    //RandomSem = OSSemCreate(1);             /* Random number semaphore                              */
    OSTaskCreate(TaskStart, (void *)0, (void *)&TaskStartStk[TASK_STK_SIZE - 1], 0);

    OSStart();                              /* Start multitasking                                       */
	
	while(1);	
}
