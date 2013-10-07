/*
 * GInotify - A GObject based inotify facility.
 *
 * @Author: Miguel Azevedo <migullazev@gmail.com>
 */

#include <stdio.h>
#include <sys/inotify.h>

#include "GInotify.h"

G_DEFINE_TYPE (GInotify, g_inotify, G_TYPE_OBJECT)

#define G_INOTIFY_GET_PRIVATE(obj)  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), G_TYPE_INOTIFY, GInotifyPrivate))

#define INOTIFY_BUF (10 * (sizeof (struct inotify_event) + NAME_MAX + 1))

struct _GInotifyPrivate
{
  gint fd;
  GIOChannel *gio;
};

enum
{
  DIR_SPEC,
  F_CHANGE,
  F_NOCHANGE,
  INF_GEN,
  LAST_SIGNAL
};

static guint g_inotify_signals [LAST_SIGNAL] = { 0 };

/* Uitls */
static void
g_inotify_emit (const gchar* name,
                gint wd,
                unsigned int event,
                unsigned int cookie,
                GInotify* self)
{
  GInotifyEvent *ev = (GInotifyEvent*) g_malloc0 (sizeof (GInotifyEvent));
  gboolean isdir =  (event & IN_ISDIR) ? TRUE : FALSE;

  ev->wd = wd;
  ev->name = (name) ? g_string_new(name) : NULL;
  ev->isdir = (event & IN_ISDIR) ? TRUE : FALSE;
  ev->cookie = cookie;
  ev->type = 0;

  if (event & (IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE | IN_CREATE))
    {
      ev->type = (event & IN_MOVED_FROM) ? G_IN_MOVED_FROM : ev->type;
      ev->type = (event & IN_MOVED_TO) ? G_IN_MOVED_TO : ev->type;
      ev->type = (event & IN_DELETE) ? G_IN_DELETE : ev->type;
      ev->type = (event & IN_CREATE) ? G_IN_CREATE : ev->type;

      g_signal_emit (self, g_inotify_signals[DIR_SPEC], 0, (gpointer) ev);
      if (ev->name)
        g_string_free (ev->name, TRUE);
      g_free(ev);
      return;
    }

  if (event & (IN_ATTRIB | IN_MODIFY | IN_MOVE_SELF | IN_DELETE_SELF))
    {
      ev->type = (event & IN_ATTRIB) ? G_IN_ATTRIB : ev->type;
      ev->type = (event & IN_MODIFY) ? G_IN_MODIFY : ev->type;
      ev->type = (event & IN_MOVE_SELF) ? G_IN_MOVE_SELF : ev->type;
      ev->type = (event & IN_DELETE_SELF) ? G_IN_DELETE_SELF : ev->type;

      g_signal_emit (self, g_inotify_signals[F_CHANGE], 0, (gpointer) ev);
      if (ev->name)
        g_string_free (ev->name, TRUE);
      g_free(ev);
      return;
    }

  if (event & (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_OPEN | IN_ACCESS))
    {
      ev->type = (event & IN_CLOSE_WRITE) ? G_IN_CLOSE_WRITE : ev->type;
      ev->type = (event & IN_CLOSE_NOWRITE) ? G_IN_CLOSE_NOWRITE : ev->type;
      ev->type = (event & IN_OPEN) ? G_IN_OPEN : ev->type;
      ev->type = (event & IN_ACCESS) ? G_IN_ACCESS : ev->type;

      g_signal_emit (self, g_inotify_signals[F_NOCHANGE], 0, (gpointer) ev);
      if (ev->name)
        g_string_free (ev->name, TRUE);
      g_free(ev);
      return;
    }

  if (event & (IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED))
    {
      ev->type = (event & IN_UNMOUNT) ? G_IN_UNMOUNT : ev->type;
      ev->type = (event & IN_Q_OVERFLOW) ? G_IN_Q_OVERFLOW : ev->type;
      ev->type = (event & IN_IGNORED) ? G_IN_IGNORED : ev->type;

      g_signal_emit (self, g_inotify_signals[INF_GEN], 0, (gpointer) ev);
      if (ev->name)
        g_string_free (ev->name, TRUE);
      g_free(ev);
      return;
    }
}

