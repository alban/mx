/*
 * Copyright 2009 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * Boston, MA 02111-1307, USA.
 *
 */
#include <mx/mx.h>

static void
stage_size_notify_cb (ClutterActor *stage,
                      GParamSpec *pspec,
                      ClutterActor *table)
{
  gfloat width, height;

  clutter_actor_get_size (stage, &width, &height);
  clutter_actor_set_size (table, width, height);
}

int
main (int argc, char** argv)
{
  ClutterActor *stage, *text;
  MxWidget *w, *table, *button, *scrollview, *entry;

  clutter_init (&argc, &argv);

  mx_style_load_from_file (mx_style_get_default (), "custom.css", NULL);

  stage = clutter_stage_get_default ();
  clutter_stage_set_user_resizable (CLUTTER_STAGE (stage), TRUE);

  table = mx_table_new ();
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (table));
  clutter_actor_set_position (CLUTTER_ACTOR (table), 0, 0);

  button = mx_button_new_with_label ("Back");
  mx_table_add_actor_with_properties (MX_TABLE (table), (ClutterActor*) button, 0, 0,
                                        "x-expand", FALSE, "y-expand", FALSE, NULL);

  button = mx_button_new_with_label ("Forward");
  mx_table_add_actor_with_properties (MX_TABLE (table), (ClutterActor*) button, 0, 1,
                                        "x-expand", FALSE,  "y-expand", FALSE, NULL);

  entry = mx_entry_new ("");
  mx_table_add_actor_with_properties (MX_TABLE (table), (ClutterActor*) entry, 0, 2,
                                        "y-expand", FALSE, NULL);


  button = mx_button_new_with_label ("Preferences");
  mx_table_add_actor_with_properties (MX_TABLE (table), (ClutterActor*) button, 0, 3,
                                        "x-expand", FALSE,  "y-expand", FALSE, NULL);

  button = mx_button_new_with_label ("Close");
  clutter_actor_set_name (CLUTTER_ACTOR (button), "close-button");
  mx_table_add_actor_with_properties (MX_TABLE (table), (ClutterActor*) button, 0, 4,
                                        "x-expand", FALSE,  "y-expand", FALSE, NULL);

  scrollview = mx_scroll_view_new ();
  mx_table_add_actor_with_properties (MX_TABLE (table), (ClutterActor*) scrollview, 1, 0,
                                        "col-span", 5, "x-expand", FALSE, NULL);

  text = clutter_text_new ();
  clutter_text_set_line_wrap (CLUTTER_TEXT (text), TRUE);
  clutter_actor_set_width (text, 640);
  clutter_text_set_text (CLUTTER_TEXT (text), "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

  w = mx_viewport_new ();
  clutter_container_add_actor (CLUTTER_CONTAINER (w),text);
  clutter_container_add_actor (CLUTTER_CONTAINER (scrollview), CLUTTER_ACTOR (w));

  g_signal_connect (stage, "notify::width",
                    G_CALLBACK (stage_size_notify_cb), table);
  g_signal_connect (stage, "notify::height",
                    G_CALLBACK (stage_size_notify_cb), table);
  clutter_actor_show (stage);

  clutter_main ();

  return 0;
}
