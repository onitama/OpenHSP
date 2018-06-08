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

static GtkWidget *edit;
static GtkWidget *window;
static GtkWidget *status_bar;
static char filename[1024];
static char hspdir[1024];
static char curdir[1024];
static char complog[65535];
static char barmsg[1024];
static int	text_mod;
static gint context_id;

#define HSED_VER "ver.0.5"
#define TEMP_HSP "__hsptmp.hsp"
#define TEMP_AX "__hsptmp.ax"


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
	sprintf(title,"%s - HSP Script editor",msg);
	gtk_window_set_title (GTK_WINDOW(window), title);
}

static int statusbar_set( gchar *msg )
{
	strcpy( barmsg, msg );
	gtk_statusbar_pop(GTK_STATUSBAR(status_bar), context_id );
	gtk_statusbar_push(GTK_STATUSBAR(status_bar), context_id, barmsg);
	return 0;
}


/* Links can also be activated by clicking or tapping.
 */
static gboolean
event_cb (GtkWidget *text_view,
          GdkEvent  *ev)
{
  text_mod = 1;
  return FALSE;
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
 
//	free(buf);

	char title[1024];
	sprintf(title,"%s - HSP Script editor",filename);
	gtk_window_set_title (GTK_WINDOW(window), title);
}

void file_open_ok(GtkWidget *widget, GtkFileSelection *fsel)
{
	const gchar* fname=gtk_file_selection_get_filename(GTK_FILE_SELECTION(fsel));
	file_open_read(fname);
	//g_free(fname);

	status=1;
	gtk_main_quit();
}

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

void file_save_ok(GtkWidget *widget, GtkFileSelection *fsel)
{
	const gchar* fname=gtk_file_selection_get_filename(GTK_FILE_SELECTION(fsel));
	file_save_write(fname);
	//g_free(fname);

	status=1;
	text_mod = 0;
	gtk_main_quit();
}

static void file_save(GtkWidget *w,int d)
{
	GtkWidget *fsel;
	if (filename[0] && d) {
		file_save_write(NULL);
		text_mod = 0;
		return;
	}
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
}

static void file_open(GtkWidget *w,int d)
{
	GtkWidget *fsel;

	if ( text_mod ) {
		int res;
		res = dialog_openyn( "Warning", "現在の内容が保存されていません。保存しますか?" );
		if ( res == 1 ) {
			file_save( w, 0 );
			if ( status == 2 ) return;		// キャンセルされているか?
		}
	}

	status=0;
	fsel=gtk_file_selection_new ("save");
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
}

static void file_new(GtkWidget *w,int d)
{
	if ( text_mod ) {
		int res;
		res = dialog_openyn( "Warning", "現在の内容が保存されていません。保存しますか?" );
		if ( res == 1 ) {
			file_save( w, 0 );
			if ( status == 2 ) return;		// キャンセルされているか?
		}
	}
	GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(edit),buf);
	g_object_unref(buf);
	//gtk_editable_delete_text(GTK_EDITABLE(edit),0,-1);
	titlebar_set( "Untitled" );
	filename[0]=0;
	text_mod = 0;
}

static gboolean delete_event( GtkWidget *w, GdkEvent *event, gpointer data )
{
	if ( text_mod ) {
		int res;
		res = dialog_openyn( "Warning", "現在の内容が保存されていません。保存しますか?" );
		if ( res == 1 ) {
			file_save( w, 0 );
			if ( status == 2 ) return TRUE;		// キャンセルされているか?
		}
	}
	return FALSE;
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
	gtk_window_set_default_size(GTK_WINDOW(window),320,240);

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

/*
	int pid;
	sprintf(cmd,"%s/hsp3dish",hspdir);
	sigignore( SIGCLD );
	if( (pid=fork()) < 0 ){ 
	    return; 
	} else if( pid == 0 ){ 
	    execlp( cmd, cmd, "__hsptmp.ax", NULL );
	    exit( -1 );
	}
*/

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

	needres = 1;
	option[0] = '-';
	option[1] = 'e';
	option[2] = 0;
	if ( strcmp(runtime,"hsp3cl")==0 ) needres = 0;		// hsp3clはそのまま
	printf("hsed: Runtime [%s].\n",runtime);

#ifdef HSPRASPBIAN
	if ( needres ) {
		option[1] = 'r';			// エラー時に停止するオプション
	}
	sprintf(cmd,"/usr/bin/lxterminal --working-directory=\"%s\" --command=\"%s/hspcmp %s %s --syspath=%s/\""
			, mydir, hspdir, option, TEMP_AX, hspdir );
#else
	sprintf(cmd,"%s/hspcmp %s %s --syspath=%s/",hspdir, option, TEMP_AX, hspdir);
#endif

	result = system(cmd);
	if ( WIFEXITED(result) ) {
		result = WEXITSTATUS(result);
		printf("hsed: Process end %d.\n",result);
	} else {
		printf("hsed: Process error.\n");
	}

	//	error detect
	
/*
	if ( needres ) {
		p = 0;
		sprintf(cmd,"%s/.hspres", hspdir);

		fp=fopen(cmd,"r");
		while(feof(fp)==0){
			p+=fread(complog+p,1,400,fp);
		}
		complog[p]='\0';

		err = strstr( complog, "#Error" );
		if( err != NULL ){
			HSP_view_log();
			return;
		}
	}
*/
}

