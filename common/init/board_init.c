#include <config.h>
#include <init.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * 从“顶部”地址分配预留空间用作“全局数据区”，并返回已分配空间的“底部”地址
 *
 * 说明：
 *
 * 实际的预留操作无法在此函数内完成，因为这会需要改变C栈指针，因此该操作将由调用者在从此函数返回后进行。
 *
 * 重要事项：
 *
 * 每个分配的“内存块”可能具有不同的对齐约束。目前：
 * - GD（全局数据）向下对齐到16字节边界
 * - 早期malloc内存池未进行对齐，因此它遵循我们所构建架构的栈对齐约束。
 * - GD最后分配，因此此函数的返回值既是预留区域的底部地址，也是GD的地址（如果调用上下文需要的话）。
 */

ulong board_init_f_alloc_reserve(ulong top)
{
	/* Reserve early malloc arena */
#ifndef CFG_MALLOC_F_ADDR
#if CONFIG_IS_ENABLED(SYS_MALLOC_F)
	top -= CONFIG_VAL(SYS_MALLOC_F_LEN);
#endif
#endif
	/* LAST : reserve GD (rounded up to a multiple of 16 bytes) */
	top = rounddown(top-sizeof(struct global_data), 16);

	return top;
}

/*
 * 初始化预留空间（该空间已由C运行时环境处理代码安全地分配在C栈上）。
 *
 * 说明：
 *
 * 实际的预留操作由调用者完成；从 base 到 base+size-1 的位置（其中 'size' 是上述分配函数返回的值）可以自由访问，
 * 而不会有破坏C运行时环境的风险。
 *
 * 重要事项：
 *
 * 从上述分配函数返回后，在某些架构上，调用者会将 gd 设置为最低的预留位置。因此，在此初始化函数中，
 * 全局数据必须放置在 base 处。
 *
 * 同样重要：
 *
 * 在某些架构上，进入此函数时 gd 已经是有效的。而在其他架构上，只有在 arch_setup_gd() 返回后 gd 才会变为有效。
 * 因此，访问全局数据必须遵循以下方式：
 *
 * - 如果在调用 arch_setup_gd() 之前，需通过 gd_ptr 访问；
 *
 * - 一旦 arch_setup_gd() 被调用后，则可通过 gd 访问。
 *
 * 在调用 arch_setup_gd() 之前，切勿使用 'gd->'！
 *
 * 也很重要：
 *
 * 每个“内存块”（GD、早期malloc区域……）的初始化都以一行递增语句结束，形式为 'base += <某个大小>'。
 * 最后一条这样的递增语句看似无用，因为在此递增之后 base 将不再被使用；但是，如果/当有新的“内存块”被追加时，
 * 这条递增语句将至关重要，因为它会为这个新块提供正确的 base 值（而该新块也必须以自身的递增语句结束）。
 * 此外，编译器的优化器会静默地检测并移除最后那条 base 递增语句，
 * 因此保留那条最后的（看似无用的）递增语句不会导致代码量增加。
 */

void board_init_f_init_reserve(ulong base)
{
	struct global_data *gd_ptr;

	/*
	 * clear GD entirely and set it up.
	 * Use gd_ptr, as gd may not be properly set yet.
	 */

	gd_ptr = (struct global_data *)base;
	/* zero the area */
	memset(gd_ptr, '\0', sizeof(*gd));
	/* set GD unless architecture did it already */
#if !defined(CONFIG_ARM)
	arch_setup_gd(gd_ptr);
#endif

	/* next alloc will be higher by one GD plus 16-byte alignment */
	base += roundup(sizeof(struct global_data), 16);

	/*
	 * record early malloc arena start.
	 * Use gd as it is now properly set for all architectures.
	 */

#if CONFIG_IS_ENABLED(SYS_MALLOC_F)
	/* go down one 'early malloc arena' */
	gd->malloc_base = base;
#if CONFIG_IS_ENABLED(ZERO_MEM_BEFORE_USE)
	memset((void *)base, '\0', CONFIG_VAL(SYS_MALLOC_F_LEN));
#endif
#endif
}
