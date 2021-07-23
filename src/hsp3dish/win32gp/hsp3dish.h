
//
//	hsp3dish.cpp header
//
#ifndef __hsp3dish_h
#define __hsp3dish_h

int app_init(void);
void app_bye(void);

int hsp3dish_exec( void );
int hsp3dish_init( HINSTANCE hInstance, char *startfile, HWND hParent );
int hsp3dish_reset(void);
int hsp3dish_setwindow(int bootprm, int wx, int wy);
void hsp3dish_bye(void);

void hsp3dish_dialog( char *mes );
char *hsp3dish_getlog(void);

void* hsp3dish_getinstance(void);

#endif
