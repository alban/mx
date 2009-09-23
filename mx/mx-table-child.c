/*
 * mx-table-child.h: Table child implementation
 *
 * Copyright 2008, 2009 Intel Corporation.
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
 * Written by: Thomas Wood  <thomas.wood@intel.com>
 *
 */

#include "mx-table-child.h"
#include "mx-private.h"
#include <mx/mx-widget.h>
#include <mx/mx-table.h>

/*
 * ClutterChildMeta Implementation
 */

/**
 * SECTION:mx-table-child
 * @short_description: The child property store for #MxTable
 *
 * The #ClutterChildMeta implementation for the #MxTable container widget.
 *
 */

enum {
  CHILD_PROP_0,

  CHILD_PROP_COL,
  CHILD_PROP_ROW,
  CHILD_PROP_COL_SPAN,
  CHILD_PROP_ROW_SPAN,
  CHILD_PROP_X_EXPAND,
  CHILD_PROP_Y_EXPAND,
  CHILD_PROP_X_ALIGN,
  CHILD_PROP_Y_ALIGN,
  CHILD_PROP_X_FILL,
  CHILD_PROP_Y_FILL,
  CHILD_PROP_ALLOCATE_HIDDEN,
};

G_DEFINE_TYPE (MxTableChild, mx_table_child, CLUTTER_TYPE_CHILD_META);

