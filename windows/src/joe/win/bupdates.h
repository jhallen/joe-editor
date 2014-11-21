#ifndef _JOEWIN_BUPDATES_H
#define _JOEWIN_BUPDATES_H

void notify_new_buffer(B *buffer);
void notify_renamed_buffer(B *buffer);
void notify_changed_buffer(B *buffer);
void notify_deleting_buffer();
void notify_selection();
void send_buffer_changes();

#endif