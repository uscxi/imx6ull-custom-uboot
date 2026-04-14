#include <config.h>
#include <init.h>
#include <log.h>

/*
 * LED init
 */
void led_init(void)
{
    // Enable clock
    CCM_CCGR1 = 0xffffffff;

    /* 1、初始化IO复用 */
	SW_MUX_GPIO1_IO03 = 0x5;	/* 复用为GPIO1_IO03 */
    /* 2、配置GPIO1_IO03的IO属性 */
    SW_PAD_GPIO1_IO03 = 0X10B0;

	/* 3、初始化GPIO */
	GPIO1_GDIR = 0X0000008;	/* GPIO1_IO03设置为输出 */
	/* 4、设置GPIO1_IO03输出低电平，打开LED0 */
	GPIO1_DR = 0X0;
}

/* IMX6ULL-Mini板的LED0是低电平点亮 */
void led_on(void)
{
	/* 将GPIO1_DR的bit3清零	*/
	GPIO1_DR &= ~(1<<3); 
}

void led_off(void)
{
	/* 将GPIO1_DR的bit3置1 */
	GPIO1_DR |= (1<<3);
}

/* 使用空操作循环实现短时间延时 */
void _delay(volatile unsigned int n)
{
	while(n--){}
}

/* 延时函数,在396Mhz的主频下，延时时间大约为1ms */
void delay(volatile unsigned int n)
{
	while(n--)
	{
		_delay(0x7ff);
	}
}

void main_loop(void)
{
    led_init();			/* 初始化led */

    log(LOGC_TEST, LOGL_INFO, "LOG TEST!\n");
    debug("Early Debug UART for IMX6ULL\n");

	while(1)			/* 死循环 */
	{	
		led_off();		/* 关闭LED */
		delay(1000);		/* 延时大约1s */

		led_on();		/* 打开LED		 	*/
		delay(1000);		/* 延时大约1s */
	}
}
