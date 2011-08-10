#include <libeda/edaconfig.h>

/**
 * SECTION:edaconfig
 * @short_description: An object representing a configuration.
 * @title: EdaConfig
 *
 * XXX
 */

struct _EdaConfig
{
    GKeyFile *keyfile;
};

static EdaConfig *
make_default_config()
{
    EdaConfig *cfg;
    GKeyFile *keyfile;

    cfg = g_slice_new(EdaConfig);
    cfg->keyfile = keyfile = g_key_file_new();

    eda_config_set_int(cfg, "graphical", "net-width", 10);
    return cfg;
}

/**
 * eda_config_get_default:
 *
 * Returns the default, built-in config.
 *
 * Returns: an #EdaConfig.
 */
EdaConfig *
eda_config_get_default()
{
    static EdaConfig *cfg = NULL;

    if(cfg == NULL)
        cfg = make_default_config();
    return cfg;
}

/* XXX: we should translate errors from the glib domain to eda domain */
gint
eda_config_lookup_int(EdaConfig *cfg, const gchar *group, const gchar *key, GError **err)
{
    return g_key_file_get_integer(cfg->keyfile, group, key, err);
}

gdouble
eda_config_lookup_double(EdaConfig *cfg, const gchar *group, const gchar *key, GError **err)
{
    return g_key_file_get_double(cfg->keyfile, group, key, err);
}

gboolean
eda_config_lookup_boolean(EdaConfig *cfg, const gchar *group, const gchar *key, GError **err)
{
    return g_key_file_get_boolean(cfg->keyfile, group, key, err);
}

gint
eda_config_get_int(EdaConfig *cfg, const gchar *group, const gchar *key)
{
    gint result;
    GError *err = NULL;
    
    result = eda_config_lookup_int(cfg, group, key, &err);
    if(err) {
        g_clear_error(&err);
        result = eda_config_lookup_int(eda_config_get_default(), group, key, &err);
        g_assert(err == NULL);
    }
    return result;
}

gdouble
eda_config_get_double(EdaConfig *cfg, const gchar *group, const gchar *key)
{
    gdouble result;
    GError *err = NULL;
    
    result = eda_config_lookup_double(cfg, group, key, &err);
    if(err) {
        g_clear_error(&err);
        result = eda_config_lookup_double(eda_config_get_default(), group, key, &err);
        g_assert(err == NULL);
    }
    return result;
}

gboolean
eda_config_get_boolean(EdaConfig *cfg, const gchar *group, const gchar *key)
{
    gboolean result;
    GError *err = NULL;
    
    result = eda_config_lookup_boolean(cfg, group, key, &err);
    if(err) {
        g_clear_error(&err);
        result = eda_config_lookup_boolean(eda_config_get_default(), group, key, &err);
        g_assert(err == NULL);
    }
    return result;
}

void
eda_config_set_int(EdaConfig *cfg, const char *group, const char *key, gint value)
{
    g_key_file_set_integer(cfg->keyfile, group, key, value);
}

void
eda_config_set_double (EdaConfig *cfg, const char *group, const char *key, gdouble value)
{
    g_key_file_set_double(cfg->keyfile, group, key, value);
}

void
eda_config_set_boolean(EdaConfig *cfg, const char *group, const char *key, gboolean value)
{
    g_key_file_set_boolean(cfg->keyfile, group, key, value);
}

