//
//		settings for HSP
//

char 	s_rec[1]= "", *hsp_prestr[] =
{
	//
	//	label check
	//	  | opt value
	//	  | |
	//	  | | type
	//	  | | | keyword
	//	  | | |  |
	//	  | | |  |
	//	"$000 8 goto",
	//
	s_rec,

	//	program control commands (ver3.0)

	"$000 15 goto",
	"$001 15 gosub",
	"$002 15 return",
	"$003 15 break",
	"$004 15 repeat",
	"$005 15 loop",
	"$006 15 continue",
	"$007 15 wait",
	"$008 15 await",

	"$009 15 dim",
	"$00a 15 sdim",
//	"$00d 15 skiperr",				// delete
	"$00b 15 foreach",				// (ver3.0)
//	"$00c 15 eachchk",				// (ver3.0) hidden
	"$00d 15 dimtype",				// (ver3.0)
	"$00e 15 dup",
	"$00f 15 dupptr",				// (ver3.0)

	"$010 15 end",
	"$011 15 stop",
	"$012 15 newmod",				// (ver3.0)
//	"$013 15 setmod",				// (ver3.0)
	"$014 15 delmod",				// (ver3.0)
//	"$015 15 alloc",				// (changed 3.0)
	"$016 15 mref",					// (ver2.5)
	"$017 15 run",
	"$018 15 exgoto",				// ver2.6
	"$019 15 on",					// ver2.6
	"$01a 15 mcall",				// (ver3.0)
	"$01b 15 assert",				// (ver3.0)
	"$01c 15 logmes",				// (ver3.0)
	"$01d 15 newlab",				// (ver3.2)
	"$01e 15 resume",				// (ver3.2)
	"$01f 15 yield",				// (ver3.2)
	"$020 15 strexchange",			// (ver3.6)

//	"$015 15 logmode",				// (ver2.55)
//	"$016 15 logmes",				// (ver2.55)

	//	enhanced command (ver2.6)

	"$10000 8 onexit",
	"$10001 8 onerror",
	"$10002 8 onkey",
	"$10003 8 onclick",
	"$10004 8 oncmd",

	"$011 8 exist",
	"$012 8 delete",
	"$013 8 mkdir",
	"$014 8 chdir",

	"$015 8 dirlist",
	"$016 8 bload",
	"$017 8 bsave",
	"$018 8 bcopy",
	"$019 8 memfile",				// (changed on ver2.6*)

	//	no macro command (ver2.61)
	//
	"$000 11 if",
	"$001 11 else",

	//	normal commands

	"$01a 8 poke",
	"$01b 8 wpoke",
	"$01c 8 lpoke",
	"$01d 8 getstr",
	"$01e 8 chdpm",					// (3.0)
	"$01f 8 memexpand",				// (3.0)
	"$020 8 memcpy",				// (ver2.55)
	"$021 8 memset",				// (ver2.55)

	"$022 8 notesel",				// (changed on ver2.55)
	"$023 8 noteadd",				// (changed on ver2.55)
	"$024 8 notedel",				// (changed on ver2.55)
	"$025 8 noteload",				// (changed on ver2.6*)
	"$026 8 notesave",				// (changed on ver2.6*)
	"$027 8 randomize",				// (changed on ver3.0)
	"$028 8 noteunsel",				// (changed on ver3.0)
	"$029 8 noteget",				// (changed on ver2.55)
	"$02a 8 split",					// (3.2)
	"$02b 8 strrep",				// (3.4)
	"$02c 8 setease",				// (3.4)
	"$02d 8 sortval",				// (3.5)
	"$02e 8 sortstr",				// (3.5)
	"$02f 8 sortnote",				// (3.5)
	"$030 8 sortget",				// (3.5)

	//	enhanced command (ver2.2)

	"$10000 9 button",

	"$001 9 chgdisp",
	"$002 9 exec",
	"$003 9 dialog",

//	"$007 9 palfade",				// delete
//	"$009 9 palcopy",				// delete

	"$008 9 mmload",
	"$009 9 mmplay",
	"$00a 9 mmstop",
	"$00b 9 mci",

	"$00c 9 pset",
	"$00d 9 pget",
	"$00e 9 syscolor",				// (ver3.0)

	"$00f 9 mes",
	"$00f 9 print",
	"$010 9 title",
	"$011 9 pos",
	"$012 9 circle",				// (ver3.0)
	"$013 9 cls",
	"$014 9 font",
	"$015 9 sysfont",
	"$016 9 objsize",
	"$017 9 picload",
	"$018 9 color",
	"$019 9 palcolor",
	"$01a 9 palette",
	"$01b 9 redraw",
	"$01c 9 width",
	"$01d 9 gsel",
	"$01e 9 gcopy",
	"$01f 9 gzoom",
	"$020 9 gmode",
	"$021 9 bmpsave",

//	"$022 9 text",					// delete

	"$022 9 hsvcolor",				// (ver3.0)
	"$023 9 getkey",

	"$024 9 listbox",
	"$025 9 chkbox",
	"$026 9 combox",

	"$027 9 input",
	"$028 9 mesbox",
	"$029 9 buffer",
	"$02a 9 screen",
	"$02b 9 bgscr",

	"$02c 9 mouse",
	"$02d 9 objsel",
	"$02e 9 groll",
	"$02f 9 line",

	"$030 9 clrobj",
	"$031 9 boxf",

	//	enhanced command (ver2.3)

	"$032 9 objprm",
	"$033 9 objmode",
	"$034 9 stick",
//	"$041 9 objsend",				// delete
	"$035 9 grect",					// (ver3.0)
	"$036 9 grotate",				// (ver3.0)
	"$037 9 gsquare",				// (ver3.0)
	"$038 9 gradf",					// (ver3.2)
	"$039 9 objimage",				// (ver3.2)
	"$03a 9 objskip",				// (ver3.2)
	"$03b 9 objenable",				// (ver3.2)
	"$03c 9 celload",				// (ver3.2)
	"$03d 9 celdiv",				// (ver3.2)
	"$03e 9 celput",				// (ver3.2)

	"$03f 9 gfilter",				// (ver3.5)
	"$040 9 setreq",				// (ver3.5)
	"$041 9 getreq",				// (ver3.5)
	"$042 9 mmvol",					// (ver3.5)
	"$043 9 mmpan",					// (ver3.5)
	"$044 9 mmstat",				// (ver3.5)
	"$045 9 mtlist",				// (ver3.5)
	"$046 9 mtinfo",				// (ver3.5)
	"$047 9 devinfo",				// (ver3.5)
	"$048 9 devinfoi",				// (ver3.5)
	"$049 9 devprm",				// (ver3.5)
	"$04a 9 devcontrol",			// (ver3.5)
	"$04b 9 httpload",				// (ver3.5)
	"$04c 9 httpinfo",				// (ver3.5)
	"$05d 9 gmulcolor",				// (ver3.5)
	"$05e 9 setcls",				// (ver3.5)
	"$05f 9 celputm",				// (ver3.5)

	"$05c 9 celbitmap",				// (ver3.6)
	"$04d 9 objcolor",				// (ver3.6)
	"$04e 9 rgbcolor",				// (ver3.6)
	"$04f 9 viewcalc",				// (ver3.6)
	"$050 9 layerobj",				// (ver3.6)

	//	enhanced command (ver3.0)

	"$000 17 newcom",
	"$001 17 querycom",
	"$002 17 delcom",
	"$003 17 cnvstow",
	"$004 17 comres",
	"$005 17 axobj",
	"$006 17 winobj",
	"$007 17 sendmsg",
	"$008 17 comevent",
	"$009 17 comevarg",
	"$00a 17 sarrayconv",
	//"$00b 17 variantref",
	"$00c 17 cnvstoa",				// (ver3.5)

	"$100 17 callfunc",
	"$101 17 cnvwtos",
	"$102 17 comevdisp",
	"$103 17 libptr",
	"$104 17 cnvatos",				// (ver3.5)

	//	3.0 system vals

	"$000 14 system",
	"$001 14 hspstat",
	"$002 14 hspver",
	"$003 14 stat",
	"$004 14 cnt",
	"$005 14 err",
	"$006 14 strsize",
	"$007 14 looplev",					// (ver2.55)
	"$008 14 sublev",					// (ver2.55)
	"$009 14 iparam",					// (ver2.55)
	"$00a 14 wparam",					// (ver2.55)
	"$00b 14 lparam",					// (ver2.55)
	"$00c 14 refstr",
	"$00d 14 refdval",					// (3.0)

	//	3.0 internal function
	"$000 13 int",
	"$001 13 rnd",
	"$002 13 strlen",
	"$003 13 length",					// (3.0)
	"$004 13 length2",					// (3.0)
	"$005 13 length3",					// (3.0)
	"$006 13 length4",					// (3.0)
	"$007 13 vartype",					// (3.0)
	"$008 13 gettime",
	"$009 13 peek",
	"$00a 13 wpeek",
	"$00b 13 lpeek",
	"$00c 13 varptr",					// (3.0)
	"$00d 13 varuse",					// (3.0)
	"$00e 13 noteinfo",					// (3.0)
	"$00f 13 instr",
	"$010 13 abs",						// (3.0)
	"$011 13 limit",					// (3.0)
	"$012 13 getease",					// (3.4)
	"$013 13 notefind",					// (3.5)
	"$014 13 varsize",					// (3.5)

	//	3.0 string function
	"$100 13 str",
	"$101 13 strmid",
	"$103 13 strf",
	"$104 13 getpath",
	"$105 13 strtrim",					// (3.2)

	//	3.0 math function
	"$180 13 sin",
	"$181 13 cos",
	"$182 13 tan",
	"$183 13 atan",
	"$184 13 sqrt",
	"$185 13 double",
	"$186 13 absf",
	"$187 13 expf",
	"$188 13 logf",
	"$189 13 limitf",
	"$18a 13 powf",						// (3.3)
	"$18b 13 geteasef",					// (3.4)

	//	3.0 external sysvar,function

	"$000 10 mousex",
	"$001 10 mousey",
	"$002 10 mousew",					// (3.0)
	"$003 10 hwnd",						// (3.0)
	"$004 10 hinstance",				// (3.0)
	"$005 10 hdc",						// (3.0)

	"$100 10 ginfo",
	"$101 10 objinfo",
	"$102 10 dirinfo",
	"$103 10 sysinfo",

	"$ffffffff 5 thismod",

	"*"
};


char 	*hsp_prepp[] =
{
	s_rec,

	"$000 0 #addition",
	"$000 0 #cmd",
	"$000 0 #comfunc",
	"$000 0 #const",
	"$000 0 #deffunc",
	"$000 0 #defcfunc",
	"$000 0 #define",
	"$000 0 #else",
	"$000 0 #endif",
	"$000 0 #enum",
	"$000 0 #epack",
	"$000 0 #func",
	"$000 0 #cfunc",
	"$000 0 #global",
	"$000 0 #if",
	"$000 0 #ifdef",
	"$000 0 #ifndef",
	"$000 0 #include",
	"$000 0 #modfunc",
	"$000 0 #modcfunc",					// (3.2)
	"$000 0 #modinit",
	"$000 0 #modterm",
	"$000 0 #module",
	"$000 0 #pack",
	"$000 0 #packopt",
	"$000 0 #regcmd",
	"$000 0 #runtime",
	"$000 0 #undef",
	"$000 0 #usecom",
	"$000 0 #uselib",
	"$000 0 #cmpopt",
	"$000 0 #defint",					// (3.3)
	"$000 0 #defdouble",
	"$000 0 #defnone",
	"$000 0 #bootopt",					// (3.5)

	"*"
};

