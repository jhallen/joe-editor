/*
 *	Software virtual memory system
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

static VFILE vfiles = { {&vfiles, &vfiles} };	/* Known vfiles */
static VPAGE *freepages = NULL;	/* Linked list of free pages */
static VPAGE *htab[HTSIZE];	/* Hash table of page headers */
static long curvalloc = 0;	/* Amount of memory in use */
static long maxvalloc = ILIMIT;	/* Maximum allowed */
unsigned char *vbase;			/* Data first entry in vheader refers to */
VPAGE **vheaders = NULL;	/* Array of header addresses */
static int vheadsz = 0;		/* No. entries allocated to vheaders */

void vflsh(void)
{
	VPAGE *vp;
	VPAGE *vlowest;
	off_t addr;
	off_t last;
	VFILE *vfile;
	int x;

	for (vfile = vfiles.link.next; vfile != &vfiles; vfile = vfile->link.next) {
		last = -1;
	      loop:
		addr = MAXOFF;
		vlowest = NULL;
		for (x = 0; x != HTSIZE; x++)
			for (vp = htab[x]; vp; vp = vp->next)
				if (vp->addr < addr && vp->addr > last && vp->vfile == vfile && (vp->addr >= vfile->size || (vp->dirty && !vp->count))) {
					addr = vp->addr;
					vlowest = vp;
				}
		if (vlowest) {
			if (!vfile->name)
				vfile->name = mktmp(NULL);
			if (!vfile->fd)
				vfile->fd = open((char *)(vfile->name), O_RDWR);
			if (vfile->fd < 0)
				ttsig(-2);
			lseek(vfile->fd, addr, 0);
			if (addr + PGSIZE > vsize(vfile)) {
				if (joe_write(vfile->fd, vlowest->data, (int) (vsize(vfile) - addr)) < 0)
					ttsig(-2);
				vfile->size = vsize(vfile);
			} else {
				if (joe_write(vfile->fd, vlowest->data, PGSIZE) < 0)
					ttsig(-2);
				if (addr + PGSIZE > vfile->size)
					vfile->size = addr + PGSIZE;
			}
			vlowest->dirty = 0;
			last = addr;
			goto loop;
		}
	}
}

void vflshf(VFILE *vfile)
{
	VPAGE *vp;
	VPAGE *vlowest;
	off_t addr;
	int x;

      loop:
	addr = MAXOFF;
	vlowest = NULL;
	for (x = 0; x != HTSIZE; x++)
		for (vp = htab[x]; vp; vp = vp->next)
			if (vp->addr < addr && vp->dirty && vp->vfile == vfile && !vp->count) {
				addr = vp->addr;
				vlowest = vp;
			}
	if (vlowest) {
		if (!vfile->name)
			vfile->name = mktmp(NULL);
		if (!vfile->fd) {
			vfile->fd = open((char *)(vfile->name), O_RDWR);
		}
		if (vfile->fd < 0)
			ttsig(-2);
		lseek(vfile->fd, addr, 0);
		if (addr + PGSIZE > vsize(vfile)) {
			if (joe_write(vfile->fd, vlowest->data, (int) (vsize(vfile) - addr)) < 0)
				ttsig(-2);
			vfile->size = vsize(vfile);
		} else {
			if (joe_write(vfile->fd, vlowest->data, PGSIZE) < 0)
				ttsig(-2);
			if (addr + PGSIZE > vfile->size)
				vfile->size = addr + PGSIZE;
		}
		vlowest->dirty = 0;
		goto loop;
	}
}

static unsigned char *mema(int align, int size)
{
	unsigned char *z = (unsigned char *) joe_malloc(align + size);

	return z + align - physical(z) % align;
}

