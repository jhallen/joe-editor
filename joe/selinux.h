int copy_security_context(const char *from_file, const char *to_file);
int match_default_security_context(const char *from_file);
int reset_default_security_context();
int output_security_context(const char *from_file);
