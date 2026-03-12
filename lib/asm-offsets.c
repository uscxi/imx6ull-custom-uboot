#include <asm-offsets.h>
#include <asm/global_data.h>
#include <linux/kbuild.h>

int main(void)
{
	/* Round up to make sure size gives nice stack alignment */
	DEFINE(GENERATED_GBL_DATA_SIZE,
		(sizeof(struct global_data) + 15) & ~15);
	DEFINE(GD_SIZE, sizeof(struct global_data));

	return 0;
}
