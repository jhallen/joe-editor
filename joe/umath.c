/*
 *	Math
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

const char *merr;

int mode_hex;
int mode_eng;
int mode_ins;

double vzero = 0.0;

static RETSIGTYPE fperr(int unused)
{
	if (!merr) {
		merr = joe_gettext(_("Float point exception"));
	}
	REINSTALL_SIGHANDLER(SIGFPE, fperr);
}

struct var {
	char *name;
	double (*func)(double n);
	int set;
	double val;
	struct var *next;
} *vars = NULL;

static struct var *get(const char *str)
{
	struct var *v;

	for (v = vars; v; v = v->next) {
		if (!zcmp(v->name, str)) {
			return v;
		}
	}
	v = (struct var *) joe_malloc(SIZEOF(struct var));

	v->set = 0;
	v->func = 0;
	v->val = 0;
	v->next = vars;
	vars = v;
	v->name = zdup(str);
	return v;
}

char *ptr;
struct var *dumb;
static double eval(char *s, int secure);

int recur=0;

/* en means enable evaluation */

static double expr(int prec, int en,struct var **rtv, int secure)
{
	double x = 0.0, y, z;
	struct var *v = NULL;

	while (*ptr == ' ' || *ptr == '\t') {
		++ptr;
	}
	if ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z')
	    || *ptr == '_') {
		char *s = ptr, c;

		while ((*ptr >= 'a' && *ptr <= 'z')
		       || (*ptr >= 'A' && *ptr <= 'Z')
		       || *ptr == '_' || (*ptr >= '0' && *ptr <= '9')) {
			++ptr;
		}
		c = *ptr;
		*ptr = 0;
		if (!secure && !zcmp(s,"joe")) {
			*ptr = c;
			v = 0;
			x = 0.0;
			while (*ptr==' ' || *ptr=='\t')
				++ptr;
			if (*ptr=='(') {
				char *q = ++ptr;
				MACRO *m;
				ptrdiff_t sta;
				while (*q && *q!=')')
					++q;
				if (*q!=')') {
					if (!merr)
						merr = joe_gettext(_("Missing )"));
				} else
					*q++ = 0;
				if (en) {
					m = mparse(NULL,ptr,&sta,0);
					ptr = q;
					if (m) {
						x = !exmacro(m,1);
						rmmacro(m);
					} else {
						if (!merr)
							merr = joe_gettext(_("Syntax error in macro"));
					}
				} else {
					ptr = q;
				}
			} else {
				if (!merr)
					merr = joe_gettext(_("Missing ("));
			}
			c = *ptr;
		} else if (!en) {
			v = 0;
			x = 0.0;
		} else if (!zcmp(s,"hex")) {
			mode_hex = 1;
			mode_eng = 0;
			v = get("ans");
			x = v->val;
		} else if (!zcmp(s,"dec")) {
			mode_hex = 0;
			mode_eng = 0;
			v = get("ans");
			x = v->val;
		} else if (!zcmp(s,"eng")) {
			mode_hex = 0;
			mode_eng = 1;
			v = get("ans");
			x = v->val;
		} else if (!zcmp(s,"ins")) {
			mode_ins = 1;
			v = get("ans");
			x = v->val;
		} else if (!zcmp(s,"sum")) {
			double xsq;
			int cnt = blksum(&x, &xsq);
			if (!merr && cnt<=0)
				merr = joe_gettext(_("No numbers in block"));
			v = 0;
		} else if (!zcmp(s,"cnt")) {
			double xsq;
			int cnt = blksum(&x, &xsq);
			if (!merr && cnt<=0)
				merr = joe_gettext(_("No numbers in block"));
			v = 0;
			x = cnt;
		} else if (!zcmp(s,"avg")) {
			double xsq;
			int cnt = blksum(&x, &xsq);
			if (!merr && cnt<=0)
				merr = joe_gettext(_("No numbers in block"));
			v = 0;
			if (cnt)
				x /= (double)cnt;
		} else if (!zcmp(s,"dev")) {
			double xsq;
			double avg;
			int cnt = blksum(&x, &xsq);
			if (!merr && cnt<=0)
				merr = joe_gettext(_("No numbers in block"));
			v = 0;
			if (cnt) {
				avg = x / (double)cnt;
				x = sqrt(xsq + (double)cnt*avg*avg - 2.0*avg*x);
			}
		} else if (!zcmp(s,"eval")) {
			char *save = ptr;
			char *e = blkget();
			if (e) {
				v = 0;
				x = eval(e,secure);
				joe_free(e);
				ptr = save;
			} else if (!merr) {
				merr = joe_gettext(_("No block"));
			}
		} else {
			v = get(s);
			x = v->val;
		}
		*ptr = c;
	} else if ((*ptr >= '0' && *ptr <= '9') || *ptr == '.') {
		char *eptr;
		x = strtod(ptr,&eptr);
		ptr = eptr;
	} else if (*ptr == '(') {
		++ptr;
		x = expr(0, en, &v, secure);
		if (*ptr == ')')
			++ptr;
		else {
			if (!merr)
				merr = joe_gettext(_("Missing )"));
		}
	} else if (*ptr == '-') {
		++ptr;
		x = -expr(10, en, &dumb, secure);
	} else if (*ptr == '!') {
		++ptr;
		x = !expr(10, en, &dumb, secure);
	}
      loop:
	while (*ptr == ' ' || *ptr == '\t')
		++ptr;
	if (*ptr == '(' && 11 > prec) {
		++ptr;
		y = expr(0, en, &dumb, secure);
		if (*ptr == ')')
			++ptr;
		else {
			if (!merr)
				merr = joe_gettext(_("Missing )"));
		}
		if (v && v->func)
			x = v->func(y);
		else {
			if (!merr)
				merr = joe_gettext(_("Called object is not a function"));
		}
		goto loop;
	} else if (*ptr == '!' && ptr[1]!='=' && 10 >= prec) {
		++ptr;
		if (x == (int)x && x>=1.0 && x<70.0) {
			y = 1.0;
			while (x>1.0) {
				y *= x;
				x -= 1.0;
			}
			x = y;
		} else {
			if (!merr)
				merr = joe_gettext(_("Factorial can only take positive integers"));
		}
		v = 0;
		goto loop;
	} else if (*ptr == '*' && ptr[1] == '*' && 8 > prec) {
		ptr+=2;
		x = pow(x, expr(8, en, &dumb, secure));
		v = 0;
		goto loop;
	} else if (*ptr == '^' && 8 > prec) {
		++ptr;
		x = pow(x, expr(8, en, &dumb, secure));
		v = 0;
		goto loop;
	} else if (*ptr == '*' && 7 > prec) {
		++ptr;
		x *= expr(7, en, &dumb, secure);
		v = 0;
		goto loop;
	} else if (*ptr == '/' && 7 > prec) {
		++ptr;
		x /= expr(7, en, &dumb, secure);
		v = 0;
		goto loop;
	} else if(*ptr=='%' && 7>prec) {
		++ptr;
		y = expr(7, en, &dumb, secure);
		if ((int)y == 0) x = 1.0/vzero;
		else x = ((int) x) % (int)y;
		v = 0;
		goto loop;
	} else if (*ptr == '+' && 6 > prec) {
		++ptr;
		x += expr(6, en, &dumb, secure);
		v = 0;
		goto loop;
	} else if (*ptr == '-' && 6 > prec) {
		++ptr;
		x -= expr(6, en, &dumb, secure);
		v = 0;
		goto loop;
	} else if (*ptr == '<' && 5 > prec) {
		++ptr;
		if (*ptr == '=') ++ptr, x = (x <= expr(5, en, &dumb, secure));
		else x = (x < expr(5,en,&dumb, secure));
		v = 0;
		goto loop;
	} else if (*ptr == '>' && 5 > prec) {
		++ptr;
		if (*ptr == '=') ++ptr, x=(x >= expr(5,en,&dumb, secure));
		else x = (x > expr(5,en,&dumb, secure));
		v = 0;
		goto loop;
	} else if (*ptr == '=' && ptr[1] == '=' && 5 > prec) {
		++ptr, ++ptr;
		x = (x == expr(5,en,&dumb, secure));
		v = 0;
		goto loop;
	} else if (*ptr == '!' && ptr[1] == '=' && 5 > prec) {
		++ptr, ++ptr;
		x = (x != expr(5,en,&dumb,secure));
		v = 0;
		goto loop;
	} else if (*ptr == '&' && ptr[1] == '&' && 3 > prec) {
		++ptr, ++ptr;
		y = expr(3,x!=0.0 && en,&dumb,secure);
		x = (int)x && (int)y;
		v = 0;
		goto loop;
	} else if (*ptr=='|' && ptr[1]=='|' &&  3 > prec) {
		++ptr, ++ptr;
		y = expr(3,x==0.0 && en,&dumb,secure);
		x = (int)x || (int)y;
		v= 0;
		goto loop;
	} else if (*ptr=='?' && 2 >= prec) {
		++ptr;
		y = expr(2,x!=0.0 && en,&dumb,secure);
		if (*ptr==':') {
			++ptr;
			z = expr(2,x==0.0 && en,&dumb,secure);
			if (x != 0.0)
				x = y;
			else
				x = z;
			v = 0;  
		} else if (!merr) {
			merr = ": missing after ?";
		}
		goto loop;
	} else if (*ptr == '=' && 1 >= prec) {
		++ptr;
		x = expr(1, en,&dumb,secure);
		if (v) {
			v->val = x;
			v->set = 1;
		} else {
			if (!merr)
				merr = joe_gettext(_("Left side of = is not an l-value"));
		}
		v = 0;
		goto loop;
	}
	*rtv = v;
	return x;
}