unsigned char *vlock(VFILE *vfile, off_t addr)
{
	VPAGE *vp, *pp;
	int x, y;
	off_t ofst = (addr & (PGSIZE - 1));

	addr -= ofst;

	for (vp = htab[((addr >> LPGSIZE) + (unsigned long) vfile) & (HTSIZE - 1)]; vp; vp = vp->next)
		if (vp->vfile == vfile && vp->addr == addr) {
			++vp->count;
			return vp->data + ofst;
		}

	if (freepages) {
		vp = freepages;
		freepages = vp->next;
		goto gotit;
	}

	if (curvalloc + PGSIZE <= maxvalloc) {
		vp = (VPAGE *) joe_malloc(sizeof(VPAGE) * INC);
		if (vp) {
			vp->data = (unsigned char *) mema(PGSIZE, PGSIZE * INC);
			if (vp->data) {
				int q;

				curvalloc += PGSIZE * INC;
				if (!vheaders) {
					vheaders = (VPAGE **) joe_malloc((vheadsz = INC) * sizeof(VPAGE *));
					vbase = vp->data;
				} else if (physical(vp->data) < physical(vbase)) {
					VPAGE **t = vheaders;
					int amnt = (physical(vbase) - physical(vp->data)) >> LPGSIZE;

					vheaders = (VPAGE **) joe_malloc((amnt + vheadsz) * sizeof(VPAGE *));
					mmove(vheaders + amnt, t, vheadsz * sizeof(VPAGE *));
					vheadsz += amnt;
					vbase = vp->data;
					joe_free(t);
				} else if (((physical(vp->data + PGSIZE * INC) - physical(vbase)) >> LPGSIZE) > vheadsz) {
					vheaders = (VPAGE **)
					    joe_realloc(vheaders, (vheadsz = (((physical(vp->data + PGSIZE * INC) - physical(vbase)) >> LPGSIZE))) * sizeof(VPAGE *));
				}
				for (q = 1; q != INC; ++q) {
					vp[q].next = freepages;
					freepages = vp + q;
					vp[q].data = vp->data + q * PGSIZE;
					vheader(vp->data + q * PGSIZE) = vp + q;
				}
				vheader(vp->data) = vp;
				goto gotit;
			}
			joe_free(vp);
			vp = NULL;
		}
	}

	for (y = HTSIZE, x = (random() & (HTSIZE - 1)); y; x = ((x + 1) & (HTSIZE - 1)), --y)
		for (pp = (VPAGE *) (htab + x), vp = pp->next; vp; pp = vp, vp = vp->next)
			if (!vp->count && !vp->dirty) {
				pp->next = vp->next;
				goto gotit;
			}
	vflsh();
	for (y = HTSIZE, x = (random() & (HTSIZE - 1)); y; x = ((x + 1) & (HTSIZE - 1)), --y)
		for (pp = (VPAGE *) (htab + x), vp = pp->next; vp; pp = vp, vp = vp->next)
			if (!vp->count && !vp->dirty) {
				pp->next = vp->next;
				goto gotit;
			}
	if (-1 == write(2, sz(joe_gettext(_("vfile: out of memory\n")))))
		exit(2);
	else
		exit(1);

      gotit:
	vp->addr = addr;
	vp->vfile = vfile;
	vp->dirty = 0;
	vp->count = 1;
	vp->next = htab[((addr >> LPGSIZE) + (unsigned long)vfile) & (HTSIZE - 1)];
	htab[((addr >> LPGSIZE) + (unsigned long)vfile) & (HTSIZE - 1)] = vp;

	if (addr < vfile->size) {
		if (!vfile->fd) {
			vfile->fd = open((char *)(vfile->name), O_RDWR);
		}
		if (vfile->fd < 0)
			ttsig(-2);
		lseek(vfile->fd, addr, 0);
		if (addr + PGSIZE > vfile->size) {
			if (joe_read(vfile->fd, vp->data, (int) (vfile->size - addr)) < 0)
				ttsig(-2);
			mset(vp->data + vfile->size - addr, 0, PGSIZE - (int) (vfile->size - addr));
		} else
			if (joe_read(vfile->fd, vp->data, PGSIZE) < 0)
				ttsig(-2);
	} else
		mset(vp->data, 0, PGSIZE);

	return vp->data + ofst;
}

