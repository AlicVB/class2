#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <glib.h>
#include "classe.h"
#include "utils.h"
#include "exiv.h"

int preformat(cl_file_t* f, char *np)
{
  char p1[1024] = "";
  char* p2;
  p2 = np;
  
  while (strlen(p2)>0)
  {
    //we read the first char to see if it's a '%'
    if (strchr(p2,'%') == p2)
    {
      //we just look at the next char
      p2 += 1;
      if (strlen(p2)<1) break;
      if (strchr(p2,'%') == p2)
      {
        //we just add the '%' char
        strncat(p1,"%",1);
        p2 += 1;
      }
      else if (strchr(p2,'N') == p2 && strlen(p2)>1)
      {
        //this is a number... we don't change it
        strncat(p1,"%",1);
        strncat(p1,p2,2);
        p2 += 2;
      }
      else
      {
        //we should have 2 other char after
        if (strlen(p2)<2) break;
        char tag[2] = "";
        strncpy(tag, p2, 2);
        char val[30]="";
        if (strcmp(tag, "Y4") == 0)
        {
          strftime(val,30,"%Y",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "Y2") == 0)
        {
          strftime(val,30,"%y",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "M2") == 0)
        {
          strftime(val,30,"%m",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "Ml") == 0)
        {
          strftime(val,30,"%B",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "D2") == 0)
        {
          strftime(val,30,"%d",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "Dl") == 0)
        {
          strftime(val,30,"%A",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "HH") == 0)
        {
          strftime(val,30,"%H",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "mm") == 0)
        {
          strftime(val,30,"%M",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "ss") == 0)
        {
          strftime(val,30,"%S",localtime(&f->tt));
          strncat(p1,val,30);
        }
        else if (strcmp(tag, "I1") == 0)
        {
          strncat(p1,f->i1,2048);
        }
        else if (strcmp(tag, "I2") == 0)
        {
          strncat(p1,f->i2,2048);
        }
        else if (strcmp(tag, "I3") == 0)
        {
          strncat(p1,f->i3,2048);
        }
        else if (strcmp(tag, "Fe") == 0)
        {
          if (fileexists(f->raw))
          {
            strncat(p1,".",1);
            for (int i=strlen(f->raw)-3; i<strlen(f->raw); i++)
            {
              int l = tolower(*(f->raw+i));
              strncat(p1,(const char*) &l,1);
            }
          }
          else if (fileexists(f->jpg)) strncat(p1,".jpg",4);
        }
        else if (strcmp(tag, "FE") == 0)
        {
          if (fileexists(f->raw))
            strncat(p1,f->raw+strlen(f->raw)-4,4);
          else if (fileexists(f->jpg))
            strncat(p1,f->jpg+strlen(f->jpg)-4,4);
        }
        else if (strcmp(tag, "FN") == 0)
        {
          if (fileexists(f->raw)) strncat(p1,f->raw,1024);
          else if (fileexists(f->jpg)) strncat(p1,f->jpg,1024);
        }
        else if (strcmp(tag, "Fn") == 0)
        {
          char *ff = NULL;
          if (fileexists(f->raw)) ff = f->raw;
          else if (fileexists(f->jpg)) ff = f->jpg;
          if (ff)
          {
            char *fn = strrchr(ff,'/');
            if (fn) strncat(p1,fn,1024);
          }
        }
        p2 += 2;
      }
    }
    else
    {
      strncat(p1,p2,1);
      p2 += 1;
    }
  }

  strncpy(np,p1,1024);
  return 1;
}