static void
table_child_set_property (GObject      *gobject,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  MxTableChild *child = MX_TABLE_CHILD (gobject);
  MxTable *table = MX_TABLE (CLUTTER_CHILD_META(gobject)->container);

  switch (prop_id)
    {
    case CHILD_PROP_COL:
      child->col = g_value_get_int (value);
      _mx_table_update_row_col (table, -1, child->col);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_ROW:
      child->row = g_value_get_int (value);
      _mx_table_update_row_col (table, child->row, -1);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_COL_SPAN:
      child->col_span = g_value_get_int (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_ROW_SPAN:
      child->row_span = g_value_get_int (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_X_EXPAND:
      child->x_expand = g_value_get_boolean (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_Y_EXPAND:
      child->y_expand = g_value_get_boolean (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_X_ALIGN:
      child->x_align = g_value_get_double (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_Y_ALIGN:
      child->y_align = g_value_get_double (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_X_FILL:
      child->x_fill = g_value_get_boolean (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_Y_FILL:
      child->y_fill = g_value_get_boolean (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;
    case CHILD_PROP_ALLOCATE_HIDDEN:
      child->allocate_hidden = g_value_get_boolean (value);
      clutter_actor_queue_relayout (CLUTTER_ACTOR (table));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
table_child_get_property (GObject    *gobject,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  MxTableChild *child = MX_TABLE_CHILD (gobject);

  switch (prop_id)
    {
    case CHILD_PROP_COL:
      g_value_set_int (value, child->col);
      break;
    case CHILD_PROP_ROW:
      g_value_set_int (value, child->row);
      break;
    case CHILD_PROP_COL_SPAN:
      g_value_set_int (value, child->col_span);
      break;
    case CHILD_PROP_ROW_SPAN:
      g_value_set_int (value, child->row_span);
      break;
    case CHILD_PROP_X_EXPAND:
      g_value_set_boolean (value, child->x_expand);
      break;
    case CHILD_PROP_Y_EXPAND:
      g_value_set_boolean (value, child->y_expand);
      break;
    case CHILD_PROP_X_ALIGN:
      g_value_set_double (value, child->x_align);
      break;
    case CHILD_PROP_Y_ALIGN:
      g_value_set_double (value, child->y_align);
      break;
    case CHILD_PROP_X_FILL:
      g_value_set_boolean (value, child->x_fill);
      break;
    case CHILD_PROP_Y_FILL:
      g_value_set_boolean (value, child->y_fill);
      break;
    case CHILD_PROP_ALLOCATE_HIDDEN:
      g_value_set_boolean (value, child->allocate_hidden);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
mx_table_child_class_init (MxTableChildClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  gobject_class->set_property = table_child_set_property;
  gobject_class->get_property = table_child_get_property;

  pspec = g_param_spec_int ("col",
                            "Column Number",
                            "The column the widget resides in",
                            0, G_MAXINT,
                            0,
                            MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_COL, pspec);

  pspec = g_param_spec_int ("row",
                            "Row Number",
                            "The row the widget resides in",
                            0, G_MAXINT,
                            0,
                            MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_ROW, pspec);

  pspec = g_param_spec_int ("row-span",
                            "Row Span",
                            "The number of rows the widget should span",
                            1, G_MAXINT,
                            1,
                            MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_ROW_SPAN, pspec);

  pspec = g_param_spec_int ("col-span",
                            "Column Span",
                            "The number of columns the widget should span",
                            1, G_MAXINT,
                            1,
                            MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_COL_SPAN, pspec);

  pspec = g_param_spec_boolean ("x-expand",
                                "X Expand",
                                "Whether the child should receive priority "
                                "when the container is allocating spare space "
                                "on the horizontal axis",
                                TRUE,
                                MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_X_EXPAND, pspec);

  pspec = g_param_spec_boolean ("y-expand",
                                "Y Expand",
                                "Whether the child should receive priority "
                                "when the container is allocating spare space "
                                "on the vertical axis",
                                TRUE,
                                MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_Y_EXPAND, pspec);

  pspec = g_param_spec_double ("x-align",
                               "X Alignment",
                               "X alignment of the widget within the cell",
                               0, 1,
                               0.5,
                               MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_X_ALIGN, pspec);

  pspec = g_param_spec_double ("y-align",
                               "Y Alignment",
                               "Y alignment of the widget within the cell",
                               0, 1,
                               0.5,
                               MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_Y_ALIGN, pspec);

  pspec = g_param_spec_boolean ("x-fill",
                                "X Fill",
                                "Whether the child should be allocated its "
                                "entire available space, or whether it should "
                                "be squashed and aligned.",
                                TRUE,
                                MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_X_FILL, pspec);

  pspec = g_param_spec_boolean ("y-fill",
                                "Y Fill",
                                "Whether the child should be allocated its "
                                "entire available space, or whether it should "
                                "be squashed and aligned.",
                                TRUE,
                                MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_Y_FILL, pspec);

  pspec = g_param_spec_boolean ("allocate-hidden",
                                "Allocate Hidden",
                                "Whether the child should be allocate even "
                                "if it is hidden",
                                TRUE,
                                MX_PARAM_READWRITE);

  g_object_class_install_property (gobject_class, CHILD_PROP_ALLOCATE_HIDDEN, pspec);
}

static void
mx_table_child_init (MxTableChild *self)
{
  self->col_span = 1;
  self->row_span = 1;

  self->x_align = 0.5;
  self->y_align = 0.5;

  self->x_expand = TRUE;
  self->y_expand = TRUE;

  self->x_fill = TRUE;
  self->y_fill = TRUE;

  self->allocate_hidden = TRUE;
}

static MxTableChild*
get_child_meta (MxTable    *table,
                 ClutterActor *child)
{
  MxTableChild *meta;

  meta = (MxTableChild*) clutter_container_get_child_meta (CLUTTER_CONTAINER (table), child);

  return meta;
}

/**
 * mx_table_child_get_col_span:
 * @table: an #MxTable
 * @child: a #ClutterActor
 *
 * Get the column span of the child. Defaults to 1.
 *
 * Returns: the column span of the child
 */
gint
mx_table_child_get_col_span (MxTable    *table,
                               ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  return meta->col_span;
}

/**
 * mx_table_child_set_col_span:
 * @table: An #MxTable
 * @child: An #ClutterActor
 * @span: The number of columns to span
 *
 * Set the column span of the child.
 *
 */
void
mx_table_child_set_col_span (MxTable    *table,
                               ClutterActor *child,
                               gint          span)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));
  g_return_if_fail (span > 1);

  meta = get_child_meta (table, child);

  meta->col_span = span;

  clutter_actor_queue_relayout (child);
}

/**
 * mx_table_child_get_row_span:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Get the row span of the child. Defaults to 1.
 *
 * Returns: the row span of the child
 */
gint
mx_table_child_get_row_span (MxTable    *table,
                               ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  return meta->row_span;
}

/**
 * mx_table_child_set_row_span:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @span: the number of rows to span
 *
 * Set the row span of the child.
 *
 */
void
mx_table_child_set_row_span (MxTable    *table,
                               ClutterActor *child,
                               gint          span)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));
  g_return_if_fail (span > 1);

  meta = get_child_meta (table, child);

  meta->row_span = span;

  clutter_actor_queue_relayout (child);
}

/**
 * mx_table_child_get_x_fill:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Get the x-fill state of the child
 *
 * Returns: #TRUE if the child is set to x-fill
 */
gboolean
mx_table_child_get_x_fill (MxTable    *table,
                             ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  return meta->x_fill;
}

/**
 * mx_table_child_set_x_fill:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @fill: the fill state
 *
 * Set the fill state of the child on the x-axis. This will cause the child to
 * be allocated the maximum available space.
 *
 */
void
mx_table_child_set_x_fill (MxTable    *table,
                             ClutterActor *child,
                             gboolean      fill)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));

  meta = get_child_meta (table, child);

  meta->x_fill = fill;

  clutter_actor_queue_relayout (child);
}


/**
 * mx_table_child_get_y_fill:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Get the y-fill state of the child
 *
 * Returns: #TRUE if the child is set to y-fill
 */
gboolean
mx_table_child_get_y_fill (MxTable    *table,
                             ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  return meta->y_fill;
}

/**
 * mx_table_child_set_y_fill:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @fill: the fill state
 *
 * Set the fill state of the child on the y-axis. This will cause the child to
 * be allocated the maximum available space.
 *
 */
void
mx_table_child_set_y_fill (MxTable    *table,
                             ClutterActor *child,
                             gboolean      fill)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));

  meta = get_child_meta (table, child);

  meta->y_fill = fill;

  clutter_actor_queue_relayout (child);
}

