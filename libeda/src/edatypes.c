#include <libeda/edaobject.h>
#include <libeda/edanet.h>

/* For deserialization we need to have a mapping of
 * characters to type classes they represent */
static EdaObjectClass *char_to_typeclass[128] = {0};

void
eda_types_init()
{
    char_to_typeclass['N'] = g_type_class_ref(EDA_TYPE_NET);
}

void
eda_types_deinit()
{
    g_type_class_unref(char_to_typeclass['N']);
}

EdaObjectClass *
_eda_object_class_from_char(guchar c)
{
    if (c < G_N_ELEMENTS(char_to_typeclass))
        return char_to_typeclass[c];
    return NULL;
}

