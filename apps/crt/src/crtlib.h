#ifndef _CRTLIB_H
#define _CRTLIB_H

#include "../include/nsstructs.h"

#define WCALL_VERSION 12
enum
{
	// System Calls V1.0
	#if WCALL_VERSION >= 10
		CALL_NOTHING,
		//Video Driver calls:
		VID_GET_SCREEN_WIDTH,
		VID_GET_SCREEN_HEIGHT,
		VID_PLOT_PIXEL,
		VID_FILL_SCREEN, //Actually fills the context, not the screen
		VID_DRAW_H_LINE,
		VID_DRAW_V_LINE,
		VID_DRAW_LINE,
		VID_SET_FONT,
		VID_PLOT_CHAR,
		VID_BLIT_IMAGE,
		VID_TEXT_OUT,
		VID_TEXT_OUT_INT,
		VID_DRAW_TEXT,
		VID_SHIFT_SCREEN,
		VID_FILL_RECT,
		VID_DRAW_RECT,
		VID_FILL_RECT_H_GRADIENT,
		VID_FILL_RECT_V_GRADIENT,
		
		//Window Manager calls:
		WIN_CREATE,
		WIN_HANDLE_MESSAGES,
		WIN_DEFAULT_PROC,
		WIN_DESTROY,//don't use
		WIN_MESSAGE_BOX,
		WIN_ADD_CONTROL,
	#endif
	
	// System Calls V1.1
	#if WCALL_VERSION >= 11
		WIN_REQUEST_REPAINT,
		WIN_SET_LABEL_TEXT,
		WIN_ADD_MENUBAR_ITEM,
		WIN_SET_SCROLL_BAR_MIN,
		WIN_SET_SCROLL_BAR_MAX,
		WIN_SET_SCROLL_BAR_POS,
		WIN_GET_SCROLL_BAR_POS,
		WIN_ADD_ELEM_TO_LIST,
		WIN_REM_ELEM_FROM_LIST,
		WIN_GET_ELEM_STR_FROM_LIST,
		WIN_CLEAR_LIST,
		
		CON_PUTSTRING,
		CON_READCHAR,
		CON_READSTR,
		
		MM_ALLOCATE_D,
		MM_FREE,
		MM_DEBUG_DUMP,
		
		FI_OPEN_D,
		FI_CLOSE,
		FI_READ,
		FI_WRITE,
		FI_TELL,
		FI_TELLSIZE,
		FI_SEEK,
	#endif
	
	// System Calls V1.2
	#if WCALL_VERSION >= 12
		WIN_SET_HUGE_LABEL_TEXT,
		WIN_SET_INPUT_TEXT_TEXT,
		WIN_SET_WINDOW_ICON,
		WIN_SET_WINDOW_TITLE,
		WIN_REGISTER_EVENT,
		WIN_REGISTER_EVENT2,
		
		TM_GET_TICK_COUNT,
		TM_GET_TIME,
		
		CPU_GET_TYPE,
		CPU_GET_NAME,
		
		CON_GET_CURRENT_CONSOLE,
		
		//V1.21
		VID_BLIT_IMAGE_RESIZE,
		TM_SLEEP,
		
		//V1.22
		WIN_SET_ICON,
		
		//V1.23
		NS_GET_VERSION,
		
		//V1.24
		WIN_GET_THEME_PARM,
		WIN_SET_THEME_PARM,
	#endif
};

int    memcmp     (const void* ap, const void* bp, size_t size);
void*  memcpy     (void* restrict dstptr, const void* restrict srcptr, size_t size);
void*  memmove    (void* restrict dstptr, const void* restrict srcptr, size_t size);
void*  memset     (void* bufptr, BYTE val, size_t size);
size_t strlen     (const char* str);
void*  strcpy     (const char* ds, const char* ss);
int    strcmp     (const char* as, const char* bs);
void   strcat     (char* dest, char* after);
void   strtolower (char* as);
void   strtoupper (char* as);
void   memtolower (char* as, int w);
void   memtoupper (char* as, int w);
size_t strgetlento(const char* str, char chr);
int    atoi       (const char* str);
char*  Tokenize   (TokenState* pState, char* pString, char* separator);

#endif//_CRTLIB_H