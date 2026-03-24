#include <debug_uart.h>
#include <log.h>


static void early_putchar(char ch)
{
    printch(ch);
}

static void early_puts(const char *str)
{
    if (str)
        printascii(str);
}

/* 判断字符是否为数字 */
static int is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

/* 从字符串中解析数字，返回解析后的值，同时移动指针 */
static int parse_number(const char **fmt)
{
    int num = 0;
    while (is_digit(**fmt)) {
        num = num * 10 + (**fmt - '0');
        (*fmt)++;
    }
    return num;
}

/*
 * 无符号整数转字符串
 * 
 * @val:    要转换的值
 * @buf:    输出缓冲区（必须足够大，建议至少 24 字节）
 * @base:   进制（10 或 16）
 * @upper:  十六进制时是否使用大写字母
 * 
 * @return: 指向转换结果字符串的指针（在 buf 内部）
 */
static char *uint_to_str(unsigned long val, char *buf, int base, int upper)
{
    static const char digits_lower[] = "0123456789abcdef";
    static const char digits_upper[] = "0123456789ABCDEF";
    const char *digits = upper ? digits_upper : digits_lower;
    char *p = buf + 23;  /* 从缓冲区末尾开始填充 */

    *p = '\0';

    if (val == 0) {
        *--p = '0';
        return p;
    }

    while (val) {
        *--p = digits[val % base];
        val /= base;
    }

    return p;
}

/*
 * 有符号整数转字符串
 */
static char *int_to_str(long val, char *buf)
{
    int negative = 0;
    char *p;
    
    if (val < 0) {
        negative = 1;
        val = -val;
    }
    
    p = uint_to_str((unsigned long)val, buf, 10, 0);
    
    if (negative)
        *--p = '-';
    
    return p;
}

/*=============================================================================
 * 格式化输出辅助函数
 *============================================================================*/

static inline int early_strlen(const char *s)
{
    int len = 0;
    if (!s)
        return 0;
    while (*s++)
        len++;
    return len;
}

static void print_padding(int count, char pad_char)
{
    while (count-- > 0)
        early_putchar(pad_char);
}

static void print_with_width(const char *str, int width, int pad_zero, int left)
{
    int len = early_strlen(str);
    int padding = (width > len) ? (width - len) : 0;
    char pad_char = pad_zero ? '0' : ' ';
    
    /* 零填充时，负号要在前面 */
    if (pad_zero && *str == '-') {
        early_putchar('-');
        str++;
        len--;
        padding = (width > len) ? (width - len) : 0;
    }
    
    /* 右对齐：先输出填充 */
    if (!left && padding > 0)
        print_padding(padding, pad_char);
    
    /* 输出字符串本身 */
    early_puts(str);
    
    /* 左对齐：后输出填充 */
    if (left && padding > 0)
        print_padding(padding, ' ');
}

/*
 * early_printf - 极简格式化输出
 * 
 * 使用示例:
 *   early_printf("Hello %s!\n", "World");
 *   early_printf("Value: %d (0x%08X)\n", -42, 0xDEADBEEF);
 *   early_printf("Pointer: %p\n", &some_var);
 */
void early_printf(const char *fmt, ...)
{
    __builtin_va_list args;
    char buf[24];           /* 足够容纳 64 位整数的字符串表示 */
    char *str;
    int width;
    int pad_zero;
    int left_align;
    int is_long;
    
    __builtin_va_start(args, fmt);
    
    while (*fmt) {
        if (*fmt != '%') {
            early_putchar(*fmt);
            fmt++;
            continue;
        }
        
        /* 跳过 '%' */
        fmt++;
        
        /* 处理 %% */
        if (*fmt == '%') {
            early_putchar('%');
            fmt++;
            continue;
        }
        
        /* 解析标志位 */
        pad_zero = 0;
        left_align = 0;
        if (*fmt == '-') {
            left_align = 1;
            fmt++;
        } else if (*fmt == '0') {
            pad_zero = 1;
            fmt++;
        }
        
        /* 解析宽度 */
        width = 0;
        if (is_digit(*fmt)) {
            width = parse_number(&fmt);
        }
        
         /* 解析长度修饰符 */
        is_long = 0;
        if (*fmt == 'l') {
            is_long = 1;
            fmt++;
            if (*fmt == 'l')
                fmt++;
        }

        /* 解析格式符并处理 */
        switch (*fmt) {
        
        case 'c': {
            /* 字符 */
            char c = (char)__builtin_va_arg(args, int);
            if (width > 1) {
                print_padding(width - 1, ' ');
            }
            early_putchar(c);
            break;
        }
        
        case 's': {
            /* 字符串 */
            const char *s = __builtin_va_arg(args, const char *);
            if (!s)
                s = "(null)";
            print_with_width(s, width, 0, left_align);
            break;
        }
        
        case 'd':
        case 'i': {
            /* 有符号十进制 */
            long val;
            
            if (is_long)
                val = __builtin_va_arg(args, long);
            else
                val = __builtin_va_arg(args, int);
            
            str = int_to_str(val, buf);
            print_with_width(str, width, pad_zero, left_align);
            break;
        }
        
        case 'u': {
            /* 无符号十进制 */
            unsigned long val;
            
            if (is_long)
                val = __builtin_va_arg(args, unsigned long);
            else
                val = __builtin_va_arg(args, unsigned int);
            
            str = uint_to_str(val, buf, 10, 0);
            print_with_width(str, width, pad_zero, left_align);
            break;
        }
        
        case 'x':
        case 'X': {
            /* 十六进制 */
            unsigned long val;
            int upper = (*fmt == 'X');
            
            if (is_long)
                val = __builtin_va_arg(args, unsigned long);
            else
                val = __builtin_va_arg(args, unsigned int);
            
            str = uint_to_str(val, buf, 16, upper);
            print_with_width(str, width, pad_zero, left_align);
            break;
        }
        
        case 'p': {
            /* 指针 */
            void *ptr = __builtin_va_arg(args, void *);
            unsigned long val = (unsigned long)ptr;
            
            early_puts("0x");
            str = uint_to_str(val, buf, 16, 0);
            /* 指针默认 8 位宽度（32位系统）零填充 */
            print_with_width(str, 8, 1, 0);
            break;
        }

        case 'b': {
            unsigned long val = is_long ? __builtin_va_arg(args, unsigned long)
                                        : __builtin_va_arg(args, unsigned int);
            early_puts("0b");
            str = uint_to_str(val, buf, 2, 0);
            print_with_width(str, width, pad_zero, left_align);
            break;
        }
        
        default:
            /* 未知格式符，原样输出 */
            early_putchar('%');
            if (pad_zero)
                early_putchar('0');
            if (width) {
                char wbuf[12];
                char *wp = uint_to_str(width, wbuf, 10, 0);
                early_puts(wp);
            }
            if (is_long)
                early_putchar('l');
            early_putchar(*fmt);
            break;
        }
        
        fmt++;
    }
    
    __builtin_va_end(args);
}
