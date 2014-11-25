/*
 *  This file is part of Joe's Own Editor for Windows.
 *  Copyright (c) 2014 John J. Jordan.
 *
 *  Joe's Own Editor for Windows is free software: you can redistribute it 
 *  and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of the 
 *  License, or (at your option) any later version.
 *
 *  Joe's Own Editor for Windows is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Joe's Own Editor for Windows.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#include <wchar.h>
#include "types.h"
#include "jwbentries.h"

static int make_update(B *buffer, struct buffer_update *bu, int include_name);
static B *get_selected();

static int changes_pending = 0;
static int deletes_pending = 0;
static int selection_changed = 0;
static struct buffer_entry *local_copy = NULL;

void send_buffer_changes()
{
	int updated = 0;

	/* Handle deletes */
	if (deletes_pending)
	{
		struct buffer_entry *be, *next;
		B *b;

		/* Quadratic search sucks, but it's really the best we can do short of heavier data structures */
		for (be = local_copy; be; be = next)
		{
			int found = 0;

			for (b = bufs.link.next; b != &bufs; b = b->link.next)
			{
				if (be->id == b)
				{
					found = 1;
					break;
				}
			}

			next = be->next;

			if (!found)
			{
				struct buffer_update bu;
				bu.change_flags = JOE_BUFFER_DELETED;
				bu.entry.id = be->id;
				bu.entry.fname[0] = 0;

				/* If this was the selected buffer, then there will be a new selection. */
				if (be->flags & JOE_BUFFER_SELECTED)
				{
					selection_changed = 1;
				}

				send_buffer_update(&bu);
				apply_buffer_updates(&local_copy, &bu);
				++updated;
			}
		}
	}

	/* Selection change? */
	if (selection_changed)
	{
		B *b = get_selected();

		if (b)
		{
			struct buffer_entry *be;

			/* Make sure selection did actually change and clear old flags */
			for (be = local_copy; be; be = be->next)
			{
				if (be->flags & JOE_BUFFER_SELECTED)
				{
					if (be->id == b)
					{
						selection_changed = 0;
						break;
					}
					else
					{
						/* No need to notify_changed_buffer here...
						** When the other end hears about the new
						** selection, it will unset any previously
						** selected bentry, so we must do the same. */
						be->flags &= ~JOE_BUFFER_SELECTED;
					}
				}
			}

			if (selection_changed)
			{
				notify_changed_buffer(b);
			}
		}

		selection_changed = 0;
	}

	/* Send adds & changes for each buffer */
	if (changes_pending)
	{
		B *b;
		for (b = bufs.link.next; b != &bufs; b = b->link.next)
		{
			if (b->pchanges)
			{
				struct buffer_update bu;

				/* Precedence */
				if (b->pchanges & JOE_BUFFER_ADDED || !(b->pchanges & JOE_BUFFER_SYNCED))
				{
					bu.change_flags = JOE_BUFFER_ADDED;
				}
				else if (b->pchanges & JOE_BUFFER_RENAMED)
				{
					bu.change_flags = JOE_BUFFER_RENAMED | JOE_BUFFER_CHANGED;
				}
				else if (b->pchanges & JOE_BUFFER_CHANGED)
				{
					bu.change_flags = JOE_BUFFER_CHANGED;
				}
				else
				{
					continue;
				}

				if (make_update(b, &bu, b->pchanges & (JOE_BUFFER_ADDED | JOE_BUFFER_RENAMED)))
				{
					send_buffer_update(&bu);
					apply_buffer_updates(&local_copy, &bu);
					++updated;
				}

				b->pchanges = JOE_BUFFER_SYNCED;
			}
		}
	}

	changes_pending = deletes_pending = 0;

	if (updated)
	{
		finish_buffer_updates();
	}
}

void notify_new_buffer(B *buffer)
{
	buffer->pchanges |= JOE_BUFFER_ADDED;
	changes_pending++;
	/* An add probably implies a selection change too */
	notify_selection();
}

void notify_renamed_buffer(B *buffer)
{
	buffer->pchanges |= JOE_BUFFER_RENAMED | JOE_BUFFER_CHANGED;
	changes_pending++;
}

void notify_changed_buffer(B *buffer)
{
	buffer->pchanges |= JOE_BUFFER_CHANGED;
	changes_pending++;
}

void notify_deleting_buffer()
{
	deletes_pending++;
}

void notify_selection()
{
	selection_changed++;
}

static B *get_selected()
{
	W *w;

	for (w = maint->curwin; w != NULL; w = w->win)
	{
		if (w->watom->what & TYPETW)
		{
			BW* bw = (BW*)w->object;
			return bw->b;
		}
	}

	return NULL;
}

static int make_update(B *buffer, struct buffer_update *bu, int include_name)
{
	W *w;
	struct buffer_entry *be; /* The one we'll send */
	struct buffer_entry *loc; /* Our local copy */
	int namechanged = 0;

	be = &bu->entry;
	
	for (loc = local_copy; loc; loc = loc->next)
	{
		if (loc->id == buffer)
		{
			break;
		}
	}

	be->next = NULL;
	be->id = buffer;
	be->fname[0] = 0;

	if (include_name && buffer->name)
	{
		utf8towcs(be->fname, (char*)buffer->name, MAX_PATH);

		if (loc && !wcscmp(be->fname, loc->fname))
		{
			/* Name hasn't changed */
			be->fname[0] = 0;
			bu->change_flags &= ~JOE_BUFFER_RENAMED;
		}
		else
		{
			namechanged = 1;
		}
	}

	be->flags =
		  (buffer->orphan ? JOE_BUFFER_ORPHANED : 0)
		| (buffer->changed ? JOE_BUFFER_MODIFIED : 0)
		| (buffer->scratch ? JOE_BUFFER_NEWFILE : 0)
		| (buffer->internal ? JOE_BUFFER_INTERNAL : 0)
		| (buffer == get_selected() ? JOE_BUFFER_SELECTED : 0);

	return !loc || namechanged || be->flags != loc->flags;
}
