#define FONT15_WIDTH	3
#define FONT15_HEIGHT	5
#define FONT15_LENGTH	96
#define FONT15_BEGIN	0x20
#define FONT15_END		0x7E

extern BYTE g_anFont[FONT15_LENGTH][FONT15_WIDTH * FONT15_HEIGHT];

int Font15_GetCharIndex(char c);