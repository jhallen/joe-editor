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

#ifndef _JOEWIN_JWCOMM_H
#define _JOEWIN_JWCOMM_H

#define JW_FROM_EDITOR		0
#define JW_TO_UI		0
#define JW_FROM_UI		1
#define JW_TO_EDITOR		1

#define EDITOR_TO_UI_BUFSZ	1024
#define UI_TO_EDITOR_BUFSZ	1024
#define MPX_BUFSZ		1024

#if UI_TO_EDITOR_BUFSZ < PATH_MAX || EDITOR_TO_UI_BUFSZ < PATH_MAX
#error ERROR: Buffers are not large enough for file names
#endif

#define COMM_WINRESIZE		1
#define COMM_EXIT		2
#define COMM_DROPFILES		3
#define COMM_COLORSCHEME	4
#define COMM_UPDATEBUFFER	5
#define COMM_DONEBUFFERUPDATE	6
#define COMM_RENDEZVOUS		7
#define COMM_CONTEXTMENU	8
#define COMM_EXEC		9
#define COMM_ACK		10
#define COMM_MPXDATA		11
#define COMM_VTSIZE		12

#define COMM_IO_1		0x80
#define COMM_IO_2		0x81
#define COMM_IO_3		0x82
#define COMM_IO_4		0x83
#define COMM_IO_5		0x84
#define COMM_IO_6		0x85
#define COMM_IO_7		0x86
#define COMM_IO_8		0x87
#define COMM_IO_9		0x88
#define COMM_IO_10		0x89
#define COMM_IO_11		0x8A
#define COMM_IO_12		0x8B
#define COMM_IO_13		0x8C
#define COMM_IO_14		0x8D
#define COMM_IO_15		0x8E
#define COMM_IO_16		0x8F
#define COMM_IO_N		0x90

#define JWISIO(m)		((m)->msg >= COMM_IO_1 && (m)->msg <= COMM_IO_N)

#define JW_PACKIOSZ		16
#define EDITOR_TO_UI_IOSZ	(JW_PACKIOSZ + EDITOR_TO_UI_BUFSZ)
#define UI_TO_EDITOR_IOSZ	(JW_PACKIOSZ + UI_TO_EDITOR_BUFSZ)

struct CommBuffer
{
    int                 size;
    struct CommBuffer   *next;
    char                buffer[0];
};

struct CommMessage
{
    int                 msg;
    int                 arg1;
    int                 arg2;
    int                 arg3;
    int                 arg4;
    void                *ptr;
    struct CommBuffer   *buffer;
};

int jwCreateWake(void);
HANDLE jwGetWakeEvent(int hwake);
int jwCreateQueue(int bufsz, int hwake);
void jwCloseQueue(int qd);
HANDLE jwInitializeComm(void);
void jwShutdownComm(void);
struct CommMessage *jwWaitForComm(int *qds, int nqds, int timeout, int *outqueue);
struct CommMessage *jwRecvComm(int qd);
void jwSendComm(int qd, int msg, int arg1, int arg2, int arg3, int arg4, void *ptr, int sz, const char *data);
void jwReleaseComm(int qd, struct CommMessage *msg);

int jwRendezvous(int readqd, int writeqd);
void jwWriteIO(int qd, void *data, int size);
int jwReadIO(struct CommMessage *m, void *data);

#define jwSendComm0(side,op)			(jwSendComm((side),(op),0,0,0,0,0,0,0))
#define jwSendComm0s(side,op,s)			(jwSendComm((side),(op),0,0,0,0,0,strlen(s)+1,(s)))
#define jwSendComm0p(side,op,p)			(jwSendComm((side),(op),0,0,0,0,(p),0,0))
#define jwSendComm0d(side,op,d,s)		(jwSendComm((side),(op),0,0,0,0,0,(s),(d)))
#define jwSendComm0pd(side,op,p,d,s)		(jwSendComm((side),(op),0,0,0,0,(p),(s),(d)))
#define jwSendComm1(side,op,a1)			(jwSendComm((side),(op),(a1),0,0,0,0,0,0))
#define jwSendComm2(side,op,a1,a2)		(jwSendComm((side),(op),(a1),(a2),0,0,0,0,0))
#define jwSendComm3s(side,op,a1,a2,a3,s)	(jwSendComm((side),(op),(a1),(a2),(a3),0,0,strlen(s) + 1,(s)))
#define jwSendComm3(side,op,a1,a2,a3)		(jwSendComm((side),(op),(a1),(a2),(a3),0,0,0,0))

#endif // _JOEWIN_JWCOMM_H
