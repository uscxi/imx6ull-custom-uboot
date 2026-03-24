#include <config.h>
#include <initcall.h>
#include <init.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/sections.h>
#include <hang.h>
#include <log.h>

DECLARE_GLOBAL_DATA_PTR;


static int setup_mon_len(void)
{
#if defined(__ARM__)
    gd->mon_len = (ulong)__bss_end - (ulong)_start;
#endif
    return 0;
}

int test_early_printf(void)
{
    early_printf("\n");
    early_printf("=================================\n");
    early_printf("  early_printf 功能测试\n");
    early_printf("=================================\n");
    
    /* 测试基本格式 */
    early_printf("字符: %c\n", 'A');
    early_printf("字符串: %s\n", "Hello, U-Boot!");
    
    /* 测试十进制（关键：验证无除法实现） */
    early_printf("十进制: %d\n", 12345);
    early_printf("负数: %d\n", -67890);
    early_printf("无符号: %u\n", 4294967295U);
    
    /* 测试十六进制（验证位运算实现） */
    early_printf("十六进制(小写): 0x%x\n", 0xdeadbeef);
    early_printf("十六进制(大写): 0x%X\n", 0xDEADBEEF);
    
    /* 测试宽度和填充 */
    early_printf("右对齐: [%8d]\n", 42);
    early_printf("零填充: [%08x]\n", 0xFF);
    early_printf("左对齐: [%-8s]\n", "test");
    
    /* 测试边界值 */
    early_printf("零: %d\n", 0);
    early_printf("最大值: %u\n", 0xFFFFFFFFUL);
    
    /* 测试指针 */
    early_printf("指针: %p\n", (void *)0x80000000);
    
    /* 测试二进制（如果启用） */
    early_printf("二进制: %b\n", 0b10101010);
    
    early_printf("=================================\n");
    early_printf("  测试完成\n");
    early_printf("=================================\n\n");
    
    return 0;
}

static const init_fnc_t init_sequence_f[] = {
    test_early_printf,
	setup_mon_len,
    NULL,
};

void board_init_f(ulong boot_flags)
{
	// struct board_f boardf;

	gd->flags = boot_flags;
	gd->flags &= ~GD_FLG_HAVE_CONSOLE;
	// gd->boardf = &boardf;

    if (initcall_run_list(init_sequence_f))
		hang();

	main_loop();
}