VFILE *vtmp(void)
{
	VFILE *new = (VFILE *) joe_malloc(sizeof(VFILE));

	new->fd = 0;
	new->name = NULL;
	new->alloc = 0;
	new->size = 0;
	new->left = 0;
	new->lv = 0;
	new->vpage = NULL;
	new->flags = 1;
	new->vpage1 = NULL;
	new->addr = -1;
	return enqueb_f(VFILE, link, &vfiles, new);
}

#ifdef junk

VFILE *vopen(name)
unsigned char *name;
{
	struct stat buf;
	VFILE *new = (VFILE *) joe_malloc(sizeof(VFILE));

	new->name = vsncpy(NULL, 0, sz(name));
	new->fd = open(name, O_RDWR);
	if (!new->fd) {
		fprintf(stderr, (char *)joe_gettext(_("Couldn\'t open file \'%s\'\n")), name);
		joe_free(new);
		return NULL;
	}
	fstat(new->fd, &buf);
	new->size = buf.st_size;
	new->alloc = new->size;
	new->left = 0;
	new->lv = 0;
	new->vpage = NULL;
	new->flags = 0;
	new->vpage1 = NULL;
	new->addr = -1;
	return enqueb_f(VFILE, link, &vfiles, new);
}

#endif

void vclose(VFILE *vfile)
{
	VPAGE *vp, *pp;
	int x;

	if (vfile->vpage)
		vunlock(vfile->vpage);
	if (vfile->vpage1)
		vunlock(vfile->vpage1);
	if (vfile->name) {
		if (vfile->flags) {
		        if (vfile->fd) {
		                /* Unlink fails if file is open on some systems. */
		                close(vfile->fd);
		                vfile->fd = 0;
		        }
			unlink((char *)vfile->name);
		} else
			vflshf(vfile);
		vsrm(vfile->name);
	}
	if (vfile->fd)
		close(vfile->fd);
	joe_free(deque_f(VFILE, link, vfile));
	for (x = 0; x != HTSIZE; x++)
		for (pp = (VPAGE *) (htab + x), vp = pp->next; vp;)
			if (vp->vfile == vfile) {
				pp->next = vp->next;
				vp->next = freepages;
				freepages = vp;
				vp = pp->next;
			} else {
				pp = vp;
				vp = vp->next;
			}
}

#ifdef junk
/* this is now broken */
void vlimit(amount)
long amount;
{
	VPAGE *vp, *pp;
	int x, y;

	maxvalloc = amount;
	while (curvalloc > maxvalloc)
		if (freepages) {
			vp = freepages;
			freepages = vp->next;
			joe_free(vp->data);
			joe_free(vp);
			curvalloc -= PGSIZE;
		} else {
		      again:
			for (y = HTSIZE, x = (random() & (HTSIZE - 1)); y; x = ((x + 1) & (HTSIZE - 1)), --y)
				for (pp = (VPAGE *) (htab + x), vp = pp->next; vp; pp = vp, vp = vp->next)
					if (!vp->count && !vp->dirty) {
						pp->next = vp->next;
						joe_free(vp->data);
						joe_free(vp);
						if ((curvalloc -= PGSIZE)
						    <= maxvalloc)
							return;
						goto again;
					}
			vflsh();
		      again1:
			for (y = HTSIZE, x = (random() & (HTSIZE - 1)); y; x = ((x + 1) & (HTSIZE - 1)), --y)
				for (pp = (VPAGE *) (htab + x), vp = pp->next; vp; pp = vp, vp = vp->next)
					if (!vp->count && !vp->dirty) {
						pp->next = vp->next;
						joe_free(vp->data);
						joe_free(vp);
						if ((curvalloc -= PGSIZE)
						    <= maxvalloc)
							return;
						goto again1;
					}
			return;
		}
}
#endif

off_t my_valloc(VFILE *vfile, off_t size)
{
	off_t start = vsize(vfile);

	vfile->alloc = start + size;
	if (vfile->lv) {
		if (vheader(vfile->vpage)->addr + PGSIZE > vfile->alloc)
			vfile->lv = PGSIZE - (vfile->alloc - vheader(vfile->vpage)->addr);
		else
			vfile->lv = 0;
	}
	return start;
}