int get_num(char *res, char **files, int *nbf, char **dirs, int *nbd, char *p1, int digit, char *p2)
{
  char r[2048] = "";
  strncpy(r,p1,2048);
  
  if (digit > 0 && digit < 9)
  {
    int nb = 1;
    //is it a file or a directory ??
    if (strchr(p2,'/')) //this is a dir
    {
      //we get the basedir and the dir prefix
      char *pre = strrchr(p1,'/');
      char dd[2048] = "";
      strncpy(dd,p1,strlen(p1)-strlen(pre)+1);
      char findos[2048] = "";
      char *p3 = strchr(p2,'/');
      strncpy(findos,p2,strlen(p2)-strlen(p3));
      
      //we get all the subdirs inside the dir
      char **ff = (char**) malloc(4096*sizeof(char*));
      memset(ff,0,4096*sizeof(char*));
      for (int i=0; i<4096; i++)
      {
        ff[i] = (char*) malloc(512*sizeof(char));
        memset(ff[i],0,512*sizeof(char));
      }
      int nbff = getalldirs(dd, ff, dirs, *nbd);
      
      //we test each subdirs from the filesystem
      for (int i=0; i<nbff; i++)
      {
        //does the dir start with prefix ?
        if (strstr(ff[i],pre+1) == ff[i])
        {
          //is there a number with 'digit' digits after that ?
          char txt[9] = "";
          if (strlen(pre)+digit < strlen(ff[i])) strncpy(txt,ff[i]+strlen(pre)-1,digit);
          int n = atoi(txt);
          if (n > 0)
          {
            //is the end of the dos the same ?
            char fin2[1024];
            strncpy(fin2,ff[i]+strlen(pre)-1+digit,1024);
            if (strcmp(fin2,findos)==0)
            {
              nb = n;
            }
            else
            {
              if (n >= nb) nb = n + 1;
            }
          }
        }
      }
      for (int i=0; i<4096; i++) free(ff[i]);
      free(ff);
    }
    else //this is a file
    {
      //we get the dir and the filename prefix
      char *pre = strrchr(p1,'/');
      char dd[2048] = "";
      strncpy(dd,p1,strlen(p1)-strlen(pre)+1);

      //we get all the files inside the dir
      char **ff = (char**) malloc(4096*sizeof(char*));
      memset(ff,0,4096*sizeof(char*));
      for (int i=0; i<4096; i++)
      {
        ff[i] = (char*) malloc(512*sizeof(char));
        memset(ff[i],0,512*sizeof(char));
      }
      int nbff = getallfiles(dd, ff, files, *nbf);

      //for each files, we 
      for (int i=0; i<nbff; i++)
      {
        //does the file start with prefix ?
        if (strstr(ff[i],pre+1) == ff[i])
        {
          char txt[9] = "";
          if (strlen(pre)+digit < strlen(ff[i])) strncpy(txt,ff[i]+strlen(pre)-1,digit);
          int n = atoi(txt);
          if (nb < n + 1) nb = n+1;
        }
      }

      for (int i=0; i<4096; i++) free(ff[i]);
      free(ff);
    }
    //and we add the number in text with 00...
    char tx1[5];
    snprintf(tx1,5,"%%0%dd",digit);
    char tx2[10];
    snprintf(tx2,10,tx1,nb);
    strncat(r,tx2,2048);
  }
  strncat(r,p2,2048);
  strncpy(res,r,2048);
  return 1;
}

int calc_dest(cl_file_t* f, char *pattern, char **files, int *nbf, char **dirs, int *nbd)
{
  //we replace all tags except numbers
  char np[1024] = "";
  strncpy(np,pattern,1024);
  preformat(f,np);

  //we replace the numbers
  while(strstr(np,"%N"))
  {
    char *p2 = strstr(np,"%N");
    char d[1];
    strncpy(d,p2+2,1);
    char p1[1024] = "";
    strncpy(p1,np,strlen(np)-strlen(p2));
    int digit = atoi(d);
    get_num(np, files, nbf, dirs, nbd, p1, digit, p2+3);
  }
  
  //we add the filename and the foldername to the "plus" tables
  strncpy(files[*nbf],np,512);
  *nbf += 1;
  char *fn = strrchr(np,'/');
  strncpy(dirs[*nbd],np,strlen(np)-strlen(fn));
  *nbd += 1;
  
  //and we set the destination files
  if (fileexists(f->raw))
  {
    strncpy(f->draw,np,1024);
    strncpy(f->djpg,substring(f->draw,0,strlen(f->draw)-3),1024);
    strcat(f->djpg,"jpg");
    strncpy(f->dxmp,f->draw,1024);
    strcat(f->dxmp,".xmp");
  }
  else if (fileexists(f->jpg))
  {
    strncpy(f->djpg,np,1024);
    strncpy(f->dxmp,f->djpg,1024);
    strcat(f->dxmp,".xmp");
  }
  
  return 1;
}

