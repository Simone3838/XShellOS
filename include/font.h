//  ***************************************************************
//  font.h - Creation date: 03/03/2023
//  -------------------------------------------------------------
//  NanoShell Copyright (C) 2023 - Licensed under GPL V3
//
//  ***************************************************************
//  Programmer(s):  iProgramInCpp (iprogramincpp@gmail.com)
//  ***************************************************************

#ifndef _FONT_H
#define _FONT_H

/**
 * An enum with the font types available.
 */
enum {
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

enum
{
	FONTTYPE_MONOSPACE,
	FONTTYPE_SMALL,//varying width
	FONTTYPE_GLCD,
	FONTTYPE_BIG,
	FONTTYPE_BITMAP,
};

typedef struct
{
	uint8_t m_width;
	int     m_offset;
}
CharacterData;

typedef struct
{
	int           m_charID;
	CharacterData m_charData;
}
UnicodeCharacterData;

typedef struct
{
	uint8_t  m_fontType;
	const
	uint8_t *m_pFontData;
	int      m_charWidth;
	int      m_charHeight;
	bool     m_bAlreadyBold;
	int      m_altFontID;    // Alternate variant. If the font is 'already bold', this will point to the regular version of the font.
	int      m_unicodeTableSize;            // unused as of 3 March 2023
	UnicodeCharacterData* m_pUnicodeTable;
	CharacterData m_asciiData[256];
	CharacterData m_replacementChar;
}
ScreenFont;

#endif//_FONT_H