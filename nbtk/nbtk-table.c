/*
 * nbtk-table.c: Table layout widget
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
 * Written by: Thomas Wood <thomas@linux.intel.com>
 *
 */

/**
 * SECTION:nbtk-table
 * @short_description: A multi-child layout container based on rows
 * and columns
 *
 * #NbtkTable is a mult-child layout container based on a table arrangement
 * with rows and columns. #NbtkTable adds several child properties to it's
 * children that control their position and size in the table.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "nbtk-table.h"

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <clutter/clutter.h>

#include "nbtk-enum-types.h"
#include "nbtk-marshal.h"
#include "nbtk-private.h"
#include "nbtk-table-child.h"
#include "nbtk-stylable.h"

enum
{
  PROP_0,

  PROP_PADDING,

  PROP_COL_SPACING,
  PROP_ROW_SPACING,

  PROP_HOMOGENEOUS,

  PROP_ROW_COUNT,
  PROP_COL_COUNT,
};

#define NBTK_TABLE_GET_PRIVATE(obj)    \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NBTK_TYPE_TABLE, NbtkTablePrivate))

struct _NbtkTablePrivate
{
  GSList *children;

  gint col_spacing;
  gint row_spacing;

  gint n_rows;
  gint n_cols;

  gint active_row;
  gint active_col;

  GArray *min_widths;
  GArray *pref_widths;
  GArray *min_heights;
  GArray *pref_heights;

  GArray *is_expand_col;
  GArray *is_expand_row;

  GArray *col_widths;
  GArray *row_heights;

  guint homogeneous : 1;
};

static void nbtk_container_iface_init (ClutterContainerIface *iface);

G_DEFINE_TYPE_WITH_CODE (NbtkTable, nbtk_table, NBTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTAINER,
                                                nbtk_container_iface_init));



/*
 * ClutterContainer Implementation
 */
static void
nbtk_container_add_actor (ClutterContainer *container,
                          ClutterActor     *actor)
{
  NbtkTablePrivate *priv = NBTK_TABLE (container)->priv;

  clutter_actor_set_parent (actor, CLUTTER_ACTOR (container));


  priv->children = g_slist_append (priv->children, actor);

  clutter_actor_queue_relayout (CLUTTER_ACTOR (container));

  g_signal_emit_by_name (container, "actor-added", actor);
}

static void
nbtk_container_remove_actor (ClutterContainer *container,
                             ClutterActor     *actor)
{
  NbtkTablePrivate *priv = NBTK_TABLE (container)->priv;

  GSList *item = NULL;

  item = g_slist_find (priv->children, actor);

  if (item == NULL)
    {
      g_warning ("Widget of type '%s' is not a child of container of type '%s'",
                 g_type_name (G_OBJECT_TYPE (actor)),
                 g_type_name (G_OBJECT_TYPE (container)));
      return;
    }

  g_object_ref (actor);

  priv->children = g_slist_delete_link (priv->children, item);
  clutter_actor_unparent (actor);

  clutter_actor_queue_relayout (CLUTTER_ACTOR (container));

  g_signal_emit_by_name (container, "actor-removed", actor);

  g_object_unref (actor);
}

static void
nbtk_container_foreach (ClutterContainer *container,
                        ClutterCallback   callback,
                        gpointer          callback_data)
{
  NbtkTablePrivate *priv = NBTK_TABLE (container)->priv;

  g_slist_foreach (priv->children, (GFunc) callback, callback_data);
}

static void
nbtk_container_lower (ClutterContainer *container,
                      ClutterActor     *actor,
                      ClutterActor     *sibling)
{
  /* XXX: not yet implemented */
  g_warning ("%s() not yet implemented", __FUNCTION__);
}

static void
nbtk_container_raise (ClutterContainer *container,
                  ClutterActor     *actor,
                  ClutterActor     *sibling)
{
  /* XXX: not yet implemented */
  g_warning ("%s() not yet implemented", __FUNCTION__);
}

static void
nbtk_container_sort_depth_order (ClutterContainer *container)
{
  /* XXX: not yet implemented */
  g_warning ("%s() not yet implemented", __FUNCTION__);
}

static void
nbtk_container_iface_init (ClutterContainerIface *iface)
{
  iface->add = nbtk_container_add_actor;
  iface->remove = nbtk_container_remove_actor;
  iface->foreach = nbtk_container_foreach;
  iface->lower = nbtk_container_lower;
  iface->raise = nbtk_container_raise;
  iface->sort_depth_order = nbtk_container_sort_depth_order;
  iface->child_meta_type = NBTK_TYPE_TABLE_CHILD;
}

/* NbtkTable Class Implementation */

