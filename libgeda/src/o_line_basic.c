/* gEDA - GPL Electronic Design Automation
 * libgeda - gEDA's library
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

#include <gtk/gtk.h>
#include <libguile.h>

#ifdef HAS_LIBGDGEDA
#include <gdgeda/gd.h>
#endif

#include "defines.h"
#include "struct.h"
#include "globals.h"
#include "o_types.h"
#include "colors.h"
#include "funcs.h"

#include "../include/prototype.h"

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif

/*! \brief Create and add line OBJECT to list.
 *  \par Function Description
 *  This function creates a new object representing a line.
 *  This object is added to the end of the list <B>object_list</B>
 *  pointed object belongs to.
 *  The line is described by its two ends - <B>x1</B>,<B>y1</B> and
 *  <B>x2</B>,<B>y2</B>.
 *  The <B>type</B> parameter must be equal to #OBJ_LINE.
 *  The <B>color</B> parameter corresponds to the color the box
 *  will be drawn with.
 *
 *  The #OBJECT structure is allocated with the
 *  #s_basic_init_object() function. The structure describing
 *  the line is allocated and initialized with the parameters given
 *  to the function.
 *
 *  Both the line type and the filling type are set to default
 *  values : solid line type with a width of 0, and no filling.
 *  It can be changed after with the #o_set_line_options() and
 *  #o_set_fill_options().
 *
 *  The object is added to the end of the list described by the 
 *  <B>object_list</B> parameter by the #s_basic_link_object().
 *
 *  \param [in]     w_current    The TOPLEVEL object.
 *  \param [in,out] object_list  OBJECT list to add line to.
 *  \param [in]     type         Must be OBJ_LINE.
 *  \param [in]     color        Circle line color.
 *  \param [in]     x1           Upper x coordinate.
 *  \param [in]     y1           Upper y coordinate.
 *  \param [in]     x2           Lower x coordinate.
 *  \param [in]     y2           Lower y coordinate.
 *  \return A pointer to the new end of the object list.
 */
OBJECT *o_line_add(TOPLEVEL *w_current, OBJECT *object_list,
		   char type, int color, 
		   int x1, int y1, int x2, int y2)
{
  OBJECT *new_node;

  /* create the object */
  new_node        = s_basic_init_object("line");
  new_node->type  = type;
  new_node->color = color;
  
  new_node->line  = (LINE *) g_malloc(sizeof(LINE));
  
  /* describe the line with its two ends */
  new_node->line->x[0] = x1;
  new_node->line->y[0] = y1;
  new_node->line->x[1] = x2;
  new_node->line->y[1] = y2;
  
  /* line type and filling initialized to default */
  o_set_line_options(w_current, new_node,
		     END_NONE, TYPE_SOLID, 0, -1, -1);
  o_set_fill_options(w_current, new_node,
		     FILLING_HOLLOW, -1, -1, -1, -1, -1);
  
  /* \todo questionable cast */
  new_node->draw_func = (void *) line_draw_func;
  /* \todo questionable cast */
  new_node->sel_func = (void *) select_func;  
  
  /* compute bounding box and screen coords */
  o_line_recalc(w_current, new_node);
  
  object_list = (OBJECT *) s_basic_link_object(new_node, object_list);
    
  return(object_list);
}

/*! \brief Create a copy of a line.
 *  \par Function Description
 *  This function creates a verbatim copy of the
 *  object pointed by <B>o_current</B> describing a line. The new object
 *  is added at the end of the list following the <B>list_tail</B>
 *  parameter.
 *
 *  \param [in]  w_current  The TOPLEVEL object.
 *  \param [out] list_tail  OBJECT list to copy to.
 *  \param [in]  o_current  Line OBJECT to copy.
 *  \return A new pointer to the end of the object list.
 */
OBJECT *o_line_copy(TOPLEVEL *w_current, OBJECT *list_tail, OBJECT *o_current)
{
  OBJECT *new_obj;
  ATTRIB *a_current;
  int color;

  if (o_current->saved_color == -1) {
    color = o_current->color;
  } else {
    color = o_current->saved_color;
  }

  /*
   * A new line object is added a the end of the object list with
   * #o_line_add(). Values for its fields are default and need to
   * be modified.
   */
  new_obj = o_line_add(w_current, list_tail,
		       OBJ_LINE, color,
		       0, 0, 0, 0);

  /*
   * The coordinates of the ends of the new line are set with the ones
   * of the original line. The two lines have the sale line type and
   * filling options.
   *
   * The coordinates and the values in screen unit are computed with
   * #o_line_recalc().
   */

  /* modify the line ends of the new line */
  new_obj->line->x[0] = o_current->line->x[0];
  new_obj->line->y[0] = o_current->line->y[0];
  new_obj->line->x[1] = o_current->line->x[1];
  new_obj->line->y[1] = o_current->line->y[1];
  
  /* copy the line type and filling options */
  o_set_line_options(w_current, new_obj, o_current->line_end,
		     o_current->line_type, o_current->line_width,
		     o_current->line_length, o_current->line_space);
  o_set_fill_options(w_current, new_obj,
		     o_current->fill_type, o_current->fill_width,
		     o_current->fill_pitch1, o_current->fill_angle1,
		     o_current->fill_pitch2, o_current->fill_angle2);
  
  /* calc the screen coords */
  o_line_recalc(w_current, o_current);
  
  /* new_obj->attribute = 0;*/
  a_current = o_current->attribs;
  if (a_current) {
    while ( a_current ) {
      /* head attrib node has prev = NULL */
      if (a_current->prev != NULL) {
	a_current->copied_to = new_obj;
      }
      a_current = a_current->next;
    }
  }
  
  /* return the new tail of the object list */
  return(new_obj);
}

/*! \brief Modify the description of a line OBJECT.
 *  \par Function Description
 *  This function modifies the coordinates of one of the two ends of
 *  the line described by <B>*object</B>. The new coordinates of this end,
 *  identified by <B>whichone</B>, are given by <B>x</B> and <B>y</B>
 *  in world unit.
 *
 *  The coordinates of the end of line is modified in the world
 *  coordinate system. Screen coordinates and boundings are then updated.
 *
 *  \param [in]     w_current  The TOPLEVEL object.
 *  \param [in,out] object     Line OBJECT to modify.
 *  \param [in]     x          New x coordinate.
 *  \param [in]     y          New y coordinate.
 *  \param [in]     whichone   Which line parameter to modify.
 *
 *  <B>whichone</B> can have the following values:
 *  <DL>
 *    <DT>*</DT><DD>LINE_END1
 *    <DT>*</DT><DD>LINE_END2
 *  </DL>
 *
 *  \par Author's note
 *  pb20011009 - modified
 */
