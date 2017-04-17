if [ -z "$menues_sh" ] ; then
menues_sh=t

##
## Location of desktop menu directories 
##

gnome_main_menu_dir=/usr/share/gnome/apps/Games
kde_main_menu_dir=/etc/X11/applnk/Games

gnome_home_menu_dir="$HOME/.gnome/apps/Games"
kde_home_menu_dir="$HOME/.kde/share/applnk/Games"

main_mimelnk_dir=/usr/share/mimelnk
home_mimelnk_dir="$HOME/.kde/share/mimelnk"

fi ## [ -z "$menues_sh" ] 
