/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2000 Ales V. Hvezda
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */
#include <config.h>

#include <stdio.h>
#include <math.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libgeda/libgeda.h>

#include "../include/globals.h"
#include "../include/prototype.h"

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_draw(TOPLEVEL *w_current, OBJECT *o_current)
{
  int left, right, top, bottom;

  if (!w_current->DONT_REDRAW) {
    o_redraw(w_current, o_current->complex->prim_objs);
  }

  get_complex_bounds(w_current, o_current->complex->prim_objs,
                     &left, &top, &right, &bottom);
  o_current->left   = left;
  o_current->top    = top;
  o_current->right  = right;
  o_current->bottom = bottom;

  WORLDtoSCREEN(w_current,
                o_current->complex->x,
                o_current->complex->y,
                &o_current->complex->screen_x,
                &o_current->complex->screen_y);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_erase(TOPLEVEL *w_current, OBJECT *o_current)
{
  w_current->override_color = w_current->background_color;
  o_complex_draw(w_current, o_current);
  w_current->override_color = -1;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_draw_xor(TOPLEVEL *w_current, int dx, int dy, OBJECT *complex)
{
  OBJECT *o_current = complex;

  while(o_current != NULL) {
    switch(o_current->type) {
      case(OBJ_LINE):
        o_line_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_NET):
        o_net_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_BUS):
        o_bus_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_BOX):
        o_box_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_PICTURE):
        o_picture_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_CIRCLE):
        o_circle_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_COMPLEX):
      case(OBJ_PLACEHOLDER):
        o_complex_draw_xor(w_current, dx, dy,
                           o_current->complex->prim_objs);
        break;

      case(OBJ_TEXT):
        o_text_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_PIN):
        o_pin_draw_xor(w_current, dx, dy, o_current);
        break;

      case(OBJ_ARC):
        o_arc_draw_xor(w_current, dx, dy, o_current);
        break;
    }
    o_current = o_current->next;
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_start(TOPLEVEL *w_current, int screen_x, int screen_y)
{
  int x, y;
  int i, temp;

  w_current->last_x = w_current->start_x = fix_x(w_current, screen_x);
  w_current->last_y = w_current->start_y = fix_y(w_current, screen_y);

  w_current->last_drawb_mode = -1;

  /* make sure list is null first, so that you don't have a mem
   * leak */
  SCREENtoWORLD(w_current,
                w_current->start_x,
                w_current->start_y,
                &x,
                &y);

  w_current->DONT_DRAW_CONN = 1;
  w_current->ADDING_SEL = 1; /* reuse this flag, rename later hack */
  w_current->page_current->complex_place_tail =
  (OBJECT *) o_complex_add(
                           w_current,
                           w_current->page_current->complex_place_head,
                           OBJ_COMPLEX, WHITE, x, y, 0, 0,
                           w_current->internal_clib,
                           w_current->internal_basename, 1, TRUE);
  w_current->ADDING_SEL = 0;
  w_current->DONT_DRAW_CONN = 0;

  if (w_current->complex_rotate) {
    temp = w_current->complex_rotate / 90;
    for (i = 0; i < temp; i++) {
      o_complex_place_rotate(w_current);
    }
  }

  o_drawbounding(w_current, 
                 w_current->page_current->complex_place_head->next,
                 NULL,
                 x_get_darkcolor(w_current->bb_color), TRUE);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_place_rotate(TOPLEVEL *w_current)
{
  OBJECT *o_current;
  int screen_x_local = -1;
  int screen_y_local = -1;
  int new_angle;

  o_current = w_current->page_current->complex_place_head->next;
  while(o_current) {
    switch(o_current->type) {	
      case(OBJ_COMPLEX):
        screen_x_local = o_current->complex->screen_x; 
        screen_y_local = o_current->complex->screen_y;
        break;
    }
    o_current = o_current->next;
  }

  if (screen_x_local == -1) {
    printf(_("Could not find complex in new componet placement!\n"));
    return;
  }

  o_current = w_current->page_current->complex_place_head->next;
  while(o_current) {
    switch(o_current->type) {	

      case(OBJ_TEXT):
        new_angle = (o_current->text->angle + 90) % 360;
        o_text_rotate(w_current, screen_x_local, screen_y_local,
                      new_angle, 90, o_current);
        break;

      case(OBJ_COMPLEX):
        new_angle = (o_current->complex->angle + 90) % 360;
        o_complex_rotate(w_current, screen_x_local, screen_y_local,
                         new_angle, 90, o_current);
        break;

    }
    o_current = o_current->next;
  }

}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_end(TOPLEVEL *w_current, int screen_x, int screen_y)
{
  int diff_x, diff_y;
  int x, y;
  int rleft, rtop, rbottom, rright;
  OBJECT *o_current;
  OBJECT *o_start;
  OBJECT *o_temp;
  char *include_filename;
  int temp, new_angle, i;
  GList *connected_objects=NULL;

  diff_x = w_current->last_x - w_current->start_x;
  diff_y = w_current->last_y - w_current->start_y;

  SCREENtoWORLD(w_current, screen_x, screen_y, &x, &y);

#if 0
  x = snap_grid(w_current, x);
  y = snap_grid(w_current, y);
#endif

#if DEBUG
  printf("place_basename: %s\n",internal_basename);
  printf("place_clib: %s\n",internal_clib);
#endif

  if (w_current->include_complex) {
    include_filename = g_strconcat (w_current->internal_clib,
                                    G_DIR_SEPARATOR_S,
                                    w_current->internal_basename,
                                    NULL);

    w_current->ADDING_SEL=1;
    o_start = w_current->page_current->object_tail;
    w_current->page_current->object_tail =
      o_read(w_current,
             w_current->page_current->object_tail,
             include_filename);
    o_start = o_start->next;
    w_current->ADDING_SEL=0;

    o_complex_world_translate(w_current, x, y, o_start);

    o_temp = o_start;
    while (o_temp != NULL) {
      if (o_temp->type == OBJ_NET || o_temp->type == OBJ_PIN ||
          o_temp->type == OBJ_BUS) {
        s_conn_update_object(w_current, o_temp);
                  
        connected_objects = s_conn_return_others(connected_objects,
                                                 o_temp);
      }
      o_temp = o_temp->next;
    }
    o_cue_undraw_list(w_current, connected_objects);
    o_cue_draw_list(w_current, connected_objects);
    g_list_free(connected_objects);

    g_free(include_filename);

    if (w_current->actionfeedback_mode == OUTLINE) {
#if 0
      printf("inside draw outline here\n");
#endif
      /* erase outline */
      o_complex_translate_display(
                                  w_current,
                                  diff_x, diff_y,
                                  w_current->page_current->
                                  complex_place_head->next);
    } else {
#if 0
      printf("inside draw bounding here\n");
#endif
      get_complex_bounds(
                         w_current,
                         w_current->page_current->
                         complex_place_head->next,
                         &rleft, &rtop, &rright, &rbottom);
      gdk_gc_set_foreground(
                            w_current->gc,
                            x_get_color(w_current->background_color));
      gdk_draw_rectangle(w_current->window, w_current->gc,
                         FALSE,
                         rleft   + diff_x,
                         rtop    + diff_y,
                         rright  - rleft,
                         rbottom - rtop);
    }

    o_redraw(w_current, o_start);
    w_current->page_current->CHANGED = 1;
    o_undo_savestate(w_current, UNDO_ALL);
    i_update_menus(w_current);
    o_list_delete_rest(w_current, w_current->page_current->
                       complex_place_head);
    return;
  }

  o_temp = w_current->page_current->object_tail;
  w_current->page_current->object_tail =
    o_complex_add(
                  w_current,
                  w_current->page_current->object_tail,
                  OBJ_COMPLEX, WHITE, x, y, w_current->complex_rotate, 0,
                  w_current->internal_clib,
                  w_current->internal_basename, 1, TRUE);

  /* complex rotate post processing */
  o_temp = o_temp->next; /* skip over last object */
  while (o_temp != NULL) {
    switch(o_temp->type) {
      case(OBJ_TEXT):
        temp = w_current->complex_rotate / 90;
        for (i = 0; i < temp; i++) {
          new_angle = (o_temp->
                       text->angle + 90) % 360;
          o_text_rotate(w_current, 
                        screen_x, screen_y,
                        new_angle, 90, o_temp);
        }
        break;
    }
		
    o_temp = o_temp->next;
  }

  /* 1 should be define fix everywhere hack */
	
  o_current = w_current->page_current->object_tail;

  if (scm_hook_empty_p(add_component_hook) == SCM_BOOL_F &&
      o_current != NULL) {
    scm_run_hook(add_component_hook,
                 scm_cons(g_make_attrib_smob_list(w_current, o_current),
                          SCM_EOL));
  }

  if (scm_hook_empty_p(add_component_object_hook) == SCM_BOOL_F &&
      o_current != NULL) {
    scm_run_hook(add_component_object_hook,
		 scm_cons(g_make_object_smob(w_current, o_current),
			  SCM_EOL));
  }

  /* put code here to deal with emebedded stuff */
  if (w_current->embed_complex) {
    char* new_basename;

    g_free(o_current->complex_clib);

    o_current->complex_clib = g_strdup ("EMBEDDED");

    new_basename = g_strconcat ("EMBEDDED",
                                o_current->complex_basename,
                                NULL);

    g_free(o_current->complex_basename);

    o_current->complex_basename = g_strdup (new_basename);

    g_free(new_basename);
  }

  /* check for nulls in all this hack */
  if (w_current->actionfeedback_mode == OUTLINE) {
#if 0
    printf("inside draw outline here\n");
#endif
    /* erase outline */
    o_complex_translate_display(
                                w_current,
                                diff_x, diff_y,
                                w_current->page_current->complex_place_head->next);
  } else {
#if 0
    printf("inside draw bounding here\n");
#endif
    get_complex_bounds(
                       w_current,
                       w_current->page_current->complex_place_head->next,
                       &rleft, &rtop,
                       &rright, &rbottom);
    gdk_gc_set_foreground(
                          w_current->gc,
                          x_get_color(w_current->background_color));
    gdk_draw_rectangle(w_current->window, w_current->gc, FALSE,
                       rleft   + diff_x,
                       rtop    + diff_y,
                       rright  - rleft,
                       rbottom - rtop);
  }

  /*! \todo redraw has to happen at the end of all this hack or
   * maybe not? */
  o_list_delete_rest(w_current,
                     w_current->page_current->complex_place_head);

  /* This doesn't allow anything else to be in the selection
   * list when you add a component */

  o_selection_remove_most(w_current,
                          w_current->page_current->selection2_head);
  o_selection_add(w_current->page_current->selection2_head, 
                  w_current->page_current->object_tail);
  /* the o_redraw_selected is in x_events.c after this call
   * returns */
  o_attrib_add_selected(w_current, w_current->page_current->selection2_head,
                        w_current->page_current->object_tail);

  s_conn_update_complex(w_current, o_current->complex->prim_objs);
  connected_objects = s_conn_return_complex_others(connected_objects,
                                                   o_current);
  o_cue_undraw_list(w_current, connected_objects);
  o_cue_draw_list(w_current, connected_objects);
  g_list_free(connected_objects);
  o_cue_draw_single(w_current, w_current->page_current->object_tail);
        
  w_current->page_current->CHANGED = 1;
  o_undo_savestate(w_current, UNDO_ALL);
  i_update_menus(w_current);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_rubbercomplex(TOPLEVEL *w_current)
{
  o_drawbounding(w_current,
                 w_current->page_current->complex_place_head->next,
                 NULL,
                 x_get_darkcolor(w_current->bb_color), FALSE);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_translate_display(TOPLEVEL *w_current,
				 int x1, int y1, OBJECT *complex)
{
  OBJECT *o_current = complex;

  while (o_current != NULL) {
    switch(o_current->type) {
      case(OBJ_LINE):
        o_line_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_NET):
        o_net_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_BUS):
        o_bus_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_BOX):
        o_box_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_PICTURE):
        o_picture_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_CIRCLE):
        o_circle_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_COMPLEX):
      case(OBJ_PLACEHOLDER):
        o_complex_draw_xor(w_current, x1, y1, 
                           o_current->complex->prim_objs);
        break;

      case(OBJ_TEXT):
        o_text_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_PIN):
        o_pin_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_ARC):
        o_arc_draw_xor(w_current, x1, y1, o_current);
        break;
    }
    o_current = o_current->next;
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_translate_display_selection(TOPLEVEL *w_current,
					   int x1, int y1, SELECTION *head)
{
  SELECTION *s_current = head;
  OBJECT *o_current;

  while (s_current != NULL) {

    o_current = s_current->selected_object;

    if (!o_current) {
      fprintf(stderr, _("Got NULL in o_complex_translate_display_selection\n"));
      exit(-1);
    }

    switch(o_current->type) {
      case(OBJ_LINE):
        o_line_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_NET):
        o_net_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_BUS):
        o_bus_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_BOX):
        o_box_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_PICTURE):
        o_picture_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_CIRCLE):
        o_circle_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_COMPLEX):
      case(OBJ_PLACEHOLDER):
        o_complex_draw_xor(w_current, x1, y1, 
                           o_current->complex->prim_objs);
        break;

      case(OBJ_TEXT):
        o_text_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_PIN):
        o_pin_draw_xor(w_current, x1, y1, o_current);
        break;

      case(OBJ_ARC):
        o_arc_draw_xor(w_current, x1, y1, o_current);
        break;
    }
    s_current = s_current->next;
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *  \note
 *  experimental
 */
