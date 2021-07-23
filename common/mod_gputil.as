;
;	utility header for HGIMG4
;
#ifndef __posthsp3gp__
#define __posthsp3gp__

#module "utilhsp3gp"

#deffunc gptree_init int _p1

	sdim buf,$1000
	objid = _p1
	i = 0
	return

#deffunc gptree_getsub str _p1, int _p2, local child

	;	
	;
	;
	repeat _p2 : buf+="    " : loop
	buf+=_p1+"\n"
	;
	child=""
	gpnodeinfo child,objid,GPNODEINFO_SKINROOT, _p1
	if child!="" {
		gptree_getsub child,_p2+1
	}
	child=""
	gpnodeinfo child,objid,GPNODEINFO_CHILD, _p1

	repeat
		if child="" : break
		gptree_getsub child,_p2+1
		gpnodeinfo child,objid,GPNODEINFO_SIBLING, child
	loop
	return

#deffunc gptree_get var _p1, int _p2

	gptree_init _p2
	gptree_getsub "",0
	_p1=buf
	return


#global

#endif
