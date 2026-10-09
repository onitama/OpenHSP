
//
//	hsp3dish.cpp header
//
#ifndef __hsp3dish_h
#define __hsp3dish_h

int app_init(void);
void app_bye(void);

int hsp3dish_exec( void );
int hsp3dish_init(HINSTANCE hInstance, const char* startfile, HWND hParent);
int hsp3dish_reset(void);
void hsp3dish_dialog( char *mes );
void hsp3dish_bye(void);

#endif
