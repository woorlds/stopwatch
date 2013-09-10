/*
 * =====================================================================
 * NAME         : Lcd.c
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
#include "DEMO256.h"
#include "my_lib.h"
#include <stdarg.h>

#define LCD_SIZE_X (480)
#define LCD_SIZE_Y (272)
#define CLKVAL  	4
#define HOZVAL 	   (LCD_SIZE_X-1)
#define LINEVAL    (LCD_SIZE_Y-1)

//#define VBPD (10)
//#define VFPD (10)
//#define VSPW (5)
//#define HBPD (10)
//#define HFPD (10)
//#define HSPW (5)

#define VBPD (10)
#define VFPD (4)
#define VSPW (10)
#define HBPD (45)
#define HFPD (8)
#define HSPW (41)

#define M5D(n) ((n) & 0x1fffff)	// To get lower 21bits

//#define  NonPal_Fb   ((volatile unsigned short(*)[480]) FRAME_BUFFER)
volatile unsigned short (*NonPal_Fb)[480]=(unsigned short (*)[480])0x33800000;

#define  Fb   ((volatile unsigned long(*)[120]) FRAME_BUFFER)

#include ".\fonts\ENG8X16.H"
#include ".\fonts\HAN16X16.H"
#include ".\fonts\HANTABLE.H"

// Macro

#define COPY(A,B) for(loop=0;loop<32;loop++) *(B+loop)=*(A+loop);
#define OR(A,B) for(loop=0;loop<32;loop++) *(B+loop)|=*(A+loop);

static unsigned char _first[]={0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };
static unsigned char _middle[]={0,0,0,1,2,3,4,5,0,0,6,7,8,9,10,11,0,0,12,13,14,15,16,17,0,0,18,19,20,21};
static unsigned char _last[]={0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0,17,18,19,20,21,22,23,24,25,26,27};
static unsigned char cho[]={0,0,0,0,0,0,0,0,0,1,3,3,3,1,2,4,4,4,2,1,3,0};
static unsigned char cho2[]={0,5,5,5,5,5,5,5,5,6,7,7,7,6,6,7,7,7,6,6,7,5};
static unsigned char jong[]={0,0,2,0,2,1,2,1,2,3,0,2,1,3,3,1,2,1,3,3,1,1};


unsigned int Fbuf[2] = {0x33800000, 0x33c00000};

//Function Declaration
void Lcd_Port_Init(void);
void Lcd_Init(void);
void NonPal_Lcd_Init(void);
void Palette_Init(void);
void Put_Pixel(unsigned long x, unsigned long y, unsigned long pal_addr);
void NonPal_Put_Pixel(int x, int y, int color);
void Lcd_Draw_BMP(int x, int y, const unsigned char *fp);

static unsigned short bfType;
static unsigned int bfSize;
static unsigned int bfOffbits;
static unsigned int biWidth, biWidth2;
static unsigned int biHeight;

void Lcd_Port_Init(void)
{
	
	rGPCUP = 0xffffffff; 
	rGPCCON &= ~(0xffffffff);
	rGPCCON |= 0xaaaaaaaa;
	
	rGPDUP = 0xffffffff; 
	rGPDCON &= ~(0xffffffff);
	rGPDCON |= 0xaaaaaaaa;
	
	/* GPG4 is setted as LCD_PWREN */
	rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
	rGPGCON=rGPGCON&(~(3<<8))|(3<<8); //GPG4=LCD_PWREN		
}


void Lcd_Set_Address(unsigned int fp)
{
	rLCDSADDR1= ((fp>>22)<<21)+(0x1fffff&(fp>>1));
	rLCDSADDR2 = (0x1fffff&(fp>>1))+(0x1fffff & ((fp+ (LCD_SIZE_X)*(LCD_SIZE_Y)*2)>>1));
	rLCDSADDR3= LCD_SIZE_X;
}

void Lcd_Select_Frame_Buffer(unsigned int id)
{
   	NonPal_Fb = (unsigned short (*)[480])(Fbuf[id]);
	Lcd_Set_Address(Fbuf[id]);
}