void o_line_modify(TOPLEVEL *w_current, OBJECT *object, 
		   int x, int y, int whichone)
{
	/* change one of the end of the line */
	switch(whichone) {
		case LINE_END1:
		object->line->x[0] = x;
		object->line->y[0] = y;
		break;
		
		case LINE_END2:
		object->line->x[1] = x;
		object->line->y[1] = y;
		break;
		
		default:
		return;
	}
	
	/* recalculate the screen coords and the boundings */
	o_line_recalc(w_current, object);
	
}

/*! \brief Create line OBJECT from character string.
 *  \par Function Description
 *  This function creates a line OBJECT from the character string
 *  <B>*buf</B> the description of a box. The new box is added to the
 *  list of objects of which <B>*object_list</B> is the last element
 *  before the call.
 *  The function returns a pointer on the new last element, that is
 *  the added line object.
 *
 *  Depending on <B>*version</B>, the correct file format is considered.
 *  Currently two file format revisions are supported :
 *  <DL>
 *    <DT>*</DT><DD>the file format used until 20010704 release.
 *    <DT>*</DT><DD>the file format used for the releases after 20010704.
 *  </DL>
 *
 *  \param [in]  w_current       The TOPLEVEL object.
 *  \param [out] object_list     OBJECT list to create line in.
 *  \param [in]  buf             Character string with line description.
 *  \param [in]  release_ver     libgeda release version number.
 *  \param [in]  fileformat_ver  libgeda file format version number.
 *  \return A pointer to the new line object.
 */
OBJECT *o_line_read(TOPLEVEL *w_current, OBJECT *object_list, char buf[],
		    unsigned int release_ver, unsigned int fileformat_ver)
{
  char type; 
  int x1, y1;
  int x2, y2;
  int d_x1, d_y1;
  int d_x2, d_y2;
  int line_width, line_space, line_length;
  int line_end;
  int line_type;
  int color;

  if(release_ver <= VERSION_20000704) {
    /*
     * The old geda file format, i.e. releases 20000704 and older, does
     * not handle the line type and the filling - here filling is irrelevant.
     * They are set to default.
     */
    sscanf(buf, "%c %d %d %d %d %d\n", &type,
	   &x1, &y1, &x2, &y2, &color);

    line_width = 0;
    line_end   = END_NONE;
    line_type  = TYPE_SOLID;
    line_length= -1;
    line_space = -1;
  } else {
    /*
     * The current line format to describe a line is a space separated
     * list of characters and numbers in plain ASCII on a single line.
     * The meaning of each item is described in the file format documentation.
     */
    sscanf(buf, "%c %d %d %d %d %d %d %d %d %d %d\n", &type,
	   &x1, &y1, &x2, &y2, &color,
	   &line_width, &line_end, &line_type, &line_length, &line_space);
  }
  
  d_x1 = x1; /* \todo PB : Needed ? */
  d_y1 = y1; 
  d_x2 = x2; 
  d_y2 = y2; 
    
  /*
   * Null length line are not allowed. If such a line is detected a
   * message is issued.
   *
   * It also checks is the required color is valid.
   */
  if (x1 == x2 && y1 == y2) {
    fprintf(stderr, "Found a zero length line [ %c %d %d %d %d %d ]\n",
            type, x1, y1, x2, y2, color);
    s_log_message("Found a zero length line [ %c %d %d %d %d %d ]\n",
                  type, x1, y1, x2, y2, color);
  }
  
  if (color < 0 || color > MAX_COLORS) {
    fprintf(stderr, "Found an invalid color [ %s ]\n", buf);
    s_log_message("Found an invalid color [ %s ]\n", buf);
    s_log_message("Setting color to WHITE\n");
    color = WHITE;
  }

  /*
   * A line is internally described by its two ends. A new object is
   * allocated, initialized and added to the list of objects. Its line
   * type is set according to the values of the fields on the line.
   */
  /* create and add the line to the list */
  object_list = o_line_add(w_current, object_list,
			   type, color, d_x1, d_y1, d_x2, d_y2);
  /* set its line options */
  o_set_line_options(w_current, object_list,
		     line_end, line_type, line_width, line_length, 
		     line_space);
  /* filling is irrelevant for line, just set to default */
  o_set_fill_options(w_current, object_list,
		     FILLING_HOLLOW, -1, -1, -1, -1, -1);

  return(object_list);
}

/*! \brief Create a character string representation of a line OBJECT.
 *  \par Function Description
 *  The function formats a string in the buffer <B>*buff</B> to describe
 *  the box object <B>*object</B>.
 *  It follows the post-20000704 release file format that handle the
 *  line type and fill options - filling is irrelevant here.
 *
 *  \param [in] object  Line OBJECT to create string from.
 *  \return A pointer to the line OBJECT character string.
 *
 *  \note
 *  Caller must free returned character string.
 *
 */
char *o_line_save(OBJECT *object)
{
  int x1, x2, y1, y2;
  int color;
  int line_width, line_space, line_length;
  char *buf;
  OBJECT_END line_end;
  OBJECT_TYPE line_type;

  /* get the two ends */
  x1 = object->line->x[0];
  y1 = object->line->y[0];
  x2 = object->line->x[1];
  y2 = object->line->y[1];
  
  /* description of the line type */
  line_width = object->line_width;
  line_end   = object->line_end;
  line_type  = object->line_type;
  line_length= object->line_length;
  line_space = object->line_space;
  
  /* Use the right color */
  if (object->saved_color == -1) {
    color = object->color;
  } else {
    color = object->saved_color;
  }
  
  buf = g_strdup_printf("%c %d %d %d %d %d %d %d %d %d %d", object->type,
			x1, y1, x2, y2, color,
			line_width, line_end, line_type,
			line_length, line_space);

  return(buf);
}
      
/*! \brief Translate a line position by a delta.
 *  \par Function Description
 *  This function applies a translation of (<B>dx</B>,<B>dy</B>) to the
 *  line described by <B>*object</B>. <B>dx</B> and <B>dy</B> are in
 *  screen unit.
 *
 *  \param [in]     w_current   The TOPLEVEL object.
 *  \param [in]     dx          x distance to move.
 *  \param [in]     dy          y distance to move.
 *  \param [in,out] object      Line OBJECT to translate.
 */
