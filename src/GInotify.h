/*
 * GInotify - backend for really simple inotify example
 *
 * @Author: Miguel Azevedo <migullazev@gmail.com>
 */

/* inclusion guard */
#ifndef __G_INOTIFY_H__
#define __G_INOTIFY_H__

#include <glib-object.h>

/*
 * Type macros.
 */
#define G_TYPE_INOTIFY                  (g_inotify_get_type ())
#define G_INOTIFY(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_INOTIFY, GInotify))
#define G_IS_INOTIFY(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_INOTIFY))
#define G_INOTIFY_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_INOTIFY, GInotifyClass))
#define G_IS_INOTIFY_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_INOTIFY))
#define G_INOTIFY_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_INOTIFY, GInotifyClass))

typedef struct _GInotify        GInotify;
typedef struct _GInotifyClass   GInotifyClass;
typedef struct _GInotifyPrivate GInotifyPrivate;

struct _GInotify
{
  GObject parent_instance;

  /*< private >*/
  GInotifyPrivate *priv;
};

struct _GInotifyClass
{
  GObjectClass parent_class;

  /* class members */
};

/* used by G_TYPE_INOTIFY */
GType g_inotify_get_type (void);

/* GInotifyEvent*/

typedef struct GInotifyEvent
{
  guint wd;
  guint32 what;
  gboolean isdir;
  guint32 cookie;
  GString *name;
} GInotifyEvent;

/* Option and event mask macros */

#define G_IN_ACCESS		0x00000001	/* File was accessed */
#define G_IN_MODIFY		0x00000002	/* File was modified */
#define G_IN_ATTRIB		0x00000004	/* Metadata changed */
#define G_IN_CLOSE_WRITE	0x00000008	/* Writtable file was closed */
#define G_IN_CLOSE_NOWRITE	0x00000010	/* Unwrittable file closed */
#define G_IN_OPEN		0x00000020	/* File was opened */
#define G_IN_MOVED_FROM		0x00000040	/* File was moved from X */
#define G_IN_MOVED_TO		0x00000080	/* File was moved to Y */
#define G_IN_CREATE		0x00000100	/* Subfile was created */
#define G_IN_DELETE		0x00000200	/* Subfile was deleted */
#define G_IN_DELETE_SELF	0x00000400	/* Self was deleted */
#define G_IN_MOVE_SELF          0x00000800      /* Self was moved */

/* the following are legal events.  they are sent as needed to any watch */
#define G_IN_UNMOUNT		0x00002000	/* Backing fs was unmounted */
#define G_IN_Q_OVERFLOW		0x00004000	/* Event queued overflowed */
#define G_IN_IGNORED		0x00008000	/* File was ignored */

/* helper events */
#define G_IN_CLOSE		(G_IN_CLOSE_WRITE | G_IN_CLOSE_NOWRITE) /* close */
#define G_IN_MOVE      		(G_IN_MOVED_FROM | G_IN_MOVED_TO) /* moves */

/* special flags */
#define G_IN_ISDIR		0x40000000	/* event occurred against dir */
#define G_IN_ONESHOT		0x80000000	/* only send event once */

#define G_IN_ALL_EVENTS	(G_IN_ACCESS | G_IN_MODIFY | G_IN_ATTRIB | G_IN_CLOSE_WRITE | \
			 G_IN_CLOSE_NOWRITE | G_IN_OPEN | G_IN_MOVED_FROM | G_IN_MOVE_SELF | \
			 G_IN_MOVED_TO | G_IN_DELETE | G_IN_CREATE | G_IN_DELETE_SELF)

/*
 * Method definitions.
 */
GInotify *g_inotify_new ();
gint g_inotify_add_watch (GInotify *g_inf, const gchar *name, guint32 mask);
gint g_inotify_rm_watch (GInotify *g_inf, gint wd);

#endif /* __G_INOTIFY_H__ */