void Lcd_Init()
{	

	//	Fb = (unsigned long (*)[80])FRAME_BUFFER; 
	
	/* TO DO : setting for LCD control 
	* CLKVAL=1, PNRMODE=TFT Lcd panel, BPPMODE=8bpp for TFT, ENVID=disable
	*/
//	rLCDCON1 |= (CLKVAL<<8)|(0<<7)|(3<<5)|(0xb<<1)|(0<<0); // (���� 16bpp�̸� 0xc<<1)
//	rLCDCON2 |= (VBPD<<24)|(LINEVAL<<14)|(VFPD<<6)|(VSPW<0);
//	rLCDCON3 |= (HBPD<<19)|(HOZVAL<<8)|(HFPD<<0);
//	rLCDCON4 |= (13<<8)|(HSPW<07);
//	rLCDCON5 |= (0<<12)|(1<<11)|(0<<10)|(1<<9)|(1<<8)|(0<<3)|(0<<1)|(0<<0);

	rLCDCON1 = (CLKVAL<<8)|(0<<7)|(3<<5)|(0xb<<1)|(0<<0); // (���� 16bpp�̸� 0xc<<1)
	rLCDCON2 = (VBPD<<24)|(LINEVAL<<14)|(VFPD<<6)|(VSPW<0);
	rLCDCON3 = (HBPD<<19)|(HOZVAL<<8)|(HFPD<<0);
	rLCDCON4 = (0<<8)|(HSPW<07);
	rLCDCON5 = (0<<12)|(0<<11)|(0<<10)|(1<<9)|(1<<8)|(1<<3)|(0<<1)|(0<<0);	
	
	rLCDSADDR1 = ((FRAME_BUFFER>>22)<<21)+(0x1fffff&(FRAME_BUFFER>>1));
	rLCDSADDR2 = (0x1fffff & (((FRAME_BUFFER)+((LCD_SIZE_X)*(LCD_SIZE_Y)))>>1));
	rLCDSADDR3 = (LCD_SIZE_X);

	rTPAL = 0;   
	rTCONSEL &= ~(0x7);
	
	/* TO DO : ENVID Enable for video output and LCD control signal */  
	rLCDCON1 |= 1;
}

void NonPal_Lcd_Init()
{	
	 
	//	NonPal_Fb = (unsigned short int (*)[320])FRAME_BUFFER; 
	
	/* TO DO : setting for LCD control 
	* CLKVAL=1, PNRMODE=TFT Lcd panel, BPPMODE=16bpp for TFT, ENVID=disable
	*/
	rLCDCON1 = (CLKVAL<<8)|(0<<7)|(3<<5)|(0xc<<1)|(0<<0);
	rLCDCON2 = (VBPD<<24)|(LINEVAL<<14)|(VFPD<<6)|(VSPW<0);
	rLCDCON3 = (HBPD<<19)|(HOZVAL<<8)|(HFPD<<0);
	rLCDCON4 = (0<<8)|(HSPW<07);
	rLCDCON5 = (0<<12)|(0<<11)|(0<<10)|(1<<9)|(1<<8)|(1<<3)|(0<<1)|(0<<0);
	
	/* TO DO : Half-Word swap Enable  */
	rLCDCON5 |= 1;	
	Lcd_Select_Frame_Buffer(0);
	/*
	rLCDSADDR1= ((FRAME_BUFFER>>22)<<21)+(0x1fffff&(FRAME_BUFFER>>1));
	rLCDSADDR2 = (0x1fffff&(FRAME_BUFFER>>1))+(0x1fffff & ((FRAME_BUFFER + (LCD_SIZE_X)*(LCD_SIZE_Y)*2)>>1));
	rLCDSADDR3= LCD_SIZE_X;
	*/
	rTPAL = 0;
	rTCONSEL &= ~(0x7);     
	
	/* TO DO : ENVID Enable for video output and LCD control signal */
	rLCDCON1 |= 1;
}

 
void Put_Pixel(unsigned long x, unsigned long y, unsigned long pal_addr)
{	
	Fb[y][x/4] = (Fb[y][x/4]
	&~(0xff000000>>((x)%4)*8))|((pal_addr&0x000000ff)<<((4-1-((x)%4))*8));	
}

void NonPal_Put_Pixel(int x, int y, int color)
{
	/* TO DO : Fill FRAMEBUFFER with color value  */
	NonPal_Fb[y][x] = (unsigned short int)color;	
}

void Palette_Init()
{
	int i;
	unsigned int * palette;
	rLCDCON5 |= (1<<11);
	palette = (unsigned int *)PALETTE;
	for(i=0;i<256;i++)
		*palette ++ = DEMO256pal[i];
		
}

void Lcd_Draw_BMP(int x, int y, const unsigned char *fp)
{
     int xx=0, yy=0;	
     unsigned int tmp;
     unsigned char tmpR, tmpG, tmpB;
	
     bfType=*(unsigned short *)(fp+0);
     bfSize=*(unsigned short *)(fp+2);
     tmp=*(unsigned short *)(fp+4);
     bfSize=(tmp<<16)+bfSize;
     bfOffbits=*(unsigned short *)(fp+10);
     biWidth=*(unsigned short *)(fp+18);    
     biHeight=*(unsigned short *)(fp+22);    
     biWidth2=(bfSize-bfOffbits)/biHeight;	
     for(yy=0;yy<biHeight;yy++)
     {
         for(xx=0;xx<biWidth;xx++)
         {
             tmpB=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+0);
             tmpG=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+1);
             tmpR=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+2);
             tmpR>>=3;
             tmpG>>=3;
             tmpB>>=3;
             
             if(xx<biWidth2) NonPal_Put_Pixel(x+xx,y+yy,(tmpR<<11)+(tmpG<<6)+(tmpB<<1));
         } 
     }
    
}