void o_line_translate(TOPLEVEL *w_current, int dx, int dy, OBJECT *object)
{
  int x, y;

  if (object == NULL) printf("lt NO!\n");

  /* Do screen coords */
  object->line->screen_x[0] = object->line->screen_x[0] + dx;
  object->line->screen_y[0] = object->line->screen_y[0] + dy;
  object->line->screen_x[1] = object->line->screen_x[1] + dx;
  object->line->screen_y[1] = object->line->screen_y[1] + dy;
  
  /* do we want snap grid here? hack */
  SCREENtoWORLD(w_current,
		object->line->screen_x[0], object->line->screen_y[0], 
		&x, &y);  
  object->line->x[0] = snap_grid(w_current, x);
  object->line->y[0] = snap_grid(w_current, y);
  
  SCREENtoWORLD(w_current,
		object->line->screen_x[1], object->line->screen_y[1], 
		&x, &y);  
  object->line->x[1] = snap_grid(w_current, x);
  object->line->y[1] = snap_grid(w_current, y);
  
}

/*! \brief Translate a line position in WORLD coordinates by a delta.
 *  \par Function Description
 *  This function applies a translation of (<B>x1</B>,<B>y1</B>) to the line
 *  described by <B>*object</B>. <B>x1</B> and <B>y1</B> are in world unit.
 *
 *  \param [in]     w_current  The TOPLEVEL object.
 *  \param [in]     x1         x distance to move.
 *  \param [in]     y1         y distance to move.
 *  \param [in,out] object     Line OBJECT to translate.
 */
void o_line_translate_world(TOPLEVEL *w_current,
			    int x1, int y1, OBJECT *object)
{
  int screen_x1, screen_y1;
  int screen_x2, screen_y2;	
  int left, right, top, bottom;

  if (object == NULL) printf("ltw NO!\n");

  /* Do world coords */
  object->line->x[0] = object->line->x[0] + x1;
  object->line->y[0] = object->line->y[0] + y1;
  object->line->x[1] = object->line->x[1] + x1;
  object->line->y[1] = object->line->y[1] + y1;
  
  /* update screen coords */
  WORLDtoSCREEN(w_current, object->line->x[0], 
		object->line->y[0], 
		&screen_x1,
		&screen_y1);  
  object->line->screen_x[0] = screen_x1;
  object->line->screen_y[0] = screen_y1;
  
  WORLDtoSCREEN(w_current, object->line->x[1], 
		object->line->y[1], 
		&screen_x2,
		&screen_y2);  
  object->line->screen_x[1] = screen_x2;
  object->line->screen_y[1] = screen_y2;
  
  /* update bounding box */
  get_line_bounds(w_current, object->line, &left, &top, &right, &bottom);
  
  object->left   = left;
  object->top    = top;
  object->right  = right;
  object->bottom = bottom;
    
}

/*! \brief Rotate a line OBJECT.
 *  \par Function Description
 *  This function applies a rotation of center (<B>centerx</B>,<B>centery</B>)
 *  and angle <B>angle</B> to the line object <B>*object</B>.
 *  The coordinates of the rotation center are in screen units.
 *  <B>angle</B> mst be a 90 degree multiple. If not, no rotation is applied.
 *
 *  The rotation is made by the #o_line_rotate_world() function
 *  that perform a rotation of angle <B>angle</B> and center
 *  (<B>world_centerx</B>,<B>world_centery</B>) in world unit.
 *
 *  \param [in]     w_current  The TOPLEVEL object.
 *  \param [in]     centerx    Rotation center x coordinate in SCREEN units.
 *  \param [in]     centery    Rotation center y coordinate in SCREEN units.
 *  \param [in]     angle      Rotation angle in degrees (unused).
 *  \param [in,out] object     Line OBJECT to rotate.
 *
 *  \note
 *  takes in screen coordinates for the centerx,y, and then does the rotate 
 *  in world space
 *  also ignores angle argument... for now, rotate only in 90 degree 
 *  increments
 */
void o_line_rotate(TOPLEVEL *w_current, int centerx, int centery, int angle,
		   OBJECT *object)
{
  int world_centerx, world_centery;

  /* convert the center of rotation to world unit */
  SCREENtoWORLD(w_current, centerx, centery, 
                &world_centerx,
                &world_centery);  

  /* rotate the line */
  /* the check on the rotation angle is in o_line_rotate_world() */
  o_line_rotate_world(w_current,
					  world_centerx, world_centery, angle,
					  object);
  
}

/*! \brief Rotate Line OBJECT using WORLD coordinates. 
 *  \par Function Description 
 *  This function rotates the line described by
 *  <B>*object</B> around the (<B>world_centerx</B>,<B>world_centery</B>)
 *  point by <B>angle</B> degrees.
 *  The center of rotation is in world units.
 *
 *  \param [in]      w_current      The TOPLEVEL object.
 *  \param [in]      world_centerx  Rotation center x coordinate in WORLD units.
 *  \param [in]      world_centery  Rotation center y coordinate in WORLD units.
 *  \param [in]      angle          Rotation angle in degrees (See note below).
 *  \param [in,out]  object         Line OBJECT to rotate.
 */
void o_line_rotate_world(TOPLEVEL *w_current, 
			 int world_centerx, int world_centery, int angle,
			 OBJECT *object)
{
  int newx, newy;
	
  if (angle == 0) 
  return;

  /* angle must be positive */
  if(angle < 0) angle = -angle;
  /* angle must be 90 multiple or no rotation performed */
  if((angle % 90) != 0) return;

  /*
   * The center of rotation (<B>world_centerx</B>,<B>world_centery</B>)
   * is translated to the origin. The rotation of the two ends of
   * the line is performed. FInally, the rotated line is translated
   * back to its previous location.
   */
  /* translate object to origin */
  o_line_translate_world(w_current, -world_centerx, -world_centery, object);

  /* rotate line end 1 */
  rotate_point_90(object->line->x[0], object->line->y[0], angle,
		  &newx, &newy);

  object->line->x[0] = newx;
  object->line->y[0] = newy;
  
  /* rotate line end 2 */
  rotate_point_90(object->line->x[1], object->line->y[1], angle,
		  &newx, &newy);
  
  object->line->x[1] = newx;
  object->line->y[1] = newy;

  /* translate object back to normal position */
  o_line_translate_world(w_current, world_centerx, world_centery, object);
  
}

