#ifndef HELP_MENU_HH
#define HELP_MENU_HH

#include "about.hh"

#include "../dep/strings.hh"

#include <gtk/gtkstock.h>

#define HELP_MENU \
  { "/"N_("_Help"),            NULL,         \
    NULL,           0,                  "<Branch>",    0},\
  { "/Help/"N_("_About"),                      NULL,        \
    &New_About,  str_about,    "<StockItem>", GTK_STOCK_ABOUT},

#endif /* ! HELP_MENU_HH */