static double eval(char *s, int secure)
{
	double result = 0.0;
	struct var *v;
	if(++recur==1000) {
		merr = joe_gettext(_("Recursion depth exceeded"));
		--recur;
		return 0.0;
	}
	ptr = s;
	while (!merr && *ptr) {
		result = expr(0, 1, &dumb,secure);
		v = get("ans");
		v->val = result;
		v->set = 1;
		if (!merr) {
			while (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n') {
				++ptr;
			}
			if (*ptr == ':') {
				++ptr;
				while (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n') {
					++ptr;
				}
			} else if (*ptr) {
				merr = joe_gettext(_("Extra junk after end of expr"));
			}
		}
	}
	--recur;
	return result;
}

/* These don't all exist on some systems... */

#ifdef HAVE_SIN
static double m_sin(double n) { return sin(n); }
#else
#ifdef sin
static double m_sin(double n) { return sin(n); }
#endif
#endif

#ifdef HAVE_COS
static double m_cos(double n) { return cos(n); }
#else
#ifdef cos
static double m_cos(double n) { return cos(n); }
#endif
#endif

#ifdef HAVE_TAN
static double m_tan(double n) { return tan(n); }
#else
#ifdef tan
static double m_tan(double n) { return tan(n); }
#endif
#endif

#ifdef HAVE_EXP
static double m_exp(double n) { return exp(n); }
#else
#ifdef exp
static double m_exp(double n) { return exp(n); }
#endif
#endif

