#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "exiv.h"
#include "structs.h"
#include "classe.h"
#include "utils.h"

cl_base_t base;



int get_file_type(char* fn)
{
  //we search the extension
  char* ext = (char*) malloc (sizeof (*fn) * (4));
  for (int j = 0; j < 3; j++) ext[j] = tolower(fn[j+strlen(fn)-3]);
  ext[3] = '\0';
  printf("extension %s -%s-\n",fn,ext);
  if (strcmp(ext,"jpg")==0) return 1;
  if (strcmp(ext,"xmp")==0) return 3;
  if (strstr(";bay;bmq;cr2;crw;cs1;dc2;dcr;dng;erf;fff;hdr;k25;kdc;mdc;mos;mrw;nef;orf;pef;pxn;raf;raw;rdc;sr2;srf;x3f;arw;",ext)) return 2;
  //if (strstr(";avi;mov;mpg;flv;",ext)) return 4;
  return 0;
}

int search_raw(char* fbase, char* fraw)
{
  char txt[1024] = "";
  const char* exts[54] = {"bay","bmq","cr2","crw","cs1","dc2","dcr","dng","erf","fff","hdr","k25","kdc","mdc","mos","mrw","nef","orf","pef","pxn","raf","raw","rdc","sr2","srf","x3f","arw",
                    "BAY","BMQ","CR2","CRW","CS1","DC2","DCR","DNG","ERF","FFF","HDR","K25","KDC","MDC","MOS","MRW","NEF","ORF","PEF","PXN","RAF","RAW","RDC","SR2","SRF","X3F","ARW"};
  const int exts_nb = 54;
  
  for (int i=0; i<exts_nb; i++)
  {
    snprintf(txt,1024,"%s%s",fbase,exts[i]);
    if (fileexists(txt))
    {
      strncpy(fraw,txt,1024);
      return 1;
    }
  }
  
  return 0;
}

int group_files(int argc,char **argv)
{
  int *lv = (int*) malloc(sizeof(int)*argc);
  memset(lv,0,sizeof(int)*argc);
  
  for (int i=1; i<argc; i++)
  {
    if (!argv[i]) continue;
    if (lv[i]) continue;
    //we initialise a new file_group
    cl_file_t* f = (cl_file_t*) malloc(sizeof(cl_file_t));
    memset(f,0,sizeof(cl_file_t));
    //we store the filename
    int ftype = get_file_type(argv[i]);
    if (ftype==1) strncpy(f->jpg,argv[i],1024);
    else if (ftype==2) strncpy(f->raw,argv[i],1024);
    else if (ftype==3) strncpy(f->xmp,argv[i],1024);
    else
    {
      //we have an unknown file type...
      free(f);
      continue;
    }
    //we get the base name of the file
    char* fbase = NULL;
    if (ftype == 1 || ftype == 2)
    {
      fbase = (char*) malloc (sizeof (*argv[i]) * (strlen(argv[i])-2));
      int j;
      for (j = 0; j < strlen(argv[i])-3; j++) fbase[j] = argv[i][j];
      fbase[j] = '\0';
    }
    else if (ftype == 3)
    {
      fbase = (char*) malloc (sizeof (*argv[i]) * (strlen(argv[i])-6));
      int j;
      for (j = 0; j < strlen(argv[i])-7; j++) fbase[j] = argv[i][j];
      fbase[j] = '\0';
    }
         
    //we search for other files in same group in the command line
    for (int j=i+1; j<argc; j++)
    {
      if (!argv[j]) continue;
      //have the 2 entry the same base ?
      if (strstr(argv[j],fbase) == argv[j])
      {
        int ftype2 = get_file_type(argv[j]);
        if (ftype2==1) strncpy(f->jpg,argv[j],1024);
        else if (ftype2==2) strncpy(f->raw,argv[j],1024);
        else if (ftype2==3) strncpy(f->xmp,argv[j],1024);
        //note sure we have to free argv[j] as we don't have allocated it...
        lv[j] = 1;
      }
    }
    
    //we do the same in filesystem
    char txt[1024] = "";
    if (strcmp(f->jpg,"")==0)
    {
      snprintf(txt,1024,"%sjpg",fbase);
      if (fileexists(txt)) strncpy(f->jpg,txt,1024);
      else
      {
        snprintf(txt,1024,"%sJPG",fbase);
        if (fileexists(txt)) strncpy(f->jpg,txt,1024);
      }
    }
    if (strcmp(f->raw,"")==0) search_raw(fbase,f->raw);
    if (strcmp(f->xmp,"")==0 && strcmp(f->raw,"")!=0)
    {
      snprintf(txt,1024,"%s.xmp",f->raw);
      if (fileexists(txt)) strncpy(f->xmp,txt,1024);
      else
      {
        snprintf(txt,1024,"%s.XMP",f->raw);
        if (fileexists(txt)) strncpy(f->xmp,txt,1024);
      }
    }
    if (strcmp(f->xmp,"")==0 && strcmp(f->jpg,"")!=0)
    {
      snprintf(txt,1024,"%s.xmp",f->jpg);
      if (fileexists(txt)) strncpy(f->xmp,txt,1024);
      else
      {
        snprintf(txt,1024,"%s.XMP",f->jpg);
        if (fileexists(txt)) strncpy(f->xmp,txt,1024);
      }
    }
    
    //and we store our new file
    base.files[base.files_nb] = f;
    base.files_nb += 1;
  }
  
  return base.files_nb;
}

