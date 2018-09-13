//		スクリプトエディタ
//		created by K-K (http://www.binzume.net/)
//		modified by onitama for OpenHSP
//
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>

#include "supio.h"

static GtkWidget *edit;
static GtkWidget *window;
static GtkWidget *wmenu;
static GtkWidget *status_bar;
static char filename[1024];
static char hspdir[1024];
static char curdir[1024];
static char complog[65535];
static char barmsg[1024];
static int	text_mod;
static int	current_line;
static gint context_id;

static int jpflag;				// 日本語環境フラグ
static char langstr[8];

static GdkColor colorBg;
static GdkColor colorText;

#define HSED_VER "ver.0.8"
#define TEMP_HSP "__hsptmp.hsp"
#define TEMP_AX "__hsptmp.ax"
#define TEMP_HSPRES ".hspres"

#define HSED_INI ".hsedconf"

static int event_timer;
static int event_jump;			// 行ジャンプフラグ(-1=なし)
static int event_errflag;		// エラー発生フラグ(-1=no error)

#define STR_HSPERROR "#Error "		// ランタイムエラー識別用タグ
#define STR_HSPERROR2 "in line "	// ランタイムエラー識別用タグ

/*--------------------------------------------------------------------------------*/

static char conf_bgcolor[64];
static char conf_fgcolor[64];
static char conf_fontname[64];

void config_init( void )
{
	strcpy( conf_bgcolor, "#000000" );
	strcpy( conf_fgcolor, "#E0E0E0" );
	strcpy( conf_fontname, "Monospace 12" );
}

void config_load( char *fname )
{
	char *buf = dpm_readalloc( fname );
	if ( buf == NULL ) return;
	//printf( "Use config file : %s\r\n",fname );
	strsp_ini();
	strsp_get( buf, conf_bgcolor, 0, 63 );
	strsp_get( buf, conf_fgcolor, 0, 63 );
	strsp_get( buf, conf_fontname, 0, 63 );
	mem_bye( buf );
}

/*--------------------------------------------------------------------------------*/

//	MESSAGE
//
enum
{
MSG_TITLE = 0,
MSG_NEEDSAVE,
MSG_MAX
};

static	char *message_pool[] = {
	"HSP Script editor",
	"Script modified. Do you want to save?",
	NULL };

static	char *message_pool_ja[] = {
	"HSPスクリプトエディタ",
	"現在の内容が保存されていません。保存しますか?",
	NULL };

char *getmes( int id )
{
	if ((id<0)||(id>=MSG_MAX)) return "";
	if ( jpflag ) {
		return message_pool_ja[id];
	}
	return message_pool[id];
}

/*--------------------------------------------------------------------------------*/

// dialog

static int dialog_open( gchar *title, gchar *msg )
{
  GtkWidget *dialog;
  GtkWidget *label;
  gint response;
  int res = -1;

  //OKボタン付きのダイアログを生成
  dialog = gtk_dialog_new_with_buttons(title, GTK_WINDOW(window),
  			(GtkDialogFlags)(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);

  label = gtk_label_new(msg);
  //ダイアログにlabelを加える
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
  //ダイアログのサイズを設定
  gtk_widget_set_size_request(dialog, 400, 100);
  //ダイアログを表示
  gtk_widget_show_all(dialog);

  //ボタンをクリックした時に戻り値を返す
  response = gtk_dialog_run(GTK_DIALOG(dialog));
  //「OK」ボタンをクリックした時
  if( response == GTK_RESPONSE_ACCEPT ){
	res = 0;
  }
  //ダイアログを閉じる
  gtk_widget_destroy(dialog);
}

static int dialog_openyn( gchar *title, gchar *msg )
{
  GtkWidget *dialog;
  GtkWidget *label;
  gint response;
  int res = -1;

  //YES、NOボタン付きのダイアログを生成
  dialog = gtk_dialog_new_with_buttons(title, GTK_WINDOW(window),
  			(GtkDialogFlags)(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_YES, GTK_RESPONSE_YES, GTK_STOCK_NO, GTK_RESPONSE_NO, NULL);

  label = gtk_label_new(msg);
  //ダイアログにlabelを加える
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
  //ダイアログのサイズを設定
  gtk_widget_set_size_request(dialog, 400, 100);
  //ダイアログを表示
  gtk_widget_show_all(dialog);

  //ボタンをクリックした時に戻り値を返す
  response = gtk_dialog_run(GTK_DIALOG(dialog));
  //「はい(Y)」ボタンをクリックした時
  if( response == GTK_RESPONSE_YES ){
	res = 1;
  }
  //「いいえ(N)」ボタンをクリックした時
  else if( response == GTK_RESPONSE_NO ){
	res = 0;
  }
  //ダイアログを閉じる
  gtk_widget_destroy(dialog);
  return res;
}

static void titlebar_set( char *msg )
{
	static char title[1024];
	sprintf(title,"%s - %s",msg,getmes(MSG_TITLE));
	gtk_window_set_title (GTK_WINDOW(window), title);
}

static int statusbar_set( gchar *msg )
{
	strcpy( barmsg, msg );
	gtk_statusbar_pop(GTK_STATUSBAR(status_bar), context_id );
	gtk_statusbar_push(GTK_STATUSBAR(status_bar), context_id, barmsg);
	return 0;
}

static int statusbar_setline( int line )
{
	sprintf( barmsg,"line:%d", line );
	gtk_statusbar_pop(GTK_STATUSBAR(status_bar), context_id );
	gtk_statusbar_push(GTK_STATUSBAR(status_bar), context_id, barmsg);
	return 0;
}

static void reset_modified( void )
{
	gtk_text_buffer_set_modified( gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit)), FALSE );
}