#ifdef HAVE_SQRT
static double m_sqrt(double n) { return sqrt(n); }
#else
#ifdef sqrt
static double m_sqrt(double n) { return sqrt(n); }
#endif
#endif

#ifdef HAVE_CBRT
static double m_cbrt(double n) { return cbrt(n); }
#else
#ifdef cbrt
static double m_cbrt(double n) { return cbrt(n); }
#endif
#endif

#ifdef HAVE_LOG
static double m_log(double n) { return log(n); }
#else
#ifdef log
static double m_log(double n) { return log(n); }
#endif
#endif

#ifdef HAVE_LOG10
static double m_log10(double n) { return log10(n); }
#else
#ifdef log10
static double m_log10(double n) { return log10(n); }
#endif
#endif

#ifdef HAVE_ASIN
static double m_asin(double n) { return asin(n); }
#else
#ifdef asin
static double m_asin(double n) { return asin(n); }
#endif
#endif

#ifdef HAVE_ACOS
static double m_acos(double n) { return acos(n); }
#else
#ifdef acos
static double m_acos(double n) { return acos(n); }
#endif
#endif

#ifdef HAVE_ATAN
static double m_atan(double n) { return atan(n); }
#else
#ifdef atan
static double m_atan(double n) { return atan(n); }
#endif
#endif

