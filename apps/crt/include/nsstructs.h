#ifndef _NSSTRUCTS_H
#define _NSSTRUCTS_H

// Basic includes everyone should have
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

typedef uint8_t BYTE;
typedef uint8_t bool;
#define false 0
#define true 1

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(*array))

#define UNUSED __attribute__((unused))

// Defines
#define TRANSPARENT 0xFFFFFFFF

#define FLAGS_TOO(flags, color) (flags | (color & 0XFFFFFF))

#define TEXT_RENDER_TRANSPARENT 0xFFFFFFFF
#define TEXT_RENDER_BOLD        0x01000000

#define TEXTSTYLE_HCENTERED   (1)
#define TEXTSTYLE_VCENTERED   (2)
#define TEXTSTYLE_WORDWRAPPED (4)
#define TEXTSTYLE_RJUSTIFY    (8)
#define TEXTSTYLE_FORCEBGCOL  (16)//VidDrawText does nothing to prevent that, but it's useful for CONTROL_TEXTCENTER.

#define TITLE_BAR_HEIGHT 18

#define WINDOW_BACKGD_COLOR (GetThemingParameter(P_WINDOW_BACKGD_COLOR)) //TODO

#define RECT(rect,x,y,w,h) do {\
	rect.left = x, rect.top = y, rect.right = x+w, rect.bottom = y+h;\
} while (0)

// Mark your system callbacks with this anyway!!!
#define CALLBACK

#define KEY_UNDEFINED_0 0
#define KEY_ESC 1
#define KEY_1 2
#define KEY_2 3
#define KEY_3 4
#define KEY_4 5
#define KEY_5 6
#define KEY_6 7
#define KEY_7 8
#define KEY_8 9
#define KEY_9 10
#define KEY_0 11
#define KEY_MINUS 12
#define KEY_HYPHEN KEY_MINUS
#define KEY_EQUALS 13
#define KEY_BACKSPACE 14
#define KEY_TAB 15
#define KEY_A 0x1e
#define KEY_B 0x30
#define KEY_C 0x2e
#define KEY_D 0x20
#define KEY_E 0x12
#define KEY_F 0x21
#define KEY_G 0x22
#define KEY_H 0x23
#define KEY_I 0x17
#define KEY_J 0x24
#define KEY_K 0x25
#define KEY_L 0x26
#define KEY_M 0x32
#define KEY_N 0x31
#define KEY_O 0x18
#define KEY_P 0x19
#define KEY_Q 0x10
#define KEY_R 0x13
#define KEY_S 0x1f
#define KEY_T 0x14
#define KEY_U 0x16
#define KEY_V 0x2f
#define KEY_W 0x11
#define KEY_X 0x2d
#define KEY_Y 0x15
#define KEY_Z 0x2c
#define KEY_BRACKET_LEFT 0x1a
#define KEY_BRACKET_RIGHT 0x1b
#define KEY_ENTER 0x1c
#define KEY_CONTROL 0x1d
#define KEY_CTRL KEY_CONTROL
#define KEY_SEMICOLON 0x27
#define KEY_APOSTROPHE 0x28
#define KEY_BACKTICK 0x29
#define KEY_LSHIFT 0x2a
#define KEY_BACKSLASH 0x2b
#define KEY_COMMA 0x33
#define KEY_DOT 0x34
#define KEY_SLASH 0x35
#define KEY_RSHIFT 0x36
#define KEY_PRINTSCREEN 0x37
#define KEY_ALT 0x38
#define KEY_SPACE 0x39
#define KEY_CAPSLOCK 0x3a
#define KEY_F1 0x3b
#define KEY_F2 0x3c
#define KEY_F3 0x3d
#define KEY_F4 0x3e
#define KEY_F5 0x3f
#define KEY_F6 0x40
#define KEY_F7 0x41
#define KEY_F8 0x42
#define KEY_F9 0x43
#define KEY_F10 0x44
#define KEY_NUMLOCK 0x45
#define KEY_SCROLLLOCK 0x46
#define KEY_HOME 0x47
#define KEY_ARROW_UP 0x48
#define KEY_PAGEUP 0x49
#define KEY_NUMPAD_MINUS 0x4a
#define KEY_NUMPAD_HYPHEN KEY_NUMPAD_MINUS
#define KEY_ARROW_LEFT 0x4b
#define KEY_LEFT KEY_ARROW_LEFT
#define KEY_UNDEFINED_4C 0x4c
#define KEY_ARROW_RIGHT 0x4d
#define KEY_RIGHT KEY_ARROW_RIGHT
#define KEY_NUMPAD_PLUS 0x4e
#define KEY_END 0x4f
#define KEY_ARROW_DOWN 0x50
#define KEY_DOWN KEY_ARROW_DOWN
#define KEY_PAGEDOWN 0x51
#define KEY_INSERT 0x52
#define KEY_DELETE 0x53
#define KEY_UNDEFINED_54 0x54
#define KEY_UNDEFINED_55 0x55
#define KEY_UNDEFINED_56 0x56
#define KEY_F11 0x57
#define KEY_F12 0x58
#define KEY_UP KEY_ARROW_UP
#define KEY_MENU 0x5D

