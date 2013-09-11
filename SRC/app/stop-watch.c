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


void Task (void *data)
{
	int  hour=0, min=0 , sec=0 , mm=0;
	
	while(1)
	{
		Lcd_Printf(300, 30+20 ,BLUE,GREEN,1,1,"%02d:%02d:%02d:%02d",hour, min, sec, mm);

		if( mm++ == 59 )
		{
			sec++; mm = 0 ;
		}
		if( sec == 59 )
		{
			min++; sec = 0 ;
		}
		if( min == 59 )
		{
			hour++; min = 0 ;
		}

	}
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

    	OSTaskCreate(Task, (void *)0, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);

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
						if(Uart_Get_Pressed()==0x1b)	break;
					}
                }

                OSTimeDlyHMSM(0, 0, 1, 0);      /* Wait one second */

        }
}