void o_complex_translate2(TOPLEVEL *w_current, int dx, int dy, OBJECT *object)
{
  if (object == NULL)  {
    printf("cmt2 NO!\n");
    return;
  }

  o_complex_translate_display(w_current, dx, dy, object);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *  \note
 *  don't know if this belongs yet
 */
void o_complex_translate_all(TOPLEVEL *w_current, int offset)
{
  int rleft, rtop, rright, rbottom;
  OBJECT *o_current;
  int x, y;

  /* first zoom extents */
  a_zoom_extents(w_current, w_current->page_current->object_head, 
                 A_PAN_DONT_REDRAW);
  o_redraw_all(w_current);

  get_complex_bounds(w_current, w_current->page_current->object_head,
                     &rleft,
                     &rtop,
                     &rright,
                     &rbottom);

  /*! \todo do we want snap grid here? */
  SCREENtoWORLD(w_current,
                fix_x(w_current, rleft  ),
                fix_y(w_current, rbottom),
                &x,
                &y);

  o_current = w_current->page_current->object_head;
  while(o_current != NULL) {
    if (o_current->type != OBJ_COMPLEX && o_current->type != OBJ_PLACEHOLDER) {
      s_conn_remove(w_current, o_current);
    } else {
      s_conn_remove_complex(w_current, o_current);
    }
    o_current = o_current->next;
  }
        
  if (offset == 0) {
    s_log_message(_("Translating schematic [%d %d]\n"), -x, -y);
    o_complex_world_translate(
                              w_current,
                              -x, -y,
                              w_current->page_current->object_head);
  } else {
    s_log_message(_("Translating schematic [%d %d]\n"),
                  offset, offset);
    o_complex_world_translate(
                              w_current,
                              offset, offset,
                              w_current->page_current->object_head);
  }

  o_current = w_current->page_current->object_head;
  while(o_current != NULL) {
    if (o_current->type != OBJ_COMPLEX && o_current->type != OBJ_PLACEHOLDER) {
      s_conn_update_object(w_current, o_current);
    } else {
      s_conn_update_complex(w_current, o_current->complex->prim_objs);
    }
    o_current = o_current->next;
  }

  /* this is an experimental mod, to be able to translate to all
   * places */
#if 0
  o_complex_world_translate(1000, 1000, object_head);
  printf("symbol -%d -%d\n", x, y);
  o_complex_world_translate(-x, -y, object_head); /* to zero, zero */
#endif

  a_zoom_extents(w_current, w_current->page_current->object_head, 
                 A_PAN_DONT_REDRAW);
  o_unselect_all(w_current);
  o_redraw_all(w_current);
  w_current->page_current->CHANGED=1;
  o_undo_savestate(w_current, UNDO_ALL);
  i_update_menus(w_current);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_translate_selection(TOPLEVEL *w_current, int dx, int dy, 
				   SELECTION *head)
{
  if (head == NULL)  {
    printf(_("Got NULL in o_complex_translate_selection!\n"));
    return;
  }

  o_complex_translate_display_selection(w_current, dx, dy, head);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_rotate(TOPLEVEL *w_current, int centerx, int centery,
		      int angle, int angle_change, OBJECT *object)
{
  int x, y;
  int newx, newy;
  int world_centerx, world_centery;

  SCREENtoWORLD(w_current,
                centerx,
                centery,
                &world_centerx,
                &world_centery);

  x = object->complex->x + (-world_centerx);
  y = object->complex->y + (-world_centery);

  rotate_point_90(x, y, 90, &newx, &newy);

  x = newx + (world_centerx);
  y = newy + (world_centery);

  o_complex_world_translate_toplevel(w_current,
                                     -object->complex->x, 
                                     -object->complex->y, object);
  o_complex_rotate_lowlevel(w_current,
                            0, 0, angle, angle_change, object);

  object->complex->x = 0;
  object->complex->y = 0;

  o_complex_world_translate_toplevel(w_current, x, y, object);

  object->complex->angle = angle;

#if DEBUG
  printf("setting final rotated angle to: %d\n\n", object->angle);
#endif
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
int o_complex_mirror(TOPLEVEL *w_current, int centerx, int centery,
		     OBJECT *object)
{
  int x, y;
  int newx, newy;
  int origx, origy;
  int world_centerx, world_centery;
  int change = 0;

  SCREENtoWORLD(w_current,
                centerx,
                centery,
                &world_centerx,
                &world_centery);

  origx = object->complex->x;
  origy = object->complex->y;

  x = object->complex->x + (-world_centerx);
  y = object->complex->y + (-world_centery);

  newx = -x;
  newy = y;

  x = newx + (world_centerx);
  y = newy + (world_centery);

  o_complex_world_translate_toplevel(w_current,
                                     -object->complex->x, 
                                     -object->complex->y, object);

  o_complex_mirror_lowlevel(w_current, 0, 0, object);

  switch(object->complex->angle) {
    case(90):
      object->complex->angle = 270;
#if 0
      o_text_change_angle(w_current, object->complex->prim_objs,
                          object->complex->angle);
#endif

      change = 1;
      break;

    case(270):
      object->complex->angle = 90;
#if 0
      o_text_change_angle(w_current, object->complex->prim_objs
                          object->complex->angle);
#endif
      change = 1;
      break;

  }
#if 0
  object->complex->angle = (object->complex->angle + 180) % 360;
#endif

  object->complex->mirror = !object->complex->mirror;
#if 0
  object->complex->x = 0;
  object->complex->y = 0;
#endif

  o_complex_world_translate_toplevel(w_current, x, y, object);

#if DEBUG
  printf("final res %d %d\n", object->complex->x,  object->complex->y);
#endif
#if 0
  object->complex->x = x;
  object->complex->y = y;
#endif
  return(change);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *  \note
 *  this is a special mirror which doesn't mirror the object in memory,
 *  but read the new "correctly mirrored/rotated" object from disk
 *  \todo yes this is aweful, and I will eventually fix it, but for now
 *        it has to do hack
 */
OBJECT *o_complex_mirror2(TOPLEVEL *w_current, OBJECT *list,
			  int centerx, int centery,
			  OBJECT *object)
{
  OBJECT *new_obj = NULL;
  int x, y;
  int newx, newy;
  int origx, origy;
  int world_centerx, world_centery;
  int change=0;
  int color;

  SCREENtoWORLD(w_current,
                centerx,
                centery,
                &world_centerx,
                &world_centery);

  origx = object->complex->x;
  origy = object->complex->y;

  x = object->complex->x + (-world_centerx);
  y = object->complex->y + (-world_centery);

  newx = -x;
  newy = y;

  x = newx + (world_centerx);
  y = newy + (world_centery);

  switch(object->complex->angle) {
    case(90):
      object->complex->angle = 270;
#if 0
      o_text_change_angle(w_current, object->complex->prim_objs,
                          object->complex->angle);
#endif
      change = 1;
      break;

    case(270):
      object->complex->angle = 90;
#if 0
      o_text_change_angle(w_current, object->complex->prim_objs,
                          object->complex->angle);
#endif
      change=1;
      break;

  }

  object->complex->mirror = !object->complex->mirror;

  if (object->saved_color == -1) {
    color = object->color;
  } else {
    color = object->saved_color;
  }


  new_obj = o_complex_add(w_current,
                          list, object->type,
                          color,
                          x, y,
                          object->complex->angle, 
                          object->complex->mirror,
                          object->complex_clib, object->complex_basename,
                          1, FALSE);

  /*! \todo fix up name sometime ... */
  new_obj->sid = object->sid;

  new_obj->attribs = o_attrib_copy_all(
                                       w_current, new_obj, object->attribs);

  o_attrib_slot_update(w_current, new_obj);

  o_complex_delete(w_current, object);

  /* need to do the following, because delete severs links */
  o_attrib_reattach(new_obj->attribs);
  o_attrib_set_color(w_current, new_obj->attribs);


#if 0
  w_current->page_current->object_tail = (OBJECT *)
  return_tail(w_current->page_current->object_head);
#endif

  return(new_obj);
}