#ifndef _ERRNO_H
#define _ERRNO_H

#include <linux/errno.h>

static const char error_message[] = "";

static inline const char *errno_str(int errno)
{
	return error_message;
}

#endif /* _ERRNO_H */
