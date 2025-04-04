//
// Classify Routine for hsed
// (C)LonelyWolf
//

#ifndef __classify_h
#define __classify_h

// Define type of structure
typedef struct tagMyColorRef{
	COLORREF Conf;
	COLORREF Combo;
} MYCOLORREF;

typedef struct tagMyColor{
	struct{
		MYCOLORREF Default;
		MYCOLORREF Function;
		MYCOLORREF Preprocessor;
		MYCOLORREF String;
		MYCOLORREF Macro;
		MYCOLORREF Comment;
		MYCOLORREF Label;
	} Character;

	struct{
		MYCOLORREF HalfSpace;
		MYCOLORREF FullSpace;
		MYCOLORREF Tab;
		MYCOLORREF NewLine;
		MYCOLORREF EndOfFile;
	} NonCharacter;

	struct{
		MYCOLORREF Background;
		MYCOLORREF CaretUnderLine;
		MYCOLORREF BoundaryLineNumber;
	} Edit;

	struct{
		MYCOLORREF Number;
		MYCOLORREF CaretLine;
	} LineNumber;

	struct{
		MYCOLORREF Number;
		MYCOLORREF Background;
		MYCOLORREF Division;
		MYCOLORREF Caret;
	} Ruler;

} MYCOLOR;

typedef struct tagClassifyTable{
	char Word1[256];
	char Word2[256];
	int Type;
	COLORREF *color;
	int Status;
	int Level;
	int pLevel;
} CLASSIFY_TABLE;

// Default color macroes
/*
#define DEFCOLOR_COMMENT          RGB(  0, 128,   0)
#define DEFCOLOR_FONT             RGB(  0,   0,   0)
#define DEFCOLOR_FUNC             RGB(  0,   0, 255)
#define DEFCOLOR_STRING           RGB(  0,   0,   0)
#define DEFCOLOR_LABEL            RGB(255,   0,   0)
#define DEFCOLOR_LINENUM          RGB( 49, 117, 189)
#define DEFCOLOR_MACRO            RGB(  0,   0,   0)
#define DEFCOLOR_RULER_FONT       RGB(  0,   0,   0)
#define DEFCOLOR_EOF              RGB( 74, 207, 189)
#define DEFCOLOR_FULL_SPACE       RGB(255, 150, 148)
#define DEFCOLOR_HALF_SPACE       RGB(255, 150, 148)
#define DEFCOLOR_NEWLINE          RGB(140,  81, 206)
#define DEFCOLOR_TAB              RGB(206, 182,  41)
#define DEFCOLOR_BACKGROUND       RGB(198, 195, 198)
#define DEFCOLOR_BOUNDARY_LINENUM RGB( 41, 178, 132)
#define DEFCOLOR_CARET_LINE       RGB(239, 231,  49)
#define DEFCOLOR_CARET_UNDERLINE  RGB(198,  48,  90)
#define DEFCOLOR_RULER_BACKGROUND RGB(239, 235, 222)
#define DEFCOLOR_RULER_CARET      RGB(  0, 203, 206)
#define DEFCOLOR_RULER_DIVISION   RGB(  0,   0,   0)
*/

// Default color macroes (onitamaê›íË)
#define DEFCOLOR_COMMENT          RGB(  0, 255,   0)
#define DEFCOLOR_FONT             RGB(255, 255, 255)
#define DEFCOLOR_FUNC             RGB(  0, 255, 255)
#define DEFCOLOR_PREPROCESSOR     RGB(  0, 255, 255)
#define DEFCOLOR_STRING           RGB(255, 255, 255)
#define DEFCOLOR_LABEL            RGB(255, 255,   0)
#define DEFCOLOR_LINENUM          RGB( 49, 117, 189)
#define DEFCOLOR_MACRO            RGB(192, 255, 255)
#define DEFCOLOR_RULER_FONT       RGB(  0,   0,   0)
#define DEFCOLOR_EOF              RGB(  0, 128, 128)
#define DEFCOLOR_FULL_SPACE       RGB(128,   0,   0)
#define DEFCOLOR_HALF_SPACE       RGB(  0, 128, 128)
#define DEFCOLOR_NEWLINE          RGB(  0, 128, 128)
#define DEFCOLOR_TAB              RGB(  0, 128, 128)
#define DEFCOLOR_BACKGROUND       RGB(  0,   0,   0)
#define DEFCOLOR_BOUNDARY_LINENUM RGB( 41, 178, 132)
#define DEFCOLOR_CARET_LINE       RGB(239, 231,  49)
#define DEFCOLOR_CARET_UNDERLINE  RGB(198,  48,  90)
#define DEFCOLOR_RULER_BACKGROUND RGB(239, 235, 222)
#define DEFCOLOR_RULER_CARET      RGB(  0, 203, 206)
#define DEFCOLOR_RULER_DIVISION   RGB(  0,   0,   0)

void InitClassify();
void SetClassify(int);
void ResetClassify();
void ByeClassify();

void SetEditColor(int);
void SetAllEditColor();

#endif