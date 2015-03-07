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

#include "jwwin.h"
#include <wchar.h>

#include "jwbentries.h"
#include "jwutils.h"
#include "jwcomm.h"

static struct buffer_entry **find_matching_entry(struct buffer_entry **head, struct buffer_entry *mod)
{
	struct buffer_entry **p = head;
	for (; *p; p = &(*p)->next)
	{
		if ((*p)->id == mod->id)
		{
			break;
		}
	}
	return p;
}

void apply_buffer_updates(struct buffer_entry **head, struct buffer_update *update)
{
	struct buffer_entry **e = find_matching_entry(head, &update->entry);
	struct buffer_entry *m = *e;

	if (update->change_flags & JOE_BUFFER_ADDED)
	{
		struct buffer_entry *created = (struct buffer_entry*)malloc(sizeof(struct buffer_entry));
		wcscpy(created->fname, update->entry.fname);
		created->id = update->entry.id;
		created->flags = update->entry.flags;
		created->next = NULL;

		if (!m)
		{
			/* e points to tail of list. */
			*e = created;
		}
		else
		{
			/* oops!  this was already there! */
			/* deal with it cleanly */
			created->next = m->next;
			*e = created;
			free(m);
		}
	}
	else if (update->change_flags & JOE_BUFFER_DELETED)
	{
		if (m)
		{
			*e = m->next;
			free(m);
		}
	}
	else if (update->change_flags & JOE_BUFFER_CHANGED)
	{
		/* Selected flag is special: Old selected buffer doesn't have to send an update, it is implied. */
		if (update->entry.flags & JOE_BUFFER_SELECTED)
		{
			/* Clear old selected flag */
			struct buffer_entry *be;
			for (be = *head; be; be = be->next)
			{
				if (be->flags & JOE_BUFFER_SELECTED)
				{
					be->flags &= ~JOE_BUFFER_SELECTED;
				}
			}
		}

		if (!m)
		{
			update->change_flags = JOE_BUFFER_ADDED;
			apply_buffer_updates(head, update);
			return;
		}

		/* Set flags now */
		m->flags = update->entry.flags;
	}

	if (update->change_flags & JOE_BUFFER_RENAMED)
	{
		wcscpy((*e)->fname, update->entry.fname);
	}
}

void send_buffer_update(struct buffer_update *bu)
{
	int fnamelen = wcslen(bu->entry.fname);

	if (!fnamelen)
	{
		jwSendComm(JW_FROM_EDITOR,
			    COMM_UPDATEBUFFER,
			    bu->change_flags,
			    bu->entry.flags, 0, 0,
			    bu->entry.id, 0, NULL);
	}
	else
	{
		jwSendComm(JW_FROM_EDITOR,
			    COMM_UPDATEBUFFER,
			    bu->change_flags,
			    bu->entry.flags, 0, 0,
			    bu->entry.id,
			    (fnamelen + 1) * sizeof(wchar_t),
			    (const char*)bu->entry.fname);
	}
}

void finish_buffer_updates()
{
	jwSendComm0(JW_FROM_EDITOR, COMM_DONEBUFFERUPDATE);
}

void unmarshal_buffer_update(struct CommMessage *msg, struct buffer_update *bu)
{
	bu->change_flags = msg->arg1;
	bu->entry.flags = msg->arg2;
	bu->entry.id = msg->ptr;

	if (msg->buffer)
	{
		wcsncpy(bu->entry.fname, (wchar_t*)msg->buffer->buffer, MAX_PATH);
		bu->entry.fname[MAX_PATH - 1] = 0;
	}
	else
	{
		bu->entry.fname[0] = 0;
	}

	bu->entry.next = NULL;
}

int any_buffers_modified(struct buffer_entry *be)
{
	for (; be; be = be->next)
	{
		if (be->flags & JOE_BUFFER_MODIFIED && !(be->flags & JOE_BUFFER_INTERNAL))
		{
			return 1;
		}
	}

	return 0;
}
