;
; HSP3CL define and macros
;
#ifndef __hsp3cl__
#define __hsp3cl__
#runtime "hsp3cl"
#endif

;
;	Socket Enhance
;
#regcmd 18
#cmd sockopen $60
#cmd sockclose $61
#cmd sockreadbyte $62
#cmd sockget $63
#cmd sockgetc $64
#cmd sockgetm $65
#cmd sockgetb $66
#cmd sockgetbm $67
#cmd sockput $68
#cmd sockputc $69
#cmd sockputb $6a
#cmd sockmake $6b
#cmd sockwait $6c

;
;	Raspberry Pi Enhance
;
#module __rpgpio
#defcfunc gpioin int _p1

	devcontrol "gpioin",_p1
	return stat

#deffunc gpio int _p1, int _p2

	devcontrol "gpio",_p1,_p2
	return

#defcfunc i2cread int _p1

	devcontrol "i2cread",_p1
	return stat

#defcfunc i2creadw int _p1

	devcontrol "i2creadw",_p1
	return stat

#deffunc i2copen int _p1, int _p2

	devcontrol "i2copen",_p1,_p2
	return

#deffunc i2cclose int _p1

	devcontrol "i2cclose",_p1
	return

#deffunc i2cwrite int _p1,int _p2,int _p3

	devcontrol "i2cwrite",_p1,_p2,_p3
	return

#deffunc spireadw int _p1
	devcontrol "spireadw",_p1
	return

#deffunc spiread int _p1
	devcontrol "spiread",_p1
	return

#deffunc spiwrite int _p1,int _p2,int _p3
	devcontrol "spiwrite",_p1,_p2,_p3
	return

#deffunc spiopen int _p1
	devcontrol "spiopen",_p1,_p1
	return

#defcfunc spiget int _p1, int _p2
	devcontrol "readmcpduplex",_p1,_p2
	return stat

#deffunc spiclose int _p1
	devcontrol "spiclose",_p1
	return

#global