#ifdef junk

void vseek(vfile, addr)
VFILE *vfile;
long addr;
{
	vfile->alloc = vsize(vfile);
	if (addr > vfile->alloc)
		vfile->alloc = addr;
	if (!vfile->vpage)
		vfile->vpage = vlock(vfile, addr & ~(long) (PGSIZE - 1));
	else if (vheader(vfile->vpage)->addr != (addr & ~(long) (PGSIZE - 1))) {
		vunlock(vfile->vpage);
		vfile->vpage = vlock(vfile, addr & ~(long) (PGSIZE - 1));
	}
	vfile->bufp = vfile->vpage + (addr & (PGSIZE - 1));
	vfile->left = vfile->vpage + PGSIZE - vfile->bufp;
	if (vheader(vfile->vpage)->addr + PGSIZE > vfile->alloc)
		vfile->lv = PGSIZE - (vfile->alloc - vheader(vfile->vpage)->addr);
	else
		vfile->lv = 0;
}

int _vrgetc(vfile)
VFILE *vfile;
{
	if (vtell(vfile) == 0)
		return NO_MORE_DATA;
	vseek(vfile, vtell(vfile) - 1);
	++vfile->bufp;
	--vfile->left;
	return vrgetc(vfile);
}

int _vgetc(vfile)
VFILE *vfile;
{
	if (vtell(vfile) == vsize(vfile))
		return NO_MORE_DATA;
	vseek(vfile, vtell(vfile));
	return vgetc(vfile);
}

int nmvgetc(v)
VFILE *v;
{
	return vgetc(v);
}

int _vputc(vfile, c)
VFILE *vfile;
unsigned char c;
{
	vseek(vfile, vtell(vfile));
	return vputc(vfile, c);
}

short vgetw(vfile)
VFILE *vfile;
{
	short w;

	if (vtell(vfile) + 2 > vsize(vfile))
		return -1;
	w = vgetc(vfile);
	w += ((short) vgetc(vfile) << 8);
	return w;
}

short vputw(vfile, w)
VFILE *vfile;
short w;
{
	vputc(vfile, w);
	vputc(vfile, w >> 8);
	return w;
}

long vgetl(vfile)
VFILE *vfile;
{
	long w;

	if (vtell(vfile) + 4 > vsize(vfile))
		return -1;
	w = vgetc(vfile);
	w += ((long) vgetc(vfile) << 8);
	w += ((long) vgetc(vfile) << 16);
	w += ((long) vgetc(vfile) << 24);
	return w;
}

long vputl(vfile, w)
VFILE *vfile;
long w;
{
	vputc(vfile, w);
	vputc(vfile, w >> 8);
	vputc(vfile, w >> 16);
	vputc(vfile, w >> 24);
	return w;
}

int _rc(vfile, addr)
VFILE *vfile;
long addr;
{
	if (vfile->vpage1)
		vunlock(vfile->vpage1);
	vfile->vpage1 = vlock(vfile, vfile->addr = (addr & ~(long) (PGSIZE - 1)));
	return rc(vfile, addr);
}

int _wc(vfile, addr, c)
VFILE *vfile;
long addr;
unsigned char c;
{
	if (addr + 1 > vsize(vfile))
		my_valloc(vfile, addr + 1 - vsize(vfile));
	if (vfile->vpage1)
		vunlock(vfile->vpage1);
	vfile->vpage1 = vlock(vfile, vfile->addr = (addr & ~(long) (PGSIZE - 1)));
	return wc(vfile, addr, c);
}

short rw(vfile, addr)
VFILE *vfile;
long addr;
{
	short c;

	if (addr + 2 > vsize(vfile))
		return -1;
	c = rc(vfile, addr);
	c += ((short) rc(vfile, addr + 1) << 8);
	return c;
}