//	Add
// Lcd_Han_Putch
void Lcd_Han_Putch(int x,int y,int color,int bkcolor, int data, int zx, int zy)
{
	unsigned int first,middle,last;	
	unsigned int offset,loop;
	unsigned char xs,ys;
	unsigned char temp[32];
	unsigned char bitmask[]={128,64,32,16,8,4,2,1};     

	first=(unsigned)((data>>8)&0x00ff);
	middle=(unsigned)(data&0x00ff);
	offset=(first-0xA1)*(0x5E)+(middle-0xA1);
	first=*(HanTable+offset*2);
	middle=*(HanTable+offset*2+1);
	data=(int)((first<<8)+middle);    

	first=_first[(data>>10)&31];
	middle=_middle[(data>>5)&31];
	last=_last[(data)&31];     

	if(last==0)
	{
		offset=(unsigned)(cho[middle]*640); 
		offset+=first*32;
		COPY(han16x16+offset,temp);

		if(first==1||first==24) offset=5120;  
		else offset=5120+704;
		offset+=middle*32;
		OR(han16x16+offset,temp);
	}
	else 
	{
		offset=(unsigned)(cho2[middle]*640); 
		offset+=first*32;
		COPY(han16x16+offset,temp);

		if(first==1||first==24) offset=5120+704*2; 
		else offset=5120+704*3;
		offset+=middle*32;
		OR(han16x16+offset,temp);

		offset=(unsigned)(5120+2816+jong[middle]*896);
		offset+=last*32;
		OR(han16x16+offset,temp);
	}

	for(ys=0;ys<16;ys++)
	{
		for(xs=0;xs<8;xs++)
		{
			if(temp[ys*2]&bitmask[xs])
			{
				if( (zx==1)&&(zy==1) ) NonPal_Put_Pixel(x+xs,y+ys,color);
				else if( (zx==2)&&(zy==1) )
				{
					NonPal_Put_Pixel(x+2*xs,y+ys,color);
					NonPal_Put_Pixel(x+2*xs+1,y+ys,color);
				}
				else if( (zx==1)&&(zy==2) )
				{
					NonPal_Put_Pixel(x+xs,y+2*ys,color);
					NonPal_Put_Pixel(x+xs,y+2*ys+1,color);
				}
				else if( (zx==2)&&(zy==2) )
				{
					NonPal_Put_Pixel(x+2*xs,y+2*ys+1,color);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys,color);
					NonPal_Put_Pixel(x+2*xs,y+2*ys,color);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys+1,color);
				}
			}
			else
			{
				if( (zx==1)&&(zy==1) ) 
				{
					//NonPal_Put_Pixel(x+xs,y+ys,bkcolor);
				}
				else if( (zx==2)&&(zy==1) )
				{
					//NonPal_Put_Pixel(x+2*xs,y+ys,bkcolor);
					//NonPal_Put_Pixel(x+2*xs+1,y+ys,bkcolor);
				}
				else if( (zx==1)&&(zy==2) )
				{
					//NonPal_Put_Pixel(x+xs,y+2*ys,bkcolor);
					//NonPal_Put_Pixel(x+xs,y+2*ys+1,bkcolor);
				}
				else if( (zx==2)&&(zy==2) )
				{
					//NonPal_Put_Pixel(x+2*xs,y+2*ys+1,bkcolor);
					//NonPal_Put_Pixel(x+2*xs+1,y+2*ys,bkcolor);
					//NonPal_Put_Pixel(x+2*xs,y+2*ys,bkcolor);
					//NonPal_Put_Pixel(x+2*xs+1,y+2*ys+1,bkcolor);
				}	   	
			}
		}

		for(xs=0;xs<8;xs++)
		{
			if(temp[ys*2+1]&bitmask[xs])
			{
				if( (zx==1)&&(zy==1) )
				NonPal_Put_Pixel(x+xs+8,y+ys,color);
				else if( (zx==2)&&(zy==1) ){
				NonPal_Put_Pixel(x+2*(xs+8),y+ys,color);
				NonPal_Put_Pixel(x+2*(xs+8)+1,y+ys,color);
				}
				else if( (zx==1)&&(zy==2) ){
				NonPal_Put_Pixel(x+(xs+8),y+2*ys,color);
				NonPal_Put_Pixel(x+(xs+8),y+2*ys+1,color);
				}
				else if( (zx==2)&&(zy==2) ){
				NonPal_Put_Pixel(x+2*(xs+8),y+2*ys+1,color);
				NonPal_Put_Pixel(x+2*(xs+8)+1,y+2*ys,color);
				NonPal_Put_Pixel(x+2*(xs+8),y+2*ys,color);
				NonPal_Put_Pixel(x+2*(xs+8)+1,y+2*ys+1,color);
				}
			}

			else
			{	   	
				if( (zx==1)&&(zy==1) )
				{
					//NonPal_Put_Pixel(x+xs+8,y+ys,bkcolor);	
				}
				else if( (zx==2)&&(zy==1) )
				{
					//NonPal_Put_Pixel(x+2*(xs+8),y+ys,bkcolor);
					//NonPal_Put_Pixel(x+2*(xs+8)+1,y+ys,bkcolor);
				}
				else if( (zx==1)&&(zy==2) )
				{
					//NonPal_Put_Pixel(x+(xs+8),y+2*ys,bkcolor);
					//NonPal_Put_Pixel(x+(xs+8),y+2*ys+1,bkcolor);
				}
				else if( (zx==2)&&(zy==2) )
				{
					//NonPal_Put_Pixel(x+2*(xs+8),y+2*ys+1,bkcolor);
					//NonPal_Put_Pixel(x+2*(xs+8)+1,y+2*ys,bkcolor);
					//NonPal_Put_Pixel(x+2*(xs+8),y+2*ys,bkcolor);
					//NonPal_Put_Pixel(x+2*(xs+8)+1,y+2*ys+1,bkcolor);
				}	   	
			}
		}
	}
}

