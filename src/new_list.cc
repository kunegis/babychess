#include "list.hh"

bool List::Open(Pointer <Widget> parent,
		Open_Info open_info)
{
	store= gtk_list_store_new
		(open_info.column_count, 
		 G_TYPE_STRING, G_TYPE_STRING);
	if (store == NULL)
		return false;

	GtkWidget *tree= gtk_tree_view_new_with_model
		(GTK_TREE_MODEL (store));
	if (tree == NULL)
	{
		gobject_destroy(store);
		return false;
	}
	
	if (! Widget::Open(parent, tree))
	{
		gobject_destroy(store);
		return false;
	}

	return true; 
}

void List::Clear()
{
	gtk_list_store_clear(store); 
}

void List::Append_Row()
{
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
}

void List::Set(int column, int row, const char *text)
{
	GtkTreePath *path= gtk_tree_path_new_from_indices(row, -1);
	GtkListStore *list_store;
	gtk_tree_model_get_iter(GTK_TREE_MODEL (store),
				&iter,
				path);
	gtk_tree_path_free (path);
	gtk_list_store_set (list_store, &iter,
			    column, text,
			    -1);
}

