#include <glib.h>

void rotate_point_90(gint *x, gint *y, gint angle)
{
    double costheta = 1;
    double sintheta = 0;
    gint x_, y_;

    /* I could have used sine/cosine for this, but I want absolute
     * accuracy */
    switch(angle) {

        case(0):
            return;
            break;

        case(90):
            costheta = 0;
            sintheta = 1;
            break;

        case(180):
            costheta = -1;
            sintheta = 0;
            break;

        case(270):
            costheta = 0;
            sintheta = -1;
            break;
    }

    x_ = *x;
    y_ = *y;

    *x = x_ * costheta - y_ * sintheta;
    *y = x_ * sintheta + y_ * costheta;
}