short ww(vfile, addr, c)
VFILE *vfile;
long addr;
short c;
{
	if (addr + 2 > vsize(vfile))
		my_valloc(vfile, addr + 2 - vsize(vfile));
	wc(vfile, addr, c);
	wc(vfile, addr + 1, c >> 8);
	return c;
}

long rl(vfile, addr)
VFILE *vfile;
long addr;
{
	long c;

	if (addr + 4 > vsize(vfile))
		return -1;
	c = rc(vfile, addr);
	c += ((long) rc(vfile, addr + 1) << 8);
	c += ((long) rc(vfile, addr + 2) << 16);
	c += ((long) rc(vfile, addr + 3) << 24);
	return c;
}

long wl(vfile, addr, c)
VFILE *vfile;
long addr;
long c;
{
	if (addr + 4 > vsize(vfile))
		my_valloc(vfile, addr + 4 - vsize(vfile));
	wc(vfile, addr, c);
	wc(vfile, addr + 1, c >> 8);
	wc(vfile, addr + 2, c >> 16);
	wc(vfile, addr + 3, c >> 24);
	return c;
}

void vread(v, blk, size)
VFILE *v;
unsigned char *blk;
int size;
{
	long addr = vtell(v);
	unsigned char *src;
	int x;

	while (size) {
		src = vlock(v, addr);
		x = PGSIZE - (addr & (PGSIZE - 1));
		if (x >= size) {
			vseek(v, addr + size);
			mcpy(blk, src, size);
			vunlock(src);
			return;
		}
		size -= x;
		addr += x;
		mcpy(blk, src, x);
		blk += x;
		vunlock(src);
	}
	vseek(v, addr);
}

void vwrite(v, blk, size)
VFILE *v;
unsigned char *blk;
int size;
{
	long addr = vtell(v);
	unsigned char *src;
	int x;

	if (addr + size > vsize(v))
		my_valloc(v, addr + size - vsize(v));
	while (size) {
		src = vlock(v, addr);
		x = PGSIZE - (addr & (PGSIZE - 1));
		if (x >= size) {
			vseek(v, addr + size);
			mcpy(src, blk, size);
			vchanged(src);
			vunlock(src);
			return;
		}
		size -= x;
		addr += x;
		mcpy(src, blk, x);
		blk += x;
		vchanged(src);
		vunlock(src);
	}
	vseek(v, addr);
}

/* Write zstring to vfile */

void vputs(v, s)
VFILE *v;
unsigned char *s;
{
	while (*s) {
		vputc(v, *s);
		++s;
	}
}

/* Read a line from a file.  Remove '\n' if there was any */

