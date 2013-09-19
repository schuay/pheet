#ifdef UTS_TEST

#include <stdlib.h>

extern "C"
void impl_abort(int err) {
  exit(err);
}

extern "C"
char * impl_getName() {
	static char name[] = "pheet";
	return name;
}

extern "C"
// construct string with all parameter settings 
int impl_paramsToStr(char *, int ind) {
  return ind;
}

extern "C"
int impl_parseParam(char *, char *) {
  int err = 1;  // Return 0 on a match, nonzero on an error
  return err;
}

extern "C"
void impl_helpMessage() {
}

#endif
