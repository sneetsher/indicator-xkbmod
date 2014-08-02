/*
 * indicator-xkbmod.c
 *
 * Copyright 2014 Abdellah Chelli <abdellahchelli@gmail.com>
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

#include "../config.h"


//commandline options
static gboolean show_label = FALSE;
static GOptionEntry option_entries[] = {
  { "use-label", 'l', 0, G_OPTION_ARG_NONE, &show_label, "Use indicator label instead of icon", NULL },
  { NULL }
};


//gtk loop callback data structure
typedef struct _AppData {
  Display *_display;
  int *_deviceId;
  AppIndicator *indicator;
  const gchar *theme_path;
} AppData;

//menu ui
void show_about ();
static GtkActionEntry entries[] = {
  //todo: add about, toggle lable
  { "About",     "application-about", "_About", NULL,
    "Show about window", G_CALLBACK (show_about) },
  { "Quit",     "application-exit", "_Quit", "<control>Q",
    "Exit the application", G_CALLBACK (gtk_main_quit) },
};

static guint n_entries = G_N_ELEMENTS (entries);

static const gchar *ui_info =
"<ui>"
"  <popup name='IndicatorPopup'>"
"    <menuitem action='About' />"
"    <menuitem action='Quit' />"
"  </popup>"
"</ui>";

//callback function, get xkb state, update indicator label (icon have limitation)
static gboolean update_xkb_state (gpointer data)
{
  //current xkb state
  XkbStateRec xkbState;
  //used to refresh icon, skipping unneeded cycles
  static XkbStateRec xkbState_prev;
  static int counter = 0;
  
  XkbGetState(((AppData*) data)->_display, *(((AppData*) data)->_deviceId), &xkbState);

  if (xkbState.mods!=xkbState_prev.mods || xkbState.locked_mods!=xkbState_prev.locked_mods || counter==0)
    xkbState_prev = xkbState;
  else
    return TRUE;

  GError **error = NULL;
  //construct label

  //symbols: shift U21E7, ctrl U22C0, alt U2325, altgr U2387, cmd U2318
  //from font: DejaVu Sans, FreeSans
  GString *label_template[] = {
    g_string_new("⇧"), 
    g_string_new("⇬"), 
    g_string_new("⋀"), 
    g_string_new("⌥"), 
    g_string_new("①"), 
    g_string_new("5"), 
    g_string_new("⌘"), 
    g_string_new("⎇")};

  GString *label = g_string_new("");
  //construct icon
  GString *svg_template[] = {
    g_string_new("\
<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n\
<svg width='144' xmlns='http://www.w3.org/2000/svg' version='1.1' height='22'>\n\
 <defs>\n\
  <mask id='m0'>\n\
   <rect y='2' x='0' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='9' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>⇧</text>\n\
  </mask>\n\
  <mask id='m1'>\n\
   <rect y='2' x='18' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='27' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>⇬</text>\n\
  </mask>\n\
  <mask id='m2'>\n\
   <rect y='2' x='36' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='45' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>⋀</text>\n\
  </mask>\n\
  <mask id='m3'>\n\
   <rect y='2' x='54' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='63' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>⌥</text>\n\
  </mask>\n\
  <mask id='m4'>\n\
   <rect y='2' x='72' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='81' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>①</text>\n\
  </mask>\n\
  <mask id='m5'>\n\
   <rect y='2' x='90' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='99' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>5</text>\n\
  </mask>\n\
  <mask id='m6'>\n\
   <rect y='2' x='108' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='117' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>⌘</text>\n\
  </mask>\n\
  <mask id='m7'>\n\
   <rect y='2' x='126' style='fill:#fff' height='18' width='18'/>\n\
   <text y='14.5' x='135' style='text-anchor:middle;font-size:16;font-family:FreeMono;font-weight:500;fill:black'>⎇</text>\n\
  </mask>\n\
 </defs>\n"),
    g_string_new("<rect style='fill:%s' mask='url(#m%i)' rx='2' height='16' width='16' y='3' x='%i'/>\n"),
    g_string_new("<rect style='fill:#f00' mask='url(#m%i)' rx='2' height='4' width='4' y='14' x='%i'/>\n"),
    g_string_new("</svg>")};
  GString *svg = g_string_new(svg_template[0]->str);
  gsize *bytes_written;

  register int i;
  unsigned bit;

  g_debug("xkbState - base %02X, latched %02X, locked %02X, effective %02X, compact %02X", xkbState.base_mods, xkbState.latched_mods, xkbState.locked_mods, xkbState.mods, xkbState.compat_state);

  //loop taken from xkbwatch source
  for (i = XkbNumModifiers - 1, bit = 0x80; i >= 0; i--, bit >>= 1)
  {
    //todo: change constant with xkb modifier constant (defined in the headers)

    if (xkbState.mods & bit) {
      g_string_prepend (label, label_template[i]->str);
      g_string_append_printf (svg, svg_template[1]->str,"#dfdbd2", i, 18*i+1);
    }
    else {
      g_string_append_printf (svg, svg_template[1]->str,"#7E7D77", i, 18*i+1);
    }

    if (xkbState.locked_mods & bit) {
      g_string_prepend (label,  " ˳");
      g_string_append_printf (svg, svg_template[2]->str, i, 18*i+2);
    }
    else {
      g_string_prepend (label,  " ");
    }
  }

  //g_string_prepend (label,  "");
  g_string_append (svg, svg_template[3]->str);

  counter++;

  if (show_label)
    app_indicator_set_label (((AppData*) data)->indicator, label->str, NULL);
  else {
    GIOChannel *svg_file = g_io_channel_new_file(g_strdup_printf("%s/icon.svg", ((AppData*) data)->theme_path), "w", error);
    g_io_channel_write_chars (svg_file, svg->str, -1, bytes_written, error);
    g_io_channel_shutdown (svg_file, TRUE, error);
    app_indicator_set_icon_theme_path (((AppData*) data)->indicator, (counter % 2)? ((AppData*) data)->theme_path : g_strdup_printf("%s/.", ((AppData*) data)->theme_path));
    app_indicator_set_icon (((AppData*) data)->indicator, "icon");
  }

  //g_free(label);
  return TRUE;
}

void show_about (){
  gtk_show_about_dialog (NULL,
                       "title" , g_strdup_printf("About %s", PACKAGE_NAME),
                       "program-name", PACKAGE_NAME,
                       "version", PACKAGE_VERSION,
                       "copyright", g_strdup_printf("Copyright %s %s", "2014", "Abdellah Chelli"),
                       "comments", "Simple XKB Modifiers Indicator",
                       "license", "GPL 3.0",
                       "website", "https://github.com/sneetsher/indicator-xkbmod/",
                       NULL);
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
  GOptionContext *option_context;

  option_context = g_option_context_new ("");
  g_option_context_set_summary (option_context, "Simple XKB Modifiers Indicator");
  g_option_context_set_help_enabled (option_context, TRUE);
  g_option_context_add_main_entries (option_context, option_entries, NULL);
  g_option_context_add_group (option_context, gtk_get_option_group (TRUE));
  if (!g_option_context_parse (option_context, &argc, &argv, &error))
    {
      g_message ("Option parsing failed: %s\n\n%s", error->message, g_option_context_get_help(option_context, FALSE, NULL));
      return 5;
    }

  gtk_init (&argc, &argv);

  XkbIgnoreExtension(False);

  g_message("Xkb client lib ver. %d.%d" , major , minor );
  _display = XkbOpenDisplay(displayName, &eventCode, &errorReturn,
                            &major, &minor, &reasonReturn);
  g_message("Xkb server lib ver. %d.%d" , major , minor );

  if (reasonReturn != XkbOD_Success)
  {
    g_error("Unable to open display!");
    return 1;
  }

  XkbDescRec* kbdDescPtr = XkbAllocKeyboard();
  if (kbdDescPtr == NULL)
  {
    g_error("Failed to get keyboard description.");
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


  const gchar *theme_path = g_get_tmp_dir();

  indicator = app_indicator_new_with_path (
                                        "Simple XKB Modifiers Indicator",
                                        "icon",
                                        APP_INDICATOR_CATEGORY_HARDWARE,
                                        theme_path);

  if (show_label)
    app_indicator_set_icon_theme_path (indicator, DATA_PATH);


  uim = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (uim, action_group, 0);
  if (!gtk_ui_manager_add_ui_from_string (uim, ui_info, -1, &error))
  {
    g_error ("Failed to build menus: %s", error->message);
    g_error_free (error);
    error = NULL;
    return 3;
  }

  indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");
  app_indicator_set_menu (indicator, GTK_MENU (indicator_menu));
  app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);

  appdata._display = _display;
  appdata._deviceId = &_deviceId;
  appdata.indicator = indicator;
  appdata.theme_path = theme_path;
  gtk_timeout_add (50, (GtkFunction) update_xkb_state, &appdata);

  gtk_main ();

  XCloseDisplay (_display);
  return 0;
}