static void
nbtk_table_set_property (GObject       *gobject,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  NbtkTable *table = NBTK_TABLE (gobject);

  switch (prop_id)
    {
    case PROP_COL_SPACING:
      nbtk_table_set_col_spacing (table, g_value_get_int (value));
      break;

    case PROP_ROW_SPACING:
      nbtk_table_set_row_spacing (table, g_value_get_int (value));
      break;

    case PROP_HOMOGENEOUS:
      if (table->priv->homogeneous != g_value_get_boolean (value))
        {
          table->priv->homogeneous = g_value_get_boolean (value);
          clutter_actor_queue_relayout ((ClutterActor *)gobject);
        }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
nbtk_table_get_property (GObject    *gobject,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  NbtkTablePrivate *priv = NBTK_TABLE (gobject)->priv;

  switch (prop_id)
    {
    case PROP_COL_SPACING:
      g_value_set_int (value, priv->col_spacing);
      break;

    case PROP_ROW_SPACING:
      g_value_set_int (value, priv->row_spacing);
      break;

    case PROP_HOMOGENEOUS:
      g_value_set_boolean (value, priv->homogeneous);
      break;

    case PROP_COL_COUNT:
      g_value_set_int (value, priv->n_cols);
      break;

    case PROP_ROW_COUNT:
      g_value_set_int (value, priv->n_rows);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
nbtk_table_finalize (GObject *gobject)
{
  NbtkTablePrivate *priv = NBTK_TABLE (gobject)->priv;

  g_array_free (priv->min_widths, TRUE);
  g_array_free (priv->pref_widths, TRUE);

  g_array_free (priv->min_heights, TRUE);
  g_array_free (priv->pref_heights, TRUE);

  g_array_free (priv->is_expand_col, TRUE);
  g_array_free (priv->is_expand_row, TRUE);

  g_array_free (priv->col_widths, TRUE);
  g_array_free (priv->row_heights, TRUE);

  G_OBJECT_CLASS (nbtk_table_parent_class)->finalize (gobject);
}

static void
nbtk_table_dispose (GObject *gobject)
{
  NbtkTablePrivate *priv = NBTK_TABLE (gobject)->priv;
  GSList *l, *next;

  for (l = priv->children; l;)
    {
      next = l->next;
      clutter_container_remove_actor ((ClutterContainer *)gobject,
                                      CLUTTER_ACTOR (l->data));
      l = next;
    }

  G_OBJECT_CLASS (nbtk_table_parent_class)->dispose (gobject);
}

#define CLAMP_TO_PIXEL(x) ((float) ((int) (x)))

/* Utility function to modify a child allocation box with respect to the
 * x/y-fill child properties. Expects childbox to contain the available
 * allocation space.
 */
static void
nbtk_table_allocate_fill (ClutterActor *child,
                          ClutterActorBox *childbox,
                          gdouble x_align,
                          gdouble y_align,
                          gboolean x_fill,
                          gboolean y_fill)
{
  gfloat natural_width, natural_height;
  gfloat min_width, min_height;
  gfloat child_width, child_height;
  gfloat available_width, available_height;
  ClutterRequestMode request;
  ClutterActorBox allocation = { 0, };

  available_width  = childbox->x2 - childbox->x1;
  available_height = childbox->y2 - childbox->y1;

  if (available_width < 0)
    available_width = 0;

  if (available_height < 0)
    available_height = 0;

  if (x_fill)
    {
      allocation.x1 = childbox->x1;
      allocation.x2 = (int) (allocation.x1 + available_width);
    }

  if (y_fill)
    {
      allocation.y1 = childbox->y1;
      allocation.y2 = (int) (allocation.y1 + available_height);
    }

  /* if we are filling horizontally and vertically then we're done */
  if (x_fill && y_fill)
    {
      *childbox = allocation;
      return;
    }

  request = CLUTTER_REQUEST_HEIGHT_FOR_WIDTH;
  g_object_get (G_OBJECT (child), "request-mode", &request, NULL);

  if (request == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
    {
      clutter_actor_get_preferred_width (child, available_height,
                                         &min_width,
                                         &natural_width);

      child_width = CLAMP (natural_width, min_width, available_width);

      clutter_actor_get_preferred_height (child, child_width,
                                          &min_height,
                                          &natural_height);

      child_height = CLAMP (natural_height, min_height, available_height);
    }
  else
    {
      clutter_actor_get_preferred_height (child, available_width,
                                          &min_height,
                                          &natural_height);

      child_height = CLAMP (natural_height, min_height, available_height);

      clutter_actor_get_preferred_width (child, child_height,
                                         &min_width,
                                         &natural_width);

      child_width = CLAMP (natural_width, min_width, available_width);
    }

  if (!x_fill)
    {
      allocation.x1 = childbox->x1 + (int) ((available_width - child_width) * x_align);
      allocation.x2 = allocation.x1 + (int) child_width;
    }

  if (!y_fill)
    {
      allocation.y1 = childbox->y1 + (int) ((available_height - child_height) * y_align);
      allocation.y2 = allocation.y1 + (int) child_height;
    }

  *childbox = allocation;

}

static void
nbtk_table_homogeneous_allocate (ClutterActor          *self,
                                const ClutterActorBox *box,
                                gboolean               flags)
{
  GSList *list;
  gfloat col_width, row_height;
  gint row_spacing, col_spacing;
  NbtkTablePrivate *priv = NBTK_TABLE (self)->priv;
  NbtkPadding padding;

  nbtk_widget_get_padding (NBTK_WIDGET (self), &padding);

  col_spacing = priv->col_spacing;
  row_spacing = priv->row_spacing;

  col_width = (box->x2 - box->x1
               - padding.left - padding.right
               - (col_spacing * (priv->n_cols - 1)))
            / priv->n_cols;
  row_height = (box->y2 - box->y1
                - padding.top - padding.bottom
                - (row_spacing * (priv->n_rows - 1)))
             / priv->n_rows;

  for (list = priv->children; list; list = g_slist_next (list))
    {
      gint row, col, row_span, col_span;
      NbtkTableChild *meta;
      ClutterActor *child;
      ClutterActorBox childbox;
      gdouble x_align, y_align;
      gboolean x_fill, y_fill;

      child = CLUTTER_ACTOR (list->data);

      meta = (NbtkTableChild *) clutter_container_get_child_meta (CLUTTER_CONTAINER (self), child);

      if (!meta->allocate_hidden && !CLUTTER_ACTOR_IS_VISIBLE (child))
        continue;

      /* get child properties */
      col = meta->col;
      row = meta->row;
      row_span = meta->row_span;
      col_span = meta->col_span;
      x_align = meta->x_align;
      y_align = meta->y_align;
      x_fill = meta->x_fill;
      y_fill = meta->y_fill;

      childbox.x1 = padding.left + (col_width + col_spacing) * col;
      childbox.x2 = childbox.x1 + (col_width * col_span) + (col_spacing * (col_span - 1));

      childbox.y1 = padding.top + (row_height + row_spacing) * row;
      childbox.y2 = childbox.y1 + (row_height * row_span) + (row_spacing * (row_span - 1));

      nbtk_table_allocate_fill (child, &childbox, x_align, y_align, x_fill, y_fill);

      clutter_actor_allocate (child, &childbox, flags);
    }

}


static gint *
nbtk_table_calculate_col_widths (NbtkTable *table, gint for_width)
{
  gint total_min_width, i;
  NbtkTablePrivate *priv = table->priv;
  gboolean *is_expand_col;
  gint extra_col_width, n_expanded_cols = 0, expanded_cols = 0;
  gint *pref_widths, *min_widths;
  GSList *list;
  NbtkPadding padding;

  g_array_set_size (priv->is_expand_col, 0);
  g_array_set_size (priv->is_expand_col, priv->n_cols);
  is_expand_col = (gboolean *)priv->is_expand_col->data;

  g_array_set_size (priv->pref_widths, 0);
  g_array_set_size (priv->pref_widths, priv->n_cols);
  pref_widths = (gint *)priv->pref_widths->data;

  g_array_set_size (priv->min_widths, 0);
  g_array_set_size (priv->min_widths, priv->n_cols);
  min_widths = (gint *)priv->min_widths->data;


  /* take off the padding values to calculate the allocatable width */
  nbtk_widget_get_padding (NBTK_WIDGET (table), &padding);

  for_width -= (int) (padding.left + padding.right);

  for (list = priv->children; list; list = g_slist_next (list))
    {
      gint row, col;
      gfloat w_min, w_pref;
      gboolean x_expand;
      NbtkTableChild *meta;
      ClutterActor *child;
      gint col_span, row_span;

      child = CLUTTER_ACTOR (list->data);

      meta = (NbtkTableChild *) clutter_container_get_child_meta (CLUTTER_CONTAINER (table), child);

      if (!meta->allocate_hidden && !CLUTTER_ACTOR_IS_VISIBLE (child))
        continue;

      /* get child properties */
      col = meta->col;
      row = meta->row;
      x_expand = meta->x_expand;
      col_span = meta->col_span;
      row_span = meta->row_span;

      if (x_expand)
        is_expand_col[col] = TRUE;

      clutter_actor_get_preferred_width (child, -1, &w_min, &w_pref);
      if (col_span == 1 && w_pref > pref_widths[col])
        {
          pref_widths[col] = w_pref;
        }
      if (col_span == 1 && w_min > min_widths[col])
        {
          min_widths[col] = w_min;
        }

    }

  total_min_width = priv->col_spacing * (priv->n_cols - 1);
  for (i = 0; i < priv->n_cols; i++)
    total_min_width += pref_widths[i];

  /* calculate the remaining space and distribute it evenly onto all rows/cols
   * with the x/y expand property set. */
  for (i = 0; i < priv->n_cols; i++)
    if (is_expand_col[i])
      {
        expanded_cols += pref_widths[i];
        n_expanded_cols++;
      }

  /* for_width - total_min_width */
  extra_col_width = for_width - total_min_width;
  if (extra_col_width)
    for (i = 0; i < priv->n_cols; i++)
      if (is_expand_col[i])
       {
          if (extra_col_width < 0)
            {
              pref_widths[i] =
                MAX (min_widths[i],
                     pref_widths[i]
                    + (extra_col_width * (pref_widths[i] / (float) expanded_cols)));

              /* if we reached the minimum width for this column, we need to
               * stop counting it as expanded */
              if (pref_widths[i] == min_widths[i])
                {
                  /* restart calculations :-( */
                  expanded_cols -= pref_widths[i];
                  is_expand_col[i] = 0;
                  n_expanded_cols--;
                  i = -1;
                }
            }
          else
            pref_widths[i] += extra_col_width / n_expanded_cols;
        }

  return pref_widths;
}

static gint *
nbtk_table_calculate_row_heights (NbtkTable *table,
                                  gint       for_height,
                                  gint*      col_widths)
{
  NbtkTablePrivate *priv = NBTK_TABLE (table)->priv;
  GSList *list;
  gint *is_expand_row, *min_heights, *pref_heights, *row_heights, extra_row_height;
  gint i, total_min_height;
  gint expanded_rows = 0;
  gint n_expanded_rows = 0;
  NbtkPadding padding;

  nbtk_widget_get_padding (NBTK_WIDGET (table), &padding);

  /* take padding off available height */
  for_height -= (int) (padding.top + padding.bottom);

  g_array_set_size (priv->row_heights, 0);
  g_array_set_size (priv->row_heights, priv->n_rows);
  row_heights = (gboolean *)priv->row_heights->data;

  g_array_set_size (priv->is_expand_row, 0);
  g_array_set_size (priv->is_expand_row, priv->n_rows);
  is_expand_row = (gboolean *)priv->is_expand_row->data;

  g_array_set_size (priv->min_heights, 0);
  g_array_set_size (priv->min_heights, priv->n_rows);
  min_heights = (gboolean *)priv->min_heights->data;

  g_array_set_size (priv->pref_heights, 0);
  g_array_set_size (priv->pref_heights, priv->n_rows);
  pref_heights = (gboolean *)priv->pref_heights->data;

  /* calculate minimum row widths and column heights */
  for (list = priv->children; list; list = g_slist_next (list))
    {
      gint row, col, cell_width;
      gfloat h_min, h_pref;
      gboolean x_expand, y_expand;
      NbtkTableChild *meta;
      ClutterActor *child;
      gint col_span, row_span;

      child = CLUTTER_ACTOR (list->data);

      meta = (NbtkTableChild *) clutter_container_get_child_meta (CLUTTER_CONTAINER (table), child);

      if (!meta->allocate_hidden && !CLUTTER_ACTOR_IS_VISIBLE (child))
        continue;

      /* get child properties */
      col = meta->col;
      row = meta->row;
      x_expand = meta->x_expand;
      y_expand = meta->y_expand;
      col_span = meta->col_span;
      row_span = meta->row_span;

      if (y_expand)
        is_expand_row[row] = TRUE;

      /* calculate the cell width by including any spanned columns */
      cell_width = 0;
      for (i = 0; i < col_span; i++)
        cell_width += (float) (col_widths[col + i]);

      if (!meta->x_fill)
        {
          gfloat width;
          clutter_actor_get_preferred_width (child, -1, NULL, &width);
          cell_width = MIN (cell_width, width);
        }

      clutter_actor_get_preferred_height (child, cell_width, &h_min, &h_pref);

      if (row_span == 1 && h_pref > pref_heights[row])
        {
          pref_heights[row] = (int) (h_pref);
        }
      if (row_span == 1 && h_min > min_heights[row])
        {
          min_heights[row] = (int) (h_min);
        }
    }

  total_min_height = 0;// priv->row_spacing * (priv->n_rows - 1);
  for (i = 0; i < priv->n_rows; i++)
    total_min_height += pref_heights[i];

  /* calculate the remaining space and distribute it evenly onto all rows/cols
   * with the x/y expand property set. */
  for (i = 0; i < priv->n_rows; i++)
    if (is_expand_row[i])
      {
        expanded_rows += pref_heights[i];
        n_expanded_rows++;
      }

  /* extra row height = for height - row spacings - total_min_height */
  for_height -= (priv->row_spacing * (priv->n_rows - 1));
  extra_row_height = for_height - total_min_height;


  if (extra_row_height < 0)
    {
      gint *skip = g_slice_alloc0 (sizeof (gint) * priv->n_rows);
      gint total_shrink_height;

      /* If we need to shrink rows, we need to do multiple passes.
       *
       * We start by assuming all rows can shrink. All rows are sized
       * proportional to their height in the total table size. If a row would be
       * sized smaller than its minimum size, we mark it as non-shrinkable, and
       * reduce extra_row_height by the amount it has been shrunk. The amount
       * it has been shrunk by is the difference between the preferred and
       * minimum height, since all rows start at their preferred height. We
       * also then reduce the total table size (stored in total_shrink_height) by the height
       * of the row we are going to be skipping.
       *
       */

      /* We start by assuming all rows can shrink */
      total_shrink_height = total_min_height;
      for (i = 0; i < priv->n_rows; i++)
        {
          if (!skip[i])
            {
              gint tmp;

              /* Calculate the height of the row by starting with the preferred
               * height and taking away the extra row height proportional to
               * the preferred row height over the rows that are being shrunk
               */
              tmp = pref_heights[i]
                + (extra_row_height * (pref_heights[i] / (float) total_shrink_height));

              if (tmp < min_heights[i])
                {
                  /* This was a row we *were* set to shrink, but we now find it would have
                   * been shrunk too much. We remove it from the list of rows to shrink and
                   * adjust extra_row_height and total_shrink_height appropriately */
                  skip[i] = TRUE;
                  row_heights[i] = min_heights[i];

                  /* Reduce extra_row_height by the amount we have reduced this
                   * actor by */
                  extra_row_height += (pref_heights[i] - min_heights[i]);
                  /* now take off the row from the total shrink height */
                  total_shrink_height -= pref_heights[i];

                  /* restart the loop */
                  i = -1;
                }
              else
                {
                  skip[i] = FALSE;
                  row_heights[i] = tmp;
                }
            }

        }

      g_slice_free1 (sizeof (gint) * priv->n_rows, skip);
    }
  else
    {
      for (i = 0; i < priv->n_rows; i++)
        {
          if (is_expand_row[i])
            row_heights[i] = pref_heights[i] + (extra_row_height / n_expanded_rows);
          else
            row_heights[i] = pref_heights[i];
        }
    }


  return row_heights;
}

static void
nbtk_table_preferred_allocate (ClutterActor          *self,
                               const ClutterActorBox *box,
                               gboolean               flags)
{
  GSList *list;
  gint row_spacing, col_spacing;
  gint i, table_width, table_height;
  gint *col_widths, *row_heights;
  NbtkTable *table;
  NbtkTablePrivate *priv;
  NbtkPadding padding;

  table = NBTK_TABLE (self);
  priv = NBTK_TABLE (self)->priv;

  nbtk_widget_get_padding (NBTK_WIDGET (self), &padding);

  col_spacing = (priv->col_spacing);
  row_spacing = (priv->row_spacing);


  table_height = (int) (box->y2 - box->y1
                                       - padding.top
                                       - padding.bottom);
  table_width = (int) (box->x2 - box->x1
                                      - padding.right
                                      - padding.left);

  col_widths =
    nbtk_table_calculate_col_widths (table,
                                     (int) (box->x2 - box->x1));

  row_heights =
    nbtk_table_calculate_row_heights (table,
                                      (int) (box->y2 - box->y1),
                                      col_widths);


  for (list = priv->children; list; list = g_slist_next (list))
    {
      gint row, col, row_span, col_span;
      gint col_width, row_height;
      NbtkTableChild *meta;
      ClutterActor *child;
      ClutterActorBox childbox;
      gint child_x, child_y;
      gdouble x_align, y_align;
      gboolean x_fill, y_fill;

      child = CLUTTER_ACTOR (list->data);

      meta = (NbtkTableChild *) clutter_container_get_child_meta (CLUTTER_CONTAINER (self), child);

      if (!meta->allocate_hidden && !CLUTTER_ACTOR_IS_VISIBLE (child))
        continue;

      /* get child properties */
      col = meta->col;
      row = meta->row;
      row_span = meta->row_span;
      col_span = meta->col_span;
      x_align = meta->x_align;
      y_align = meta->y_align;
      x_fill = meta->x_fill;
      y_fill = meta->y_fill;


      /* initialise the width and height */
      col_width = col_widths[col];
      row_height = row_heights[row];

      /* Add the widths of the spanned columns:
       *
       * First check that we have a non-zero span. Then we loop over each of
       * the columns that we're spanning but we stop short if we go past the
       * number of columns in the table. This is necessary to avoid accessing
       * uninitialised memory. We add the spacing in here too since we only
       * want to add as much spacing as times we successfully span.
       */
      if (col_span > 1)
        {
          for (i = col + 1; i < col + col_span && i < priv->n_cols; i++)
            {
              col_width += col_widths[i];
              col_width += col_spacing;
            }
        }

      /* add the height of the spanned rows */
      if (row_span > 1)
        {
          for (i = row + 1; i < row + row_span && i < priv->n_rows; i++)
            {
              row_height += row_heights[i];
              row_height += row_spacing;
            }
        }

      /* calculate child x */
      child_x = (int) padding.left
              + col_spacing * col;
      for (i = 0; i < col; i++)
        child_x += col_widths[i];

      /* calculate child y */
      child_y = (int) padding.top
              + row_spacing * row;
      for (i = 0; i < row; i++)
        child_y += row_heights[i];

      /* set up childbox */
      childbox.x1 = (float) child_x;
      childbox.x2 = (float) MAX (0, child_x + col_width);

      childbox.y1 = (float) child_y;
      childbox.y2 = (float) MAX (0, child_y + row_height);


      nbtk_table_allocate_fill (child, &childbox, x_align, y_align, x_fill, y_fill);

      clutter_actor_allocate (child, &childbox, flags);
    }
}

static void
nbtk_table_allocate (ClutterActor          *self,
                     const ClutterActorBox *box,
                     ClutterAllocationFlags flags)
{
  NbtkTablePrivate *priv = NBTK_TABLE (self)->priv;

  CLUTTER_ACTOR_CLASS (nbtk_table_parent_class)->allocate (self, box, flags);

  if (priv->n_cols < 1 || priv->n_rows < 1)
    {
      return;
    };

  if (priv->homogeneous)
    nbtk_table_homogeneous_allocate (self, box, flags);
  else
    nbtk_table_preferred_allocate (self, box, flags);
}

static void
nbtk_table_get_preferred_width (ClutterActor *self,
                                gfloat        for_height,
                                gfloat       *min_width_p,
                                gfloat       *natural_width_p)
{
  gint *min_widths, *pref_widths;
  gfloat total_min_width, total_pref_width;
  NbtkTablePrivate *priv = NBTK_TABLE (self)->priv;
  GSList *list;
  gint i;
  NbtkPadding padding;

  nbtk_widget_get_padding (NBTK_WIDGET (self), &padding);

  if (priv->n_cols < 1)
  {
    *min_width_p = 0;
    *natural_width_p = 0;
    return;
  }

  /* Setting size to zero and then what we want it to be causes a clear if
   * clear flag is set (which it should be.)
   */
  g_array_set_size (priv->min_widths, 0);
  g_array_set_size (priv->pref_widths, 0);
  g_array_set_size (priv->min_widths, priv->n_cols);
  g_array_set_size (priv->pref_widths, priv->n_cols);

  min_widths = (gint *)priv->min_widths->data;
  pref_widths = (gint *)priv->pref_widths->data;

  /* calculate minimum row widths */
  for (list = priv->children; list; list = g_slist_next (list))
    {
      gint col, col_span;
      gfloat w_min, w_pref;
      NbtkTableChild *meta;
      ClutterActor *child;

      child = CLUTTER_ACTOR (list->data);

      meta = (NbtkTableChild *) clutter_container_get_child_meta (CLUTTER_CONTAINER (self), child);

      if (!meta->allocate_hidden && !CLUTTER_ACTOR_IS_VISIBLE (child))
        continue;

      /* get child properties */
      col = meta->col;
      col_span = meta->col_span;

      clutter_actor_get_preferred_width (child, -1, &w_min, &w_pref);

      if (col_span == 1 && w_min > min_widths[col])
        min_widths[col] = w_min;
      if (col_span == 1 && w_pref > pref_widths[col])
        pref_widths[col] = w_pref;
    }

  total_min_width = padding.left
                  + padding.right
                  + (priv->n_cols - 1)
                  * (float) priv->col_spacing;
  total_pref_width = total_min_width;

  for (i = 0; i < priv->n_cols; i++)
    {
      total_min_width += min_widths[i];
      total_pref_width += pref_widths[i];
    }

  if (min_width_p)
    *min_width_p = total_min_width;
  if (natural_width_p)
    *natural_width_p = total_pref_width;
}

static void
nbtk_table_get_preferred_height (ClutterActor *self,
                                 gfloat        for_width,
                                 gfloat       *min_height_p,
                                 gfloat       *natural_height_p)
{
  gint *min_heights, *pref_heights;
  gfloat total_min_height, total_pref_height;
  NbtkTablePrivate *priv = NBTK_TABLE (self)->priv;
  GSList *list;
  gint i;
  gint *min_widths;
  NbtkPadding padding;

  if (priv->n_rows < 1)
  {
    *min_height_p = 0;
    *natural_height_p = 0;
    return;
  }

  /* Setting size to zero and then what we want it to be causes a clear if
   * clear flag is set (which it should be.)
   */
  g_array_set_size (priv->min_heights, 0);
  g_array_set_size (priv->pref_heights, 0);
  g_array_set_size (priv->min_heights, priv->n_rows);
  g_array_set_size (priv->pref_heights, priv->n_rows);

  /* use min_widths to help allocation of height-for-width widgets */
  min_widths = nbtk_table_calculate_col_widths (NBTK_TABLE (self), for_width);

  min_heights = (gint *)priv->min_heights->data;
  pref_heights = (gint *)priv->pref_heights->data;

  /* calculate minimum row heights */
  for (list = priv->children; list; list = g_slist_next (list))
    {
      gint row, col, col_span, cell_width, row_span;
      gfloat min, pref;
      NbtkTableChild *meta;
      ClutterActor *child;

      child = CLUTTER_ACTOR (list->data);

      meta = (NbtkTableChild *) clutter_container_get_child_meta (CLUTTER_CONTAINER (self), child);

      if (!meta->allocate_hidden && !CLUTTER_ACTOR_IS_VISIBLE (child))
        continue;

      /* get child properties */
      row = meta->row;
      col = meta->col;
      col_span = meta->col_span;
      row_span = meta->row_span;

      cell_width = 0;
      for (i = 0; i < col_span; i++)
        cell_width += min_widths[col + i];


      clutter_actor_get_preferred_height (child,
              (float) cell_width, &min, &pref);

      if (row_span == 1 && min > min_heights[row])
        min_heights[row] = min;
      if (row_span == 1 && pref > pref_heights[row])
        pref_heights[row] = pref;
    }

  nbtk_widget_get_padding (NBTK_WIDGET (self), &padding);

  /* start off with padding plus row spacing */
  total_min_height = padding.top + padding.bottom + (priv->n_rows - 1) *
                     (float) (priv->row_spacing);

  total_pref_height = total_min_height;

  for (i = 0; i < priv->n_rows; i++)
    {
      total_min_height += min_heights[i];
      total_pref_height += pref_heights[i];
    }

  if (min_height_p)
    *min_height_p = total_min_height;
  if (natural_height_p)
    *natural_height_p = total_pref_height;
}

static void
nbtk_table_paint (ClutterActor *self)
{
  NbtkTablePrivate *priv = NBTK_TABLE (self)->priv;
  GSList *list;

  /* make sure the background gets painted first */
  CLUTTER_ACTOR_CLASS (nbtk_table_parent_class)->paint (self);

  for (list = priv->children; list; list = g_slist_next (list))
    {
      ClutterActor *child = CLUTTER_ACTOR (list->data);
      if (CLUTTER_ACTOR_IS_VISIBLE (child))
        clutter_actor_paint (child);
    }
}

static void
nbtk_table_pick (ClutterActor       *self,
                 const ClutterColor *color)
{
  NbtkTablePrivate *priv = NBTK_TABLE (self)->priv;
  GSList *list;

  /* Chain up so we get a bounding box painted (if we are reactive) */
  CLUTTER_ACTOR_CLASS (nbtk_table_parent_class)->pick (self, color);

  for (list = priv->children; list; list = g_slist_next (list))
    {
      if (CLUTTER_ACTOR_IS_VISIBLE (list->data))
        clutter_actor_paint (CLUTTER_ACTOR (list->data));
    }
}

static void
nbtk_table_show_all (ClutterActor *table)
{
  NbtkTablePrivate *priv = NBTK_TABLE (table)->priv;
  GSList *l;

  for (l = priv->children; l; l = l->next)
    clutter_actor_show_all (CLUTTER_ACTOR (l->data));

  clutter_actor_show (table);
}

static void
nbtk_table_hide_all (ClutterActor *table)
{
  NbtkTablePrivate *priv = NBTK_TABLE (table)->priv;
  GSList *l;

  clutter_actor_hide (table);

  for (l = priv->children; l; l = l->next)
    clutter_actor_hide_all (CLUTTER_ACTOR (l->data));
}

static void
nbtk_table_class_init (NbtkTableClass *klass)
{
  GParamSpec *pspec;
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  /* NbtkWidgetClass *nbtk_widget_class = NBTK_WIDGET_CLASS (klass); */

  g_type_class_add_private (klass, sizeof (NbtkTablePrivate));

  gobject_class->set_property = nbtk_table_set_property;
  gobject_class->get_property = nbtk_table_get_property;
  gobject_class->dispose = nbtk_table_dispose;
  gobject_class->finalize = nbtk_table_finalize;

  actor_class->paint = nbtk_table_paint;
  actor_class->pick = nbtk_table_pick;
  actor_class->allocate = nbtk_table_allocate;
  actor_class->get_preferred_width = nbtk_table_get_preferred_width;
  actor_class->get_preferred_height = nbtk_table_get_preferred_height;
  actor_class->show_all = nbtk_table_show_all;
  actor_class->hide_all = nbtk_table_hide_all;

  pspec = g_param_spec_boolean ("homogeneous",
                                "Homogeneous",
                                "Homogeneous rows and columns",
                                TRUE,
                                NBTK_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_HOMOGENEOUS,
                                   pspec);

  pspec = g_param_spec_int ("col-spacing",
                            "Column Spacing",
                            "Spacing between columns",
                            0, G_MAXINT, 0,
                            NBTK_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_COL_SPACING,
                                   pspec);

  pspec = g_param_spec_int ("row-spacing",
                            "Row Spacing",
                            "Spacing between row",
                            0, G_MAXINT, 0,
                            NBTK_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_ROW_SPACING,
                                   pspec);

  pspec = g_param_spec_int ("row-count",
                            "Row Count",
                            "The number of rows in the table",
                            0, G_MAXINT, 0,
                            NBTK_PARAM_READABLE);
  g_object_class_install_property (gobject_class,
                                   PROP_ROW_COUNT,
                                   pspec);

  pspec = g_param_spec_int ("column-count",
                            "Column Count",
                            "The number of columns in the table",
                            0, G_MAXINT, 0,
                            NBTK_PARAM_READABLE);
  g_object_class_install_property (gobject_class,
                                   PROP_COL_COUNT,
                                   pspec);
}

static void
nbtk_table_init (NbtkTable *table)
{
  table->priv = NBTK_TABLE_GET_PRIVATE (table);

  table->priv->n_cols = 0;
  table->priv->n_rows = 0;

  table->priv->min_widths = g_array_new (FALSE,
                                         TRUE,
                                         sizeof (gint));
  table->priv->pref_widths = g_array_new (FALSE,
                                          TRUE,
                                          sizeof (gint));
  table->priv->min_heights = g_array_new (FALSE,
                                          TRUE,
                                          sizeof (gint));
  table->priv->pref_heights = g_array_new (FALSE,
                                           TRUE,
                                           sizeof (gint));

  table->priv->is_expand_col = g_array_new (FALSE,
                                            TRUE,
                                            sizeof (gboolean));
  table->priv->is_expand_row = g_array_new (FALSE,
                                            TRUE,
                                            sizeof (gboolean));

  table->priv->col_widths = g_array_new (FALSE,
                                         TRUE,
                                         sizeof (gint));
  table->priv->row_heights = g_array_new (FALSE,
                                          TRUE,
                                          sizeof (gint));
}

/* used by NbtkTableChild to update row/column count */
void _nbtk_table_update_row_col (NbtkTable *table,
                                 gint       row,
                                 gint       col)
{
  if (col > -1)
    table->priv->n_cols = MAX (table->priv->n_cols, col + 1);

  if (row > -1)
    table->priv->n_rows = MAX (table->priv->n_rows, row + 1);

}

/*** Public Functions ***/

/**
 * nbtk_table_new:
 *
 * Create a new #NbtkTable
 *
 * Returns: a new #NbtkTable
 */
NbtkWidget*
nbtk_table_new (void)
{
  return g_object_new (NBTK_TYPE_TABLE, NULL);
}

/**
 * nbtk_table_set_col_spacing
 * @table: a #NbtkTable
 * @spacing: spacing in pixels
 *
 * Sets the amount of spacing between columns.
 */
void
nbtk_table_set_col_spacing (NbtkTable *table,
                            gint       spacing)
{
  NbtkTablePrivate *priv;

  g_return_if_fail (NBTK_IS_TABLE (table));
  g_return_if_fail (spacing >= 0);

  priv = NBTK_TABLE (table)->priv;

  priv->col_spacing = spacing;
}

/**
 * nbtk_table_set_row_spacing
 * @table: a #NbtkTable
 * @spacing: spacing in pixels
 *
 * Sets the amount of spacing between rows.
 */
void
nbtk_table_set_row_spacing (NbtkTable *table,
                            gint       spacing)
{
  NbtkTablePrivate *priv;

  g_return_if_fail (NBTK_IS_TABLE (table));
  g_return_if_fail (spacing >= 0);

  priv = NBTK_TABLE (table)->priv;

  priv->row_spacing = spacing;
}

/**
 * nbtk_table_get_row_spacing
 * @table: a #NbtkTable
 *
 * Gets the amount of spacing between rows.
 *
 * Returns: the spacing between rows in device units
 */
gint
nbtk_table_get_row_spacing (NbtkTable *table)
{
  NbtkTablePrivate *priv;

  g_return_val_if_fail (NBTK_IS_TABLE (table), -1);
  priv = NBTK_TABLE (table)->priv;

  return priv->row_spacing;
}

/**
 * nbtk_table_get_col_spacing
 * @table: a #NbtkTable
 *
 * Gets the amount of spacing between columns.
 *
 * Returns: the spacing between columns in device units
 */
gint
nbtk_table_get_col_spacing (NbtkTable *table)
{
  NbtkTablePrivate *priv;

  g_return_val_if_fail (NBTK_IS_TABLE (table), -1);
  priv = NBTK_TABLE (table)->priv;

  return priv->col_spacing;
}

/**
 * nbtk_table_add_actor:
 * @table: a #NbtkTable
 * @actor: the child to insert
 * @row: the row to place the child into
 * @column: the column to place the child into
 *
 * Add an actor at the specified row and column
 *
 * Note, column and rows numbers start from zero
 */
void
nbtk_table_add_actor (NbtkTable    *table,
                      ClutterActor *actor,
                      gint          row,
                      gint          column)
{
  g_return_if_fail (NBTK_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));
  g_return_if_fail (row >= -1);
  g_return_if_fail (column >= -1);

  if (row < 0)
    row = table->priv->n_rows + 1;

  if (column < 0)
    column = table->priv->n_cols + 1;

  clutter_container_add_actor (CLUTTER_CONTAINER (table), actor);
  clutter_container_child_set (CLUTTER_CONTAINER (table), actor,
                               "row", row,
                               "col", column,
                               NULL);

}

/**
 * nbtk_table_add_actor_with_properties
 * @table: a #NbtkTable
 * @actor: the child #ClutterActor
 * @row: the row to place the child into
 * @column: the column to place the child into
 * @first_property_name: name of the first property to set
 * @...: value for the first property, followed optionally by more name/value pairs terminated with NULL.
 *
 * Add an actor into at the specified row and column, with additional child
 * properties to set.
 */
void
nbtk_table_add_actor_with_properties (NbtkTable    *table,
                                      ClutterActor *actor,
                                      gint          row,
                                      gint          column,
                                      const gchar  *first_property_name,
                                      ...)
{
  va_list args;
  NbtkTableChild *meta;
  ClutterContainer *container;

  g_return_if_fail (NBTK_IS_TABLE (table));
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));
  g_return_if_fail (row >= -1);
  g_return_if_fail (column >= -1);
  g_return_if_fail (first_property_name != NULL);

  if (row < 0)
    row = table->priv->n_rows + 1;

  if (column < 0)
    column = table->priv->n_cols + 1;

  container = (ClutterContainer *)table;
  clutter_container_add_actor (container, actor);
  clutter_container_child_set (CLUTTER_CONTAINER (table), actor,
                               "row", row,
                               "col", column,
                               NULL);
  meta = (NbtkTableChild*) clutter_container_get_child_meta (container,
                                                             actor);

  va_start (args, first_property_name);
  g_object_set_valist ((GObject*) meta, first_property_name, args);
  va_end (args);
}

/**
 * nbtk_table_get_row_count:
 * @table: A #NbtkTable
 *
 * Retrieve the current number rows in the @table
 *
 * Returns: the number of rows
 */
gint
nbtk_table_get_row_count (NbtkTable *table)
{
  g_return_val_if_fail (NBTK_IS_TABLE (table), -1);

  return NBTK_TABLE (table)->priv->n_rows;
}

/**
 * nbtk_table_get_column_count:
 * @table: A #NbtkTable
 *
 * Retrieve the current number of columns in @table
 *
 * Returns: the number of columns
 */
gint
nbtk_table_get_column_count (NbtkTable *table)
{
  g_return_val_if_fail (NBTK_IS_TABLE (table), -1);

  return NBTK_TABLE (table)->priv->n_cols;
}