static gboolean get_modified( void )
{
	return gtk_text_buffer_get_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit)));
}

/* Links can also be activated by clicking or tapping.
 */

void update_statusbar(GtkTextBuffer *buffer, gpointer user_data) {

  int line;
  GtkTextIter iter;

  text_mod = 1;

  gtk_text_buffer_get_iter_at_mark(buffer,
      &iter, gtk_text_buffer_get_insert(buffer));
  current_line = (int)gtk_text_iter_get_line( &iter );
  line = current_line + 1;
  statusbar_setline( line );
}

static void mark_set_callback(GtkTextBuffer *buffer,
    const GtkTextIter *iter, GtkTextMark *mark,
    gpointer data) {

  int line;
  current_line = (int)gtk_text_iter_get_line( iter );
  line = current_line + 1;
  statusbar_setline( line );
}

// file
static int status;

static void hsed_about(GtkWidget *w,int d)
{
	int res;
	res = dialog_open( "About", "HSP Script Editor " HSED_VER "\r\nCopyright 2018(C) onion software" );
}

void file_dlg_cancel(GtkWidget *widget,GtkWidget *dlg){
	status=2;
	gtk_main_quit();
}

void file_dlg_destroy(GtkWidget *widget,GtkWidget *dlg){
	if(status==0)
		gtk_main_quit();
}

void file_open_read(const char *lpPath)
{
	char *buf;
	int size;
 	char cdir[1024];
	FILE *fp;
   GtkTextIter start;

	int i=0,p=-1;
	while(lpPath[i]){
		if(lpPath[i]=='/') p=i;
		i++;
	}
	if(p>=0){
		// ファイルがあるディレクトリをカレントに
		strcpy(cdir,lpPath);
		cdir[p+1]='\0';
		chdir(cdir);
		strcpy(filename,lpPath+p+1);
	}
	i=0;

	fp=fopen(filename,"rb");
	if(fp == NULL)
		return;

	fseek(fp,0,SEEK_END); //!?
	size=ftell(fp);
	fseek(fp,0,SEEK_SET);

	buf=(char*)malloc(size+1);
	fread(buf,1,size,fp);
	buf[size]=0;

	fclose(fp);

	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	gtk_text_buffer_set_text(tbuf,buf,strlen(buf));
	current_line = 0;
 
   gtk_text_buffer_get_start_iter( tbuf, &start );
   gtk_text_buffer_place_cursor( tbuf, &start );
   gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(edit), &start, 0, FALSE, 0, 0);

//	free(buf);

	titlebar_set( filename );
	reset_modified();
}

/*
void file_open_ok(GtkWidget *widget, GtkFileSelection *fsel)
{
	const gchar* fname=gtk_file_selection_get_filename(GTK_FILE_SELECTION(fsel));
	file_open_read(fname);
	//g_free(fname);

	status=1;
	gtk_main_quit();
}
*/