/* Callbacks */
static gboolean
on_fd_input (GIOChannel *gio, GIOCondition condition, gpointer data)
{
  char buf[INOTIFY_BUF];
  GInotify *self = (GInotify*) data;
  GError *err = NULL;
  gsize len = 0;
  guint i = 0;

  /* read in as many pending events as we can */
  g_io_channel_read_chars (gio, buf, INOTIFY_BUF, &len, &err);
  if (err != NULL)
    {
      g_warning ("Error reading the inotify fd: %s\n", err->message);
      return FALSE;
    }

  /* reconstruct each event and fire a signal for that event */
  while (i < len)
    {
      const char *name = NULL;
      struct inotify_event *event;

      event = (struct inotify_event *) &buf[i];
      if (event->len)
        name = &buf[i] + sizeof (struct inotify_event);

      g_inotify_emit (name, event->wd, event->mask, event->cookie, self);


      i += sizeof (struct inotify_event) + event->len;
    }

  return TRUE;
}

/* Public Methods */
gint
g_inotify_rm_watch (GInotify *g_inf, gint wd)
{//Untested!!
  GInotifyPrivate *priv = g_inf->priv;

  gint ret = inotify_rm_watch (priv->fd, wd);
  if (ret < 0)
    {
      perror ("inotify_rm_watch error!\n");
      return -1;
    }
  return 0;
}

gint
g_inotify_add_watch (GInotify *g_inf, const gchar *name, guint32 mask)
{
  gint  wd;
  GInotifyPrivate *priv = g_inf->priv;

  wd = inotify_add_watch (priv->fd, name, mask);
  if (wd < 0)
    {
      perror ("inotify_add_watch error!\n");
      return -1;
    }

  return wd;
}

static void
g_inotify_dispose (GObject *object)
{//Untested!!
  GInotify *self = G_INOTIFY (object);
  GInotifyPrivate *priv = self->priv;
  GError *err = NULL;
  GIOStatus status;

  status = g_io_channel_shutdown (priv->gio, TRUE, &err);
  if (status == G_IO_STATUS_ERROR)
    {
      g_warning ("Couldn't close GIOChannel %s\n", err->message);
      g_error_free (err);
    }
  g_clear_object (&priv->gio);
}

GInotify *
g_inotify_new ()
{
  GInotify *g_inotify;
  GInotifyPrivate *priv;
  GError *err = NULL;

  g_inotify = G_INOTIFY (g_object_new (G_TYPE_INOTIFY, NULL, NULL) );
  priv = g_inotify->priv;

  priv->fd = inotify_init1 (IN_NONBLOCK);
  if (priv->fd < 0)
    {
      g_warning ("intotify_init error\n");
      g_object_unref (g_inotify);
      return NULL;
    }

  priv->gio = g_io_channel_unix_new (priv->fd);
  if (!priv->gio)
    {
      g_warning ("Couldn't open GIOChannel %s\n", err->message);
      g_error_free (err);
      g_object_unref (g_inotify);
      return NULL;
    }

  g_io_channel_set_encoding (priv->gio, NULL, &err);
  if (err != NULL)
    {
      g_warning ("Error Setting up encoding for gio channel: %s\n", err->message);
      g_error_free (err);
    }
  g_print ("%#010x\n", IN_MOVE_SELF);
  g_io_add_watch (priv->gio, G_IO_IN, on_fd_input, g_inotify);

  return g_inotify;
}

static void
g_inotify_init (GInotify *self)
{
  GInotifyPrivate *priv;

  self->priv = G_INOTIFY_GET_PRIVATE (self);
  priv = self->priv;

  priv->fd = 0;
  priv->gio = NULL;
}

static void
g_inotify_class_init (GInotifyClass *klass)
{
  GObjectClass *object_class;

  g_inotify_parent_class = g_type_class_peek_parent (klass);

  object_class = G_OBJECT_CLASS (klass);
  object_class->dispose = g_inotify_dispose;

  g_type_class_add_private (klass, sizeof (GInotifyPrivate));

  g_inotify_signals[DIR_SPEC] =
    g_signal_new ("directory-specific",
                  G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1, G_TYPE_POINTER);

  g_inotify_signals[F_CHANGE] =
    g_signal_new ("attrib-modify-move-delete",
                   G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST,
                   0, NULL, NULL,
                   g_cclosure_marshal_VOID__POINTER,
                   G_TYPE_NONE, 1, G_TYPE_POINTER);

  g_inotify_signals[F_NOCHANGE] =
    g_signal_new ("open-close-read",
                  G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1, G_TYPE_POINTER);

  g_inotify_signals[INF_GEN] =
    g_signal_new ("inotify-general",
                  G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1, G_TYPE_POINTER);
}
