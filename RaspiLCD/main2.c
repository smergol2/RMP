//--------------------------------------------------------------------------------------------------
//                                  _            _     
//                                 | |          | |    
//      ___ _ __ ___  ___ _   _ ___| |_ ___  ___| |__  
//     / _ \ '_ ` _ \/ __| | | / __| __/ _ \/ __| '_ \. 
//    |  __/ | | | | \__ \ |_| \__ \ ||  __/ (__| | | |
//     \___|_| |_| |_|___/\__, |___/\__\___|\___|_| |_|
//                         __/ |                       
//                        |___/    Engineering (www.emsystech.de)
//                       
//
// Filename:    main.c
// Description: Demonstration for RaspiLCD
//    
//                       
// Author:      Martin Steppuhn
// History:     05.09.2012 Initial version
//				18.10.2012 Update vor erster Auslieferung
//				05.11.2012 Version 0.9.0
//--------------------------------------------------------------------------------------------------

//=== Includes =====================================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include "std_c.h"
#include "bcm2835.h"
#include "raspilcd.h"
#include "lcd.h"

//=== Preprocessing directives (#define) ===========================================================

//=== Type definitions (typedef) ===================================================================

//=== Global constants =============================================================================

//=== Global variables =============================================================================

//=== Local constants  =============================================================================

#include "bmp_raspi.inc"
#include "bmp_men.inc"

//=== Local variables ==============================================================================

uint16  DemoCount;
uint8	DemoView;
char	TempString[32];
uint16	DemoMem[256];
int volume = 5;

//=== Local function prototypes ====================================================================

//--------------------------------------------------------------------------------------------------
// Name:      Demo Functions
// Function:  
//            
// Parameter: 
// Return:    
//--------------------------------------------------------------------------------------------------

static void die(const char message[])
{
	perror("message");
	exit(EXIT_FAILURE);
}

void DemoLogo(void)
{
	LCD_ClearScreen();
	LCD_DrawBitmap(0,0,bmp_raspi);		
	LCD_SetFont(0);	
	LCD_PrintXY(70,4 ,"Raspi-LCD");		
	LCD_PrintXY(75,14,"Project");
	LCD_PrintXY(68,32,"powered by");
	LCD_PrintXY(70,42,"Emsystech");
	LCD_PrintXY(62,52,"Engineering");
}

void clearAll(void)
{
	for(int y = 0; y < 64; y++)
	{
		for(int x = 0; x < 128; x++)
		{
			LCD_PutPixel(x,y,0);
		}
	}
	LCD_WriteFramebuffer();	
}

void clearTitle(void)
{
	for(int y = 10; y < 45; y++)
	{
		for(int x = 0; x < 128; x++)
		{
			LCD_PutPixel(x,y,0);
		}
	}
	LCD_WriteFramebuffer();	
}

void clearButtons(void)
{
	for(int y = 45; y < 64; y++)
	{
		for(int x = 0; x < 128; x++)
		{
			LCD_PutPixel(x,y,0);
		}
	}
	LCD_WriteFramebuffer();	
}

void printMiniplay(int zeile, int zeileMAX)
{	
	int zeileAnzeige;
	if(zeile > 1 && zeile < zeileMAX-1)
	{
		zeileAnzeige = 2;
	}else if(zeile < 2){
		zeileAnzeige = zeile;
	}else{
		if(zeile == zeileMAX-1)
		{
			zeileAnzeige = 3;
		}else{
			zeileAnzeige = 4;
		}
	}

	int pixelZeileUp = zeileAnzeige*13;
	int pixelZeileDown = pixelZeileUp+11;
	LCD_SetPenColor(1);
	for(int i = pixelZeileUp; i< pixelZeileDown; i++){ 
		LCD_DrawLine(0, i, 127, i);
	}
	//LCD_DrawLine(0, pixelZeileDown, 127, pixelZeileDown);

	int pixelPlayUp = pixelZeileUp +2;
	int pixelPlayDown = pixelPlayUp +7;

	for(int x = 0; x < 5; x++)
	{
		while(pixelPlayUp < pixelPlayDown){
			LCD_PutPixel(1+x, pixelPlayDown, 0);
			LCD_PutPixel(1+x, pixelPlayUp, 0);
			pixelPlayDown--;
			pixelPlayUp++;
		}	
		pixelPlayUp = pixelZeileUp +2 +x;
		pixelPlayDown = pixelZeileUp +9 -x;
	}
}