void file_save_write(const char *lpPath)
{
	char *buf;
	int size;
	FILE *fp;
 	char cdir[1024];
	if(lpPath){
		strcpy(filename,lpPath);
	}else{
		lpPath=filename;
	}
	int i=0,p=0;
	while(lpPath[i]){
		if(lpPath[i]=='/') p=i;
		i++;
	}
	strcpy(cdir,lpPath);
	cdir[p+1]='\0';
	chdir(cdir);
	if(p)strcpy(filename,lpPath+p+1);


	GtkTextIter start;
	GtkTextIter end;
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	gtk_text_buffer_get_start_iter(tbuf,&start);
	gtk_text_buffer_get_end_iter(tbuf,&end);
	buf = gtk_text_buffer_get_text(tbuf,&start,&end,TRUE);

	//buf=gtk_editable_get_chars(GTK_EDITABLE(edit),0,-1);

	size=strlen(buf);
	fp=fopen(filename,"wb");
	if(fp == NULL)
		return;

	fwrite(buf,1,size,fp);
	fclose(fp);
	g_free(buf);

	titlebar_set( filename );
}

/*
void file_save_ok(GtkWidget *widget, GtkFileSelection *fsel)
{
	const gchar* fname=gtk_file_selection_get_filename(GTK_FILE_SELECTION(fsel));
	strcpy( filename, fname );

	file_save_write(filename);
	//g_free(fname);

	status=1;
	reset_modified();
	text_mod = 0;
	gtk_main_quit();
}
*/

static void file_save(GtkWidget *w,int d)
{
	int res;
	char mydir[1024];
	GtkWidget *fsel;

	if (filename[0] && d) {
		file_save_write(NULL);
		reset_modified();
		text_mod = 0;
		return;
	}

	status=0;
	getcwd(mydir,1024);

	fsel = gtk_file_chooser_dialog_new( "save", GTK_WINDOW (window),
	    GTK_FILE_CHOOSER_ACTION_SAVE,
	    GTK_STOCK_CANCEL,
	    GTK_RESPONSE_CANCEL ,
	    GTK_STOCK_SAVE,
	    GTK_RESPONSE_ACCEPT ,
	    NULL);	

	GtkFileChooser *chooser = GTK_FILE_CHOOSER (fsel);
	gtk_file_chooser_set_current_folder (chooser,mydir);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name (filter , "HSP Script");
	gtk_file_filter_add_pattern (filter , "*.hsp");
	gtk_file_chooser_add_filter ( chooser, filter);
	filter = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter , "AllFiles");
	gtk_file_filter_add_pattern (filter , "*");
	gtk_file_chooser_add_filter ( chooser, filter);

	res = gtk_dialog_run (GTK_DIALOG (fsel));
	if (res == GTK_RESPONSE_ACCEPT) {
		char *fname;
		fname = gtk_file_chooser_get_filename (chooser);
		strcpy( filename, fname );
		if ( strstr(filename,".hsp") == NULL ) {
			strcat( filename,".hsp" );
		}
		file_save_write(filename);
		status=1;
		reset_modified();
		text_mod = 0;
		g_free (fname);
	}

	gtk_widget_destroy(fsel);
/*
	status=0;
	fsel=gtk_file_selection_new ("save");
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fsel)->ok_button),
		"clicked", (GtkSignalFunc) file_save_ok, fsel);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION (fsel)->cancel_button),
		"clicked", (GtkSignalFunc) file_dlg_cancel, fsel);
	gtk_signal_connect(GTK_OBJECT(fsel), "destroy",
		(GtkSignalFunc) file_dlg_destroy, fsel);
	gtk_widget_show(fsel);
	gtk_grab_add(GTK_WIDGET(fsel));
	gtk_main();
	gtk_grab_remove(GTK_WIDGET(fsel));
	gtk_widget_destroy(GTK_WIDGET(fsel));
*/
}

