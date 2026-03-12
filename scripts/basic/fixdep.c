/*
 * 针对内核构建，对 gcc -MD 生成的依赖列表进行“优化”
 *
 * Author       Kai Germaschewski
 * Copyright    2002 by Kai Germaschewski  <kai.germaschewski@gmx.de>
 *
 * 本软件可依据 GNU 通用公共许可证条款使用和分发，该许可证通过引用纳入于此。
 *
 * 简介：
 * gcc 能够生成完善且准确的依赖关系列表，它能告知 make 何时需要重新编译文件。
 * 然而直接使用该列表存在缺陷：内核中几乎所有文件都会包含 autoconf.h。
 *
 * 若用户重新执行 make *config，autoconf.h 将被再生成。make 检测到此情况后，会重建所有包含 autoconf.h 的文件，
 * 即几乎所有文件。当用户仅将 CONFIG_HIS_DRIVER 从 n 改为 m 时，这种行为极其烦人。
 *
 * 因此我们采用与“mkdep”相同的策略：通过将对 autoconf.h 的依赖替换为对每个列出的先决条件中涉及的所有配置项的依赖。
 *
 * kconfig 会在 include/config/ 目录下构建一个配置树，其中每个配置符号都对应一个空文件。
 * 当配置更新时，代表已变更配置选项的对应文件会被更新时间戳，这样 make 就能检测到变更，继而重新编译使用这些配置符号的文件。
 *
 * 因此当用户修改 CONFIG_HIS_DRIVER 配置时，仅依赖于 "include/config/his/driver.h" 的目标文件会被重新编译，
 * 这意味着大概率只有对应的驱动程序需要重新编译;-)
 *
 *
 * 顺带一提，上述方案的设计理念可追溯至 Michael E Chastain（据我所知）。
 *
 * 为确保依赖关系准确，需要解决两个核心问题：
 * 1.若编译器读取的任何文件发生变更，需触发重建
 * 2.若编译文件时使用的命令行参数发生变更，也应触发重建
 *
 * 前者通过处理 gcc 的 -MD 输出结果实现，后者则通过保存旧目标文件的编译命令行并与新命令行进行比对来实现。
 * 
 * 同样，这个想法相当古老，早在 kbuild-devel 邮件列表上就讨论过。因目前无法可靠联网核查，暂不具体列举参与者姓名。
 *
 * 本程序的代码部分基于 mkdep.c 实现，该文件对其历史沿革有如下说明：
 *
 *  版权已放弃，Michael Chastain，<mailto:mec@shout.net>。
 *  这是 Werner Almesberger 编写的 syncdep.pl 的 C 语言版本。
 *
 * 调用方式：
 *
 * fixdep <depfile> <target> <cmdline>
 *
 * 程序将读取<depfile>
 * 转换后的依赖片段会输出至标准输出。
 * 首先生成一行：
 *      cmd_<target> = <cmdline>
 * 随后将 .<target>.d 文件复制到标准输出，同时过滤掉对 autoconf.h 的依赖，
 * 并在任何前提条件中遇到 CONFIG_MY_OPTION 时，为其添加对 include/config/my/option.h 的依赖关系。
 *
 * 我们甚至不尝试真正解析头文件，只是简单地进行grep操作，即如果CONFIG_FOO出现在注释中，也会被捕获。
 * 这在正确性方面不成问题，因为这只会导致依赖项过多，从而确保我们不会遗漏任何重建操作。
 * 由于人们通常不会在各处随意添加无关的CONFIG_选项，因此这也不构成效率问题。
 *
 * （注：完整移植 mkdep 状态机虽易，但增加的复杂度并不值得）
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

char tmp_buf[256];  /* hack for U-Boot */

static void usage(void)
{
	fprintf(stderr, "Usage: fixdep <depfile> <target> <cmdline>\n");
	exit(1);
}

/*
 * 在本程序的预期使用场景中，标准输出会被重定向至 .*.cmd 文件。
 * 必须检查 printf() 和 putchar() 的返回值以捕获任何错误，例如“设备空间不足”。
 */