int fliesstext(char *foo, int yPx, int zeile, int zeileMAX)
{
	int i = 8;
	int j = 127;

	int len = strlen(foo);

	foo[len-1] = ' ';
	foo[len] = '-';
	foo[len+1] = '-';
	foo[len+2] = '-';
	foo[len+3] = ' ';
	foo[len+4] = '\0';
	int pxlen = getPxLength(foo);

	LCD_SetFont(0);
	LCD_SetPenColor(1);

	while(1){
		SleepMs(100);
		LCD_SetPenColor(0);
		LCD_PrintXYVz(i,yPx,foo);
		LCD_WriteFramebuffer();
		LCD_SetPenColor(1);
		i--;

		if(i <= -(pxlen-120)){
			//LCD_SetFont(0);
			LCD_SetPenColor(0);
			LCD_PrintXYVz(j,yPx,foo);
			LCD_SetPenColor(1);
			LCD_WriteFramebuffer();
			j--;
		}
		if(i == -(pxlen)+8){
			i = 127;
			j = 127;
		}

		UpdateButtons();
		if(BUTTON_PRESSED_UP)
		{
			if(zeile < zeileMAX)
			{
				zeile++;
			}
			return zeile;
		}
		if(BUTTON_PRESSED_LEFT)
		{
			if(zeile > 0)
			{
				zeile--;
			}
			return zeile;
		}
		if(BUTTON_PRESSED_CENTER)
		{
			return -1;
		}if(BUTTON_PRESSED_DOWN)
		{
			FILE *write;
			write = fopen("/home/pi/workspace/song", "w+");
			fprintf(write, "%d\n", (zeile+1));
			fclose(write);
			SleepMs(200);

			FILE *write2;
			write2 = fopen("/home/pi/workspace/song", "w+");
			fprintf(write2, "%d\n", 0);
			fclose(write2);
		}
	}
	return 0;
}

void verzeichnis(char *foo)
{
	int zeile = 0;
	char tmp0[200];
	char tmp1[200];
	char tmp2[200];
	char tmp3[200];
	char tmp4[200];
	char tmp5[200];
	//char foo[200];
	int curr = 0;
	int x = 3;
	LCD_SetPenColor(1);
	int zeileMAX = 0;
	
	
	//find maximale anzahl zeilen
	FILE *read;
	read = fopen("/home/pi/workspace/dat", "r+");
	while(fgets(tmp5,200,read) != NULL)
	{
		zeileMAX++;
	}
	fclose(read);
	while(1){
		FILE *read;
		read = fopen("/home/pi/workspace/dat", "r+");
		clearAll();
		printMiniplay(zeile, zeileMAX);
		if(zeile == zeileMAX -1)
		{
			x = 4;
		}
		if(zeile == zeileMAX)
		{
			x = 5;
		}
		fprintf(stdout, "%d\n", zeileMAX);		

		while(curr < zeile-x){
			if(fgets(foo,200,read) == NULL){ continue;}
			curr++;
		}
		if(fgets(tmp0,200,read) == NULL)
		{
			return;
		}
		if(fgets(tmp1,200,read) == NULL)
		{
			return;
		}
		if(fgets(tmp2,200,read) == NULL)
		{
			return;
		}
		if(fgets(tmp3,200,read) == NULL)
		{
			return;
		}
		if(fgets(tmp4,200,read) == NULL)
		{
			return;
		}

		if(zeile == 0)
		{
			LCD_PrintXY(0,15,tmp1);
			LCD_PrintXY(0,28,tmp2);
			LCD_PrintXY(0,41,tmp3);
			LCD_PrintXY(0,54,tmp4);
			LCD_WriteFramebuffer();
			foo = tmp0;
			zeile = fliesstext(foo, 2, zeile, zeileMAX);
			if(zeile == -1){ return;}else{curr = 0;}
		}else if(zeile == 1)
		{
			foo = tmp1;
			LCD_PrintXY(0,2,tmp0);
			LCD_PrintXY(0,28,tmp2);
			LCD_PrintXY(0,41,tmp3);
			LCD_PrintXY(0,54,tmp4);

			LCD_WriteFramebuffer();
			zeile = fliesstext(foo, 15, zeile, zeileMAX);
			if(zeile == -1){ return;}else{curr = 0;}
		}else if(zeile > 1 && zeile < zeileMAX-1)
		{
			LCD_PrintXY(0,2,tmp0);
			LCD_PrintXY(0,15,tmp1);
			LCD_PrintXY(0,41,tmp3);
			LCD_PrintXY(0,54,tmp4);

			LCD_WriteFramebuffer();
			foo = tmp2;
			zeile = fliesstext(foo,28, zeile,zeileMAX);
			if(zeile == -1){ return;}else{curr = 0;}
		}else if(zeile == zeileMAX-1)
		{
			LCD_PrintXY(0,2,tmp0);
			LCD_PrintXY(0,15,tmp1);
			LCD_PrintXY(0,28,tmp2);
			LCD_PrintXY(0,54,tmp4);

			LCD_WriteFramebuffer();
			foo = tmp3;
			zeile = fliesstext(foo,41, zeile,zeileMAX);
			if(zeile == -1){ return;}else{curr = 0;}

		}else if(zeile == zeileMAX)
		{
			LCD_PrintXY(0,2,tmp0);
			LCD_PrintXY(0,15,tmp1);
			LCD_PrintXY(0,28,tmp2);
			LCD_PrintXY(0,41,tmp3);

			LCD_WriteFramebuffer();
			foo = tmp4;
			zeile = fliesstext(foo,54, zeile,zeileMAX);
			if(zeile == -1){ return;}else{curr = 0;}
		}

		curr = 0;
		LCD_WriteFramebuffer();
		fclose(read);
	}
}

