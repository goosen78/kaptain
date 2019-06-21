#ifndef GLOBAL_HH
#define GLOBAL_HH

class Error;

extern void STATUS(const char *);
extern void DONE(void);
extern Error * glob_error;

#define MAKE_ERROR(s) (glob_error->error(s))
#define MAKE_WARNING(s) (glob_error->warning(s))
#define GO_ON() (glob_error->go_on())

#endif // GLOBAL_HH