/*! \brief Mirror a Line.
 *  \par Function Description
 *  This function mirrors the line from the point
 *  (<B>centerx</B>,<B>centery</B>) in screen unit.
 *
 *  The origin of the mirror in screen unit is converted in world unit.
 *  The line is mirrored with the function #o_line_mirror_world()
 *  for which the origin of the mirror must be given in world unit.
 *
 *  \param [in]     w_current  The TOPLEVEL object.
 *  \param [in]     centerx    Origin x coordinate in WORLD units.
 *  \param [in]     centery    Origin y coordinate in WORLD units.
 *  \param [in,out] object     Line OBJECT to mirror.
 */
void o_line_mirror(TOPLEVEL *w_current,
		   int centerx, int centery,
		   OBJECT *object)
{
  int world_centerx, world_centery;

  /* convert the origin of mirror */
  SCREENtoWORLD(w_current, centerx, centery, 
                &world_centerx,
                &world_centery);  

  /* apply the mirror in world coords */
  o_line_mirror_world(w_current,
					  world_centerx, world_centery,
					  object);
  
}

/*! \brief Mirror a line using WORLD coordinates.
 *  \par Function Description
 *  This function mirrors the line from the point
 *  (<B>world_centerx</B>,<B>world_centery</B>) in world unit.
 *
 *  The line if first translated to the origin, then mirrored
 *  and finally translated back at its previous position.
 *
 *  \param [in]     w_current      The TOPLEVEL object.
 *  \param [in]     world_centerx  Origin x coordinate in WORLD units.
 *  \param [in]     world_centery  Origin y coordinate in WORLD units.
 *  \param [in,out] object         Line OBJECT to mirror.
 */
void o_line_mirror_world(TOPLEVEL *w_current, int world_centerx,
			 int world_centery, OBJECT *object)
{
  /* translate object to origin */
  o_line_translate_world(w_current, -world_centerx, -world_centery, object);

  /* mirror the line ends */
  object->line->x[0] = -object->line->x[0];
  object->line->x[1] = -object->line->x[1];

  /* translate back in position */
  o_line_translate_world(w_current, world_centerx, world_centery, object);
  
}

/*! \brief Recalculate line coordinates in SCREEN units.
 *  \par Function Description
 *  This function recalculate the screen coords of the <B>o_current</B>
 *  pointed line object from its world coords.
 *
 *  The line ends coordinates and its bounding box are recalculated
 *  as well as the OBJECT specific fields (line width, filling ...).
 *
 *  \param [in] w_current      The TOPLEVEL object.
 *  \param [in,out] o_current  Line OBJECT to be recalculated.
 */
void o_line_recalc(TOPLEVEL *w_current, OBJECT *o_current)
{
  int screen_x1, screen_y1;
  int screen_x2, screen_y2;
  int left, right, top, bottom;

  if (o_current->line == NULL) {
    return;
  }

  /* update the screen coords of end 1 of the line */
  WORLDtoSCREEN(w_current,
		o_current->line->x[0], o_current->line->y[0], 
		&screen_x1, &screen_y1);  
  o_current->line->screen_x[0] = screen_x1;
  o_current->line->screen_y[0] = screen_y1;
  
  /* update the screen coords of end 2 of the line */
  WORLDtoSCREEN(w_current,
		o_current->line->x[1], o_current->line->y[1], 
		&screen_x2, &screen_y2);  
  o_current->line->screen_x[1] = screen_x2;
  o_current->line->screen_y[1] = screen_y2;
  
  /* update the bounding box - screen unit */
  get_line_bounds(w_current, o_current->line, 
		  &left, &top, &right, &bottom);
  o_current->left   = left;
  o_current->top    = top;
  o_current->right  = right;
  o_current->bottom = bottom;
  
  /* recalc OBJECT specific parameters */
  o_object_recalc(w_current, o_current);
  
}

/*! \brief Get line bounding rectangle.
 *  \par Function Description 
 *  This function sets the <B>left</B>, <B>top</B>, <B>right</B> and
 *  <B>bottom</B> parameters to the boundings of the line object described
 *  by <B>*line</B> in screen units.
 *
 *  \param [in]  w_current  The TOPLEVEL object.
 *  \param [in]  line       line OBJECT to read coordinates from.
 *  \param [out] left       Left line coordinate in SCREEN units.
 *  \param [out] top        Top line coordinate in SCREEN units.
 *  \param [out] right      Right line coordinate in SCREEN units.
 *  \param [out] bottom     Bottom line coordinate in SCREEN units.
 */
void get_line_bounds(TOPLEVEL *w_current, LINE *line, int *left, int *top,
		     int *right, int *bottom)
{
  *left   = w_current->width;
  *top    = w_current->height;
  *right  = 0;
  *bottom = 0;
	
  if (line->screen_x[0] < *left)   *left   = line->screen_x[0];
  if (line->screen_x[0] > *right)  *right  = line->screen_x[0];
  if (line->screen_y[0] < *top)    *top    = line->screen_y[0];
  if (line->screen_y[0] > *bottom) *bottom = line->screen_y[0];

  if (line->screen_x[1] < *left)   *left   = line->screen_x[1];
  if (line->screen_x[1] > *right)  *right  = line->screen_x[1];
  if (line->screen_y[1] < *top)    *top    = line->screen_y[1];
  if (line->screen_y[1] > *bottom) *bottom = line->screen_y[1];

  /* PB : bounding box has to take into account the width of the line */
  /* PB : but line width is unknown here */
	
  *left   = *left   - 4;
  *top    = *top    - 4;
  *right  = *right  + 4;
  *bottom = *bottom + 4;
}

/*! \brief Get line bounding rectangle in WORLD coordinates.
 *  \par Function Description
 *  This function sets the <B>left</B>, <B>top</B>, <B>right</B> and
 *  <B>bottom</B> parameters to the boundings of the line object described
 *  in <B>*line</B> in world units.
 *
 *  \param [in]  w_current  The TOPLEVEL object.
 *  \param [in]  line       Line OBJECT to read coordinates from.
 *  \param [out] left       Left line coordinate in WORLD units.
 *  \param [out] top        Top line coordinate in WORLD units.
 *  \param [out] right      Right line coordinate in WORLD units.
 *  \param [out] bottom     Bottom line coordinate in WORLD units.
 */