#ifdef HAVE_SINH
static double m_sinh(double n) { return sinh(n); }
#else
#ifdef sinh
static double m_sinh(double n) { return sinh(n); }
#endif
#endif

#ifdef HAVE_COSH
static double m_cosh(double n) { return cosh(n); }
#else
#ifdef cosh
static double m_cosh(double n) { return cosh(n); }
#endif
#endif

#ifdef HAVE_TANH
static double m_tanh(double n) { return tanh(n); }
#else
#ifdef tanh
static double m_tanh(double n) { return tanh(n); }
#endif
#endif

#ifdef HAVE_ASINH
static double m_asinh(double n) { return asinh(n); }
#else
#ifdef asinh
static double m_asinh(double n) { return asinh(n); }
#endif
#endif

#ifdef HAVE_ACOSH
static double m_acosh(double n) { return acosh(n); }
#else
#ifdef acosh
static double m_acosh(double n) { return acosh(n); }
#endif
#endif

#ifdef HAVE_ATANH
static double m_atanh(double n) { return atanh(n); }
#else
#ifdef atanh
static double m_atanh(double n) { return atanh(n); }
#endif
#endif

#ifdef HAVE_FLOOR
static double m_floor(double n) { return floor(n); }
#else
#ifdef floor
static double m_floor(double n) { return floor(n); }
#endif
#endif

#ifdef HAVE_CEIL
static double m_ceil(double n) { return ceil(n); }
#else
#ifdef ceil
static double m_ceil(double n) { return ceil(n); }
#endif
#endif

#ifdef HAVE_FABS
static double m_fabs(double n) { return fabs(n); }
#else
#ifdef fabs
static double m_fabs(double n) { return fabs(n); }
#endif
#endif

#ifdef HAVE_ERF
static double m_erf(double n) { return erf(n); }
#else
#ifdef erf
static double m_erf(double n) { return erf(n); }
#endif
#endif

#ifdef HAVE_ERFC
static double m_erfc(double n) { return erfc(n); }
#else
#ifdef erfc
static double m_erfc(double n) { return erfc(n); }
#endif
#endif

#ifdef HAVE_J0
static double m_j0(double n) { return j0(n); }
#else
#ifdef j0
static double m_j0(double n) { return j0(n); }
#endif
#endif

#ifdef HAVE_J1
static double m_j1(double n) { return j1(n); }
#else
#ifdef j1
static double m_j1(double n) { return j1(n); }
#endif
#endif

#ifdef HAVE_Y0
static double m_y0(double n) { return y0(n); }
#else
#ifdef y0
static double m_y0(double n) { return y0(n); }
#endif
#endif

#ifdef HAVE_Y1
static double m_y1(double n) { return y1(n); }
#else
#ifdef y1
static double m_y1(double n) { return y1(n); }
#endif
#endif


static double m_int(double n) { return (int)(n); }

