#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include "classe.h"
#include "utils.h"

int calc_dest(char* pattern)
{
  
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

void cb_callback(GtkComboBox *widget, GtkWidget* Table)
{
  //we retrieve the actual pattern
  GtkTreeModel *p_model = NULL;
  GtkTreeIter iter;
  char p_txt[2048] = "";
 
 
   /* On recupere le modele qu'on a cree. */
   p_model = gtk_combo_box_get_model(widget);
 
   /* On recupere le GtkTreeIter de l'element courant. */
   if (gtk_combo_box_get_active_iter(widget, &iter))
   {
      gtk_tree_model_get (p_model, &iter, 0, p_txt, -1);
   }
   
   //On applique les changements sur les listes
   calc_dest(p_txt);
   
   //On met à jour le tableau
   table_refresh(Table);
}

static void classe_callback(GtkButton *button, char* nu)
{
  
}

int classe_lance()
{
  //window creation
  GtkWidget* Fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(Fenetre), "Images classification");
  gtk_window_set_default_size(GTK_WINDOW(Fenetre), 600, 100);
  GtkWidget *vbox = gtk_vbox_new(FALSE, 3);
  
  //combobox to show the patern
  GtkListStore * p_model = gtk_list_store_new (1, G_TYPE_STRING);
  GtkWidget* Combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (p_model));
  GtkCellRenderer * p_cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (Combo), p_cell, FALSE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (Combo), p_cell, "text", 0, NULL);
  //gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX (Combo),0);
  
  //we read the saved patterns  
  FILE* fp = NULL;
  char chaine[2048] = "";
  sprintf(chaine,"%s/class2/",g_get_user_config_dir());
  struct stat st = {0};
  if (stat(chaine, &st) == -1) mkdir(chaine, 0700);
  sprintf(chaine,"%s/class2/patterns.txt",g_get_user_config_dir());
  fp = fopen(chaine, "r");
  if (fp)
  {
    while (fgets(chaine, 2048, fp))
    {
      GtkTreeIter iter;
      gtk_list_store_append (p_model, &iter);
      gtk_list_store_set (p_model, &iter, 0, chaine, -1);
    }
    fclose(fp);
  }  
  
  //if there's no pattern, we add a basic one
  if (gtk_tree_model_iter_n_children(GTK_TREE_MODEL(p_model),NULL) == 0)
  {
    sprintf(chaine,"%s/photos/%%Y/%%M/%%N3-%%F%%ex",g_get_user_data_dir());
    GtkTreeIter iter;
    gtk_list_store_append (p_model, &iter);
    gtk_list_store_set (p_model, &iter, 0, chaine, -1);
  }
  gtk_combo_box_set_active (GTK_COMBO_BOX (Combo), 0);
  
  gtk_box_pack_start(GTK_BOX(vbox), Combo, TRUE, TRUE, 0);
  
  
  //table to show before/after paths
  GtkListStore * t_model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
  GtkWidget* Table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(t_model));
  GtkCellRenderer *t_cell = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(Table),-1,"Source",t_cell,"text",0,NULL);
  t_cell = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(Table),-1,"Dest",t_cell,"text",1,NULL);
  table_refresh(Table);
  
  gtk_box_pack_start(GTK_BOX(vbox), Table, TRUE, TRUE, 0);
  
  //and we connect the callbacks
  g_signal_connect (G_OBJECT (Combo), "changed", G_CALLBACK (cb_callback), Table);
  
  //button to launch
  GtkWidget* BTok = gtk_button_new_with_label("CLASSE");
  g_signal_connect (G_OBJECT (BTok), "clicked", G_CALLBACK (classe_callback), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), BTok, FALSE, TRUE, 10);
    
  gtk_container_add(GTK_CONTAINER(Fenetre), vbox);
  
  gtk_widget_show_all(Fenetre);
  return 1;
}
