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
 * Communication backend for JoeWin
 *
 * Uses a lock-free, thread-safe queue implementation to route messages
 * between JoeWin's front and backend threads.	There is one CommQueue
 * object for each direction.  Each CommQueue is composed of two TSQueue's:
 * one delivers the messages, and the second returns the message and
 * buffer objects to the sender for reuse.  This is an effort to prevent
 * memory allocations for each message sent.
 *
 * Lock-free queue implementation based on Steve Gilham's C++ implementation from
 * http://tinesware.blogspot.com/2009/07/simple-lockfree-queue-c-windows.html
 * which is itself based on Herb Sutter's Dr. Dobbs article, found at
 * http://drdobbs.com/cpp/210604448.
 */

#include "jwwin.h"
#include <assert.h>
#include <stdio.h>

#include "jwcomm.h"


/************************* Thread-safe queue */

struct TSQueueNode;

struct TSQueueNode
{
	void			*value;
	struct TSQueueNode	*next;
};

struct TSQueue
{
	struct TSQueueNode	*first;
	struct TSQueueNode	*divider;
	struct TSQueueNode	*last;
	struct TSQueueNode	*freelist;
	struct TSQueueNode	dummy;
	int			nodeallocs;
};

typedef struct TSQueue Q;
typedef struct TSQueueNode N;

static Q *CreateTSQueue();
static void DeleteTSQueue(Q*);
static void TSEnqueue(Q*, void*);
static void *TSDequeue(Q*);


/************************* Comm Queue */

struct CommQueue
{
	struct CommBuffer	*freebuffers;
	struct CommMessage	*freemsgs;
	
	int			buffersize;
	
	Q			*queue;
	Q			*returnqueue;

	int			bufferallocs;
	int			msgallocs;
	int			hwake;
};

static struct CommQueue *CreateCommQueue(int buffersize, int hwake);
static void DeleteCommQueue(struct CommQueue *queue);
static struct CommMessage *CreateCommMessage(struct CommQueue *queue, int buffer);
static void EnqueueCommMessage(struct CommQueue *queue, struct CommMessage *m);
static struct CommMessage *DequeueCommMessage(struct CommQueue *queue);
static void ReleaseCommMessage(struct CommQueue *queue, struct CommMessage *m);


/************************* Queue globals */

static struct CommQueue **queues = NULL;
static int qlen = 0;

static HANDLE *qhandles = NULL;
static int nqhandles = 0;

/*
static struct CommQueue *editorToUi, *uiToEditor;

static HANDLE editorWaitHandle, uiWaitHandle;

#define SideQR(s) ((s) == JW_SIDE_EDITOR ? uiToEditor : editorToUi)
#define SideQW(s) ((s) == JW_SIDE_EDITOR ? editorToUi : uiToEditor)
#define SideES(s) ((s) == JW_SIDE_EDITOR ? uiWaitHandle : editorWaitHandle)
#define SideEW(s) ((s) == JW_SIDE_EDITOR ? editorWaitHandle : uiWaitHandle)
*/

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////   Thread-safe queue implementation	  //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

static Q *CreateTSQueue()
{
	Q *result = (Q*)malloc(sizeof(Q));
	result->first = result->last = result->divider = &result->dummy;
	result->freelist = NULL;
	result->dummy.next = NULL;
	result->dummy.value = NULL;
	result->nodeallocs = 0;
	return result;
}

static void DeleteTSQueue(Q *queue)
{
	N *n, *next;

	for (n = queue->freelist; n; n = next)
	{
		next = n->next;
		if (n != &queue->dummy)
		{
			free(n);
		}
	}

	free(queue);
}

static void TSEnqueue(Q *queue, void *value)
{
	N *p;
	N *node;

	if (!queue->freelist)
	{
		node = (N*)malloc(sizeof(N));
		node->value = value;
		node->next = NULL;
		queue->nodeallocs++;
	}
	else
	{
		node = queue->freelist;
		queue->freelist = node->next;
		node->next = NULL;
		node->value = value;
	}

	queue->last->next = node;
	InterlockedExchangePointer(&queue->last, queue->last->next);
	for (p = queue->first; InterlockedCompareExchangePointer((volatile PVOID *)&p, NULL, queue->divider), p; p = queue->first)
	{
		N *tmp = queue->first;
		queue->first = tmp->next;
		tmp->next = queue->freelist;
		queue->freelist = tmp;
	}
}