void printButtons(int play)
{

	//LCD_ClearScreen();
	LCD_SetPenColor(1);
	LCD_SetFont(0);

	int down;
	int up;

	clearButtons();

	//prevButton
	down = 60;
	up = 49;
	for(int x = 0; x < 7; x++)
	{
		while(up < down){
			LCD_PutPixel(16-x, down, 1);
			LCD_PutPixel(16-x, up, 1);
			down--;
			up++;
		}
		down = 60-x;
		up = 49+x;
	}
	down = 60;
	up = 49;
	for(int x = 0; x < 7; x++)
	{
		while(up < down){
			LCD_PutPixel(22-x, down, 1);
			LCD_PutPixel(22-x, up, 1);
			down--;
			up++;
		}
		down = 60-x;
		up = 49+x;
	}

	//nextButton
	down = 60;
	up = 49;
	for(int x = 0; x < 7; x++)
	{
		while(up < down){
			LCD_PutPixel(106+x, down, 1);
			LCD_PutPixel(106+x, up, 1);
			down--;
			up++;
		}
		down = 60-x;
		up = 49+x;
	}
	down = 60;
	up = 49;
	for(int x = 0; x < 7; x++)
	{
		while(up < down){
			LCD_PutPixel(112+x, down, 1);
			LCD_PutPixel(112+x, up, 1);
			down--;
			up++;
		}
		down = 60-x;
		up = 49+x;
	}


	if(play == 0)
	{	
		//playButton
		down = 62;
		up = 47;
		for(int x = 0; x < 12; x++)
		{
			while(up < down){
				LCD_PutPixel(60+x, down, 1);
				LCD_PutPixel(60+x, up, 1);
				down--;
				up++;
			}
			down = 62-x;
			up = 47+x;
		}
	}else{
		//pauseButton
		for(int x = 0; x < 5; x++)
		{
			for(int y = 0; y < 14; y++)
			{
				LCD_PutPixel(56+x, 61-y, 1);
			}
			for(int y = 0; y < 14; y++)
			{
				LCD_PutPixel(64+x, 61-y, 1);
			}
		}


	}

	LCD_WriteFramebuffer();
}

void printVolume(void)
{
	clearAll();
	LCD_DrawRect(2,25,126,37,1);
	if(volume == 0)
	{ 
		LCD_WriteFramebuffer();
		return;
	}
	for(int i = 0; i < 9; i++){
		LCD_DrawLine(4, 27+i, 4 + volume*12, 27+i);
	}
	LCD_WriteFramebuffer();
}

void pauseSong(void)
{
	FILE *write;
	write = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write, "%s\n", "--pause");
	fclose(write);	

	SleepMs(100);

	FILE *write2;
	write2 = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write2, "%s\n", "xxx");
	fclose(write2);
}

void playSong(void)
{
	FILE *write;
	write = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write, "%s\n", "--start");
	fclose(write);	

	SleepMs(100);

	FILE *write2;
	write2 = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write2, "%s\n", "xxx");
	fclose(write2);
}

void lauter(void)
{
	volume += 1;

	if(volume > 9)
	{ 
		volume = 10;
		FILE *write;
		write = fopen("/home/pi/workspace/volume", "w+");
		fprintf(write, "%d\n", 2);
		fclose(write);	

		SleepMs(100);

		FILE *write2;
		write2 = fopen("/home/pi/workspace/volume", "w+");	
		fprintf(write2, "%d\n", 0);
		fclose(write2);

	}else{

		FILE *write;
		write = fopen("/home/pi/workspace/volume", "w+");
		fprintf(write, "%d\n", volume);
		fclose(write);	

		SleepMs(100);

		FILE *write2;
		write2 = fopen("/home/pi/workspace/volume", "w+");	
		fprintf(write2, "%d\n", 0);
		fclose(write2);
	}
}