int calc_dests(char* pattern)
{
  // pattern should start with '/'
  if (pattern[0] != '/') return 0;
  
  char **files = (char**) malloc(4096*sizeof(char*));
  char **dirs = (char**) malloc(4096*sizeof(char*));
  for (int i=0; i<4096; i++)
  {
    files[i] = (char*) malloc(512*sizeof(char));
    dirs[i] = (char*) malloc(512*sizeof(char));
    memset(files[i],0,512*sizeof(char));
    memset(dirs[i],0,512*sizeof(char));
  }
  int nbf = 0;
  int nbd = 0;
  
  for (int i=0; i<base.files_nb; i++)
  {
    calc_dest(base.files[i],pattern,files,&nbf,dirs,&nbd);
  }
  
  for (int i=0; i<4096; i++)
  {
    free(files[i]);
    free(dirs[i]);
  }
  free(files);
  free(dirs);
  
  return 1;
}

int table_refresh(GtkWidget* Table)
{
  GtkTreeModel *mod = gtk_tree_view_get_model(GTK_TREE_VIEW(Table));
  //we clear the table
  gtk_list_store_clear(GTK_LIST_STORE(mod));
  
  //and we refill it twith the values
  GtkTreeIter iter;
  for (int i=0; i<base.files_nb; i++)
  {
    if (fileexists(base.files[i]->jpg))
    {
      gtk_list_store_append (GTK_LIST_STORE(mod), &iter);
      gtk_list_store_set (GTK_LIST_STORE(mod), &iter, 0, base.files[i]->jpg,1,base.files[i]->djpg, -1);
    }
    if (fileexists(base.files[i]->raw))
    {
      gtk_list_store_append (GTK_LIST_STORE(mod), &iter);
      gtk_list_store_set (GTK_LIST_STORE(mod), &iter, 0, base.files[i]->raw,1,base.files[i]->draw, -1);
    }
    if (fileexists(base.files[i]->xmp))
    {
      gtk_list_store_append (GTK_LIST_STORE(mod), &iter);
      gtk_list_store_set (GTK_LIST_STORE(mod), &iter, 0, base.files[i]->xmp,1,base.files[i]->dxmp, -1);
    }
  }
  return 1;
}

void save_pattern(char* pattern)
{
  //we read all existing pattern and compare them to the actual one
  FILE* fp = NULL;
  FILE* fp2 = NULL;
  char chaine[2048] = "";
  char patt[2048] = "";
  sprintf(patt,"%s/class2/",g_get_user_config_dir());
  struct stat st = {0};
  if (stat(patt, &st) == -1) mkdir(patt, 0700);
  sprintf(patt,"%s/class2/patterns.txt",g_get_user_config_dir());
  
  //we create the table for the patterns
  char tp[512][512];
  strncpy(tp[0],pattern,512);
  int nb = 1;
  if (fileexists(patt))
  {
    //we read it
    fp = fopen(patt, "r");
    if (fp)
    {
      while (fgets(chaine, 2048, fp))
      {
        char* ch2 = substring(chaine,0,strlen(chaine)-1);
        if (strcmp(ch2,"") != 0)
        {
          //we ensure it's not already in the table
          int already=0;
          for (int i=0; i<nb; i++)
          {
            if (strcmp(tp[i],ch2)==0)
            {
              already=1;
              break;
            }
          }
          if (already == 0)
          {
            strncpy(tp[nb],ch2,512);
            nb++;
          }
        }
      }
      fclose(fp);
    }
  }
  
  //we write all the patterns
  fp2 = fopen(patt, "w+");
  if (fp2)
  {
    for (int i=0; i<nb; i++)
    {
      fputs(tp[i], fp2);
      fputs("\n", fp2);
    }
    fclose(fp2);
  }
}

void cb_callback(GtkComboBoxText *widget, GtkWidget* Table)
{   
   //On applique les changements sur les listes
   calc_dests((char*) gtk_combo_box_text_get_active_text(widget));
   
   //On met à jour le tableau
   table_refresh(Table);
}