unsigned char *vgets(v, s)
VFILE *v;
unsigned char *s;
{
	unsigned char *b, *a, *x, *y;
	int cnt;

	/* Return with NULL if at end of file */
	if (vtell(v) == vsize(v)) {
		vsrm(s);
		return NULL;
	}

	/* Create string if it doesn't exist */
	if (!s)
		s = vsmk(80);

	/* Zero string length */
	sLen(s) = 0;

      loop:

	/* Set b to end of string, a to page pointer, and cnt to min which ever
	 * (string or page) has the least space left
	 */
	b = s + sLen(s);
	a = v->bufp;
	cnt = Imin(sSIZ(s) - sLen(s), v->left - v->lv);

	/* Copy until \n is found or until page or buffer out of space */
	if (cnt >= 16)
		do {
			if ((b[0] = a[0]) == '\n') {
				a += 1;
				b += 1;
				goto ovr;
			}
			if ((b[1] = a[1]) == '\n') {
				a += 2;
				b += 2;
				cnt -= 1;
				goto ovr;
			}
			if ((b[2] = a[2]) == '\n') {
				a += 3;
				b += 3;
				cnt -= 2;
				goto ovr;
			}
			if ((b[3] = a[3]) == '\n') {
				a += 4;
				b += 4;
				cnt -= 3;
				goto ovr;
			}
			if ((b[4] = a[4]) == '\n') {
				a += 5;
				b += 5;
				cnt -= 4;
				goto ovr;
			}
			if ((b[5] = a[5]) == '\n') {
				a += 6;
				b += 6;
				cnt -= 5;
				goto ovr;
			}
			if ((b[6] = a[6]) == '\n') {
				a += 7;
				b += 7;
				cnt -= 6;
				goto ovr;
			}
			if ((b[7] = a[7]) == '\n') {
				a += 8;
				b += 8;
				cnt -= 7;
				goto ovr;
			}
			if ((b[8] = a[8]) == '\n') {
				a += 9;
				b += 9;
				cnt -= 8;
				goto ovr;
			}
			if ((b[9] = a[9]) == '\n') {
				a += 10;
				b += 10;
				cnt -= 9;
				goto ovr;
			}
			if ((b[10] = a[10]) == '\n') {
				a += 11;
				b += 11;
				cnt -= 10;
				goto ovr;
			}
			if ((b[11] = a[11]) == '\n') {
				a += 12;
				b += 12;
				cnt -= 11;
				goto ovr;
			}
			if ((b[12] = a[12]) == '\n') {
				a += 13;
				b += 13;
				cnt -= 12;
				goto ovr;
			}
			if ((b[13] = a[13]) == '\n') {
				a += 14;
				b += 14;
				cnt -= 13;
				goto ovr;
			}
			if ((b[14] = a[14]) == '\n') {
				a += 15;
				b += 15;
				cnt -= 14;
				goto ovr;
			}
			if ((b[15] = a[15]) == '\n') {
				a += 16;
				b += 16;
				cnt -= 15;
				goto ovr;
			}
		} while (a += 16, b += 16, (cnt -= 16) >= 16);

/*
	x = a;
	y = b;
	a += cnt - 15;
	b += cnt - 15;
	switch(cnt) {
	case 15:	if((b[0]=a[0])=='\n') { a+=1; b+=1; goto zif; }
	case 14:	if((b[1]=a[1])=='\n') { a+=2; b+=2; goto zif; }
	case 13:	if((b[2]=a[2])=='\n') { a+=3; b+=3; goto zif; }
	case 12:	if((b[3]=a[3])=='\n') { a+=4; b+=4; goto zif; }
	case 11:	if((b[4]=a[4])=='\n') { a+=5; b+=5; goto zif; }
	case 10:	if((b[5]=a[5])=='\n') { a+=6; b+=6; goto zif; }
	case 9:		if((b[6]=a[6])=='\n')  { a+=7; b+=7; goto zif; }
	case 8:		if((b[7]=a[7])=='\n')  { a+=8; b+=8; goto zif; }
	case 7:		if((b[8]=a[8])=='\n')  { a+=9; b+=9; goto zif; }
	case 6:		if((b[9]=a[9])=='\n')  { a+=10; b+=10; goto zif; }
	case 5:		if((b[10]=a[10])=='\n'){ a+=11; b+=11; goto zif; }
	case 4:		if((b[11]=a[11])=='\n'){ a+=12; b+=12; goto zif; }
	case 3:		if((b[12]=a[12])=='\n'){ a+=13; b+=13; goto zif; }
	case 2:		if((b[13]=a[13])=='\n'){ a+=14; b+=14; goto zif; }
	case 1:		if((b[14]=a[14])=='\n'){ a+=15; b+=15; goto zif; }
	}
	a = x + cnt;
	b = y + cnt;
	cnt=0;
	goto ovr;
zif:	cnt -= a - x - 1;
*/

	if (cnt)
		do {
			if ((*b++ = *a++) == '\n')
				break;
		} while (--cnt);

      ovr:

	/* Update string and page data */
	sLen(s) = b - s;
	v->left -= a - v->bufp;
	v->bufp = a;

	if (!cnt)
		if (vtell(v) == vsize(v))
			b[0] = 0;
		else {
			if (sLen(s) == sSiz(s))
				s = vsensure(s, sLen(s) + (sLen(s) >> 1) + 16);
			if (!v->left)
				vseek(v, vtell(v));
			goto loop;
	} else
		b[-1] = 0;

	return s;
}
#endif