static void file_open(GtkWidget *w,int d)
{
	int res;
	char mydir[1024];
	GtkWidget *fsel;

	if ( get_modified() ) {
		int res;
		res = dialog_openyn( "Warning", getmes(MSG_NEEDSAVE) );
		if ( res == 1 ) {
			file_save( w, 0 );
			if ( status == 2 ) return;		// キャンセルされているか?
		}
	}

	status=0;
	getcwd(mydir,1024);

	fsel = gtk_file_chooser_dialog_new( "load", GTK_WINDOW (window),
	    GTK_FILE_CHOOSER_ACTION_OPEN ,
	    GTK_STOCK_CANCEL ,
	    GTK_RESPONSE_CANCEL ,
	    GTK_STOCK_OPEN ,
	    GTK_RESPONSE_ACCEPT ,
	    NULL);	

	GtkFileChooser *chooser = GTK_FILE_CHOOSER (fsel);
	gtk_file_chooser_set_current_folder (chooser,mydir);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name (filter , "HSP Script");
	gtk_file_filter_add_pattern (filter , "*.hsp");
	gtk_file_chooser_add_filter ( chooser, filter);
	filter = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter , "AllFiles");
	gtk_file_filter_add_pattern (filter , "*");
	gtk_file_chooser_add_filter ( chooser, filter);

	res = gtk_dialog_run (GTK_DIALOG (fsel));
	if (res == GTK_RESPONSE_ACCEPT) {
		char *fname;
		fname = gtk_file_chooser_get_filename (chooser);
		file_open_read(fname);
		g_free (fname);
	}

	gtk_widget_destroy(fsel);
/*
	fsel=gtk_file_selection_new ("load");

	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fsel)->ok_button),
		"clicked", (GtkSignalFunc) file_open_ok, fsel);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION (fsel)->cancel_button),
		"clicked", (GtkSignalFunc) file_dlg_cancel, fsel);
	gtk_signal_connect(GTK_OBJECT(fsel), "destroy",
		(GtkSignalFunc) file_dlg_destroy, fsel);
	gtk_widget_show(fsel);
	gtk_grab_add(GTK_WIDGET(fsel));
	gtk_main();
	gtk_grab_remove(GTK_WIDGET(fsel));
	gtk_widget_destroy(GTK_WIDGET(fsel));
*/
}

static void file_new(GtkWidget *w,int d)
{
	if ( get_modified() ) {
		int res;
		res = dialog_openyn( "Warning", getmes(MSG_NEEDSAVE) );
		if ( res == 1 ) {
			file_save( w, 0 );
			if ( status == 2 ) return;		// キャンセルされているか?
		}
	}
	GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(edit),buf);
	current_line = 0;
	g_object_unref(buf);
	//gtk_editable_delete_text(GTK_EDITABLE(edit),0,-1);
	titlebar_set( "Untitled" );
	filename[0]=0;
	reset_modified();
	text_mod = 0;
}

static gboolean delete_event( GtkWidget *w, GdkEvent *event, gpointer data )
{
	if ( get_modified() ) {
		int res;
		res = dialog_openyn( "Warning", getmes(MSG_NEEDSAVE) );
		if ( res == 1 ) {
			file_save( w, 0 );
			if ( status == 2 ) return TRUE;		// キャンセルされているか?
		}
	}
	return FALSE;
}

static void edit_cut(GtkWidget *w,int d)
{
	GtkClipboard *clip = gtk_clipboard_get( GDK_SELECTION_CLIPBOARD );
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	gtk_text_buffer_cut_clipboard( tbuf,clip,TRUE );
}

static void edit_copy(GtkWidget *w,int d)
{
	GtkClipboard *clip = gtk_clipboard_get( GDK_SELECTION_CLIPBOARD );
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	gtk_text_buffer_copy_clipboard( tbuf,clip );
}

static void edit_paste(GtkWidget *w,int d)
{
	GtkClipboard *clip = gtk_clipboard_get( GDK_SELECTION_CLIPBOARD );
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	gtk_text_buffer_paste_clipboard( tbuf,clip, NULL, TRUE );
}

static void edit_delete(GtkWidget *w,int d)
{
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	gtk_text_buffer_delete_selection( tbuf, TRUE, TRUE );
}

static void edit_undo(GtkWidget *w,int d)
{
	//GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	//gtk_source_buffer_redo(tbuf);
}

static void cursor_move(GtkWidget *w,int d)
{
    GtkTextIter start;
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));

	if ( d == 0 ) {
	    gtk_text_buffer_get_start_iter( tbuf, &start );
	    gtk_text_buffer_place_cursor( tbuf, &start );
	    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(edit), &start, 0, FALSE, 0, 0);
	    return;
	}
	if ( d == 1 ) {
	    gtk_text_buffer_get_end_iter( tbuf, &start );
	    gtk_text_buffer_place_cursor( tbuf, &start );
	    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(edit), &start, 0, FALSE, 0, 0);
	    return;
	}

}

