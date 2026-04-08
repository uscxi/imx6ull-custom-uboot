#ifndef __KERNEL_PRINTK__
#define __KERNEL_PRINTK__

#include <log.h>
#include <stdio.h>
#include <linux/compiler.h>

#define KERN_EMERG
#define KERN_ALERT
#define KERN_CRIT
#define KERN_ERR
#define KERN_WARNING
#define KERN_NOTICE
#define KERN_INFO
#define KERN_DEBUG
#define KERN_CONT

#define printk(fmt, ...) \
	printf(fmt, ##__VA_ARGS__)

/*
 * Dummy printk for disabled debugging statements to use whilst maintaining
 * gcc's format checking.
 */
#define no_printk(fmt, ...)				\
({							\
	if (0)						\
		printk(fmt, ##__VA_ARGS__);		\
	0;						\
})

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#endif
