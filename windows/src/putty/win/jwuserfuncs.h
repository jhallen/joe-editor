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

int jwCanExit();
void jwUIExit();
void jwShutdownBackend(void *dataptr, int exitcode);

#endif