int read_xmp()
{
  for (int i=0; i<base.files_nb; i++)
  {
    if (!base.files[i]->xmp) continue;
    if (!fileexists(base.files[i]->xmp)) continue;
    xmp_read_infos(base.files[i]->xmp,base.files[i]->i1,base.files[i]->i2,base.files[i]->i3);
  }
  return 1;
}

int read_preselect()
{
  
  return 1;
}

int get_all_infos(int val, char* txt)
{
  int resnb = 0;
  char res[20000] = "";
  for (int i=0; i<base.files_nb; i++)
  {
    char* tx;
    if (val == 1) tx = base.files[i]->i1;
    else if (val == 2) tx = base.files[i]->i2;
    else if (val == 3) tx = base.files[i]->i3;
    
    if (!strstr(res,tx))
    {
      resnb += 1;
      if (resnb > 1)
      {
        strncat(res,"\n",20000);
        strncat(res,tx,20000);
      }
      else snprintf(res,20000,"%s",tx);
    }
  }
  strncpy(txt,res,20000);
  return resnb;
}

static void ok_callback(GtkButton *button, char* nu)
{
  //we retrieve all text values
  const char* i1 = gtk_entry_get_text(GTK_ENTRY(base.TBinfo1));
  const char* i2 = gtk_entry_get_text(GTK_ENTRY(base.TBinfo2));
  const char* i3 = gtk_entry_get_text(GTK_ENTRY(base.TBinfo3));
  if (strcmp(i1,"###-multiple-###")==0) i1 = NULL;
  if (strcmp(i2,"###-multiple-###")==0) i2 = NULL;
  if (strcmp(i3,"###-multiple-###")==0) i3 = NULL;
  
  //we set the values for all the images
  for (int i=0; i<base.files_nb; i++)
  {
    //set the values
    int change = 0;
    if (i1 && strcmp(base.files[i]->i1,i1) != 0)
    {
      change = 1;
      strncpy(base.files[i]->i1,i1,1024);
    }
    if (i2 && strcmp(base.files[i]->i2,i2) != 0)
    {
      change = 1;
      strncpy(base.files[i]->i2,i2,1024);
    }
    if (i3 && strcmp(base.files[i]->i3,i3) != 0)
    {
      change = 1;
      strncpy(base.files[i]->i3,i3,1024);
    }
    
    //save the xmp if needed
    if (change)
    {
      //if xmp file doesn't exist, we have to set a valid name
      if (!fileexists(base.files[i]->xmp))
      {
        if (fileexists(base.files[i]->raw))
          snprintf(base.files[i]->xmp,1024,"%s.xmp",base.files[i]->raw);
        else if (fileexists(base.files[i]->jpg))
          snprintf(base.files[i]->xmp,1024,"%s.xmp",base.files[i]->jpg);
      }
      xmp_save_infos(base.files[i]->xmp, base.files[i]->i1, base.files[i]->i2, base.files[i]->i3);
    }
  }
  gtk_main_quit();
}

static void classe_callback(GtkButton *button, char* nu)
{
  classe_lance();
}