static void cursor_move_line(GtkWidget *w,int d)
{
   GtkTextIter start;
   GtkTextIter end;
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));

	gtk_text_buffer_get_iter_at_line( tbuf, &start, d );
	gtk_text_buffer_place_cursor( tbuf, &start );
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(edit), &start, 0, FALSE, 0, 0);
	return;
}

/////////////////////////  HSP ///////////////////////////////
void HSP_view_log(){
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *scroll;
	GtkWidget *log;
	//int i=0;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT(window), "destroy",
			GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	gtk_window_set_title (GTK_WINDOW(window), "HSP Compile log");
	gtk_window_set_default_size(GTK_WINDOW(window),640,240);

	vbox = gtk_vbox_new (FALSE, 1);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);

	scroll=gtk_scrolled_window_new(NULL,NULL);
	gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
	gtk_widget_show(scroll);

	//log = gtk_text_new(0,0);
	log = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(scroll), log);
	//gtk_text_set_editable(GTK_TEXT(log),TRUE);

	gdk_color_parse("#C0C0C0", &colorBg);
	gdk_color_parse("#222222", &colorText);

    gtk_widget_modify_base (log, GTK_STATE_NORMAL, &colorBg);
    gtk_widget_modify_text (log, GTK_STATE_NORMAL, &colorText);

        GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log));
        gtk_text_buffer_set_text(tbuf,complog,strlen(complog));

	gtk_widget_show(log);
	//gtk_editable_insert_text(GTK_EDITABLE(log),complog,strlen(complog),&i);

	gtk_widget_show(window);

	//gtk_grab_add(GTK_WIDGET(window));
	gtk_main();
}
static void HSP_complog(GtkWidget *w,gpointer data )
{
	HSP_view_log();

}

static gint update_edit( gpointer data )
{
	gtk_text_view_set_editable(GTK_TEXT_VIEW(edit),TRUE);
	gtk_widget_set_sensitive( wmenu, TRUE );
	g_source_remove( event_timer );

	if ( event_errflag >= 0 ) {
		char msg[1024];
		printf("hsed: Jump line [%d].\n", current_line+1 );
		if ( event_jump >= 0 ) cursor_move_line( edit, current_line );

		if ( jpflag ) {
			sprintf( msg,"#Error %d in line %d\n-->%s", event_errflag, current_line+1, gethsperror(event_errflag) );
		} else {
			sprintf( msg,"#Error %d in line %d", event_errflag, current_line+1 );
		}

		dialog_open( "Error", msg );
	}
	return 0;
}