#define SCANCODE_RELEASE 0x80

#define WINDOWS_MAX 256
#define WINDOW_TITLE_MAX 250
#define EVENT_QUEUE_MAX 256

#define KB_BUF_SIZE 512

#define O_RDONLY (1)
#define O_WRONLY (2)
#define O_RDWR   (O_RDONLY | O_WRONLY)
#define O_APPEND (4)
#define O_CREAT  (8)
#define O_EXEC   (1024)

//lseek whences
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// Structs and enums

// use with the negative prefix
enum
{
	ENOTHING,
	EACCES,
	EEXIST,
	EINTR,
	EINVAL,
	EIO,
	EISDIR,
	ELOOP,
	EMFILE,
	ENAMETOOLONG,
	ENFILE,
	ENOENT,
	ENOSR,
	ENOSPC,
	ENOTDIR,
	ENXIO,
	EOVERFLOW,
	EROFS,
	EAGAIN,
	ENOMEM,
	ETXTBUSY,
	EBADF,
	ESPIPE,
};

#define DefaultConsoleColor 0x0F

enum ConsoleType
{
	CONSOLE_TYPE_NONE, // uninitialized
	CONSOLE_TYPE_TEXT, // always full screen
	CONSOLE_TYPE_FRAMEBUFFER, // can either be the entire screen or just a portion of it. TODO
	CONSOLE_TYPE_SERIAL, // just plain old serial
	CONSOLE_TYPE_E9HACK, // Port E9 hack - qemu and bochs support this.
	CONSOLE_TYPE_WINDOW,
};

enum 
{
	ICON_NULL,
	ICON_CABINET,
	ICON_CHIP,
	ICON_CHIP_SQ,
	ICON_COMPUTER,
	ICON_COMPUTER_SHUTDOWN,
	ICON_DESKTOP,
	ICON_DRAW,
	ICON_EARTH,
	ICON_ERROR,
	ICON_EXECUTE_FILE,
	ICON_FILE,
	ICON_FILES,
	ICON_FOLDER,
	ICON_FOLDER_BLANK,
	ICON_FOLDER_MOVE,
	ICON_FOLDER_PARENT,
	ICON_FOLDER16_CLOSED,
	ICON_FOLDER16_OPEN,
	ICON_GLOBE,
	ICON_GO,
	ICON_HAND,
	ICON_HELP,
	ICON_INFO,
	ICON_KEYBOARD,
	ICON_KEYBOARD2,
	ICON_LAPTOP,
	ICON_NOTES,
	ICON_PAINT,
	ICON_SERIAL,
	ICON_STOP,
	ICON_TEXT_FILE,
	ICON_WARNING,
	ICON_NANOSHELL_LETTERS,
	ICON_NANOSHELL_LETTERS16,
	ICON_NANOSHELL,
	ICON_NANOSHELL16,
	ICON_BOMB,
	ICON_BOMB_SPIKEY,
	ICON_FILE16,
	ICON_TEXT_FILE16,
	ICON_EXECUTE_FILE16,
	ICON_FOLDER_PARENT16,
	//icons V1.1
	ICON_FOLDER_SETTINGS,
	ICON_CABINET16,
	ICON_COMPUTER16,
	ICON_COMMAND,
	ICON_COMMAND16,
	ICON_ERROR16,
	//icons V1.2
	ICON_LOCK,
	ICON_DIRECTIONS,
	ICON_CERTIFICATE,
	ICON_FILE_WRITE,
	ICON_SCRAP_FILE,
	ICON_SCRAP_FILE16,
	ICON_RESMON,
	ICON_BILLBOARD,
	ICON_FILE_CSCRIPT,
	ICON_FILE_CSCRIPT16,
	ICON_FILE_CLICK,
	ICON_KEYS,
	ICON_RESTRICTED,
	ICON_HOME,
	ICON_HOME16,
	ICON_ADAPTER,
	ICON_CLOCK,
	ICON_CLOCK16,
	//icons V1.3
	ICON_APPLICATION,
	ICON_APPLICATION16,
	ICON_TASKBAR,
	ICON_APP_DEMO,
	ICON_COMPUTER_FLAT,
	ICON_CALCULATOR,
	ICON_CALCULATOR16,
	ICON_DESKTOP2,
	ICON_MOUSE,
    //Icons V1.31
	ICON_AMBULANCE,
	//icons V1.32
	ICON_FONTS,
	ICON_FONTS16,
	//icons V1.33
	ICON_RESMON16,
	ICON_NOTES16,
	ICON_FILE_NANO,
	//icons V1.34
	ICON_CLOCK_EMPTY,//Special case which draws more stuff
	ICON_COUNT
};

