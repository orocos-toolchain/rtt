/**
 * Created on 09/04/2003 for supplying missing functions in the rtai kernel.
 */


/* Functions for handling thread-specific data.  */

/* Create a key value identifying a location in the thread-specific
 *    data area.  Each thread maintains a distinct thread-specific data
 *       area.  DESTR_FUNCTION, if non-NULL, is called with the value
 *          associated to that key when the key is destroyed.
 *             DESTR_FUNCTION is not called if the value associated is NULL when
 *                the key is destroyed.  */
extern int pthread_key_create (pthread_key_t *__key,
                           void (*__destr_function) (void *)) __THROW;

/* Destroy KEY.  */
extern int pthread_key_delete (pthread_key_t __key) __THROW;

/* Store POINTER in the thread-specific data slot identified by KEY. */
extern int pthread_setspecific (pthread_key_t __key,
                        __const void *__pointer) __THROW;

/* Return current value of the thread-specific data slot identified by KEY.  */
extern void *pthread_getspecific (pthread_key_t __key) __THROW;


/* Functions for handling initialization.  */

/* Guarantee that the initialization function INIT_ROUTINE will be called
 *    only once, even if pthread_once is executed several times with the
 *       same ONCE_CONTROL argument. ONCE_CONTROL must point to a static or
 *          extern variable initialized to PTHREAD_ONCE_INIT.  */
extern int pthread_once (pthread_once_t *__once_control,
                     void (*__init_routine) (void)) __THROW;


/* Functions for handling cancellation.  */

/* Set cancelability state of current thread to STATE, returning old
 *    state in *OLDSTATE if OLDSTATE is not NULL.  */
extern int pthread_setcancelstate (int __state, int *__oldstate) __THROW;

/* Set cancellation state of current thread to TYPE, returning the old
 *    type in *OLDTYPE if OLDTYPE is not NULL.  */
extern int pthread_setcanceltype (int __type, int *__oldtype) __THROW;

