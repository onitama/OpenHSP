/*--------------------------------------------------------
	listbox window proc
									  1997/10  onitama
  --------------------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include "poppad.h"
#include "resource.h"

static char szCDir[_MAX_PATH];
static char szDefDir[_MAX_PATH];
static char szFilt[128];
static int enc_mode;			// 暗号化モード(1=normal/2=encode)

typedef struct PFDIR
{
	//		PackFile directory list structure
	//
	int		flag;
	char	fname[128];
	char	pname[_MAX_PATH];

} PFDIR;

#define pfmax 2048
static PFDIR *mem_pf;
static PFDIR *pf;
int pid;

//
//		PackFile manager routines
//

void pf_ini( void )
{
	int a;
	mem_pf=(PFDIR *)malloc(sizeof(PFDIR)*pfmax);
	for(a=0;a<pfmax;a++) {
		pf=&mem_pf[a];
		pf->flag=0;pf->fname[0]=0;pf->pname[0]=0;
	}
	pid=0;
	enc_mode=1;
}

void pf_bye( void )
{
	free(mem_pf);
}

void pf_add( HWND hDlg, char *fname, char *pname )
{
	int a,i;
	char nam[128];
	if (*fname==0) return;
	if (pname[strlen(pname)-1]!='\\') strcat(pname,"\\");

	a=0;
	if (pid) {
		for(i=0;i<pid;i++) {
			pf=&mem_pf[i];
			if (strcmp(pf->fname,fname)==0) a++;
		}
	}
	if (a) {
#ifdef JPNMSG
		MessageBox ( hDlg, "すでに登録されているファイル名です。",
						   "packfile warning", MB_OK | MB_ICONEXCLAMATION);
#else
		MessageBox ( hDlg, "Already in use.",
						   "packfile warning", MB_OK | MB_ICONEXCLAMATION);
#endif
		return;
	}
	pf=&mem_pf[pid];
	pf->flag = enc_mode;
	strcpy(pf->fname,fname);
	strcpy(pf->pname,pname);
	pid++;

	if ( enc_mode==1 ) {
		strcpy( nam, fname );
	}
	else {
		strcpy( nam,"+" );strcat( nam, fname );
	}
	SendDlgItemMessage( hDlg,IDC_LIST3, LB_ADDSTRING,-1,(LPARAM)nam );
}

int pf_find( char *fname )
{
	int a,sel;
	char *p;
	if (pid==0) return -1;
	sel=-1;
	p = fname; if ( *p=='+' ) p++;
	for(a=0;a<pid;a++) {
		pf=&mem_pf[a];
		if (strcmp( pf->fname, p )==0) sel=a;
	}
	return sel;
}

void pf_del( int id )
{
	int a,i;
	i=id;
	if ( pid<=0 ) return;
	while(1) {
		a=i+1;
		mem_pf[i].flag = mem_pf[a].flag;
		strcpy( mem_pf[i].fname, mem_pf[a].fname );
		strcpy( mem_pf[i].pname, mem_pf[a].pname );
		i++;
		if (i>=pid) break;
	}
	pid--;
}


int pf_save( void )
{
	char szNow[_MAX_PATH];
	char szDir[_MAX_PATH];
	char stt[256];
	FILE *fp;
	int a,l;

	strcpy(szNow,szDefDir);
	if (szNow[strlen(szNow)-1]!='\\') strcat(szNow,"\\");

	fp=fopen("packfile","wb");
	if (fp==NULL) return -1;
	wsprintf( stt,";  packfile list for DPM\r\n" );
	fputs( stt,fp );
	wsprintf( stt,";  Current dir is [%s]\r\n",szNow );
	fputs( stt,fp );

	if (pid) {
		for(a=0;a<pid;a++) {
			pf=&mem_pf[a];
			if ( pf->flag ) {
				strcpy(szDir,pf->pname);l=strlen(szNow);
				if (strncmp(szNow,szDir,l)==0) {
					strcpy(szDir,szDir+l);
				}
				if ( pf->flag==1 ) {
					wsprintf( stt,"%s%s\r\n", szDir, pf->fname );
				}
				else {
					wsprintf( stt,"+%s%s\r\n", szDir, pf->fname );
				}
				fputs( stt,fp );
			}
		}
	}
	fclose(fp);
	return 0;
}

int pf_load( HWND hDlg )
{
	char s0[256];
	char s1[256];
	char s2[256];
	char a1;
	FILE *fp;
	int a,b;
	int enc_bak;

	enc_bak = enc_mode;
	fp=fopen("packfile","rb");
	if (fp==NULL) return -1;
	while(1) {
		if ( fgets(s0,256,fp)==NULL ) break;
		a1=*s0;
		if ((a1!=';')&&(a1!=0)) {
			if ( a1=='+' ) {
				enc_mode = 2;
				strcpy( s1,s0+1 );
			}
			else {
				enc_mode = 1;
				strcpy( s1,s0 );
			}
			b=-1;
			for(a=0;a<(int)strlen(s1);a++) {
				a1=s1[a];if (a1<32) s1[a]=0;
				if (a1==0x5c) b=a;
			}
			if (b==-1)  {
				strcpy( s2,szDefDir );
				pf_add( hDlg,s1,s2 );
			}
			else {
				strcpy( s2,s1+b+1 );s1[b]=0;
				pf_add( hDlg,s2,s1 );
			}
		}
	}
	fclose(fp);
	enc_mode = enc_bak;
	return 0;
}



//
//		Dialog proc
//

void set_dirlist( HWND hDlg )
{
	HWND h;
	getcwd( szCDir, _MAX_PATH ) ;
	h=GetDlgItem( hDlg,IDC_CDIR );SetWindowText( h,szCDir );
	SendDlgItemMessage( hDlg,IDC_LIST1,LB_RESETCONTENT, 0, 0L );
	SendDlgItemMessage( hDlg,IDC_LIST2,LB_RESETCONTENT, 0, 0L );
	SendDlgItemMessage( hDlg,IDC_LIST1,LB_DIR,DDL_READWRITE,(LPARAM)szFilt );
	SendDlgItemMessage( hDlg,IDC_LIST2,
		LB_DIR,DDL_DIRECTORY|DDL_DRIVES|DDL_EXCLUSIVE,(LPARAM)"*.*" );
}

BOOL CALLBACK PlistDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND h;
	char szText[128];
	int ctrl_id, i, j;

	switch (message)
	{
	case WM_INITDIALOG:
		h=GetDlgItem( hDlg,IDC_EDIT1 );
		strcpy( szFilt,"*.*" );
		SetWindowText( h,szFilt );
		getcwd( szDefDir, 128 ) ;
		set_dirlist( hDlg );
		pf_ini();
		pf_load( hDlg );
		return TRUE ;

	case WM_CLOSE:
		chdir( szDefDir );
		pf_bye();
		EndDialog (hDlg, 0);
		return TRUE;

	case WM_COMMAND:
		ctrl_id=LOWORD (wParam);
		if (HIWORD (wParam) == LBN_DBLCLK ) {
			i=SendDlgItemMessage( hDlg,ctrl_id, LB_GETCURSEL,0,0L );
			SendDlgItemMessage( hDlg,ctrl_id, LB_GETTEXT,i,(LPARAM)szText );
			if ( ctrl_id==IDC_LIST2 ) {
				szText[strlen(szText)-1]=0;
				if (szText[1]=='-') {
					szText[3]=':';chdir(szText+2);
				}
				else chdir(szText + 1);
				set_dirlist( hDlg );
			}
			if ( ctrl_id==IDC_LIST1 ) {
				pf_add( hDlg,szText,szCDir );
			}
			break;
		}
		if ( ctrl_id==IDC_EDIT1 ) {
			GetDlgItemText( hDlg,IDC_EDIT1,szFilt,128 );
			set_dirlist( hDlg );
			break;
		}
		if ( ctrl_id==IDC_CHECK1 ) {
			enc_mode = 1;
			if ( IsDlgButtonChecked( hDlg,IDC_CHECK1 )==BST_CHECKED ) enc_mode=2;
			break;
		}
		switch (wParam)
		{
		case IDCM_ADD:
			h=GetDlgItem( hDlg,IDC_LIST1 );
			j=SendMessage( h,LB_GETCOUNT,0,0L );
			if (j==0) break;
			for(i=0;i<j;i++) {
				if ( SendMessage( h,LB_GETSEL,i,0L ) ) {
					SendMessage( h, LB_GETTEXT,i,(LPARAM)szText );
					pf_add( hDlg,szText,szCDir );
				}
			}
			break;

		case IDCM_ALLSEL:
			h=GetDlgItem( hDlg,IDC_LIST1 );
			j=SendMessage( h,LB_GETCOUNT,0,0L );
			if (j==0) break;
			for(i=0;i<j;i++) {
				SendMessage( h, LB_GETTEXT,i,(LPARAM)szText );
				pf_add( hDlg,szText,szCDir );
			}
			break;

		case IDCM_DEL:
			i=SendDlgItemMessage( hDlg,IDC_LIST3, LB_GETCURSEL,0,0L );
			if (i>=0) {
				SendDlgItemMessage( hDlg,IDC_LIST3, LB_GETTEXT,i,(LPARAM)szText );
				SendDlgItemMessage( hDlg,IDC_LIST3, LB_DELETESTRING,i,0L );
				i=pf_find( szText );
				pf_del( i );
			}
			break;

		case IDCM_CLR:
			SendDlgItemMessage( hDlg,IDC_LIST3,LB_RESETCONTENT, 0, 0L );
			pf_bye();
			pf_ini();
			break;

		case IDOK:
			chdir( szDefDir );
			pf_save();
			pf_bye();
			EndDialog (hDlg, 0);
			return TRUE;

		case IDCANCEL:
			chdir( szDefDir );
			pf_bye();
			EndDialog (hDlg, 0);
			return TRUE;
		}
		break ;
	}
	return FALSE ;
}

