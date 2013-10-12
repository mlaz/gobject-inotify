/*
 * ginotify-test.c - test frontend GInotify
 *
 * @Author: Miguel Azevedo <miguellazev@gmail.com>
 */


#include "GInotify.h"

void dir_callback (GInotify *inf, gpointer data)
{
  GInotifyEvent *ev = (GInotifyEvent*) data;
  GString* name;

  if (ev->name == NULL)
    {
      const gchar* name_chr = "The watch";
      name = g_string_new (name_chr);
    }
  else
    name = ev->name;

  const char* type = (ev->isdir == TRUE) ? "dir" : "file";

  switch (ev->what)
    {
    case G_IN_MOVED_FROM:
      g_print ("wd=%d: %s (%s) was moved away,", ev->wd, name->str, type);
      break;

    case G_IN_MOVED_TO:
      g_print ("wd=%d: %s (%s) was moved here,", ev->wd, name->str, type);
      break;

    case G_IN_DELETE:
      g_print ("wd=%d: %s (%s) was deleted,", ev->wd, name->str, type);
      break;

    case G_IN_CREATE:
      g_print ("wd=%d: %s (%s) was created,", ev->wd, name->str, type);
      break;

    default:
      g_print ("This means trouble!");
      return;
    }

  g_print( " cookie=%d\n", ev->cookie);
  if (ev->name == NULL)
    g_string_free (name, TRUE);
}

void changed_callback (GInotify *inf, gpointer data)
{
  GInotifyEvent *ev = (GInotifyEvent*) data;
  GString* name;

  if (ev->name == NULL)
    {
      const gchar* name_chr = "The watch";
      name = g_string_new (name_chr);
    }
  else
    name = ev->name;

  const char* type = (ev->isdir == TRUE) ? "dir" : "file";

  switch (ev->what)
    {
    case G_IN_ATTRIB:
      g_print ("wd=%d: %s's (%s) metadata changed,", ev->wd, name->str, type);
      break;

    case G_IN_MODIFY:
      g_print ("wd=%d: %s (%s) was written,", ev->wd, name->str, type);
      break;

    case G_IN_MOVE_SELF:
      g_print ("wd=%d: %s (%s) was moved,", ev->wd, name->str, type);
      break;

    case G_IN_DELETE_SELF:
      g_print ("wd=%d: The watch was deleted,", ev->wd);
      break;

    default:
      g_print ("This means trouble!\n");
      return;
    }

  g_print( " cookie=%d\n", ev->cookie);
  if (ev->name == NULL)
    g_string_free (name, TRUE);
}

void nochange_callback (GInotify *inf, gpointer data)
{
  GInotifyEvent *ev = (GInotifyEvent*) data;
  GString* name;

  if (ev->name == NULL)
    {
      const gchar* name_chr = "The watch";
      name = g_string_new (name_chr);
    }
  else
    name = ev->name;

  const char* type = (ev->isdir == TRUE) ? "dir" : "file";

  switch (ev->what)
    {
    case G_IN_CLOSE_WRITE:
      g_print ("wd=%d: %s (%s) was closed (was writable),", ev->wd, name->str, type);
      break;

    case G_IN_CLOSE_NOWRITE:
      g_print ("wd=%d: %s (%s) was closed (was not writable),", ev->wd, name->str, type);
      break;

    case G_IN_OPEN:
      g_print ("wd=%d: %s (%s) was opened,", ev->wd, name->str, type);
      break;

    case G_IN_ACCESS:
      g_print ("wd=%d: %s (%s) was read,", ev->wd, name->str, type);
      break;

    default:
      g_print ("This means trouble!\n");
      return;
    }

  g_print( " cookie=%d\n", ev->cookie);
  if (ev->name == NULL)
    g_string_free (name, TRUE);
}

void inf_gen_callback (GInotify *inf, gpointer data)
{
  GInotifyEvent *ev = (GInotifyEvent*) data;
  GString* name;

  if (ev->name == NULL)
    {
      const gchar* name_chr = "The watch";
      name = g_string_new (name_chr);
    }
  else
    name = ev->name;

  switch (ev->what)
    {
    case G_IN_UNMOUNT:
      g_print ("wd=%d: %s was unmounted.\n", ev->wd, name->str);
      break;

    case G_IN_Q_OVERFLOW:
      g_print ("The queue overflowed!\n");
      break;

    case G_IN_IGNORED:
      g_print ("wd=%d: %s is no longer watched.\n", ev->wd, name->str);
      /** here we can check if the file stll exists, if yes restart the watch.
       *  this happens when we edit a file with vi and when we mv another file
       *  to the file we are watching
       */
      break;

    default:
      g_print ("This means trouble!\n");
      return;
    }

  if (ev->name == NULL)
    g_string_free (name, TRUE);
}