enum
{
	EVENT_NULL,
	EVENT_CREATE,  // Shall be only called once, when a window or widget is created.
	EVENT_DESTROY, // Shall be only called once, when a window or widget is destroyed.
	EVENT_PAINT,
	EVENT_MOVE,
	EVENT_SIZE,
	EVENT_ACTIVATE,
	EVENT_SETFOCUS,
	EVENT_KILLFOCUS,
	EVENT_UPDATE,
	EVENT_MOVECURSOR,
	EVENT_CLICKCURSOR,
	EVENT_RELEASECURSOR,
	EVENT_COMMAND,
	EVENT_KEYPRESS,
	EVENT_CLOSE,
	EVENT_KEYRAW,
	EVENT_MINIMIZE,//do not call this normally.
	EVENT_UNMINIMIZE,
	EVENT_UPDATE2,
	EVENT_MENU_CLOSE,
	EVENT_MAX
};

//NOTE WHEN WORKING WITH CONTROLS:
//While yes, the window manager technically supports negative comboIDs, you're not supposed
//to use them.  They are used internally by other controls (for example list views and text input views).

enum
{
	//A null control.  Does nothing.
	CONTROL_NONE,
	//A text control printing text in its top-left corner.
	CONTROL_TEXT,
	//A control displaying an icon in the center of the rectangle.
	CONTROL_ICON,
	//A clickable button which triggers an EVENT_COMMAND with its comboID
	//as its first parm.
	CONTROL_BUTTON,
	//A text input field.  Not Finished
	CONTROL_TEXTINPUT,
	//A checkbox.  Not Finished.
	CONTROL_CHECKBOX,
	//A clickable label, which renders its text in the center-left.
	//Does the same as the CONTROL_BUTTON.
	CONTROL_CLICKLABEL,
	//A text control printing text in the center of the rectangle.
	CONTROL_TEXTCENTER,
	//A clickable button which triggers an event based on this->m_parm1
	//with its comboID as its first parm.
	CONTROL_BUTTON_EVENT,
	//A list view.  Complicated.
	CONTROL_LISTVIEW,
	//A vertical scroll bar.
	CONTROL_VSCROLLBAR,
	//A horizontal scroll bar.
	CONTROL_HSCROLLBAR,
	//A menu bar attached to the top of a window.
	//Adding more than one control is considered UB
	CONTROL_MENUBAR,
	//A text control printing big text (>127 chars)
	CONTROL_TEXTHUGE,
	//Same as CONTROL_LISTVIEW but with bigger icons.
	CONTROL_ICONVIEW,
	//Does nothing except surround other controls with a rectangle.  Useful for grouping settings.
	CONTROL_SURROUND_RECT,
	//Button with a colored background (parm2)
	CONTROL_BUTTON_COLORED,
	//Button as part of a list
	CONTROL_BUTTON_LIST,
	//Button with an icon on top.  Parm1= icon type, Parm2= icon size (16 or 32)
	CONTROL_BUTTON_ICON,
	//Button with an icon on top.  Parm1= icon type, Parm2= icon size (16 or 32)
	CONTROL_BUTTON_ICON_BAR,
	//A simple line control
	CONTROL_SIMPLE_HLINE,
	//This control is purely to identify how many controls we support
	//currently.  This control is unsupported and will crash your application
	//if you use this.
	CONTROL_COUNT
};

