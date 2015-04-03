#define MAXARGS 2

enum vt_state {
	vt_idle,
	vt_esc,
	vt_args,
	vt_cmd,
	vt_utf,
	vt_osc,
	vt_osce
};

struct vt_context {
	enum vt_state state;

	/* Record attribute sequence */
	char buf[100];
	ptrdiff_t bufx;

	/* Numeric arguments */
	ptrdiff_t argv[MAXARGS + 1];
	ptrdiff_t argc;

	P *top;
	ptrdiff_t height;
	ptrdiff_t width;

	ptrdiff_t regn_top;
	ptrdiff_t regn_bot;

	P *vtcur;
	B *b;

	KBD *kbd;

	int attr; /* Current attributes */

	struct utf8_sm utf8_sm;
};


VT *mkvt(B *b, P *top, ptrdiff_t height, ptrdiff_t width);
void vtrm(VT *vt);

MACRO *vt_data(VT *vt, char **indat, ptrdiff_t *insiz);

void vt_resize(VT *vt, P *top, ptrdiff_t height, ptrdiff_t width);
