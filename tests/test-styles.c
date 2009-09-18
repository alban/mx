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
#include <stdio.h>
#include <stdlib.h>

#include <clutter/clutter.h>
#include <mx/mx.h>


static ClutterActor *stage = NULL;

static MxWidget*
create_button (ClutterActor *parent,
               const gchar  *text,
               gint          x,
               gint          y)
{
  MxWidget *button;

  button = mx_button_new_with_label (text);
  clutter_container_add_actor (CLUTTER_CONTAINER (parent),
                               CLUTTER_ACTOR (button));
  clutter_actor_set_position (CLUTTER_ACTOR (button), x, y);
  clutter_actor_set_size (CLUTTER_ACTOR (button), 150, 100);

  return button;
}

int
main (int argc, char *argv[])
{
  MxWidget *button, *table;
  ClutterColor stage_color =  { 0xff, 0xff, 0xff, 0xff };

  clutter_init (&argc, &argv);

  /* load the style sheet */
  mx_style_load_from_file (mx_style_get_default (),
                             "style/default.css", NULL);

  stage = clutter_stage_get_default ();
  clutter_stage_set_color (CLUTTER_STAGE (stage), &stage_color);

  button = create_button (stage, "Default Style", 100, 100);
  clutter_actor_set_name (CLUTTER_ACTOR (button), "default-button");

  button = create_button (stage, "Red Style", 100, 300);
  clutter_actor_set_name (CLUTTER_ACTOR (button), "red-button");

  button = create_button (stage, "Green Style", 350, 100);
  clutter_actor_set_name (CLUTTER_ACTOR (button), "green-button");

  button = create_button (stage, "Blue Style", 350, 300);
  clutter_actor_set_name (CLUTTER_ACTOR (button), "blue-button");

  table = mx_table_new ();
  clutter_actor_set_size (CLUTTER_ACTOR (table), 200, 80);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (table));
  clutter_actor_set_position (CLUTTER_ACTOR (table), 200, 215);

  button = mx_button_new_with_label ("Container Test");
  clutter_actor_set_name (CLUTTER_ACTOR (button), "container-button");
  mx_table_add_actor (MX_TABLE (table), CLUTTER_ACTOR (button), 0, 0);
  clutter_actor_show (stage);

  clutter_main ();

  return EXIT_SUCCESS;
}
