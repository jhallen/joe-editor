#define MAXARGS 2

enum vt_state {
	vt_idle,
	vt_esc,
	vt_args,
	vt_cmd
};

struct vt_context {
	enum vt_state state;

	/* Record attribute sequence */
	unsigned char buf[100];
	int bufx;

	/* Implement xn glitch */
	int xn;

	/* Numeric arguments */
	int argv[MAXARGS + 1];
	int argc;

	P *top;
	int height;
	int width;

	int regn_top;
	int regn_bot;

	P *vtcur;
	B *b;

	KBD *kbd;

	int attr; /* Current attributes */
};


VT *mkvt(B *b, P *top, int height, int width);
void vtrm(VT *vt);

MACRO *vt_data(VT *vt, unsigned char **indat, int *insiz);

void vt_resize(VT *vt, P *top, int height, int width);