enum
{
	MBID_OK = 0x10010,
	MBID_CANCEL,
	MBID_ABORT,
	MBID_RETRY,
	MBID_IGNORE,
	MBID_YES,
	MBID_NO,
	MBID_TRY_AGAIN,
	MBID_CONTINUE,
	MBID_COUNT,
};

enum
{
	FONT_TAMSYN_REGULAR,
	FONT_TAMSYN_BOLD,
	FONT_PAPERM,
	FONT_FAMISANS,
	FONT_BASIC,
	FONT_GLCD,
	FONT_TAMSYN_MED_REGULAR,
	FONT_TAMSYN_MED_BOLD,
	FONT_TAMSYN_SMALL_REGULAR,
	FONT_TAMSYN_SMALL_BOLD,
	//FONT_BIGTEST,
	//FONT_BIGTEST2,
	FONT_LAST,
};

enum {
	P_BLACK,
	P_BACKGROUND_COLOR,
	P_BUTTON_MIDDLE_COLOR,
	P_WINDOW_BACKGD_COLOR,
	P_WINDOW_EDGE_COLOR,
	P_WINDOW_TITLE_ACTIVE_COLOR,
	P_WINDOW_TITLE_INACTIVE_COLOR,
	P_WINDOW_TITLE_ACTIVE_COLOR_B,
	P_WINDOW_TITLE_INACTIVE_COLOR_B,
	P_WINDOW_TITLE_TEXT_COLOR_SHADOW,
	P_WINDOW_TITLE_TEXT_COLOR,
	P_WINDOW_TEXT_COLOR,
	P_WINDOW_TEXT_COLOR_LIGHT,
	P_SYSTEM_FONT,
	P_THEME_PARM_COUNT
};

typedef struct
{
	int seconds,
		minutes,
		hours,
		weekday,
		day,
		month,
		year,
		statusA,
		statusB;
}
TimeStruct;

typedef struct
{
	int left, top, right, bottom;
}
Rectangle;

typedef struct
{
	int x, y;
}
Point;

typedef struct
{
	short width, height;
	const uint32_t *framebuffer;
}
Image;

typedef struct
{
	bool     m_available;			    //if the vbe display is available
	unsigned m_width, m_height, m_pitch;//bytes per row
	int      m_bitdepth;                //bits per pixel, only values we support: 0=8, 1=16, 2=32
	bool     m_dirty;					//useful if the framebuffer won't directly be pushed to the screen
	union {
		uint32_t* m_framebuffer32; //for ease of addressing
		uint16_t* m_framebuffer16;
		uint8_t * m_framebuffer8;
	};
	int m_pitch32, m_pitch16;      //uint32_t's and uint16_t's per row.
}
VBEData;

//Console
typedef struct ConsoleStruct
{
	int  type; // ConsoleType enum
	int  width, height; // width and height
	uint16_t *textBuffer; // unused in fb mode
	uint16_t color; // colors
	int  curX, curY; // cursor X and Y positions
	bool pushOrWrap;// check if we should push whole screen up, or clear&wrap
	VBEData* m_vbeData;//vbe data to switch to when drawing, ONLY APPLIES TO CONSOLE_TYPE_WINDOW!!
	int  offX, offY;
	int  font;
	int  cwidth, cheight;
	bool m_dirty;
	char m_inputBuffer[KB_BUF_SIZE];
	int  m_inputBufferBeg, m_inputBufferEnd;
	int  m_cursorFlashTimer, m_cursorFlashState;
}
Console;


struct WindowStruct;
struct ControlStruct;
typedef bool (*WidgetEventHandler) (struct ControlStruct*, int eventType, int parm1, int parm2, struct WindowStruct* parentWindow);
typedef void (*WindowProc)         (struct WindowStruct*, int, int, int);

typedef struct
{
	int  m_icon;//can be blank
	char m_contents [128];
}
ListItem;