/**
 * mx_table_child_get_x_expand:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Get the x-expand property of the child
 *
 * Returns: #TRUE if the child is set to x-expand
 */
gboolean
mx_table_child_get_x_expand (MxTable    *table,
                               ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  return meta->x_expand;
}

/**
 * mx_table_child_set_x_expand:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @expand: the new value of the x expand child property
 *
 * Set x-expand on the child. This causes the column which the child
 * resides in to be allocated any extra space if the allocation of the table is
 * larger than the preferred size.
 *
 */
void
mx_table_child_set_x_expand (MxTable    *table,
                               ClutterActor *child,
                               gboolean      expand)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));

  meta = get_child_meta (table, child);

  meta->x_expand = expand;

  clutter_actor_queue_relayout (child);
}

/**
 * mx_table_child_set_y_expand:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @expand: the new value of the y-expand child property
 *
 * Set y-expand on the child. This causes the row which the child
 * resides in to be allocated any extra space if the allocation of the table is
 * larger than the preferred size.
 *
 */
void
mx_table_child_set_y_expand (MxTable    *table,
                               ClutterActor *child,
                               gboolean      expand)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));

  meta = get_child_meta (table, child);

  meta->y_expand = expand;

  clutter_actor_queue_relayout (child);
}

/**
 * mx_table_child_get_y_expand:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Get the y-expand property of the child.
 *
 * Returns: #TRUE if the child is set to y-expand
 */
gboolean
mx_table_child_get_y_expand (MxTable    *table,
                                 ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  return meta->y_expand;
}

/**
 * mx_table_child_get_x_align:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Get the x-align value of the child
 *
 * Returns: An #MxAlign value
 */
MxAlign
mx_table_child_get_x_align (MxTable    *table,
                              ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  if (meta->x_align == 0.0)
    return MX_ALIGN_START;
  else if (meta->x_align == 1.0)
    return MX_ALIGN_END;
  else
    return MX_ALIGN_MIDDLE;

}

/**
 * mx_table_child_set_x_align:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @align: A #MxAlign value
 *
 * Set the alignment of the child within its cell. This will only have an effect
 * if the the x-fill property is FALSE.
 *
 */
void
mx_table_child_set_x_align (MxTable    *table,
                              ClutterActor *child,
                              MxAlign     align)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));

  meta = get_child_meta (table, child);

  switch (align)
    {
    case MX_ALIGN_START:
      meta->x_align = 0.0;
      break;
    case MX_ALIGN_MIDDLE:
      meta->x_align = 0.5;
      break;
    case MX_ALIGN_END:
      meta->x_align = 1.0;
      break;
    }

  clutter_actor_queue_relayout (child);
}

/**
 * mx_table_child_get_y_align:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Get the y-align value of the child
 *
 * Returns: An #MxAlign value
 */
MxAlign
mx_table_child_get_y_align (MxTable    *table,
                              ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), 0);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), 0);

  meta = get_child_meta (table, child);

  if (meta->y_align == 0.0)
    return MX_ALIGN_START;
  else if (meta->y_align == 1.0)
    return MX_ALIGN_END;
  else
    return MX_ALIGN_MIDDLE;

}

/**
 * mx_table_child_set_y_align:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @align: A #MxAlign value
 *
 * Set the value of the y-align property. This will only have an effect if
 * y-fill value is set to FALSE.
 *
 */
void
mx_table_child_set_y_align (MxTable    *table,
                              ClutterActor *child,
                              MxAlign     align)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));

  meta = get_child_meta (table, child);

  switch (align)
    {
    case MX_ALIGN_START:
      meta->y_align = 0.0;
      break;
    case MX_ALIGN_MIDDLE:
      meta->y_align = 0.5;
      break;
    case MX_ALIGN_END:
      meta->y_align = 1.0;
      break;
    }

  clutter_actor_queue_relayout (child);
}

/**
 * mx_table_child_set_allocate_hidden:
 * @table: A #MxTable
 * @child: A #ClutterActor
 * @value: #TRUE if the actor should be allocated when hidden
 *
 * Set whether the child should be allocate even if it is hidden
 */
void
mx_table_child_set_allocate_hidden (MxTable    *table,
                                      ClutterActor *child,
                                      gboolean      value)
{
  MxTableChild *meta;

  g_return_if_fail (MX_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));

  meta = get_child_meta (table, child);

  if (meta->allocate_hidden != value)
    {
      meta->allocate_hidden = value;

      clutter_actor_queue_relayout (child);

      g_object_notify (G_OBJECT (meta), "allocate-hidden");
    }
}

/**
 * mx_table_child_get_allocate_hidden:
 * @table: A #MxTable
 * @child: A #ClutterActor
 *
 * Determine if the child is allocated even if it is hidden
 *
 * Returns: #TRUE if the actor is allocated when hidden
 */
gboolean
mx_table_child_get_allocate_hidden (MxTable    *table,
                                      ClutterActor *child)
{
  MxTableChild *meta;

  g_return_val_if_fail (MX_IS_TABLE (table), TRUE);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), TRUE);

  meta = get_child_meta (table, child);

  return meta->allocate_hidden;
}