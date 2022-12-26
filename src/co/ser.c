/*****************************************
		NanoShell Operating System
	   (C) 2021, 2022 iProgramInCpp

        Serial port console module 
******************************************/

#include <main.h>
#include <console.h>
#include <video.h>
#include <window.h>

void CoSerClearScreen(UNUSED Console *this)
{
	//send clear screen escape sequence?
}
void CoSerPlotChar (UNUSED Console *this, UNUSED int x, UNUSED int y, UNUSED char c)
{
	
}
void CoSerRefreshChar (UNUSED Console *this, UNUSED int x, UNUSED int y)
{
	
}
void CoSerScrollUpByOne(UNUSED Console *this)
{
	
}
bool CoSerPrintCharInt (UNUSED Console* this, char c, UNUSED bool bDontUpdateCursor)
{
	// Skip all nanoshell specific color codes:
	if (c == 0x01 || c == 0x02)
		return true;
	
	//WritePort(0xE9, c);
	
	//TODO: write to the correct FIFO
	
	return false;
}
void CoSerUpdateCursor(UNUSED Console* this)
{
	
}
void CoSerInit(UNUSED Console *this)
{
	
}