void world_get_line_bounds(TOPLEVEL *w_current, LINE *line,
			   int *left, int *top, int *right, int *bottom)
{
  *left   = w_current->init_right;
  *top    = w_current->init_bottom;
  *right  = 0;
  *bottom = 0;
	
  if (line->x[0] < *left)   *left   = line->x[0];
  if (line->x[0] > *right)  *right  = line->x[0];
  if (line->y[0] < *top)    *top    = line->y[0];
  if (line->y[0] > *bottom) *bottom = line->y[0];

  if (line->x[1] < *left)   *left   = line->x[1];
  if (line->x[1] > *right)  *right  = line->x[1];
  if (line->y[1] < *top)    *top    = line->y[1];
  if (line->y[1] > *bottom) *bottom = line->y[1];
  
}

/*! \brief Print line to Postscript document.
 *  \par Function Description
 *  This function prints the line described by the <B>o_current</B>
 *  parameter to a Postscript document.
 *  The Postscript document is described by the <B>fp</B> file pointer.
 *
 *  Parameters of the line are extracted from object pointed by
 *  <B>o_current</B>.
 *  
 *  \param [in] w_current  The TOPLEVEL object.
 *  \param [in] fp         FILE pointer to Postscript document.
 *  \param [in] o_current  Line OBJECT to write to document.
 *  \param [in] origin_x   Page x coordinate to place line OBJECT.
 *  \param [in] origin_y   Page y coordinate to place line OBJECT.
 */
void o_line_print(TOPLEVEL *w_current, FILE *fp, OBJECT *o_current, 
		  int origin_x, int origin_y)
{
  int x1, y1, x2, y2;
  int color;
  int line_width, length, space;
  void (*outl_func)() = NULL;
	
  if (o_current == NULL) {
    printf("got null in o_line_print\n");
    return;
  }

  x1    = o_current->line->x[0];
  y1    = o_current->line->y[0];
  x2    = o_current->line->x[1];
  y2    = o_current->line->y[1];
  color = o_current->color;

  /*
   * Depending on the type of the line for this particular line, the
   * appropriate function is chosen among
   * #o_line_print_solid(), #o_line_print_dotted()#, #o_line_print_dashed(),
   * #o_line_print_center() and #o_line_print_phantom().
   *
   * The needed parameters for each of these types are extracted from the
   * <B>o_current</B> object. Depending on the type, unused parameters are
   * set to -1.
   *
   * In the eventuality of a length and/or space null, the line is printed
   * solid to avoid and endless loop produced by other functions.
   */
  line_width = o_current->line_width;
  if(line_width <= 2) line_width = 2;
  length = o_current->line_length;
  space  = o_current->line_space;
  
  switch(o_current->line_type) {
    case(TYPE_SOLID):
      length = -1; space = -1;
      outl_func = (void *) o_line_print_solid;
      break;
      
    case(TYPE_DOTTED):
      length = -1;
      outl_func = (void *) o_line_print_dotted;
      break;
      
    case(TYPE_DASHED):
      outl_func = (void *) o_line_print_dashed;
      break;
      
    case(TYPE_CENTER):
      outl_func = (void *) o_line_print_center;
      break;
      
    case(TYPE_PHANTOM):
      outl_func = (void *) o_line_print_phantom;
      break;
      
    case(TYPE_ERASE):
      /* Unused for now, print it solid */
      length = -1; space = -1;
      outl_func = (void *) o_line_print_solid;
      break;
  }

  if((length == 0) || (space == 0)) {
    length = -1; space = -1;
    outl_func = (void *) o_line_print_solid;
  }
  
  (*outl_func)(w_current, fp,
	       x1 - origin_x, y1 - origin_y,
	       x2 - origin_x, y2 - origin_y,
	       color,
	       line_width, length, space,
	       origin_x, origin_y);
}

/*! \brief Print a solid line to Postscript document.
 *  \par Function Description
 *  This function prints a line when a solid line type is required.
 *  The line is defined by the coordinates of its two ends in
 *  (<B>x1</B>,<B>y1</B>) and (<B>x2</B>,<B>y2</B>).
 *  The Postscript document is defined by the file pointer <B>fp</B>.
 *  The parameters <B>length</B> and <B>space</B> are ignored whereas
 *  <B>line_width</B> specifies the width of the printed line.
 *
 *  \param [in] w_current     The TOPLEVEL object.
 *  \param [in] fp            FILE pointer to Postscript document.
 *  \param [in] x1            Upper x coordinate.
 *  \param [in] y1            Upper y coordinate.
 *  \param [in] x2            Lower x coordinate.
 *  \param [in] y2            Lower y coordinate.
 *  \param [in] color         Line color.
 *  \param [in] line_width    Width of line.
 *  \param [in] length        (unused).
 *  \param [in] space         (unused).
 *  \param [in] origin_x      Page x coordinate to place line OBJECT.
 *  \param [in] origin_y      Page y coordinate to place line OBJECT.
 */
void o_line_print_solid(TOPLEVEL *w_current, FILE *fp,
			int x1, int y1, int x2, int y2,
			int color,
			int line_width, int length, int space,
			int origin_x, int origin_y)
{
  if (w_current->print_color) {
    f_print_set_color(fp, color);
  }
  
  fprintf(fp,"%d %d %d %d %d line\n",
	  x1,y1,x2,y2, line_width);
}

/*! \brief Print a dotted line to Postscript document.
 *  \par Function Description
 *  This function prints a line when a dotted line type is required.
 *  The line is defined by the coordinates of its two ends in
 *  (<B>x1</B>,<B>y1</B>) and (<B>x2</B>,<B>y2</B>).
 *  The Postscript document is defined by the file pointer <B>fp</B>.
 *  The parameter <B>length</B> is ignored whereas <B>line_width</B>
 *  specifies the diameter of the dots and <B>space</B> the distance
 *  between two dots.
 *
 *  A negative value for <B>space</B> leads to an endless loop.
 *
 *  All dimensions are in mils.
 *
 *  The function sets the color in which the line will be printed with.
 *
 *  \param [in] w_current     The TOPLEVEL object.
 *  \param [in] fp            FILE pointer to Postscript document.
 *  \param [in] x1            Upper x coordinate.
 *  \param [in] y1            Upper y coordinate.
 *  \param [in] x2            Lower x coordinate.
 *  \param [in] y2            Lower y coordinate.
 *  \param [in] color         Line color.
 *  \param [in] line_width    Width of line.
 *  \param [in] length        (unused).
 *  \param [in] space         Space between dots.
 *  \param [in] origin_x      Page x coordinate to place line OBJECT.
 *  \param [in] origin_y      Page y coordinate to place line OBJECT.
 */