void print_event_q (gpointer data)
{
  GInotifyEvent *ev = (GInotifyEvent*) data;
  GString* name;

  if (ev->name == NULL)
    {
      const gchar* name_chr = "The watch";
      name = g_string_new (name_chr);
    }
  else
    name = ev->name;

  const char* type = (ev->isdir == TRUE) ? "dir" : "file";

  switch (ev->what)
    {
    case G_IN_MOVED_FROM:
      g_print ("wd=%d: %s (%s) was moved away,", ev->wd, name->str, type);
      break;

    case G_IN_MOVED_TO:
      g_print ("wd=%d: %s (%s) was moved here,", ev->wd, name->str, type);
      break;

    case G_IN_DELETE:
      g_print ("wd=%d: %s (%s) was deleted,", ev->wd, name->str, type);
      break;

    case G_IN_CREATE:
      g_print ("wd=%d: %s (%s) was created,", ev->wd, name->str, type);
      break;

    case G_IN_ATTRIB:
      g_print ("wd=%d: %s's (%s) metadata changed,", ev->wd, name->str, type);
      break;

    case G_IN_MODIFY:
      g_print ("wd=%d: %s (%s) was written,", ev->wd, name->str, type);
      break;

    case G_IN_MOVE_SELF:
      g_print ("wd=%d: %s (%s) was moved,", ev->wd, name->str, type);
      break;

    case G_IN_DELETE_SELF:
      g_print ("wd=%d: The watch was deleted,", ev->wd);
      break;

    case G_IN_CLOSE_WRITE:
      g_print ("wd=%d: %s (%s) was closed (was writable),", ev->wd, name->str, type);
      break;

    case G_IN_CLOSE_NOWRITE:
      g_print ("wd=%d: %s (%s) was closed (was not writable),", ev->wd, name->str, type);
      break;

    case G_IN_OPEN:
      g_print ("wd=%d: %s (%s) was opened,", ev->wd, name->str, type);
      break;

    case G_IN_ACCESS:
      g_print ("wd=%d: %s (%s) was read,", ev->wd, name->str, type);
      break;

    case G_IN_UNMOUNT:
      g_print ("wd=%d: %s was unmounted.\n", ev->wd, name->str);
      break;

    case G_IN_Q_OVERFLOW:
      g_print ("The queue overflowed!\n");
      break;

    case G_IN_IGNORED:
      g_print ("wd=%d: %s is no longer watched.\n", ev->wd, name->str);
      break;

    default:
      g_print ("This means trouble!");
      return;
    }

  g_print( " cookie=%d\n", ev->cookie);
  if (ev->name == NULL)
    g_string_free (name, TRUE);
}

void event_q_callback (GInotify *inf, gpointer data)
{
  GList* list = data;
  list = g_list_first (list);
  g_list_foreach (list, (GFunc) print_event_q, NULL);
}

int
main (int argc, char *argv[])
{
  GMainLoop *mainloop;
  GInotify *inf;
  int i;

  if (argc < 2) {
    g_warning ("Usage: %s [directories or files ...]\n", argv[0]);
    return -1;
  }


  inf = g_inotify_new ();
  if (!inf)
    return -1;

  g_signal_connect (inf, "directory-specific", G_CALLBACK (dir_callback), inf);
  g_signal_connect (inf, "attrib-modify-move-delete", G_CALLBACK (changed_callback), inf);
  g_signal_connect (inf, "open-close-read", G_CALLBACK (nochange_callback), inf);
  g_signal_connect (inf, "inotify-general", G_CALLBACK (inf_gen_callback), inf);
  g_signal_connect (inf, "event-queue", G_CALLBACK (event_q_callback), inf);

  for (i = 1; i < argc; i++) {
    g_print ("Adding watch on %s\n", argv[i]);
    g_inotify_add_watch (inf, argv[i], G_IN_ALL_EVENTS);
  }

  mainloop = g_main_new (FALSE);
  g_main_run (mainloop);

  g_object_unref(inf);
  return 0;
}