int main(int argc,char **argv)
{
    GtkWidget* Fenetre = NULL;
    GtkWidget* Label = NULL;
    GtkWidget* Boxinfo = NULL;
    GtkWidget* LBinfo = NULL;
    int inb;
    
    //we initialise
    memset(&base,0,sizeof(cl_base_t));
    
    //we get the list of files, grouped (jpg,rw,xmp)
    group_files(argc,argv);
    
    //for each file, we read xmp datas (if existing)
    read_xmp();
    
    //we read the preselection list
    read_preselect();
    
    gtk_init(&argc, &argv);
 
    Fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(Fenetre), "Images tagging");
    gtk_window_set_default_size(GTK_WINDOW(Fenetre), 300, 100);
    
    GtkWidget *vbox = gtk_vbox_new(FALSE, 3);
    
    //the label with the nb of image
    char txt[20000];
    snprintf(txt,20000,"There's %d images to tag",base.files_nb);
    
    char tt[200000]="";
    char* cur = tt;
    for (int i=0; i<base.files_nb; i++) cur += snprintf(cur,200000,"\n%d/%d - %s - %s - %s",i,argc,base.files[i]->jpg,base.files[i]->raw,base.files[i]->xmp);
    Label=gtk_label_new(txt);
    g_object_set(G_OBJECT(Label), "tooltip-text", tt, (char *)NULL);
    gtk_label_set_justify(GTK_LABEL(Label), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), Label, TRUE, TRUE, 0);
    
    //the first info entry
    Boxinfo = gtk_hbox_new(FALSE, 3);
    LBinfo = gtk_label_new("info 1");
    base.TBinfo1 = gtk_entry_new();
    inb = get_all_infos(1,txt);
    if (inb == 1) gtk_entry_set_text(GTK_ENTRY(base.TBinfo1),txt);
    else if (inb>1) gtk_entry_set_text(GTK_ENTRY(base.TBinfo1),"###-multiple-###");
    g_object_set(G_OBJECT(base.TBinfo1), "tooltip-text", txt, (char *)NULL);
    gtk_box_pack_start(GTK_BOX(Boxinfo), LBinfo, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(Boxinfo), base.TBinfo1, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), Boxinfo, TRUE, FALSE, 0);
    //the first info entry
    Boxinfo = gtk_hbox_new(FALSE, 3);
    LBinfo = gtk_label_new("info 2");
    base.TBinfo2 = gtk_entry_new();
    inb = get_all_infos(2,txt);
    if (inb == 1) gtk_entry_set_text(GTK_ENTRY(base.TBinfo2),txt);
    else if (inb>1) gtk_entry_set_text(GTK_ENTRY(base.TBinfo2),"###-multiple-###");
    g_object_set(G_OBJECT(base.TBinfo2), "tooltip-text", txt, (char *)NULL);
    gtk_box_pack_start(GTK_BOX(Boxinfo), LBinfo, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(Boxinfo), base.TBinfo2, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), Boxinfo, TRUE, FALSE, 0);
    //the first info entry
    Boxinfo = gtk_hbox_new(FALSE, 3);
    LBinfo = gtk_label_new("info 3");
    base.TBinfo3 = gtk_entry_new();
    inb = get_all_infos(3,txt);
    if (inb == 1) gtk_entry_set_text(GTK_ENTRY(base.TBinfo3),txt);
    else if (inb>1) gtk_entry_set_text(GTK_ENTRY(base.TBinfo3),"###-multiple-###");
    g_object_set(G_OBJECT(base.TBinfo3), "tooltip-text", txt, (char *)NULL);
    gtk_box_pack_start(GTK_BOX(Boxinfo), LBinfo, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(Boxinfo), base.TBinfo3, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), Boxinfo, TRUE, FALSE, 0);
    
    //the validation button
    GtkWidget* BTok = gtk_button_new_with_label("OK");
    g_signal_connect (G_OBJECT (BTok), "clicked", G_CALLBACK (ok_callback), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), BTok, FALSE, FALSE, 5);
    
    //the classe button
    BTok = gtk_button_new_with_label("CLASSE");
    g_signal_connect (G_OBJECT (BTok), "clicked", G_CALLBACK (classe_callback), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), BTok, FALSE, TRUE, 10);
    
    gtk_container_add(GTK_CONTAINER(Fenetre), vbox);
    gtk_widget_show_all(Fenetre);
 
    g_signal_connect(G_OBJECT(Fenetre), "delete-event", G_CALLBACK(gtk_main_quit), NULL); 
    
    gtk_main();
 
    return EXIT_SUCCESS;
}
