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

#include "stop-watch.h"
#include "common.h"


void Main()
{
	// HW init
	hardware_init();

	// Clear screen to green
	clear_screen(GREEN);

	Uart_Send_String("Stop Watch in uC/OS-II. Designed by Hyunwoo Lee\n");
	Lcd_Printf(10*8,0,BLUE,GREEN,1,1,"Stop Watch. Designed by Hyunwoo Lee");  

	Timer_Init();

	OSInit();                               /* Initialize uC/OS-II */
	//RandomSem = OSSemCreate(1);             /* Random number semaphore */
	OSTaskCreate(TaskStart, (void *)0, (void *)&TaskStartStk[TASK_STK_SIZE - 1], 0);

	OSStart();                              /* Start multitasking */

	while(1);	
}


void Timer0_ISR(void)
{
	rSRCPND |= (1<<10);
	rINTPND |= (1<<10);

	OSTimeTick();
}


void Inval_ISR(void)
{
	Uart_Printf("Invalid Interrupt");
}


void C_IRQHandler(void)
{
	irqtbl[rINTOFFSET]();
}


void Task1 (void *data)
{
	int  hour=0, min=0, sec=0, mm=0;

	while(1)
	{
#if HIGH_QUALITY
		Lcd_Printf(80, 50 ,BLUE,GREEN,1,1,"%02d:%02d:%02d:%02d",hour, min, sec, mm);
#else
		Lcd_Printf(80, 50 ,BLUE,GREEN,1,1,"%02d:%02d:%02d",hour, min, sec);
#endif


#if HIGH_QUALITY
		OSTimeDlyHMSM(0,0,0,20);
		if( mm++ == 60 )
		{
			sec++; mm = 0 ;
		}
#else
		OSTimeDlyHMSM(0,0,1,0);
		sec++;
#endif
		if( sec == 60 )
		{
			min++; sec = 0 ;
		}
		if( min == 60 )
		{
			hour++; min = 0 ;
		}
	}
}

void Task2(void *data)
{
	int  hour=0, min=0, sec=0, mm=0;

	while(1)
	{
#if HIGH_QUALITY
		Lcd_Printf(80, 70 ,BLUE,GREEN,1,1,"%02d:%02d:%02d:%02d",hour, min, sec, mm);
#else
		Lcd_Printf(80, 70 ,BLUE,GREEN,1,1,"%02d:%02d:%02d",hour, min, sec);
#endif

#if HIGH_QUALITY
		OSTimeDlyHMSM(0,0,0,30);
		if( mm++ == 60 )
		{
			sec++; mm = 0 ;
		}
#else
		OSTimeDlyHMSM(0,0,2,0);
		sec++;
#endif
		if( sec == 60 )
		{
			min++; sec = 0 ;
		}
		if( min == 60 )
		{
			hour++; min = 0 ;
		}
	}
}

void Task3 (void *data)
{
	int  hour=0, min=0, sec=0, mm=0;

	while(1)
	{
#if HIGH_QUALITY
		Lcd_Printf(80, 90 ,BLUE,GREEN,1,1,"%02d:%02d:%02d:%02d",hour, min, sec, mm);
#else
		Lcd_Printf(80, 90 ,BLUE,GREEN,1,1,"%02d:%02d:%02d",hour, min, sec);
#endif

#if HIGH_QUALITY
		OSTimeDlyHMSM(0,0,0,40);
		if( mm++ == 60 )
		{
			sec++; mm = 0 ;
		}
#else
		OSTimeDlyHMSM(0,0,3,0);
		sec++;
#endif
		if( sec == 60 )
		{
			min++; sec = 0 ;
		}
		if( min == 60 )
		{
			hour++; min = 0 ;
		}
	}
}

void Task4 (void *data)
{
	int  hour=0, min=0, sec=0, mm=0;

	while(1)
	{
#if HIGH_QUALITY
		Lcd_Printf(80, 110 ,BLUE,GREEN,1,1,"%02d:%02d:%02d:%02d",hour, min, sec, mm);
#else
		Lcd_Printf(80, 110 ,BLUE,GREEN,1,1,"%02d:%02d:%02d",hour, min, sec);
#endif

#if HIGH_QUALITY
		OSTimeDlyHMSM(0,0,0,50);
		if( mm++ == 60 )
		{
			sec++; mm = 0 ;
		}
#else
		OSTimeDlyHMSM(0,0,4,0);
		sec++;
#endif
		if( sec == 60 )
		{
			min++; sec = 0 ;
		}
		if( min == 60 )
		{
			hour++; min = 0 ;
		}
	}
}


void TaskStart (void *data)
{
	U8 i;
	char key;
	int idx=0;

	/* Prevent compiler warning */
	data = data;

	Timer_Start(OS_TICKS_PER_SEC);

	/* Initialize uC/OS-II's statistics */
	OSStatInit();

	for(idx=0; idx<NO_TASKS; idx++)
	{
		TaskData[idx] = 1 + idx;
		Lcd_Printf(308,50+(20*idx),BLUE,GREEN,1,1,"Task %d",idx+1);
	}

	OSTaskCreate(Task1, (void *)0, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);
	OSTaskCreate(Task2, (void *)0, (void *)&TaskStk[1][TASK_STK_SIZE - 1], 2);
	OSTaskCreate(Task3, (void *)0, (void *)&TaskStk[2][TASK_STK_SIZE - 1], 3);
	OSTaskCreate(Task4, (void *)0, (void *)&TaskStk[3][TASK_STK_SIZE - 1], 4);

	Uart_Printf("<-POWER OFF TO QUIT->\n");

	Lcd_Printf(5*8,16*16,BLUE,GREEN,1,1,"Task Count: ",OSTaskCtr);  
	Lcd_Printf(25*8,16*16,BLUE,GREEN,1,1,"CPU Usage: ",OSCPUUsage);  
	Lcd_Printf(40*8,16*16,BLUE,GREEN,1,1,"Context SW: ",OSCtxSwCtr);  

	while(1)
	{
		Lcd_Printf(16*8,16*16,BLUE,GREEN,1,1,"%-3d",OSTaskCtr);  
		Lcd_Printf(35*8,16*16,BLUE,GREEN,1,1,"%-3d",OSCPUUsage);  
		Lcd_Printf(51*8,16*16,BLUE,GREEN,1,1,"%-3d",OSCtxSwCtr);  

		OSCtxSwCtr = 0;

		if(Uart_Get_Pressed()==0x1b)
		{
			while(1)
			{
				if(Uart_Get_Pressed()==0x1b)
					break;
			}
		}

		OSTimeDlyHMSM(0, 0, 1, 0);      /* Wait one second */
	}
}