static void xprintf(const char *format, ...)
{
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vprintf(format, ap);
	if (ret < 0) {
		perror("fixdep");
		exit(1);
	}
	va_end(ap);
}

static void xputchar(int c)
{
	int ret;

	ret = putchar(c);
	if (ret == EOF) {
		perror("fixdep");
		exit(1);
	}
}

/*
    * 从符号名称打印出依赖路径
 */
static void print_dep(const char *m, int slen, const char *dir)
{
	int c, prev_c = '/', i;

	xprintf("    $(wildcard %s/", dir);
	for (i = 0; i < slen; i++) {
		c = m[i];
		if (c == '_')
			c = '/';
		else
			c = tolower(c);
		if (c != '/' || prev_c != '/')
			xputchar(c);
		prev_c = c;
	}
	xprintf(".h) \\\n");
}

struct item {
	struct item	*next;
	unsigned int	len;
	unsigned int	hash;
	char		name[0];
};

#define HASHSZ 256
static struct item *hashtab[HASHSZ];

static unsigned int strhash(const char *str, unsigned int sz)
{
	/* fnv32 hash */
	unsigned int i, hash = 2166136261U;

    for (i = 0; i < sz; i++)
        hash = (hash ^ str[i]) * 0x01000193;
    return hash;
}

/*
 * Lookup a value in the configuration string.
 */
static int is_defined_config(const char *name, int len, unsigned int hash)
{
	struct item *aux;

	for (aux = hashtab[hash % HASHSZ]; aux; aux = aux->next) {
		if (aux->hash == hash && aux->len == len &&
		    memcmp(aux->name, name, len) == 0)
			return 1;
	}
	return 0;
}

/*
 * 向配置字符串添加新值。
 */
static void define_config(const char *name, int len, unsigned int hash)
{
	struct item *aux = malloc(sizeof(*aux) + len);

	if (!aux) {
		perror("fixdep:malloc");
		exit(1);
	}
	memcpy(aux->name, name, len);
	aux->len = len;
	aux->hash = hash;
	aux->next = hashtab[hash % HASHSZ];
	hashtab[hash % HASHSZ] = aux;
}


/*
 * Record the use of a CONFIG_* word.
 */
static void use_config(const char *m, int slen)
{
	unsigned int hash = strhash(m, slen);

	if (is_defined_config(m, slen, hash))
	    return;

	define_config(m, slen, hash);
	print_dep(m, slen, "include/config");
}

/* test if s ends in sub */
static int str_ends_with(const char *s, int slen, const char *sub)
{
	int sublen = strlen(sub);

	if (sublen > slen)
		return 0;

	return !memcmp(s + slen - sublen, sub, sublen);
}

static void parse_config_file(const char *p)
{
	const char *q, *r;
	const char *start = p;

	while ((p = strstr(p, "CONFIG_"))) {
		if (p > start && (isalnum(p[-1]) || p[-1] == '_')) {
			p += 7;
			continue;
		}
		p += 7;
		q = p;
		while (isalnum(*q) || *q == '_')
			q++;
		if (str_ends_with(p, q - p, "_MODULE"))
			r = q - 7;
		else
			r = q;
		/*
		 * U-Boot also handles
		 *   CONFIG_IS_ENABLED(...)
		 *   CONFIG_VAL(...)
		 */
		if ((q - p == 10 && !memcmp(p, "IS_ENABLED(", 11)) ||
		    (q - p == 3 && !memcmp(p, "VAL(", 4))) {
			p = q + 1;
			q = p;
			while (isalnum(*q) || *q == '_')
				q++;
			r = q;
			if (r > p && tmp_buf[0]) {
				memcpy(tmp_buf + 4, p, r - p);
				r = tmp_buf + 4 + (r - p);
				p = tmp_buf;
			}
		}
		/* end U-Boot hack */

		if (r > p)
			use_config(p, r - p);
		p = q;
	}
}

