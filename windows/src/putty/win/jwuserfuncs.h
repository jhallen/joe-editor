/*
 * JoeWin custom user command handling functions.
 */

#ifndef _JOEWIN_DAVS_H
#define _JOEWIN_DAVS_H

void jwUIProcessPacket(void *handle, struct CommMessage* m);
void jwSendFiles(HDROP, int x, int y);
void jwUpdateJoeColor(void);
void jwSendJoeColor(void *color);
void jwContextMenu(int hasblock);

int jwAnyModified();
void jwUIExit();
void jwShutdownBackend(void *dataptr, int exitcode);

#endif