// Lcd_Eng_Putch
void Lcd_Eng_Putch(int x,int y,int color,int bkcolor,int data, int zx, int zy)
{
	unsigned offset,loop;
	unsigned char xs,ys;
	unsigned char temp[32];
	unsigned char bitmask[]={128,64,32,16,8,4,2,1};     

	offset=(unsigned)(data*16);
	COPY(eng8x16+offset,temp);

	for(ys=0;ys<16;ys++)
	{
		for(xs=0;xs<8;xs++)
		{
			if(temp[ys]&bitmask[xs])
			{
				if( (zx==1)&&(zy==1) ) NonPal_Put_Pixel(x+xs,y+ys,color);
				else if( (zx==2)&&(zy==1) )
				{
					NonPal_Put_Pixel(x+2*xs,y+ys,color);
					NonPal_Put_Pixel(x+2*xs+1,y+ys,color);
				}
				else if( (zx==1)&&(zy==2) )
				{
					NonPal_Put_Pixel(x+xs,y+2*ys,color);
					NonPal_Put_Pixel(x+xs,y+2*ys+1,color);
				}
				else if( (zx==2)&&(zy==2) )
				{
					NonPal_Put_Pixel(x+2*xs,y+2*ys+1,color);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys,color);
					NonPal_Put_Pixel(x+2*xs,y+2*ys,color);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys+1,color);
				}
			} 
			else
			{
				if( (zx==1)&&(zy==1) )
				{
					NonPal_Put_Pixel(x+xs,y+ys,bkcolor);	
				}
				else if( (zx==2)&&(zy==1) )
				{
					NonPal_Put_Pixel(x+2*xs,y+ys,bkcolor);
					NonPal_Put_Pixel(x+2*xs+1,y+ys,bkcolor);
				}
				else if( (zx==1)&&(zy==2) )
				{
					NonPal_Put_Pixel(x+xs,y+2*ys,bkcolor);
					NonPal_Put_Pixel(x+xs,y+2*ys+1,bkcolor);
				}
				else if( (zx==2)&&(zy==2) )
				{
					NonPal_Put_Pixel(x+2*xs,y+2*ys+1,bkcolor);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys,bkcolor);
					NonPal_Put_Pixel(x+2*xs,y+2*ys,bkcolor);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys+1,bkcolor);
				}	   	
			} 
		}
	}
}

// Lcd_Puts
void Lcd_Puts(int x, int y, int color, int bkcolor, char *str, int zx, int zy)
{
     unsigned data;
   
     while(*str)
     {
        data=*str++;
        if(data>=128) 
        { 
             data*=256;
             data|=*str++;
             Lcd_Han_Putch(x, y, color, bkcolor, (int)data, zx, zy);
             x+=zx*16;
        }
        else 
        {
             Lcd_Eng_Putch(x, y, color, bkcolor, (int)data, zx, zy);
             x+=zx*8;
        }
     } 
} 

// Lcd_Printf
void Lcd_Printf(int x, int y, int color, int bkcolor, int zx, int zy, char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Lcd_Puts(x, y, color, bkcolor, string, zx, zy);
	va_end(ap);
}

void Lcd_Get_Info_BMP(int * x, int  * y, const unsigned short int *fp)
{
	*x =(int)fp[0];    
	*y =(int)fp[1];    
}


