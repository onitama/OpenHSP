//		スクリプトエディタ(仮)
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *edit;
GtkWidget *window;
char filename[1024];
char hspdir[1024];
char curdir[1024];
char complog[4096];

/////////////////////////  ファイル ///////////////////////////////
static void file_new(GtkWidget *w,int d)
{
	GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(edit),buf);
	g_object_unref(buf);
	//gtk_editable_delete_text(GTK_EDITABLE(edit),0,-1);
	filename[0]=0;
}

int status;
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

static void file_open(GtkWidget *w,int d)
{
	GtkWidget *fsel;
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

	char title[1024];
	sprintf(title,"%s - HSP Script editor",filename);
	gtk_window_set_title (GTK_WINDOW(window), title);
}

void file_save_ok(GtkWidget *widget, GtkFileSelection *fsel)
{
	const gchar* fname=gtk_file_selection_get_filename(GTK_FILE_SELECTION(fsel));
	file_save_write(fname);
	//g_free(fname);

	status=1;
	gtk_main_quit();
}

static void file_save(GtkWidget *w,int d)
{
	GtkWidget *fsel;
	if (filename[0] && d) {
		file_save_write(NULL);
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
static void HSP_complog(GtkWidget *w,gpointer   data )
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
	int p=0;

	FILE *fp=fopen("__hsptmp.hsp","w");
	if(fp == NULL)
		return;
	fwrite(buf,1,size,fp);
	fclose(fp);
	g_free(buf);

	getcwd(mydir,1024);
	chdir(hspdir);

	if(flag==0){
		sprintf(cmd,"./hspcmp -d -i -u %s/__hsptmp",mydir);  // -o obj
	}else{
		sprintf(cmd,"./hspcmp -i -u %s/__hsptmp",mydir);
	}

	printf("hsed: RUN(%s)\n",cmd);

	fp=popen(cmd,"r");
	while(feof(fp)==0){
		p+=fread(complog+p,1,400,fp);
	}
	complog[p]='\0';
	printf(complog);
	if(pclose(fp)){
	//if(system(cmd)){
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


#ifdef HSPRASPBIAN
	sprintf(cmd,"/usr/bin/lxterminal --working-directory=\"%s\" --command=\"%s/hsp3dish __hsptmp.ax\""
			, mydir, hspdir );
#else
	sprintf(cmd,"%s/hsp3dish __hsptmp.ax",hspdir);
#endif

	system(cmd);

	//system("./runhsp hsptmp&");
}

/////////////////////////  MENU ///////////////////////////////
static GtkItemFactoryEntry menu_items[] = {
	{ "/_File",		NULL,		NULL, 0, "<Branch>" },
	{ "/File/_New",		"<control>N",	(GtkSignalFunc)file_new, 0, NULL },
	{ "/File/_Open",	"<control>O",	(GtkSignalFunc)file_open, 0, NULL },
	{ "/File/_Save",	"<control>S",	(GtkSignalFunc)file_save, 0, NULL },
	{ "/File/Save _As",	NULL,		(GtkSignalFunc)file_save, 1, NULL },
	{ "/File/sep1",		NULL,		NULL, 0, "<Separator>" },
	{ "/File/Quit",		"<control>Q",	(GtkSignalFunc)gtk_main_quit, 0, NULL },
	{ "/_HSP",		NULL,		NULL, 0, "<Branch>" },
	{ "/HSP/run",		"F5",		(GtkSignalFunc)HSP_run, 0, NULL },
	{ "/HSP/make start.ax",	NULL,		(GtkSignalFunc)HSP_run, 1, NULL },
	{ "/HSP/compile log",	"F7",		(GtkSignalFunc)HSP_complog, 0, NULL },
	{ "/_Help",		NULL,		NULL, 0, "<LastBranch>" },
	{ "/_Help/About",	NULL,		NULL, 0, NULL },
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
	gtk_window_set_title (GTK_WINDOW(window), "HSP Script editor" );
	gtk_window_set_default_size(GTK_WINDOW(window),640,480);

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
	gtk_container_add(GTK_CONTAINER(scroll), edit);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(edit),TRUE);
	gtk_widget_show(edit);

	//

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
