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