static void HSP_run(GtkWidget *w,int flag)
{
	GtkTextIter start;
	GtkTextIter end;
	GtkTextBuffer *tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));
	gtk_text_buffer_get_start_iter(tbuf,&start);
	gtk_text_buffer_get_end_iter(tbuf,&end);
	char *buf = gtk_text_buffer_get_text(tbuf,&start,&end,TRUE);

	//char *buf=gtk_editable_get_chars(GTK_EDITABLE(edit),0,-1);
	int size=strlen(buf);
	char cmd[1024];
	char mydir[1024];
	char runtime[128];
	char option[16];
	int result;
	int needres=0;			// .hspresによるエラー解析を行うか?
	char *err;
	char a1;
	int p = 0;
	int sch = 0;

	FILE *fp=fopen( TEMP_HSP, "w" );
	if(fp == NULL)
		return;
	fwrite(buf,1,size,fp);
	fclose(fp);
	g_free(buf);

	getcwd(mydir,1024);
	chdir(hspdir);

	switch( flag ) {
	case 1:
		sprintf(cmd,"./hspcmp -i -u %s/start.ax",mydir);
		break;
	default:
		sprintf(cmd,"./hspcmp -d -i -u %s/%s",mydir,TEMP_HSP);  // -o obj
		break;
	}

	printf("hsed: RUN(%s)\n",cmd);

	fp=popen(cmd,"r");
	while(feof(fp)==0){
		p+=fread(complog+p,1,400,fp);
	}
	complog[p]='\0';
	printf(complog);
	if(pclose(fp)){
		printf("hsed: Compile error.\n");
		HSP_view_log();
		chdir(mydir);
		return;
	}

	chdir(mydir);

	if(flag==1){
		return;
	}

	if(flag==2){
		HSP_view_log();
		return;
	}

	// ランタイムを取得する
	p = 0;
	sprintf(cmd,"%s/hspcmp -e0 %s", hspdir,TEMP_AX);
	fp=popen(cmd,"r");
	while(feof(fp)==0){
		p+=fread(complog+p,1,400,fp);
	}
	complog[p]='\0';
	printf(complog);
	pclose(fp);

	sch = 0;
	*runtime=0;
	err = strstr( complog, "Runtime[" );
	if( err != NULL ){
		err += 8;
		while(1) {
			a1 = *err++;
			if ( a1 == 0 ) break;
			if ( a1 == ']' ) break;
			runtime[sch++] = a1;
		}
		runtime[sch++] = 0;
	}
	printf("hsed: Runtime [%s].\n",runtime);

	needres = 0;
	event_errflag = -1;
	event_jump = -1;
	option[0] = '-';
	option[1] = 'e';
	option[2] = 0;
	if ( strcmp(runtime,"hsp3cl")==0 ) needres = 1;		// hsp3clはエラー情報をファイルから取得する


	if ( needres ) {
		sprintf(cmd,"%s/%s -r %s",hspdir, runtime, TEMP_AX);
	} else {
//		sprintf(cmd,"/usr/bin/lxterminal --working-directory=\"%s\" --command=\"%s/hspcmp %s %s --syspath=%s/\""
//			, mydir, hspdir, option, TEMP_AX, hspdir );
		sprintf(cmd,"%s/hspcmp %s %s --syspath=%s/",hspdir, option, TEMP_AX, hspdir);
	}

	gtk_text_view_set_editable(GTK_TEXT_VIEW(edit),FALSE);
	gtk_widget_set_sensitive( wmenu, FALSE );

	//	HSPランタイム実行
	p = 0;

	if ( needres == 0 ) {
		//	プロセスから結果を取得
		fp=popen(cmd,"r");
		while(feof(fp)==0){
			p+=fread(complog+p,1,400,fp);
		}
		if(!pclose(fp)){
			p = 0;
		}
		complog[p]='\0';
		printf(complog);

	} else {
		//	結果をファイルから取得
		system(cmd);
		p = dpm_exist( TEMP_HSPRES );
		if ( p > 0 ) {
			dpm_read( TEMP_HSPRES, complog, p, 0 );
		} else {
			p = 0;
		}
		complog[p]='\0';
	}

	if(p){
		char *errinfo;
		char str_errid[64];

		errinfo = strstr2( complog, STR_HSPERROR );
		if ( errinfo ) {
			errinfo += strlen(STR_HSPERROR);
			strsp_ini();
			strsp_get( errinfo, str_errid, 32, 63 );
			event_errflag = atoi( str_errid );

			errinfo = strstr2( errinfo, STR_HSPERROR2 );
			if ( errinfo ) {
				errinfo += strlen(STR_HSPERROR2);
				strsp_ini();
				strsp_get( errinfo, str_errid, 32, 63 );
				event_jump = atoi( str_errid );
				current_line = event_jump - 1;
			}

			errinfo = strstr2( errinfo, TEMP_HSP );
			if ( errinfo == NULL ) {
				event_jump = -1;			// 異なるソースコードでエラーが出ている
			}

			if ( jpflag ) {
				printf( "hsed: %s (エラー%d) %d行.\n", gethsperror(event_errflag), event_errflag, current_line+1 );
			} else {
				printf( "hsed: Runtime error %d line %d.\n", event_errflag, current_line+1 );
			}

		}
	}

/*
	result = system(cmd);
	if ( WIFEXITED(result) ) {
		result = WEXITSTATUS(result);
		printf("hsed: Process end %d.\n",result);
	} else {
		printf("hsed: Process error.\n");
	}
*/
	// 復帰用タイマーをセットする
	event_timer=g_timeout_add( 500,(GSourceFunc)update_edit, NULL );
}

