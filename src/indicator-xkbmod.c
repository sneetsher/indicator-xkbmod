/*
 * indicator-xkbmod.c
 *
 * Copyright 2014 Sneetsher <sneetsher@localhost>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include <string.h>

#include <X11/XKBlib.h>

#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

//callback data structure
typedef struct _AppData {
  Display *_display;
  int *_deviceId;
  AppIndicator *indicator;
} AppData;


//menu ui
static GtkActionEntry entries[] = {
  { "Quit",     "application-exit", "_Quit", "<control>Q",
    "Exit the application", G_CALLBACK (gtk_main_quit) },
};

static guint n_entries = G_N_ELEMENTS (entries);

static const gchar *ui_info =
"<ui>"
"  <popup name='IndicatorPopup'>"
"    <menuitem action='Quit' />"
"  </popup>"
"</ui>";

//callback function, get xkb state, update indicator label (icon have limitation)
static gboolean update_xkb_state (gpointer data)
{
  //get xkb state
  XkbStateRec xkbState;
  XkbGetState(((AppData*) data)->_display, *(((AppData*) data)->_deviceId), &xkbState);

  //construct label
  GString *label = g_string_new("");

  register int i;
  unsigned bit;

  //loop taken from xkbwatch source
  for (i = XkbNumModifiers - 1, bit = 0x80; i >= 0; i--, bit >>= 1)
  {
    //printf("base%d %s  ", i, (xkbState.base_mods & bit) ? "on " : "off");
    //printf("latched%d %s  ", i, (xkbState.latched_mods & bit) ? "on " : "off");
    //printf("locked%d %s  ", i, (xkbState.locked_mods & bit) ? "on " : "off");
    //printf("effective%d %s  ", i, (xkbState.mods & bit) ? "on " : "off");
    //printf("compat%d %s\n", i, (xkbState.compat_state & bit) ? "on " : "off");

    //todo: change constant with xkb modifier constant (defined in the headers)
    // show effective modifier stat
    switch (i)
    {
      case 7:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "⎇" : ""));
        break;
      case 6:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "⌘" : ""));
        break;
      case 5:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "5" : ""));
        break;
      case 4:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "①" : ""));
        break;
      case 3:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "⌥" : ""));
        break;
      case 2:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "⋀" : ""));
        break;
      case 1:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "⇬" : ""));
        break;
      case 0:
        g_string_prepend (label,  ((xkbState.mods & bit) ? "⇧" : ""));
        break;
      default:
        break;
    };

    // show if modifier is locked
    g_string_prepend (label,  ((xkbState.locked_mods & bit) ? " ˳" : " "));
  }

  //g_string_prepend (label,  "");
  app_indicator_set_label (((AppData*) data)->indicator, label->str, NULL);

  //g_free(label);
  return TRUE;
}


int main (int argc, char **argv)
{
  AppData appdata;
  Display *_display;
  int _deviceId;

  char* displayName = strdup("");
  int eventCode;
  int errorReturn;
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;;
  int reasonReturn;


  AppIndicator *indicator;
  GtkWidget *indicator_menu;
  GtkUIManager *uim;
  GtkActionGroup *action_group;
  GError *error = NULL;

  gtk_init (&argc, &argv);


  XkbIgnoreExtension(False);

  g_printf("Xkb client lib ver: %d.%d\n" , major , minor );
  _display = XkbOpenDisplay(displayName, &eventCode, &errorReturn,
                            &major, &minor, &reasonReturn);
  g_printf("Xkb server lib ver: %d.%d\n" , major , minor );

  if (reasonReturn != XkbOD_Success)
  {
    g_printf("Unable to open display!\n");
    return 1;
  }

  XkbDescRec* kbdDescPtr = XkbAllocKeyboard();
  if (kbdDescPtr == NULL)
  {
    g_printf ("Failed to get keyboard description.\n");
    return 2;
  }
  kbdDescPtr->dpy = _display;
  _deviceId = kbdDescPtr->device_spec;

  /*
  //no need for event listener, used gtk_timeout timer
  XkbSelectEventDetails(_display, XkbUseCoreKbd, XkbStateNotify,
                     XkbAllStateComponentsMask, XkbGroupStateMask);
  */


  action_group = gtk_action_group_new ("AppActions");
  gtk_action_group_add_actions (action_group, entries, n_entries, NULL);

  indicator = app_indicator_new_with_path (
                                        "Simple XKB Modifier Indicator",
                                        "icon",
                                        APP_INDICATOR_CATEGORY_HARDWARE,
                                        DATA_PATH);
                                        //g_get_current_dir());


  uim = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (uim, action_group, 0);
  if (!gtk_ui_manager_add_ui_from_string (uim, ui_info, -1, &error))
  {
    g_printf ("Failed to build menus: %s\n", error->message);
    g_error_free (error);
    error = NULL;
    return 3;
  }

  indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");
  app_indicator_set_menu (indicator, GTK_MENU (indicator_menu));
  app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);

  //app_indicator_set_label (indicator, " ⇧ ⋀ ⌥ ⎇  ⌘ ", NULL);
  //symbols: shift U21E7 ctrl U22C0 alt/altgr U2325 U2387  cmd U2318
  //from font: DejaVu Sans

  appdata._display = _display;
  appdata._deviceId = &_deviceId;
  appdata.indicator = indicator;
  gtk_timeout_add (120, (GtkFunction) update_xkb_state, &appdata);
  //nice for realtime tasks, to replace gtk_timeout
  //gtk_idle_add ((GtkFunction) idle_func, &appdata);

  gtk_main ();

  XCloseDisplay (_display);
  return 0;
}
