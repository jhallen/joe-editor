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

#ifndef _JOEWIN_JWBENTRIES_H
#define _JOEWIN_JWBENTRIES_H

struct buffer_entry {
	struct buffer_entry*	next;
	void*			id;
	wchar_t			fname[MAX_PATH];
	int			flags;
};

struct buffer_update {
	int			change_flags;
	struct buffer_entry	entry;
};

#define JOE_BUFFER_MODIFIED		0x01
#define JOE_BUFFER_SELECTED		0x02
#define JOE_BUFFER_ORPHANED		0x04
#define JOE_BUFFER_NEWFILE		0x08
#define JOE_BUFFER_INTERNAL		0x10
#define JOE_BUFFER_VT			0x20
#define JOE_BUFFER_SCRATCH		0x40
#define JOE_BUFFER_HASPROCESS		0x80

#define JOE_IGNORE_BUFFERS		(JOE_BUFFER_VT | JOE_BUFFER_SCRATCH | JOE_BUFFER_INTERNAL | JOE_BUFFER_ORPHANED)

#define JOE_BUFFER_ADDED		0x01
#define JOE_BUFFER_DELETED		0x02
#define JOE_BUFFER_CHANGED		0x04
#define JOE_BUFFER_RENAMED		0x08
#define JOE_BUFFER_SYNCED		0x10 /* marked on B structure if ADD was ever sent */

void apply_buffer_updates(struct buffer_entry **head, struct buffer_update *update);
void send_buffer_update(struct buffer_update *bu);
void finish_buffer_updates();
void unmarshal_buffer_update(struct CommMessage *msg, struct buffer_update *bu);
int any_buffers_modified(struct buffer_entry *be);

#endif