//	MENU
//
static GtkItemFactoryEntry menu_items[] = {
	{ "/_File",		NULL,		NULL, 0, "<Branch>" },
	{ "/File/_New",		"<control>N",	(GtkSignalFunc)file_new, 0, NULL },
	{ "/File/_Open",	"<control>O",	(GtkSignalFunc)file_open, 0, NULL },
	{ "/File/_Save",	"<control>S",	(GtkSignalFunc)file_save, 1, NULL },
	{ "/File/Save _As",	NULL,		(GtkSignalFunc)file_save, 0, NULL },
	{ "/File/sep1",		NULL,		NULL, 0, "<Separator>" },
	{ "/File/_Quit",		"<control>Q",	(GtkSignalFunc)gtk_main_quit, 0, NULL },
	{ "/_Edit",		NULL,		NULL, 0, "<Branch>" },
	{ "/Edit/_Cut",		"<control>X",	(GtkSignalFunc)edit_cut, 0, NULL },
	{ "/Edit/_Copy",	"<control>C",	(GtkSignalFunc)edit_copy, 0, NULL },
	{ "/Edit/_Paste",	"<control>V",	(GtkSignalFunc)edit_paste, 0, NULL },
	{ "/Edit/_Delete",	"Delete",	(GtkSignalFunc)edit_delete, 0, NULL },
	{ "/_Cursor",		NULL,		NULL, 0, "<Branch>" },
	{ "/Cursor/_Top",		"<control>T",	(GtkSignalFunc)cursor_move, 0, NULL },
	{ "/Cursor/_Bottom",	"<control>B",	(GtkSignalFunc)cursor_move, 1, NULL },
	{ "/_HSP",		NULL,		NULL, 0, "<Branch>" },
	{ "/HSP/_Run",		"F5",		(GtkSignalFunc)HSP_run, 0, NULL },
	{ "/HSP/_Make start.ax",	NULL,		(GtkSignalFunc)HSP_run, 1, NULL },
	{ "/HSP/_Compile log",	"F7",		(GtkSignalFunc)HSP_run, 2, NULL },
	{ "/_Help",		NULL,		NULL, 0, "<Branch>" },
	{ "/_Help/About", NULL,	(GtkSignalFunc)hsed_about,		0, NULL },
};

static GtkItemFactoryEntry menu_items_ja[] = {
	{ "/_ファイル",		NULL,		NULL, 0, "<Branch>" },
	{ "/ファイル/新規作成",		"<control>N",	(GtkSignalFunc)file_new, 0, NULL },
	{ "/ファイル/開く...",	"<control>O",	(GtkSignalFunc)file_open, 0, NULL },
	{ "/ファイル/上書き保存",	"<control>S",	(GtkSignalFunc)file_save, 1, NULL },
	{ "/ファイル/名前を付けて保存...",	NULL,		(GtkSignalFunc)file_save, 0, NULL },
	{ "/ファイル/sep1",		NULL,		NULL, 0, "<Separator>" },
	{ "/ファイル/終了",		"<control>Q",	(GtkSignalFunc)gtk_main_quit, 0, NULL },
	{ "/編集",		NULL,		NULL, 0, "<Branch>" },
	{ "/編集/_切り取り",		"<control>X",	(GtkSignalFunc)edit_cut, 0, NULL },
	{ "/編集/_コピー",	"<control>C",	(GtkSignalFunc)edit_copy, 0, NULL },
	{ "/編集/_貼り付け",	"<control>V",	(GtkSignalFunc)edit_paste, 0, NULL },
	{ "/編集/_削除",	"Delete",	(GtkSignalFunc)edit_delete, 0, NULL },
	{ "/カーソル",		NULL,		NULL, 0, "<Branch>" },
	{ "/カーソル/_先頭行に移動",		"<control>T",	(GtkSignalFunc)cursor_move, 0, NULL },
	{ "/カーソル/_最終行に移動",	"<control>B",	(GtkSignalFunc)cursor_move, 1, NULL },
	{ "/_HSP",		NULL,		NULL, 0, "<Branch>" },
	{ "/HSP/コンパイル+実行",		"F5",		(GtkSignalFunc)HSP_run, 0, NULL },
	{ "/HSP/START.AXファイル作成",	NULL,		(GtkSignalFunc)HSP_run, 1, NULL },
	{ "/HSP/コンパイルのみ",	"F7",		(GtkSignalFunc)HSP_run, 2, NULL },
	{ "/_ヘルプ",		NULL,		NULL, 0, "<Branch>" },
	{ "/_ヘルプ/バージョン情報...", NULL,	(GtkSignalFunc)hsed_about,		0, NULL },
};