static void *TSDequeue(Q *queue)
{
	N *result;
	N *choice = queue->divider;

	InterlockedCompareExchangePointer((volatile PVOID *)&choice, NULL, queue->last);

	if (choice)
	{
		result = queue->divider->next;
		choice = queue->divider;

		InterlockedExchangePointer(&queue->divider, queue->divider->next);
		return result->value;
	}

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////   Communication Queue implementation   /////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

static struct CommQueue *CreateCommQueue(int buffersize, int hwake)
{
	struct CommQueue *result = (struct CommQueue *)malloc(sizeof(struct CommQueue));

	result->freebuffers = NULL;
	result->freemsgs = NULL;
	result->buffersize = buffersize;
	result->queue = CreateTSQueue();
	result->returnqueue = CreateTSQueue();
	result->bufferallocs = 0;
	result->msgallocs = 0;
	result->hwake = hwake;
	
	return result;
}

static void FillFreeLists(struct CommQueue *queue)
{
	if (queue)
	{
		struct CommMessage *m;
		
		while ((m = (struct CommMessage*)TSDequeue(queue->returnqueue)) != NULL)
		{
			if (m->buffer)
			{
				m->buffer->next = queue->freebuffers;
				queue->freebuffers = m->buffer;
				m->buffer = NULL;
			}
		
			m->ptr = queue->freemsgs;
			queue->freemsgs = m;
		}
	}
}

static void DeleteCommQueue(struct CommQueue *queue)
{
	struct CommMessage *m;
	struct CommBuffer *b;
	
	/* Empty the queue */
	while ((m = DequeueCommMessage(queue)))
	{
		if (m->buffer)
			free(m->buffer);
		free(m);
	}

	/* Empty the return queue */
	FillFreeLists(queue);

	for (m = queue->freemsgs; m;)
	{
		struct CommMessage *next = (struct CommMessage*)m->ptr;
		free(m);
		m = next;
	}
	
	for (b = queue->freebuffers; b;)
	{
		struct CommBuffer *next = b->next;
		free(b);
		b = next;
	}
	
	DeleteTSQueue(queue->queue);
	DeleteTSQueue(queue->returnqueue);
	
	free(queue);
}

static struct CommMessage *CreateCommMessage(struct CommQueue *queue, int buffer)
{
	struct CommMessage *m;
	
	FillFreeLists(queue);
	
	// Get or create
	m = queue->freemsgs;
	if (!m)
	{
		m = (struct CommMessage*)malloc(sizeof(struct CommMessage));
		queue->msgallocs++;
	}
	else
	{
		queue->freemsgs = (struct CommMessage*)m->ptr;
	}
	
	// Clean up
	ZeroMemory(m, sizeof(struct CommMessage));
	
	if (buffer)
	{
		struct CommBuffer *b;
		
		b = queue->freebuffers;
		if (!b)
		{
			b = (struct CommBuffer*)malloc(sizeof(struct CommBuffer) + queue->buffersize);
			queue->bufferallocs++;
		}
		else
		{
			queue->freebuffers = b->next;
		}
		
		ZeroMemory(b, sizeof(struct CommBuffer) + queue->buffersize);
		b->size = queue->buffersize;
		m->buffer = b;
	}
	
	return m;
}

static void EnqueueCommMessage(struct CommQueue *queue, struct CommMessage *m)
{
	if (queue)
	{
		TSEnqueue(queue->queue, m);
	}
}

static struct CommMessage *DequeueCommMessage(struct CommQueue *queue)
{
	if (queue)
	{
		return (struct CommMessage*)TSDequeue(queue->queue);
	}
	return NULL;
}

static void ReleaseCommMessage(struct CommQueue *queue, struct CommMessage *m)
{
	if (queue)
	{
		TSEnqueue(queue->returnqueue, m);
	}
}

const char *jwQueueMemory()
{
	static char memreport[256];
	struct CommQueue *uiToEditor, *editorToUi;
	int uenodes, uesz;
	int eunodes, eusz;

	editorToUi = queues[0];
	uiToEditor = queues[1];

	uenodes = uiToEditor->queue->nodeallocs + uiToEditor->returnqueue->nodeallocs;
	uesz = sizeof(N) * uenodes;
	uesz += sizeof(struct CommMessage) * uiToEditor->msgallocs;
	uesz += (sizeof(struct CommBuffer) + uiToEditor->buffersize) * uiToEditor->bufferallocs;

	eunodes = editorToUi->queue->nodeallocs + editorToUi->returnqueue->nodeallocs;
	eusz = sizeof(N) * eunodes;
	eusz += sizeof(struct CommMessage) * editorToUi->msgallocs;
	eusz += (sizeof(struct CommBuffer) + editorToUi->buffersize) * editorToUi->bufferallocs;

	sprintf(memreport, "jwQueue VM: U->E [%d nodes + %d msgs + %d buffers = %d bytes]\n"
			   "jwQueue VM: E->U [%d nodes + %d msgs + %d buffers = %d bytes]\n"
			   "jwQueue VM: Total: %d bytes",
			   uenodes, uiToEditor->msgallocs, uiToEditor->bufferallocs, uesz,
			   eunodes, editorToUi->msgallocs, editorToUi->bufferallocs, eusz,
			   uesz + eusz);

	return memreport;
}


////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////	JoeWin communication interface	 ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int jwCreateWake(void)
{
	HANDLE ev = CreateEvent(NULL, FALSE, FALSE, NULL);
	int i;

	for (i = 0; i < nqhandles; i++) {
		if (!qhandles[i]) {
			qhandles[i] = ev;
			return i;
		}
	}

	nqhandles = max(4, nqhandles * 2);
	qhandles = (HANDLE*)realloc(qhandles, nqhandles * sizeof(HANDLE));
	ZeroMemory(&qhandles[i], (nqhandles - i) * sizeof(HANDLE));

	qhandles[i] = ev;
	return i;
}

int jwCreateQueue(int bufsz, int hwake)
{
	int i;

	for (i = 0; i < qlen; i++) {
		if (!queues[i]) {
			queues[i] = CreateCommQueue(bufsz, hwake);
			return i;
		}
	}

	qlen = max(4, qlen * 2);
	queues = (struct CommQueue**)realloc(queues, qlen * sizeof(struct CommQueue*));
	ZeroMemory(&queues[i], (qlen - i) * sizeof(struct CommQueue*));

	queues[i] = CreateCommQueue(bufsz, hwake);
	return i;
}

void jwCloseQueue(int qd)
{
	if (queues && queues[qd]) {
		int hwake = queues[qd]->hwake;
		int i;

		DeleteCommQueue(queues[qd]);
		queues[qd] = NULL;

		for (i = 0; i < qlen; i++) {
			if (queues[i] && queues[i]->hwake == hwake)
				break;
		}

		if (i == qlen) {
			/* Handle no longer used. */
			CloseHandle(qhandles[hwake]);
			qhandles[hwake] = 0;
		}
	}
}

HANDLE jwInitializeComm(void)
{
	jwCreateQueue(EDITOR_TO_UI_BUFSZ, jwCreateWake()); /* 0 */
	jwCreateQueue(UI_TO_EDITOR_BUFSZ, jwCreateWake()); /* 1 */

	/* UI side is calling this, give it the UI handle.
	** The editor always goes through jwWaitForEditorComm() */
	return qhandles[0];
}

void jwShutdownComm(void)
{
	int i;

	for (i = 0; i < qlen; i++) {
		if (queues[i])
			DeleteCommQueue(queues[i]);
	}

	for (i = 0; i < nqhandles; i++) {
		if (qhandles[i])
			CloseHandle(qhandles[i]);
	}

	qhandles = NULL;
	queues = NULL;
	qlen = 0;
	nqhandles = 0;
}

struct CommMessage *jwWaitForComm(int *qds, int nqds, int timeout, int *outqueue)
{
	int i;
	int n = 0;
	int t;
	HANDLE *hobjs = NULL;

	if (!queues)
		return NULL;

	/* Map handles */
	if (nqds > 1) {
		hobjs = (HANDLE*)malloc(sizeof(HANDLE*) * nqds);
		ZeroMemory(hobjs, sizeof(HANDLE*) * nqds);
		for (i = 0; i < nqds; i++) {
			HANDLE qhandle = qhandles[queues[qds[i]]->hwake];

			for (t = 0; t < n; t++) {
				if (hobjs[t] == qhandle)
					break;
			}

			if (t == n) {
				hobjs[n++] = qhandle;
			}
		}
	}

	for (i = 0; i < 3; i++) {
		DWORD result;

		for (t = 0; t < nqds; t++) {
			struct CommMessage *m = DequeueCommMessage(queues[qds[t]]);
			if (m) {
				if (outqueue) *outqueue = qds[t];
				if (hobjs) free(hobjs);
				return m;
			}
		}

		/* Note: pass in the same timeout each iteration, as this will */
		/* probably return immediately the first time */
		if (hobjs) {
			result = WaitForMultipleObjects(n, hobjs, FALSE, (DWORD)timeout);
		} else {
			result = WaitForSingleObject(qhandles[queues[*qds]->hwake], (DWORD)timeout);
		}

		if (result == WAIT_TIMEOUT || result == WAIT_FAILED)
		{
			/* Timeout or error */
			if (hobjs) free(hobjs);
			return NULL;
		}
		
		/* note: auto-reset */
		
		/*
		 * Note: we don't reset the event every time something is dequeued.
		 * Therefore, it's entirely possible that this loop will take three
		 * iterations to return a value.
		 */
	}

	/* Probably, TSDequeue is broken */
	assert(0);

	if (hobjs) free(hobjs);
	return NULL;
}

struct CommMessage *jwRecvComm(int qd)
{
	return queues ? DequeueCommMessage(queues[qd]) : NULL;
}

void jwSendComm(int qd, int msg, int arg1, int arg2, int arg3, int arg4, void *ptr, int sz, const char *data)
{
	struct CommMessage *m;
	struct CommQueue *q;
	int buffer;
	
	if (!queues) {
		return;
	}

	q = queues[qd];
	buffer = data ? 1 : 0;
	
	m = CreateCommMessage(q, buffer);
	m->msg = msg;
	m->arg1 = arg1;
	m->arg2 = arg2;
	m->arg3 = arg3;
	m->arg4 = arg4;
	m->ptr = ptr;
	
	if (buffer)
	{
		if (sz > m->buffer->size)
		{
			/* Do we not have the *correct* buffer sizes? */
			assert(0);
			
			/* Rather than leak memory, and instead of doing it right, */
			/* I'll just free the message */
			free(m->buffer);
			free(m);
			
			return;
		}
		
		memcpy(m->buffer->buffer, data, sz);
		m->buffer->size = sz;
	}
	else
	{
		m->buffer = NULL;
	}
	
	EnqueueCommMessage(q, m);
	SetEvent(qhandles[q->hwake]);
}

void jwReleaseComm(int qd, struct CommMessage *msg)
{
	if (queues && queues[qd]) {
		ReleaseCommMessage(queues[qd], msg);
	}
}

int jwRendezvous(int readqd, int writeqd)
{
	jwSendComm0(writeqd, COMM_RENDEZVOUS);

	for (;;)
	{
		struct CommMessage *m = jwWaitForComm(&readqd, 1, INFINITE, NULL);
		int msg = m->msg;
		
		assert(msg == COMM_RENDEZVOUS);
		if (msg != COMM_RENDEZVOUS) {
			return 1;
		}

		jwReleaseComm(readqd, m);

		if (msg == COMM_RENDEZVOUS)
			return 0;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   JoeWin IO   /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void jwWriteIO(int qd, const char *data, int size)
{
	int i;
	int maxdata = JW_PACKIOSZ + queues[qd]->buffersize;

	for (i = 0; i < size; i += maxdata)
	{
		int arg1 = 0, arg2 = 0, arg3 = 0, arg4 = 0;
		int pktsz = min(size - i, maxdata);
		int packsz = min(JW_PACKIOSZ, pktsz);
		int bufsz = pktsz - packsz;

		switch (packsz)
		{
			case 16: arg4 |= (int)((unsigned char)data[i + 15]) << 24;
			case 15: arg4 |= (int)((unsigned char)data[i + 14]) << 16;
			case 14: arg4 |= (int)((unsigned char)data[i + 13]) << 8;
			case 13: arg4 |= (int)((unsigned char)data[i + 12]);
			case 12: arg3 |= (int)((unsigned char)data[i + 11]) << 24;
			case 11: arg3 |= (int)((unsigned char)data[i + 10]) << 16;
			case 10: arg3 |= (int)((unsigned char)data[i +  9]) << 8;
			case  9: arg3 |= (int)((unsigned char)data[i +  8]);
			case  8: arg2 |= (int)((unsigned char)data[i +  7]) << 24;
			case  7: arg2 |= (int)((unsigned char)data[i +  6]) << 16;
			case  6: arg2 |= (int)((unsigned char)data[i +  5]) << 8;
			case  5: arg2 |= (int)((unsigned char)data[i +  4]);
			case  4: arg1 |= (int)((unsigned char)data[i +  3]) << 24;
			case  3: arg1 |= (int)((unsigned char)data[i +  2]) << 16;
			case  2: arg1 |= (int)((unsigned char)data[i +  1]) << 8;
			case  1: arg1 |= (int)((unsigned char)data[i]);
		}

		if (!bufsz)
		{
			jwSendComm(qd, COMM_IO_1 + packsz - 1, arg1, arg2, arg3, arg4, NULL, 0, NULL);
		}
		else
		{
			jwSendComm(qd, COMM_IO_N, arg1, arg2, arg3, arg4, NULL, bufsz, &data[JW_PACKIOSZ + i]);
		}
	}
}

int jwReadIO(struct CommMessage *m, char *output)
{
	int msg = m->msg;

	assert(JWISIO(m));

	switch (msg)
	{
		case COMM_IO_N:
		case COMM_IO_16: output[15] = (char)(0xff & (m->arg4 >> 24));
		case COMM_IO_15: output[14] = (char)(0xff & (m->arg4 >> 16));
		case COMM_IO_14: output[13] = (char)(0xff & (m->arg4 >> 8));
		case COMM_IO_13: output[12] = (char)(0xff &  m->arg4);
		case COMM_IO_12: output[11] = (char)(0xff & (m->arg3 >> 24));
		case COMM_IO_11: output[10] = (char)(0xff & (m->arg3 >> 16));
		case COMM_IO_10: output[9]  = (char)(0xff & (m->arg3 >> 8));
		case  COMM_IO_9: output[8]  = (char)(0xff &  m->arg3);
		case  COMM_IO_8: output[7]  = (char)(0xff & (m->arg2 >> 24));
		case  COMM_IO_7: output[6]  = (char)(0xff & (m->arg2 >> 16));
		case  COMM_IO_6: output[5]  = (char)(0xff & (m->arg2 >> 8));
		case  COMM_IO_5: output[4]  = (char)(0xff &  m->arg2);
		case  COMM_IO_4: output[3]  = (char)(0xff & (m->arg1 >> 24));
		case  COMM_IO_3: output[2]  = (char)(0xff & (m->arg1 >> 16));
		case  COMM_IO_2: output[1]  = (char)(0xff & (m->arg1 >> 8));
		case  COMM_IO_1: output[0]  = (char)(0xff &  m->arg1);
	}

	if (msg == COMM_IO_N)
	{
		assert(m->buffer);
		CopyMemory(&output[JW_PACKIOSZ], m->buffer->buffer, m->buffer->size);
		return JW_PACKIOSZ + m->buffer->size;
	}
	else
	{
		return msg - COMM_IO_1 + 1;
	}
}