double calc(BW *bw, char *s, int secure)
{
	/* BW *tbw = bw->parent->main->object; */
	BW *tbw = bw;
	struct var *v;
	int c = brch(bw->cursor);

	if (!vars) {
#ifdef HAVE_SIN
		v = get("sin"); v->func = m_sin;
#else
#ifdef sin
		v = get("sin"); v->func = m_sin;
#endif
#endif
#ifdef HAVE_COS
		v = get("cos"); v->func = m_cos;
#else
#ifdef cos
		v = get("cos"); v->func = m_cos;
#endif
#endif
#ifdef HAVE_TAN
		v = get("tan"); v->func = m_tan;
#else
#ifdef tan
		v = get("tan"); v->func = m_tan;
#endif
#endif
#ifdef HAVE_EXP
		v = get("exp"); v->func = m_exp;
#else
#ifdef exp
		v = get("exp"); v->func = m_exp;
#endif
#endif
#ifdef HAVE_SQRT
		v = get("sqrt"); v->func = m_sqrt;
#else
#ifdef sqrt
		v = get("sqrt"); v->func = m_sqrt;
#endif
#endif
#ifdef HAVE_CBRT
		v = get("cbrt"); v->func = m_cbrt;
#else
#ifdef cbrt
		v = get("cbrt"); v->func = m_cbrt;
#endif
#endif
#ifdef HAVE_LOG
		v = get("ln"); v->func = m_log;
#else
#ifdef log
		v = get("ln"); v->func = m_log;
#endif
#endif
#ifdef HAVE_LOG10
		v = get("log"); v->func = m_log10;
#else
#ifdef log10
		v = get("log"); v->func = m_log10;
#endif
#endif
#ifdef HAVE_ASIN
		v = get("asin"); v->func = m_asin;
#else
#ifdef asin
		v = get("asin"); v->func = m_asin;
#endif
#endif
#ifdef HAVE_ACOS
		v = get("acos"); v->func = m_acos;
#else
#ifdef acos
		v = get("acos"); v->func = m_acos;
#endif
#endif
#ifdef HAVE_ATAN
		v = get("atan"); v->func = m_atan;
#else
#ifdef atan
		v = get("atan"); v->func = m_atan;
#endif
#endif
#ifdef M_PI
		v = get("pi"); v->val = M_PI; v->set = 1;
#endif
#ifdef M_E
		v = get("e"); v->val = M_E; v->set = 1;
#endif
#ifdef HAVE_SINH
		v = get("sinh"); v->func = m_sinh;
#else
#ifdef sinh
		v = get("sinh"); v->func = m_sinh;
#endif
#endif
#ifdef HAVE_COSH
		v = get("cosh"); v->func = m_cosh;
#else
#ifdef cosh
		v = get("cosh"); v->func = m_cosh;
#endif
#endif
#ifdef HAVE_TANH
		v = get("tanh"); v->func = m_tanh;
#else
#ifdef tanh
		v = get("tanh"); v->func = m_tanh;
#endif
#endif
#ifdef HAVE_ASINH
		v = get("asinh"); v->func = m_asinh;
#else
#ifdef asinh
		v = get("asinh"); v->func = m_asinh;
#endif
#endif
#ifdef HAVE_ACOSH
		v = get("acosh"); v->func = m_acosh;
#else
#ifdef acosh
		v = get("acosh"); v->func = m_acosh;
#endif
#endif
#ifdef HAVE_ATANH
		v = get("atanh"); v->func = m_atanh;
#else
#ifdef atanh
		v = get("atanh"); v->func = m_atanh;
#endif
#endif
#ifdef HAVE_FLOOR
		v = get("floor"); v->func = m_floor;
#else
#ifdef floor
		v = get("floor"); v->func = m_floor;
#endif
#endif
#ifdef HAVE_CEIL
		v = get("ceil"); v->func = m_ceil;
#else
#ifdef ceil
		v = get("ceil"); v->func = m_ceil;
#endif
#endif
#ifdef HAVE_FABS
		v = get("abs"); v->func = m_fabs;
#else
#ifdef fabs
		v = get("abs"); v->func = m_fabs;
#endif
#endif
#ifdef HAVE_ERF
		v = get("erf"); v->func = m_erf;
#else
#ifdef erf
		v = get("erf"); v->func = m_erf;
#endif
#endif
#ifdef HAVE_ERFC
		v = get("erfc"); v->func = m_erfc;
#else
#ifdef erfc
		v = get("erfc"); v->func = m_erfc;
#endif
#endif
#ifdef HAVE_J0
		v = get("j0"); v->func = m_j0;
#else
#ifdef j0
		v = get("j0"); v->func = m_j0;
#endif
#endif
#ifdef HAVE_J1
		v = get("j1"); v->func = m_j1;
#else
#ifdef j1
		v = get("j1"); v->func = m_j1;
#endif
#endif
#ifdef HAVE_Y0
		v = get("y0"); v->func = m_y0;
#else
#ifdef y0
		v = get("y0"); v->func = m_y0;
#endif
#endif
#ifdef HAVE_Y1
		v = get("y1"); v->func = m_y1;
#else
#ifdef y1
		v = get("y1"); v->func = m_y1;
#endif
#endif
		v = get("int"); v->func = m_int;
	}

	v = get("top");
	v->val = (double)(tbw->top->line + 1);
	v->set = 1;
	v = get("lines");
	v->val = (double)(tbw->b->eof->line + 1);
	v->set = 1;
	v = get("line");
	v->val = (double)(tbw->cursor->line + 1);
	v->set = 1;
	v = get("col");
	v->val = (double)(tbw->cursor->col + 1);
	v->set = 1;
	v = get("byte");
	v->val = (double)(tbw->cursor->byte + 1);
	v->set = 1;
	v = get("size");
	v->val = (double)tbw->b->eof->byte;
	v->set = 1;
	v = get("height");
	v->val = (double)tbw->h;
	v->set = 1;
	v = get("width");
	v->val = (double)tbw->w;
	v->set = 1;
	v = get("char");
	v->val = (c == NO_MORE_DATA ? -1.0 : c);
	v->set = 1;
	v = get("markv");
	v->val = markv(1) ? 1.0 : 0.0;
	v->set = 1;
	v = get("rdonly");
	v->val = tbw->b->rdonly;
	v->set = 1;
	v = get("arg");
	v->val = current_arg;
	v->set = 1;
	v = get("argset");
	v->val = current_arg_set;
	v->set = 1;
	v = get("no_windows");
	v->val = countmain(bw->parent->t);
	v->set = 1;
	merr = 0;
	v = get("is_shell");
	v->val = tbw->b->pid;
	v->set = 1;
	merr = 0;
	return eval(s, secure);
}