static void *read_file(const char *filename)
{
	struct stat st;
	int fd;
	char *buf;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "fixdep: error opening file: ");
		perror(filename);
		exit(2);
	}
	if (fstat(fd, &st) < 0) {
		fprintf(stderr, "fixdep: error fstat'ing file: ");
		perror(filename);
		exit(2);
	}
	buf = malloc(st.st_size + 1);
	if (!buf) {
		perror("fixdep: malloc");
		exit(2);
	}
	if (read(fd, buf, st.st_size) != st.st_size) {
		perror("fixdep: read");
		exit(2);
	}
	buf[st.st_size] = '\0';
	close(fd);

	return buf;
}

/* Ignore certain dependencies */
static int is_ignored_file(const char *s, int len)
{
	return str_ends_with(s, len, "include/generated/autoconf.h") ||
	       str_ends_with(s, len, "include/generated/autoksyms.h");
}

/*
 * Important: The below generated source_foo.o and deps_foo.o variable
 * assignments are parsed not only by make, but also by the rather simple
 * parser in scripts/mod/sumversion.c.
 */
static void parse_dep_file(char *m, const char *target)
{
	char *p;
	int is_last, is_target;
	int saw_any_target = 0;
	int is_first_dep = 0;
	void *buf;

	while (1) {
		/* Skip any "white space" */
		while (*m == ' ' || *m == '\\' || *m == '\n')
			m++;

		if (!*m)
			break;

		/* Find next "white space" */
		p = m;
		while (*p && *p != ' ' && *p != '\\' && *p != '\n')
			p++;
		is_last = (*p == '\0');
		/* Is the token we found a target name? */
		is_target = (*(p-1) == ':');
		/* Don't write any target names into the dependency file */
		if (is_target) {
			/* The /next/ file is the first dependency */
			is_first_dep = 1;
		} else if (!is_ignored_file(m, p - m)) {
			*p = '\0';

			/*
			 * Do not list the source file as dependency, so that
			 * kbuild is not confused if a .c file is rewritten
			 * into .S or vice versa. Storing it in source_* is
			 * needed for modpost to compute srcversions.
			 */
			if (is_first_dep) {
				/*
				 * If processing the concatenation of multiple
				 * dependency files, only process the first
				 * target name, which will be the original
				 * source name, and ignore any other target
				 * names, which will be intermediate temporary
				 * files.
				 */
				if (!saw_any_target) {
					saw_any_target = 1;
					xprintf("source_%s := %s\n\n",
						target, m);
					xprintf("deps_%s := \\\n", target);
				}
				is_first_dep = 0;
			} else {
				xprintf("  %s \\\n", m);
			}

			buf = read_file(m);
			parse_config_file(buf);
			free(buf);
		}

		if (is_last)
			break;

		/*
		 * Start searching for next token immediately after the first
		 * "whitespace" character that follows this token.
		 */
		m = p + 1;
	}

	if (!saw_any_target) {
		fprintf(stderr, "fixdep: parse error; no targets found\n");
		exit(1);
	}

	xprintf("\n%s: $(deps_%s)\n\n", target, target);
	xprintf("$(deps_%s):\n", target);
}
#ifdef CONFIG_XX
#endif
int main(int argc, char *argv[])
{
	const char *depfile, *target, *cmdline;
	void *buf;

	if (argc != 4)
		usage();

	depfile = argv[1];
	target = argv[2];
	cmdline = argv[3];

	/* hack for U-Boot */
	if (!strncmp(target, "spl/", 4))
		strcpy(tmp_buf, "SPL_");
	else if (!strncmp(target, "tpl/", 4))
		strcpy(tmp_buf, "TPL_");
	else if (!strncmp(target, "vpl/", 4))
		strcpy(tmp_buf, "VPL_");
	/* end U-Boot hack */

	xprintf("cmd_%s := %s\n\n", target, cmdline);

	buf = read_file(depfile);
	parse_dep_file(buf, target);
	free(buf);

	return 0;
}
