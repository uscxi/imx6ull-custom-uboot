#include <asm-offsets.h>
#include <asm/global_data.h>
#include <asm/u-boot.h>

#include <linux/kbuild.h>

int main(void)
{
	/* Round up to make sure size gives nice stack alignment */
	DEFINE(GENERATED_GBL_DATA_SIZE,
		(sizeof(struct global_data) + 15) & ~15);

	DEFINE(GD_SIZE, sizeof(struct global_data));

	DEFINE(GD_BD, offsetof(struct global_data, bd));

	DEFINE(GD_FLAGS, offsetof(struct global_data, flags));

	DEFINE(GD_RELOCADDR, offsetof(struct global_data, relocaddr));

	DEFINE(GD_RELOC_OFF, offsetof(struct global_data, reloc_off));

	DEFINE(GD_START_ADDR_SP, offsetof(struct global_data, start_addr_sp));

	DEFINE(GD_NEW_GD, offsetof(struct global_data, new_gd));

	return 0;
}
