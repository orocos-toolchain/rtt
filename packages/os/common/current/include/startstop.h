
/**
 * @file startstop.h
 * Prototype definitions of common os init/exit calls.
 */

#ifdef __cplusplus
extern "C"
#endif
int __os_init(int argc, char** argv );

#ifdef __cplusplus
extern "C"
#endif
void __os_exit(void);
