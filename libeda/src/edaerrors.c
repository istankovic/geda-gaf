#include <glib.h>

/* Used by EDA_ERROR. Returns a GQuark used to identify GErrors
 * originating in libeda. Should not be called directly. */
GQuark
eda_error_quark (void)
{
  return g_quark_from_static_string ("eda-error-quark");
}