void o_line_print_dotted(TOPLEVEL *w_current, FILE *fp,
			 int x1, int y1, int x2, int y2,
			 int color,
			 int line_width, int length, int space,
			 int origin_x, int origin_y)
{
  double dx, dy, l, d;
  double dx1, dy1;
  double xa, ya;
  
  if (w_current->print_color) {
    f_print_set_color(fp, color);
  }
  
  /* The dotted line command takes an array of dots so print out the
   * beginnings of the array 
   */
  fprintf(fp,"[");
  /* PB : is the width relevant for a dot (circle) ? */
  /* f_print_set_line_width(fp, line_width); */
  
  /*
   * Depending on the slope of the line the space parameter is
   * projected on each of the two directions x and y resulting
   * in <B>dx1</B> and <B>dy1</B>. Starting from one end by increments
   * of space the dots are printed.
   *
   * A dot is represented by a filled circle. Position of the
   * circle is (<B>xa</B>, <B>ya</B>) and its radius is the <B>line_width</B>
   * parameter.
   */
  
  dx = (double) (x2 - x1);
  dy = (double) (y2 - y1);
  l = sqrt((dx * dx) + (dy * dy));
  
  dx1 = (dx * space) / l;
  dy1 = (dy * space) / l;
  
  d = 0;
  xa = x1; ya = y1;
  while(d < l) {
    
    fprintf(fp,"[%d %d] ",
	    (int)xa, (int)ya);
    d = d + space;
    xa = xa + dx1;
    ya = ya + dy1;
  }
  
  fprintf(fp,"] %d dashed\n",line_width);
  
}


/*! \brief Print a dashed line to Postscript document.
 *  \par Function Description
 *  This function prints a line when a dashed line type is required.
 *  The line is defined by the coordinates of its two ends in
 *  (<B>x1</B>,<B>y1</B>) and (<B>x2</B>,<B>y2</B>).
 *  The postscript file is defined by the file pointer <B>fp</B>.
 *
 *  A negative value for <B>space</B> or <B>length</B> leads to an
 *  endless loop.
 *
 *  All dimensions are in mils.
 *
 *  The function sets the color in which the line will be printed and
 *  the width of the line - that is the width of the dashes.
 *
 *  \param [in] w_current     The TOPLEVEL object.
 *  \param [in] fp            FILE pointer to Postscript document.
 *  \param [in] x1            Upper x coordinate.
 *  \param [in] y1            Upper y coordinate.
 *  \param [in] x2            Lower x coordinate.
 *  \param [in] y2            Lower y coordinate.
 *  \param [in] color         Line color.
 *  \param [in] line_width    Width of line.
 *  \param [in] length        Length of a dash.
 *  \param [in] space         Space between dashes.
 *  \param [in] origin_x      Page x coordinate to place line OBJECT.
 *  \param [in] origin_y      Page y coordinate to place line OBJECT.
 */
void o_line_print_dashed(TOPLEVEL *w_current, FILE *fp,
			 int x1, int y1, int x2, int y2,
			 int color,
			 int line_width, int length, int space,
			 int origin_x, int origin_y)
{
  double dx, dy, l, d;
  double dx1, dy1, dx2, dy2;
  double xa, ya, xb, yb;
  
  if (w_current->print_color) {
    f_print_set_color(fp, color);
  }
  
  /* the dashed line function takes an array of start-finish pairs
   * output the beginnings of the array now
   */
  fprintf(fp,"[");
  
  /*
   * Depending on the slope of the line the <B>length</B> (resp. <B>space</B>)
   * parameter is projected on each of the two directions x and y
   * resulting in <B>dx1</B> and <B>dy1</B> (resp. <B>dx2</B> and <B>dy2</B>).
   * Starting from one end and incrementing alternatively by <B>space</B>
   * and <B>length</B> the dashes are printed.
   *
   * It prints as many dashes of length <B>length</B> as possible.
   */
  dx = (double) (x2 - x1);
  dy = (double) (y2 - y1);
  l = sqrt((dx * dx) + (dy * dy));
  
  dx1 = (dx * length) / l;
  dy1 = (dy * length) / l;
  
  dx2 = (dx * space) / l;
  dy2 = (dy * space) / l;
  
  d = 0;
  xa = x1; ya = y1;
  while((d + length + space) < l) {
    d = d + length;
    xb = xa + dx1;
    yb = ya + dy1;
    
    fprintf(fp, "[%d %d %d %d] ", 
	    (int) xa, (int) ya,
	    (int) xb, (int) yb);
    
    d = d + space;
    xa = xb + dx2;
    ya = yb + dy2;
  }
  /*
   * When the above condition is no more satisfied, then it is not possible
   * to print a dash of length <B>length</B>. However it may be possible to
   * print the complete dash or a shorter one.
   */

  if((d + length) < l) {
    d = d + length;
    xb = xa + dx1;
    yb = ya + dy1;
  } else {
    xb = x2;
    yb = y2;
  }
	
  fprintf(fp, "[%d %d %d %d] ", 
	  (int) xa, (int) ya,
	  (int) xb, (int) yb);

  fprintf(fp,"] %d dashed\n", line_width);
}


/*! \brief Print a centered line type line to Postscript document.
 *  \par Function Description
 *  This function prints a line when a centered line type is required.
 *  The line is defined by the coordinates of its two ends in
 *  (<B>x1</B>,<B>y1</B>) and (<B>x2</B>,<B>y2</B>).
 *  The Postscript document is defined by the file pointer <B>fp</B>.
 *
 *  A negative value for <B>space</B> or <B>length</B> leads to an
 *  endless loop.
 *
 *  All dimensions are in mils.
 *
 *  The function sets the color in which the line will be printed and the
 *  width of the line - that is the width of the dashes and the diameter
 *  of the dots.
 *
 *  \param [in] w_current     The TOPLEVEL object.
 *  \param [in] fp            FILE pointer to Postscript document.
 *  \param [in] x1            Upper x coordinate.
 *  \param [in] y1            Upper y coordinate.
 *  \param [in] x2            Lower x coordinate.
 *  \param [in] y2            Lower y coordinate.
 *  \param [in] color         Line color.
 *  \param [in] line_width    Width of line.
 *  \param [in] length        Length of a dash.
 *  \param [in] space         Space between dashes.
 *  \param [in] origin_x      Page x coordinate to place line OBJECT.
 *  \param [in] origin_y      Page y coordinate to place line OBJECT.
 */