typedef struct
{
	bool m_hasIcons;
	int  m_elementCount, m_capacity;
	int  m_scrollY;
	int  m_highlightedElementIdx;
	ListItem *m_pItems;
}
ListViewData;

typedef struct
{
	bool m_isBeingDragged, m_clickedBefore;
	bool m_yMinButton, m_yMaxButton;
	int  m_min, m_max, m_pos, m_dbi;
}
ScrollBarData;

typedef struct tagMenuBarTreeItem
{
	int  m_comboID;//can be searchable
	int  m_childrenCount,
	     m_childrenCapacity;//if childrenCount reaches this and we need to add another, double this
	struct tagMenuBarTreeItem* m_childrenArray;
	char m_text [104];
	//if this value is set, it gets drawn if this is an item part of the root tree, or the parent is open too.
	bool m_isOpen;
}
MenuBarTreeItem;

typedef struct
{
	bool m_clicked,
	     m_hovered;
}
ButtonData;

typedef struct
{
	MenuBarTreeItem m_root;
}
MenuBarData;

typedef struct
{
	bool  m_focused;
	bool  m_dirty;//Has it been changed since the dirty flag was set to false?
	bool  m_onlyOneLine, m_showLineNumbers;//note that these are mutually exclusive, but both can be turned off
	int   m_textCapacity, m_textLength;//The text length needs to be 1 less than the text capacity.
	                                   //If the text capacity is 65, for example, the textLength may not be bigger than 64.
	int   m_textCursorIndex, m_textCursorSelStart, m_textCursorSelEnd,
	      m_scrollY;
	char* m_pText;
	bool  m_readOnly;
}
TextInputData;

typedef struct
{
	bool m_clicked;
	bool m_checked;
}
CheckBoxData;

typedef struct ControlStruct
{
	bool      m_active;
	int       m_type;//CONTROL_XXX
	int       m_parm1, m_parm2;
	int       m_comboID;
	char      m_text[128];
	void*     m_dataPtr;
	Rectangle m_rect;
	bool      m_bMarkedForDeletion;
	
	//data for controls:
	union
	{
		ListViewData  m_listViewData;
		ScrollBarData m_scrollBarData;
		ButtonData    m_buttonData;
		MenuBarData   m_menuBarData;
		TextInputData m_textInputData;
		CheckBoxData  m_checkBoxData;
	};
	
	int m_anchorMode;
	
	//event handler
	WidgetEventHandler OnEvent;
	
	// A rect that was tried.  This is what the control's size _should_ be,
	// but due to some limitation m_triedRect may not match m_rect.
	Rectangle m_triedRect;
	
	// The smallest rectangle a control can occupy is 10x10.
}
Control;

typedef struct WindowStruct
{
	bool       m_used;
	bool       m_minimized;
	bool       m_hidden;
	bool       m_isBeingDragged;
	bool       m_isSelected;
	
	bool       m_renderFinished;
	
	char       m_title [250];
	
	int 	   m_flags;
	
	WindowProc m_callback;
	Rectangle  m_rect;
	Rectangle  m_rectBackup;
	//uint32_t*  m_framebuffer;
	//int        m_fbWidth, m_fbHeight;
	VBEData    m_vbeData;
	
	int        m_iconID;
	
	bool       m_eventQueueLock;
	short      m_eventQueue[EVENT_QUEUE_MAX];
	int        m_eventQueueParm1[EVENT_QUEUE_MAX];
	int        m_eventQueueParm2[EVENT_QUEUE_MAX];
	int        m_eventQueueSize;
	
	int        m_minWidth, m_minHeight;
	
	bool       m_markedForDeletion;
	
	Control*   m_pControlArray;
	int        m_controlArrayLen;
	
	void*      m_data; //user data
	
	void      *m_pOwnerThread, 
	          *m_pSubThread;//in case you ever want to use this
	
	Console*   m_consoleToFocusKeyInputsTo;
	
	bool       m_bWindowManagerUpdated;
} Window;

typedef Window* PWINDOW;


//BetterStrTok: https://github.com/iProgramMC/BetterStrTok
typedef struct {
    bool m_bInitted;
    char*m_pContinuation;
    char*m_pReturnValue;
} TokenState;


typedef struct
{
	int fd;
}
FILE;

#endif//_NSSTRUCTS_H