void init_menu(GtkWidget  *window, GtkWidget **menu){
	GtkItemFactory *item_factory;
	GtkAccelGroup *accel_group;
	gint nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]);

	accel_group = gtk_accel_group_new();

	item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
				accel_group);

	if ( jpflag ) {
		gtk_item_factory_create_items(item_factory, nmenu_items, menu_items_ja, NULL);
	} else {
		gtk_item_factory_create_items(item_factory, nmenu_items, menu_items, NULL);
	}

	gtk_window_add_accel_group(GTK_WINDOW(window),accel_group);

	if(menu){
		*menu = gtk_item_factory_get_widget(item_factory, "<main>");
	}
}

int main(int argc, char *argv[], char *envp[]){
	GtkWidget *vbox;
	GtkWidget *scroll;
	GtkTextBuffer *buffer;
	char config_file[1024];

	// Init Locale
	int i=0,p=-1;
	gtk_set_locale();
	gtk_init(&argc, &argv);

	PangoLanguage *lang = gtk_get_default_language();
	strncpy( langstr, pango_language_to_string(lang), 8 );
	jpflag = 0;
	if ( strncmp(langstr,"ja",2) == 0 ) jpflag = 1;
	//printf( "lang[%s]",pango_language_to_string(lang) );

	// Init System Directory
	getcwd(curdir,512);

	readlink( "/proc/self/exe", hspdir, 1023 );
	//strcpy(hspdir,argv[0]);
	//printf( "hspdir[%s]\r\n",hspdir );
	while(hspdir[i]){
		if(hspdir[i]=='/' || hspdir[i]=='\\') p=i;
		i++;
	}
	hspdir[p]=0;
	if(p>=0){
		chdir(hspdir);
		getcwd(hspdir,512);
		chdir(curdir);
	}else{
		getcwd(hspdir,512);
	}

	// Init Config
	config_init();
	sprintf( config_file,"%s/%s",hspdir,HSED_INI );
	config_load( config_file );

	// Init Window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT(window), "destroy",
			GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	gtk_signal_connect (GTK_OBJECT(window), "delete-event",
			G_CALLBACK (delete_event), NULL);
	titlebar_set( "Untitled" );
	gtk_window_set_default_size(GTK_WINDOW(window),680,760);

	vbox = gtk_vbox_new (FALSE, 1);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);

	init_menu(window, &wmenu);
	gtk_box_pack_start(GTK_BOX(vbox), wmenu, FALSE, TRUE, 0);
	gtk_widget_show(wmenu);

	scroll=gtk_scrolled_window_new(NULL,NULL);
	gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
	gtk_widget_show(scroll);

	//edit = gtk_text_new(0,0);
	edit = gtk_text_view_new();

	gdk_color_parse(conf_bgcolor, &colorBg);
	gdk_color_parse(conf_fgcolor, &colorText);

    gtk_widget_modify_base (edit, GTK_STATE_NORMAL, &colorBg);
    gtk_widget_modify_text (edit, GTK_STATE_NORMAL, &colorText);

	 PangoFontDescription *fontDesc = pango_font_description_from_string(conf_fontname);

	gtk_widget_modify_font(edit, fontDesc);

	//g_signal_connect (G_OBJECT(window), "key-press-event",
	//	G_CALLBACK (event_cb), NULL);
	//g_signal_connect (edit, "event", G_CALLBACK (event_cb), NULL);

	gtk_container_add(GTK_CONTAINER(scroll), edit);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(edit),TRUE);

	gtk_widget_show(edit);
	text_mod = 0;
	current_line = 0;
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit));

    status_bar = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX (vbox), status_bar, FALSE, TRUE, 0);
    gtk_widget_show (status_bar);
    context_id = gtk_statusbar_get_context_id(
			GTK_STATUSBAR(status_bar), "Ready.");

	g_signal_connect(buffer, "changed",
		G_CALLBACK(update_statusbar), GINT_TO_POINTER(0) );
	g_signal_connect_object(buffer, "mark-set",
		G_CALLBACK(mark_set_callback), GINT_TO_POINTER(0),G_CONNECT_SWAPPED );

	if(argc>1){
		file_open_read(argv[1]);
		printf("%s %s\n",curdir,argv[1]);
	}

	gtk_widget_show(window);
	gtk_main();

	return 0;
}