void o_line_print_center(TOPLEVEL *w_current, FILE *fp,
			 int x1, int y1, int x2, int y2,
			 int color,
			 int line_width, int length, int space,
			 int origin_x, int origin_y)
{
  double dx, dy, l, d;
  double dx1, dy1, dx2, dy2;
  double xa, ya, xb, yb;
  
  if (w_current->print_color) {
    f_print_set_color(fp, color);
  }
  
  fprintf(fp, "[");

  /*
   * Depending on the slope of the line the <B>length</B> (resp. <B>space</B>)
   * parameter is projected on each of the two directions x and y resulting
   * in <B>dx1</B> and <B>dy1</B> (resp. <B>dx2</B> and <B>dy2</B>).
   * Starting from one end and incrementing alternatively by <B>space</B>
   * and <B>length</B> the dashes and dots are printed.
   *
   * It prints as many sets of dash and dot as possible.
   */
  dx = (double) (x2 - x1);
  dy = (double) (y2 - y1);
  l = sqrt((dx * dx) + (dy * dy));
  
  dx1 = (dx * length) / l;
  dy1 = (dy * length) / l;
  
  dx2 = (dx * space) / l;
  dy2 = (dy * space) / l;
  
  d = 0;
  xa = x1; ya = y1;
  while((d + length + 2 * space) < l) {
    d = d + length;
    xb = xa + dx1;
    yb = ya + dy1;
    
    fprintf(fp, "[%d %d %d %d] ", 
	    (int) xa, (int) ya,
	    (int) xb, (int) yb);
    
    d = d + space;
    xa = xb + dx2;
    ya = yb + dy2;
    
    fprintf(fp,"[%d %d] ",(int) xa, (int) ya);
    
    d = d + space;
    xa = xa + dx2;
    ya = ya + dy2;
  }
  /*
   * When the above condition is no more satisfied, then it is not possible
   * to print a dash of length <B>length</B>.
   * However two cases are possible :
   * <DL>
   *   <DT>*</DT><DD>it is possible to print the dash and the dot.
   *   <DT>*</DT><DD>it is possible to print the dash or a part
   *                 of the original dash.
   * </DL>
   */

  if((d + length + space) < l) {
    d = d + length;
    xb = xa + dx1;
    yb = ya + dy1;
    
    fprintf(fp, "[%d %d %d %d] ", 
	    (int) xa, (int) ya,
	    (int) xb, (int) yb);
    
    d = d + space;
    xa = xb + dx2;
    ya = yb + dy2;
    
    fprintf(fp,"[%d %d] ",(int) xa, (int) ya);
    
  } else {
    if(d + length < l) {
      xb = xa + dx1;
      yb = ya + dy1;
    } else {
      xb = x2;
      yb = y2;
    }
    
    fprintf(fp, "[%d %d %d %d] ", 
	    (int) xa, (int) ya,
	    (int) xb, (int) yb);
    
  }
  
  fprintf(fp,"] %d dashed\n", line_width);

  /*
   * A dot is represented by a filled circle. Position of the circle is
   * (<B>xa</B>, <B>ya</B>) and its radius by the <B>line_width</B> parameter.
   */
}

/*! \brief Print a phantom line type line to Postscript document.
 *  \par Function Description
 *  This function prints a line when a phantom line type is required.
 *  The line is defined by the coordinates of its two ends in 
 *  (<B>x1</B>,<B>y1</B>) and (<B>x2</B>,<B>y2</B>).
 *  The Postscript document is defined by the file pointer <B>fp</B>.
 *
 *  A negative value for <B>space</B> or <B>length</B> leads to an
 *  endless loop.
 *
 *  All dimensions are in mils.
 *
 *  The function sets the color in which the line will be printed and the
 *  width of the line - that is the width of the dashes and the diameter
 *  of the dots.
 *
 *  \param [in] w_current     The TOPLEVEL object.
 *  \param [in] fp            FILE pointer to Postscript document.
 *  \param [in] x1            Upper x coordinate.
 *  \param [in] y1            Upper y coordinate.
 *  \param [in] x2            Lower x coordinate.
 *  \param [in] y2            Lower y coordinate.
 *  \param [in] color         Line color.
 *  \param [in] line_width    Width of line.
 *  \param [in] length        Length of a dash.
 *  \param [in] space         Space between dashes.
 *  \param [in] origin_x      Page x coordinate to place line OBJECT.
 *  \param [in] origin_y      Page y coordinate to place line OBJECT.
 */