static void add_err(int cde, char* err, int *errpos)
{
  if (cde==10)
  {
    strncat(err,"original file doesn't exist :\n",100000-(*errpos));
    (*errpos) += 30;
  }
  else if (cde==11)
  {
    strncat(err,"destination file already exist :\n",100000-(*errpos));
    (*errpos) += 33;
  }
  else if (cde==15)
  {
    strncat(err,"can't create destination directory :\n",100000-(*errpos));
    (*errpos) += 37;
  }
  else if (cde==20)
  {
    strncat(err,"destination file not created :\n",100000-(*errpos));
    (*errpos) += 31;
  }
  else if (cde==21)
  {
    strncat(err,"file sizes are different :\n",100000-(*errpos));
    (*errpos) += 27;
  }
} 

static void confirm_hide_callback(GtkButton *button, GtkWidget* w)
{
  gtk_main_quit();
}

static void classe_validation(char* err, int errpos)
{
  //window creation
  GtkWidget* Fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(Fenetre), "END");
  if (errpos > 0) gtk_window_set_icon(GTK_WINDOW(Fenetre), gtk_widget_render_icon(Fenetre,GTK_STOCK_DIALOG_ERROR,GTK_ICON_SIZE_MENU,NULL));
  else gtk_window_set_icon(GTK_WINDOW(Fenetre), gtk_widget_render_icon(Fenetre,GTK_STOCK_APPLY,GTK_ICON_SIZE_MENU,NULL));
  if (errpos > 0) gtk_window_set_default_size(GTK_WINDOW(Fenetre), 800, 600);
  GtkWidget *vbox = gtk_vbox_new(FALSE, 3);
  
  //label to show the message
  char txt[1024];
  if (errpos > 0) strcpy(txt,"There has been errors during the process !");
  else strcpy(txt,"Process successfull !");
  GtkWidget* label = gtk_label_new(txt);  
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 10);
  
  //if needed, we show the error message text
  if (errpos > 0)
  {
    GtkWidget* sw = gtk_scrolled_window_new(NULL,NULL);
    GtkWidget *view;
    GtkTextBuffer *buffer;
    view = gtk_text_view_new ();
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    gtk_text_buffer_set_text (buffer, err, -1);
    gtk_container_add(GTK_CONTAINER(sw), view);
    gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);
  }
  
  //and the ok button
  GtkWidget* BTok = gtk_button_new_with_label("OK");
  gtk_box_pack_start(GTK_BOX(vbox), BTok, FALSE, TRUE, 10);
  
  //and we connect the callbacks  
  g_signal_connect (G_OBJECT (BTok), "clicked", G_CALLBACK (confirm_hide_callback), Fenetre);
  g_signal_connect(G_OBJECT(Fenetre), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    
  gtk_container_add(GTK_CONTAINER(Fenetre), vbox);
  gtk_widget_show_all(Fenetre);  
}

static bool is_video(char* fn)
{
  //we search the extension
  char* ext = (char*) malloc (sizeof (*fn) * (4));
  for (int j = 0; j < 3; j++) ext[j] = tolower(fn[j+strlen(fn)-3]);
  ext[3] = '\0';
  bool rep = FALSE;
  if (strstr(";avi;mov;mpg;flv;mp4;",ext)) rep=TRUE;
  free(ext);
  return rep;
}

