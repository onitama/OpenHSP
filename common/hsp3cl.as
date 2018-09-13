;
; HSP3CL define and macros
;
#ifndef __hsp3cl__
#define __hsp3cl__
#runtime "hsp3cl"
#endif

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

#global

