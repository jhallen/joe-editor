/*
 * Example code to show how to copy the security context from one file to
 * another.
 */
#include "types.h"

#ifdef WITH_SELINUX
#include <selinux/selinux.h>
static int selinux_enabled = -1;
#endif

int
copy_security_context(const char *from_file, const char *to_file)
{
	int status = 0;
#ifdef WITH_SELINUX
	security_context_t from_context;
	security_context_t to_context;

	if (selinux_enabled == -1)
		selinux_enabled = (is_selinux_enabled() > 0);

	if (!selinux_enabled)
		return 0;

	if (getfilecon(from_file, &from_context) < 0) {
		/*
		 * If the filesystem doesn't support extended
		 * attributes, the original had no special security
		 * context and the target cannot have one either.
		 */
		if (errno == EOPNOTSUPP)
			return 0;

		error(0, errno, joe_gettext(_("Could not get security context for %s")),
		      from_file);
		return 1;
	}

	if (getfilecon(to_file, &to_context) < 0) {
		MSG_PUTS(_(joe_gettext(_("\nCould not get security context for "))));
		msg_outtrans(to_file);
		msg_putchar('\n');
		freecon(from_context);
		return 1;
	}

	if (zcmp(from_context, to_context) != 0) {
		if (setfilecon(to_file, from_context) < 0) {
			error(0, errno,
			      joe_gettext(_("Could not set security context for %s")),
			      to_file);
			status = 1;
		}
	}

	freecon(to_context);
	freecon(from_context);
#endif
	return status;
}

int match_default_security_context(const char *from_file)
{
#ifdef WITH_SELINUX
	security_context_t scontext;

	if (selinux_enabled == -1)
		selinux_enabled = (is_selinux_enabled() > 0);

	if (!selinux_enabled)
		return 0;

	if (getfilecon(from_file, &scontext) < 0) {
		/*
		 * If the filesystem doesn't support extended
		 * attributes, the original had no special security
		 * context and the target cannot have one either.
		 */
		if (errno == EOPNOTSUPP)
			return 0;

		error(0, errno, joe_gettext(_("Could not get security context for %s")),
		      from_file);
		return 1;
	}

	if (setfscreatecon(scontext) < 0) {
		error(0, errno,
		      joe_gettext(_("Could not set default security context for %s")),
		      from_file);
		freecon(scontext);
		return 1;
	}
	freecon(scontext);
#endif
	return 0;
}


int reset_default_security_context()
{
#ifdef WITH_SELINUX
	if (selinux_enabled == -1)
		selinux_enabled = (is_selinux_enabled() > 0);

	if (!selinux_enabled)
		return 0;

	if (setfscreatecon(0) < 0) {
		error(0, errno, joe_gettext(_("Could not reset default security context")));
		return 1;
	}
#endif
	return 0;
}


int output_security_context(const char *from_file)
{
#ifdef WITH_SELINUX
	security_context_t scontext;

	if (selinux_enabled == -1)
		selinux_enabled = (is_selinux_enabled() > 0);
	if (!selinux_enabled)
		return 0;

	if (getfilecon(from_file, &scontext) < 0) {
		/*
		 * If the filesystem doesn't support extended
		 * attributes, the original had no special security
		 * context and the target cannot have one either.
		 */
		if (errno == EOPNOTSUPP)
			return 0;
		
		error(0, errno,joe_gettext(_("Could not get security context for %s")),
		      from_file);
		return 1;
	}

	error(0, 0, joe_gettext(_("%s Security Context %s")), from_file, scontext);
	freecon(scontext);
#endif
	return 0;
}

#if 0

/*
  Test program compile using the following command
  cc -o t t.c -DWITH_SELINUX -DTEST -lselinux
 */

#include <stdio.h>
#include <stdlib.h>
main(int argc, char **argv)
{

	printf("%d: %s\n", argc, argv[1]);
	if (argc == 3) {
		copy_security_context(argv[1], argv[2]);
		output_security_context(argv[2]);
	}
	if (argc == 2) {
		FILE *fd;
		char *temp;
		match_default_security_context(argv[1]);
		mkstemp(temp);
		printf("temp=%s", temp);
		fd = fopen(temp, "w");
		fclose(fd);
		output_security_context(temp);
		reset_default_security_context();
	}
}
#endif