/* Main user interface */
static int domath(W *w, char *s, void *object, int *notify, int secure)
{
	double result;
	BW *bw;
	WIND_BW(bw, w);
	result = calc(bw, s, secure);

	if (notify) {
		*notify = 1;
	}
	if (merr) {
		msgnw(bw->parent, merr);
		return -1;
	}
	vsrm(s);
	if (mode_hex)
#ifdef HAVE_LONG_LONG
		joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, "0x%llX", (long long)result);
#else
		joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, "0x%lX", (long)result);
#endif
	else if (mode_eng)
		joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, "%.16G", result);
	else
		joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, "%.16G", result);
	if (bw->parent->watom->what != TYPETW || mode_ins) {
		binsm(bw->cursor, sz(msgbuf));
		pfwrd(bw->cursor, zlen(msgbuf));
		bw->cursor->xcol = piscol(bw->cursor);
	} else {
		msgnw(bw->parent, msgbuf);
	}
	mode_ins = 0;
	return 0;
}

static int doumath(W *w, char *s, void *object, int *notify)
{
	return domath(w, s, object, notify, 0);
}

static int dosmath(W *w, char *s, void *object, int *notify)
{
	return domath(w, s, object, notify, 1);
}


B *mathhist = NULL;

int umath(W *w, int k)
{
	joe_set_signal(SIGFPE, fperr);
	if (wmkpw(w, "=", &mathhist, doumath, "Math", NULL, NULL, NULL, NULL, locale_map, 0)) {
		return 0;
	} else {
		return -1;
	}
}

/* Secure version: no macros allowed */

int usmath(W *w, int k)
{
	joe_set_signal(SIGFPE, fperr);
	if (wmkpw(w, "=", &mathhist, dosmath, "Math", NULL, NULL, NULL, NULL, locale_map, 0)) {
		return 0;
	} else {
		return -1;
	}
}