/////////////////////////  MENU ///////////////////////////////
static GtkItemFactoryEntry menu_items[] = {
	{ "/_File",		NULL,		NULL, 0, "<Branch>" },
	{ "/File/_New",		"<control>N",	(GtkSignalFunc)file_new, 0, NULL },
	{ "/File/_Open",	"<control>O",	(GtkSignalFunc)file_open, 0, NULL },
	{ "/File/_Save",	"<control>S",	(GtkSignalFunc)file_save, 1, NULL },
	{ "/File/Save _As",	NULL,		(GtkSignalFunc)file_save, 0, NULL },
	{ "/File/sep1",		NULL,		NULL, 0, "<Separator>" },
	{ "/File/Quit",		"<control>Q",	(GtkSignalFunc)gtk_main_quit, 0, NULL },
	{ "/_HSP",		NULL,		NULL, 0, "<Branch>" },
	{ "/HSP/run",		"F5",		(GtkSignalFunc)HSP_run, 0, NULL },
	{ "/HSP/make start.ax",	NULL,		(GtkSignalFunc)HSP_run, 1, NULL },
	{ "/HSP/compile log",	"F7",		(GtkSignalFunc)HSP_run, 2, NULL },
	{ "/_Help",		NULL,		NULL, 0, "<Branch>" },
	{ "/_Help/About", NULL,	(GtkSignalFunc)hsed_about,		0, NULL },
};

void init_menu(GtkWidget  *window, GtkWidget **menu){
	GtkItemFactory *item_factory;
	GtkAccelGroup *accel_group;
	gint nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]);

	accel_group = gtk_accel_group_new();

	item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
				accel_group);
	gtk_item_factory_create_items(item_factory, nmenu_items, menu_items, NULL);

	gtk_window_add_accel_group(GTK_WINDOW(window),accel_group);

	if(menu){
		*menu = gtk_item_factory_get_widget(item_factory, "<main>");
	}
}

int main(int argc, char *argv[], char *envp[]){
	GtkWidget *vbox;
	GtkWidget *menu;
	GtkWidget *scroll;

	int i=0,p=-1;
	gtk_set_locale();
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT(window), "destroy",
			GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	gtk_signal_connect (GTK_OBJECT(window), "delete-event",
			G_CALLBACK (delete_event), NULL);
	titlebar_set( "Untitled" );
	gtk_window_set_default_size(GTK_WINDOW(window),640,600);

	vbox = gtk_vbox_new (FALSE, 1);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);

	init_menu(window, &menu);
	gtk_box_pack_start(GTK_BOX(vbox), menu, FALSE, TRUE, 0);
	gtk_widget_show(menu);

	scroll=gtk_scrolled_window_new(NULL,NULL);
	gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
	gtk_widget_show(scroll);

	//edit = gtk_text_new(0,0);
	edit = gtk_text_view_new();

	g_signal_connect (G_OBJECT(window), "key-press-event",
		G_CALLBACK (event_cb), NULL);
	//g_signal_connect (edit, "event", G_CALLBACK (event_cb), NULL);

	gtk_container_add(GTK_CONTAINER(scroll), edit);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(edit),TRUE);
	gtk_widget_show(edit);
	text_mod = 0;
	//

    status_bar = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX (vbox), status_bar, FALSE, TRUE, 0);
    gtk_widget_show (status_bar);
    context_id = gtk_statusbar_get_context_id(
			GTK_STATUSBAR(status_bar), "Ready.");

	getcwd(curdir,512);
	strcpy(hspdir,argv[0]);
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

	if(argc>1){
		file_open_read(argv[1]);
		printf("%s %s\n",curdir,argv[1]);
	}

	gtk_widget_show(window);
	gtk_main();

	return 0;
}