static void classe_videos()
{
  //we get the initial directory
  char *ff = NULL;
  if (fileexists(base.files[0]->raw)) ff = base.files[0]->raw;
  else if (fileexists(base.files[0]->jpg)) ff = base.files[0]->jpg;
  else return;
  
  char *pre = strrchr(ff,'/');
  char dd[2048] = "";
  strncpy(dd,ff,strlen(ff)-strlen(pre)+1);
  
  //we look for videos
  unsigned char isFile =0x8;
  DIR *Dir = NULL;
  struct dirent *DirEntry;
  Dir = opendir(dd);
  int nb = 0;
  char **videos = (char**) malloc(4096*sizeof(char*));
  for (int i=0; i<4096; i++) videos[i] = (char*) malloc(1024*sizeof(char));
  
  if (Dir)
  {
    while(DirEntry=readdir(Dir))
    {
       if ( DirEntry->d_type == isFile)
       {
         if (is_video(DirEntry->d_name))
         {
           strncpy(videos[nb],DirEntry->d_name,1024);
           nb++;
         }
       }
    }
  }
  
  //if there's some videos, we ask what to do
  if (nb == 0) return;
  char *pre2 = strrchr(base.files[0]->dxmp,'/');
  char ddd[2048] = "";
  strncpy(ddd,base.files[0]->dxmp,strlen(base.files[0]->dxmp)-strlen(pre2)+1);
  GtkWidget* dialog = gtk_message_dialog_new (NULL,
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_QUESTION,
                                  GTK_BUTTONS_YES_NO,
                                  "There is %d videos in the base directory (%s).\nDo you want to copy them in the destination dir (%s) ?",
                                  nb,dd,ddd);
  gint result = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  if (result == GTK_RESPONSE_NO)
  {
    for (int i=0; i<4096; i++) free(videos[i]);
    free(videos);
    return;
  }
  
  //do the copy
  for (int i=0 ; i<nb ; i++)
  {
    char dest[1024];
    snprintf(dest,1024,"%s/%s",ddd,strrchr(videos[i],'/'));
    file_copy(videos[i],dest);
  }
  GtkWidget* dialog2 = gtk_message_dialog_new (NULL,
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_INFO,
                                  GTK_BUTTONS_OK,
                                  "Videos copy done !");
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  for (int i=0; i<4096; i++) free(videos[i]);
  free(videos);
}

static void classe_callback(GtkButton *button, GtkWidget* vbox)
{
  //we get all the differents widgets
  GList* wl = gtk_container_get_children(GTK_CONTAINER(vbox));
  GtkWidget* Combo = (GtkWidget*) g_list_nth_data(wl,0);
  GtkWidget* pb = (GtkWidget*) g_list_nth_data(wl,3);
  
  //we save the pattern if it's a new one
  char* pattern = (char*) gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(Combo));
  save_pattern(pattern);
  
  //we initiate the error buffer
  char *err = (char*) malloc(100000*sizeof(char));
  memset(err,0,100000*sizeof(char));
  int errpos=0;
  
  //we start the files copy
  for (int i=0; i<base.files_nb; i++)
  {
    //we copy the raw file
    if (fileexists(base.files[i]->raw))
    {
      int rep = file_copy(base.files[i]->raw,base.files[i]->draw);
      if (rep != 1)
      {
        add_err(rep,err,&errpos);
        strncat(err,"    original : ",100000-errpos);
        errpos += 15;
        strncat(err,base.files[i]->raw,100000-errpos);
        errpos += strlen(base.files[i]->raw);
        strncat(err,"\n    destination : ",100000-errpos);
        errpos += 19;
        strncat(err,base.files[i]->draw,100000-errpos);
        errpos += strlen(base.files[i]->draw);
        strncat(err,"\n\n",100000-errpos);
        errpos += 2;
      }
    }
    
    //we copy the jpg file
    if (fileexists(base.files[i]->jpg))
    {
      int rep = file_copy(base.files[i]->jpg,base.files[i]->djpg);
      if (rep != 1)
      {
        add_err(rep,err,&errpos);
        strncat(err,"    original : ",100000-errpos);
        errpos += 15;
        strncat(err,base.files[i]->jpg,100000-errpos);
        errpos += strlen(base.files[i]->jpg);
        strncat(err,"\n    destination : ",100000-errpos);
        errpos += 19;
        strncat(err,base.files[i]->djpg,100000-errpos);
        errpos += strlen(base.files[i]->djpg);
        strncat(err,"\n\n",100000-errpos);
        errpos += 2;
      }
    }
    
    //we copy the xmp file
    if (fileexists(base.files[i]->xmp))
    {
      int rep = file_copy(base.files[i]->xmp,base.files[i]->dxmp);
      if (rep != 1)
      {
        add_err(rep,err,&errpos);
        strncat(err,"    original : ",100000-errpos);
        errpos += 15;
        strncat(err,base.files[i]->xmp,100000-errpos);
        errpos += strlen(base.files[i]->xmp);
        strncat(err,"\n    destination : ",100000-errpos);
        errpos += 19;
        strncat(err,base.files[i]->dxmp,100000-errpos);
        errpos += strlen(base.files[i]->dxmp);
        strncat(err,"\n\n",100000-errpos);
        errpos += 2;
      }
    }
    
    //we write xmp tags
    xmp_save_infos(base.files[i]->dxmp,base.files[i]->i1,base.files[i]->i2,base.files[i]->i3,&base.files[i]->tt);

    //we update the progressbar
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb),(double)(i+1)/(double)base.files_nb);
    while (gtk_events_pending ()) gtk_main_iteration ();
    
  }
  
  //if there's some errors, we show them. Otherwise, we show a success window
  gtk_widget_destroy(gtk_widget_get_toplevel(vbox));
  classe_validation(err,errpos);
  free(err);
  
  //if there's some videos in the same directory, we ask what to do with them
  classe_videos();
}

