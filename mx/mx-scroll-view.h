/*
 * mx-scroll-view.h: Container with scroll-bars
 *
 * Copyright 2008 OpenedHand
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
 * Written by: Chris Lord <chris@openedhand.com>
 * Port to Mx by: Robert Staudinger <robsta@openedhand.com>
 *
 */

#if !defined(MX_H_INSIDE) && !defined(MX_COMPILATION)
#error "Only <mx/mx.h> can be included directly.h"
#endif

#ifndef __MX_SCROLL_VIEW_H__
#define __MX_SCROLL_VIEW_H__

#include <mx/mx-bin.h>

G_BEGIN_DECLS

#define MX_TYPE_SCROLL_VIEW            (mx_scroll_view_get_type())
#define MX_SCROLL_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MX_TYPE_SCROLL_VIEW, MxScrollView))
#define MX_IS_SCROLL_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MX_TYPE_SCROLL_VIEW))
#define MX_SCROLL_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MX_TYPE_SCROLL_VIEW, MxScrollViewClass))
#define MX_IS_SCROLL_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MX_TYPE_SCROLL_VIEW))
#define MX_SCROLL_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MX_TYPE_SCROLL_VIEW, MxScrollViewClass))

typedef struct _MxScrollView          MxScrollView;
typedef struct _MxScrollViewPrivate   MxScrollViewPrivate;
typedef struct _MxScrollViewClass     MxScrollViewClass;

/**
 * MxScrollView:
 *
 * The contents of this structure are private and should only be accessed
 * through the public API.
 */
struct _MxScrollView
{
  /*< private >*/
  MxBin parent_instance;

  MxScrollViewPrivate *priv;
};

struct _MxScrollViewClass
{
  MxBinClass parent_class;
};

GType mx_scroll_view_get_type (void) G_GNUC_CONST;

MxWidget *  mx_scroll_view_new             (void);

ClutterActor *  mx_scroll_view_get_hscroll_bar (MxScrollView *scroll);
ClutterActor *  mx_scroll_view_get_vscroll_bar (MxScrollView *scroll);
ClutterActor *  mx_scroll_view_get_child       (MxScrollView *scroll);

gfloat     mx_scroll_view_get_column_size (MxScrollView *scroll);
void            mx_scroll_view_set_column_size (MxScrollView *scroll,
                                                  gfloat          column_size);

gfloat     mx_scroll_view_get_row_size    (MxScrollView *scroll);
void            mx_scroll_view_set_row_size    (MxScrollView *scroll,
                                                  gfloat          row_size);

void mx_scroll_view_set_mouse_scrolling (MxScrollView *scroll, gboolean enabled);
gboolean mx_scroll_view_get_mouse_scrolling (MxScrollView *scroll);

G_END_DECLS

#endif /* __MX_SCROLL_VIEW_H__ */