void leiser(void)
{
	if(volume < 1){ volume = 0; return;}

	volume -= 1;
	FILE *write;
	write = fopen("/home/pi/workspace/volume", "w+");	
	fprintf(write, "%d\n", volume);
	fclose(write);	

	SleepMs(100);

	FILE *write2;
	write2 = fopen("/home/pi/workspace/volume", "w+");	
	fprintf(write2, "%d\n", 0);
	fclose(write2);

}


void prevSong(void)
{
	FILE *write;
	write = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write, "%s\n", "--prev");
	fclose(write);	

	SleepMs(100);

	FILE *write2;
	write2 = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write2, "%s\n", "xxx");
	fclose(write2);
}

void nextSong(void)
{
	FILE *write;
	write = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write, "%s\n", "--next");
	fclose(write);	

	SleepMs(100);

	FILE *write2;
	write2 = fopen("/home/pi/workspace/steuer", "w+");	
	fprintf(write2, "%s\n", "xxx");
	fclose(write2);
}



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{ 
	//int Contrast,Backlight;

	char *foo = malloc(200*sizeof(char));

	//Contrast = 9;
	//Backlight = 1;

	printf("RaspiLCD Demo V0.9 by Stadelmayer [" __DATE__ " " __TIME__"]\n");
	printf("RaspberryHwRevision=%i\r\n",GetRaspberryHwRevision());

	if(!RaspiLcdHwInit()) { printf("RaspiLcdHwInit() failed!\r\n"); return 1; }
	LCD_Init();			// Init Display
	SetBacklight(1);	// Turn Backlight on

	LCD_SetFont(1);

	DemoView = 0;
	int i = 128;
	int j = 128;
	int play = 1;
	int volWait = 1;

	LCD_PrintXY(34,26,"Hello :)");
	LCD_WriteFramebuffer();
	SleepMs(5000);	
	printButtons(play);
	LCD_PrintXY(1,2,"A Stadelmayer Project");

	while(1)
	{

		while(volWait > 0){

			UpdateButtons();
			if(BUTTON_PRESSED_LEFT)
			{
				lauter();
				printVolume();
				volWait = 10;
			}

			if(BUTTON_PRESSED_UP)
			{
				leiser();
				printVolume();
				volWait = 10;
			}
			SleepMs(50);
			volWait--;
		}

		volWait = 1;

		if(BUTTON_PRESSED_NEW)
		{
			nextSong();
			i = 128;
			j = 128;
		}

		if(BUTTON_PRESSED_RIGHT)
		{
			prevSong();
			i = 128;
			j = 128;	
		}

		if(BUTTON_PRESSED_DOWN)
		{
			if(play == 1)
			{
				pauseSong();
				play = 0;
				printButtons(play);
			}
			else
			{
				playSong();
				play = 1;
				printButtons(play);
			}
		}

		if(BUTTON_PRESSED_CENTER)
		{
			LCD_SetFont(0);
			verzeichnis(foo);

			clearAll();
			LCD_PrintXY(1,2,"A Stadelmayer Project");
			printButtons(play);
			LCD_SetFont(1);
		}	

		SleepMs(50);
		//***************************************** FLIESSTEXT MUSIKTITEL ***************************************************//
		FILE *read;

		char title[200];

		read = fopen("/home/pi/workspace/bla", "r");		// open as file
		if(fgets(title,200,read) != NULL){
			clearTitle();
		}else{continue;}			// get line
		int len = strlen(title);

		title[len-1] = ' ';
		title[len] = '-';
		title[len+1] = '-';
		title[len+2] = '-';
		title[len+3] = ' ';
		title[len+4] = '\0';
		int pxlen = getPxLength(title);
		LCD_SetFont(1);

		if(play == 0)
		{
			LCD_PrintXY(i,20,title);
			LCD_PrintXY(j,20,title);
			continue;
		}

		LCD_PrintXY(i,20,title);
		LCD_WriteFramebuffer();
		fclose(read);		

		i--;

		if(i <= -(pxlen-128)){
			LCD_SetFont(1);
			LCD_PrintXY(j,20,title);
			LCD_WriteFramebuffer();
			j--;
		}
		if(i == -(pxlen)){
			i = 0;
			j = 128;
		}
	}
	free(foo);
	return(0);
}
