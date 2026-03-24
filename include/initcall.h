#ifndef __INITCALL_H
#define __INITCALL_H

#include <asm/types.h>

/**
 * init_fnc_t - Init function
 *
 * Return: 0 if OK -ve on error
 */
typedef int (*init_fnc_t)(void);

/**
 * initcall_run_list() - Run through a list of function calls
 *
 * This calls functions one after the other, stopping at the first error, or
 * when NULL is obtained.
 *
 * @init_sequence: NULL-terminated init sequence to run
 * Return: 0 if OK, or -ve error code from the first failure
 */
int initcall_run_list(const init_fnc_t init_sequence[]);

#endif