void o_line_print_phantom(TOPLEVEL *w_current, FILE *fp,
			  int x1, int y1, int x2, int y2,
			  int color,
			  int line_width, int length, int space,
			  int origin_x, int origin_y)
{
  double dx, dy, l, d;
  double dx1, dy1, dx2, dy2;
  double xa, ya, xb, yb;
  
  if (w_current->print_color) {
    f_print_set_color(fp, color);
  }
  
  fprintf(fp,"[");

  /*
   * Depending on the slope of the line the <B>length</B> (resp. <B>space</B>)
   * parameter is projected on each of the two directions x and y resulting
   * in <B>dx1</B> and <B>dy1</B> (resp. <B>dx2</B> and <B>dy2</B>).
   * Starting from one end and incrementing alternatively by <B>space</B>
   * and <B>length</B> the dashes and dots are printed.
   *
   * It prints as many sets of dash-dot-dot as possible.
   */
  dx = (double) (x2 - x1);
  dy = (double) (y2 - y1);
  l = sqrt((dx * dx) + (dy * dy));
  
  dx1 = (dx * length) / l;
  dy1 = (dy * length) / l;
  
  dx2 = (dx * space) / l;
  dy2 = (dy * space) / l;
  
  d = 0;
  xa = x1; ya = y1;
  while((d + length + 3 * space) < l) {
    d = d + length;
    xb = xa + dx1;
    yb = ya + dy1;
    
    fprintf(fp,"[%d %d %d %d] ",
	    (int) xa, (int)ya,
	    (int) xb, (int)yb);
    
    d = d + space;
    xa = xb + dx2;
    ya = yb + dy2;
    
    fprintf(fp,"[%d %d] ",(int) xa, (int) ya);
    
    d = d + space;
    xa = xa + dx2;
    ya = ya + dy2;
    
    fprintf(fp,"[%d %d] ",(int) xa, (int) ya);
    
    d = d + space;
    xa = xa + dx2;
    ya = ya + dy2;
  }
  /*
   * When the above condition is no more satisfied, then it is not possible
   * to print a complete set of dash-dot-dot.
   * However three cases are possible :
   * <DL>
   *   <DT>*</DT><DD>it is possible to print a dash and a dot and a dot.
   *   <DT>*</DT><DD>it is possible to print a dash and a dot.
   *   <DT>*</DT><DD>it is possible to print the dash or a part
   *                 of the original dash.
   * </DL>
   */

  if((d + length + 2 * space) < l) {
    d = d + length;
    xb = xa + dx1;
    yb = ya + dy1;
    
    fprintf(fp,"[%d %d %d %d] ",
	    (int) xa, (int)ya,
	    (int) xb, (int)yb);
    
    d = d + space;
    xa = xb + dx2;
    ya = yb + dy2;
    
    fprintf(fp,"[%d %d] ",(int) xa, (int)ya);
    
    d = d + space;
    xa = xb + dx2;
    ya = yb + dy2;
    
    fprintf(fp,"[%d %d] ",(int) xa, (int)ya);
    
  } else {
    if(d + length + space < l) {
      d = d + length;
      xb = xa + dx1;
      yb = ya + dy1;
      
      fprintf(fp,"[%d %d %d %d] ",
	      (int) xa, (int)ya,
	      (int) xb, (int)yb);
      
      d = d + space;
      xa = xb + dx2;
      ya = yb + dy2;
      
      fprintf(fp,"[%d %d] ",(int) xa, (int)ya);
      
    } else {
      if(d + length < l) {
	xb = xa + dx1;
	yb = ya + dy1;
      } else {
	xb = x2;
	yb = y2;
      }
      
      fprintf(fp,"[%d %d %d %d] ",
	      (int) xa, (int)ya,
	      (int) xb, (int)yb);
      
    }
  }
  
  fprintf(fp,"] %d dashed\n", line_width);
}

#if 0 /* original way of printing line, no longer used */
/*! \brief Print line to Postscript document using old method.
 *  \par Function Description
 *  This function is the old function to print a line.
 *  It does not handle line type.
 *
 *  \param [in] w_current  The TOPLEVEL object.
 *  \param [in] fp         FILE pointer to Postscript document.
 *  \param [in] o_current  Line object to print.
 *  \param [in] origin_x   Page x coordinate to place line OBJECT.
 *  \param [in] origin_y   Page x coordinate to place line OBJECT.
 */
void o_line_print_old(TOPLEVEL *w_current, FILE *fp, OBJECT *o_current, 
		      int origin_x, int origin_y)
{
  if (o_current == NULL) {
    printf("got null in o_line_print\n");
    return;
  }

  if (w_current->print_color) {
    f_print_set_color(fp, o_current->color);
  }

  fprintf(fp, "newpath\n");

  fprintf(fp, "%d mils %d mils moveto\n",
          o_current->line_points->x1-origin_x,
          o_current->line_points->y1-origin_y);
  fprintf(fp, "%d mils %d mils lineto\n", 
          o_current->line_points->x2-origin_x,
          o_current->line_points->y2-origin_y);
  fprintf(fp, "stroke\n");

}
#endif

/*! \brief Draw a line in an image.
 *  \par Function Description
 *  This function draws a line in an image with the libgdgeda function
 *  #gdImageLine().
 *
 *  \param [in] w_current   The TOPLEVEL object.
 *  \param [in] o_current   Line OBJECT to draw.
 *  \param [in] origin_x    (unused).
 *  \param [in] origin_y    (unused).
 *  \param [in] color_mode  Draw line in color if TRUE, B/W otherwise.
 */

void o_line_image_write(TOPLEVEL *w_current, OBJECT *o_current, 
			int origin_x, int origin_y, int color_mode)
{
  int color;

  if (o_current == NULL) {
    printf("got null in o_line_print\n");
    return;
  }

  if (color_mode == TRUE) {
    color = o_image_geda2gd_color(o_current->color);
  } else {
    color = image_black;
  }

  /* assumes screen coords are already calculated correctly */
#ifdef HAS_LIBGDGEDA

  gdImageSetThickness(current_im_ptr, SCREENabs(w_current, 
					        o_current->line_width));

  gdImageLine(current_im_ptr, 
              o_current->line->screen_x[0],
              o_current->line->screen_y[0],
              o_current->line->screen_x[1],
              o_current->line->screen_y[1], 
              color);
#endif
}

/*! \brief
 *  \par Function Description
 *
 *  \param [in] w_current  The TOPLEVEL object.
 *  \param [in] x_scale
 *  \param [in] y_scale
 *  \param [in] object
 */
void o_line_scale_world(TOPLEVEL *w_current, int x_scale, int y_scale,
			OBJECT *object)
{
  if (object == NULL) printf("lsw NO!\n");

  /* scale the line world coords */
  object->line->x[0] = object->line->x[0] * x_scale;
  object->line->y[0] = object->line->y[0] * y_scale;
  object->line->x[1] = object->line->x[1] * x_scale;
  object->line->y[1] = object->line->y[1] * y_scale;

  /* update screen coords */
  o_line_recalc(w_current, object);
  
}

/*! \brief
 *  \par Function Description
 *
 *  \param [in] w_current  The TOPLEVEL object.
 *  \param [in] line
 *  \param [in] x1
 *  \param [in] y1
 *  \param [in] x2
 *  \param [in] y2
 *  \return int
 */
int o_line_visible(TOPLEVEL *w_current, LINE *line, 
		   int *x1, int *y1, int *x2, int *y2)
{
  int visible=0;


  /* don't do clipping if this is false */
  if (!w_current->object_clipping) {
    return(TRUE);
  }

  *x1 = line->screen_x[0];
  *y1 = line->screen_y[0];
  *x2 = line->screen_x[1];
  *y2 = line->screen_y[1];

  visible = SCREENclip_change(w_current, x1, y1, x2, y2);

  return(visible);
}

/*! \brief
 *  \par Function Description
 *
 *  \param [in] object
 *  \return double
 */
double o_line_length(OBJECT *object)
{
  double length;
  double dx, dy;
  
  if (!object->line) {
    return 0.0;
  }

  dx = object->line->x[0]-object->line->x[1];
  dy = object->line->y[0]-object->line->y[1];

  length = sqrt((dx*dx) + (dy*dy));
                
  return(length);
}