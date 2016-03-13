/**
 * @file   error_code.h
 * @Author Brendan Heal
 */
#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

enum {

/** Recoverable Errors*/
    
/** Too many tasks*/
ERR_1_TOO_MANY_TASK,
/** Too many mutexes */
ERR_2_TOO_MANY_MUTEX,

/** No such task */
ERR_3_NO_SUCH_TASK,
/** No such mutex */
ERR_4_NO_SUCH_MUTEX,


/** Unrecoverable Errors */

/** Stack space too large.*/
FAIL_1_STACK_OVERFLOW,
/** Mutex deadlock.*/
FAIL_2_DEADLOCK,
};


#endif
