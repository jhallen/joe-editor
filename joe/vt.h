#define MAXARGS 2

enum vt_state {
	vt_idle,
	vt_esc,
	vt_args,
};

struct vt_context {
	enum vt_state state;

	/* Record attribute sequence */
	char buf[100];
	int bufx;

	/* Implement xn glitch */
	int xn;

	/* Numeric arguments */
	int argv[MAXARGS + 1];
	int argc;

	int top;
	int height;
	int width;

	P *vtcur;
	B *b;

	KBD *kbd;
};


VT *mkvt(B *b, int top, int height, int width);
void vtrm(VT *vt);

void vt_data(VT *vt, unsigned char *dat, int siz);
