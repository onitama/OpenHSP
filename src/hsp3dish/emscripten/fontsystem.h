//
//	fontsystem.cpp header
//
#ifndef __fontsystem_h
#define __fontsystem_h

void hgio_fontsystem_init(char* fontname, int size, int style);
void hgio_fontsystem_delete(int id);
int hgio_fontsystem_setup(int sx, int sy, void *buffer);
int hgio_fontsystem_exec(char* msg, unsigned char* buffer, int pitch, int* out_sx, int* out_sy);

#endif