int classe_lance()
{
  //window creation
  GtkWidget* Fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(Fenetre), "Images classification");
  gtk_window_set_default_size(GTK_WINDOW(Fenetre), 900, 600);
  gtk_window_set_icon(GTK_WINDOW(Fenetre), gtk_widget_render_icon(Fenetre,GTK_STOCK_GOTO_BOTTOM,GTK_ICON_SIZE_MENU,NULL));
  GtkWidget *vbox = gtk_vbox_new(FALSE, 3);
  
  //combobox to show the patern
  GtkWidget* Combo = gtk_combo_box_text_new_with_entry();
  
  //we read the saved patterns  
  FILE* fp = NULL;
  char chaine[2048] = "";
  sprintf(chaine,"%s/class2/",g_get_user_config_dir());
  struct stat st = {0};
  if (stat(chaine, &st) == -1) mkdir(chaine, 0700);
  sprintf(chaine,"%s/class2/patterns.txt",g_get_user_config_dir());
  fp = fopen(chaine, "r");
  int nb = 0;
  if (fp)
  {
    while (fgets(chaine, 2048, fp))
    {
      char* ch2 = substring(chaine,0,strlen(chaine)-1);
      if (strcmp(ch2,"") != 0)
      {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(Combo),ch2);
        nb++;
      }
    }
    fclose(fp);
  }  
  
  //if there's no pattern, we add a basic one
  if (nb == 0)
  {
    sprintf(chaine,"%s/photos/%%Y4/%%M2/%%N3-%%I3%%Fe",g_get_user_data_dir());
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(Combo),chaine);
  }
  
  gtk_box_pack_start(GTK_BOX(vbox), Combo, FALSE, TRUE, 0);
  
  
  //table to show before/after paths
  GtkWidget* sw = gtk_scrolled_window_new(NULL,NULL);
  GtkListStore * t_model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
  GtkWidget* Table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(t_model));
  GtkCellRenderer *t_cell = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(Table),-1,"Source",t_cell,"text",0,NULL);
  t_cell = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(Table),-1,"Dest",t_cell,"text",1,NULL);
  table_refresh(Table);
  
  gtk_container_add(GTK_CONTAINER(sw), Table);
  gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);
  
  
  
  //and we connect the callbacks
  g_signal_connect (G_OBJECT (Combo), "changed", G_CALLBACK (cb_callback), Table);
  gtk_combo_box_set_active (GTK_COMBO_BOX (Combo), 0);
  
  //button to launch
  GtkWidget* BTok = gtk_button_new_with_label("CLASSE");
  g_signal_connect (G_OBJECT (BTok), "clicked", G_CALLBACK (classe_callback), vbox);
  gtk_box_pack_start(GTK_BOX(vbox), BTok, FALSE, FALSE, 5);
  
  //the progress bar
  GtkWidget* pb = gtk_progress_bar_new();
  gtk_box_pack_start(GTK_BOX(vbox), pb, FALSE, TRUE, 5);
  
  gtk_container_add(GTK_CONTAINER(Fenetre), vbox);
  
  g_signal_connect(G_OBJECT(Fenetre), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(Fenetre);
  return 1;
}
