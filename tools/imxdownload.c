#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "imxdownload.h"

#define SHELLCMD_LEN	(200)
#define BIN_OFFSET		(3072)

/* 此宏指明是否打印u-boot.imx的IVT DCD表信息，不同的开发板其IVT和DCD
 * 表的数据是不同的，因此需要获取所使用的开发板的IVT和DCD表信息，最
 * 简单的方法就是读取开发板配套资料里面的u-boot.imx的前1KB数据，理论上
 * 应该读取3KB的数据，但是表信息远远没有3K这么多，因此读1KB即可 
 */
#define PRINT_TAB		0	
/*
 * 介绍： 此软件是针对NXP的IMX6U系列芯片的，软件用来烧写bin文件到SD卡里面，
 *        本软件会自动添加IVT、DCD等信息到原始的bin文件里面，主要用于裸机和uboot的烧写。
 * 使用方法： 1、编译好原始的二进制bin文件，如，u-boot.bin等，并将编译好的.bin文件和本
 *             软件放置到同一个目录下！！！！
 *        	2、执行命令sudo ./imxdownload <soucre_bin> <sd_device>
 *             如烧写u-boot.bin到/dev/sdd中即可使用如下所示命令:
 *             sudo ./imxdownload u-boot.bin /dev/sdd
 */

# define PACK_VERSION "1.0"

static const char* get_version(void)
{
    return PACK_VERSION;
}

static const char short_option[] = "i:d:m:hV";
static struct option const long_option[] = {
    {"input",   required_argument, NULL, 'i'},
    {"sd",      required_argument, NULL, 'd'},
    {"size",    required_argument, NULL, 'm'},
    {"version", no_argument,       NULL, 'V'},
    {"help",    no_argument,       NULL, 'h'},
};

static void usage(void)
{	
	fprintf(stdout, "I.MX6ULL download software V%s\n", get_version());
	fprintf(stdout, "Flash the bare-metal binary file to the SD card.\n");
    fprintf(stdout, "Only applies to NXP I.MX6ULL series board with 256MB and "
                    "512MB(default) DDR3.\n");

    fprintf(stdout, "\nUsage:\n");
    fprintf(stdout, "\t./imxdownload -i <soucre_bin> -d <sd_device> "
                    "[-m <ddr_size>]\n");
    fprintf(stdout, "For example:\n\t./imxdownload -i u-boot.bin "
                    "-d /dev/sdX [-m 512MB]\n");
}

static void help_display(void)
{
    usage();
    fprintf(stdout, "\nRequired Option:\n");
	fprintf(stdout, "-i, --input\n");
	fprintf(stdout, "\tThe input bare-metal binary.\n");
    fprintf(stdout, "-d, --sd\n");
    fprintf(stdout, "\tDevice Partitioning of SD device. eg:/dev/sdb\n");

    fprintf(stdout, "\nOptional Option:\n");
    fprintf(stdout, "-m, --size\n");
    fprintf(stdout, "\tDDR size. 256MB or 512MB(default).\n");
    fprintf(stdout, "-V, --version\n");
    fprintf(stdout, "\tSoftware version.\n");
    fprintf(stdout, "-h, --help\n");
    fprintf(stdout, "\tHelp information.\n");
}

int main(int argc, char *argv[])
{
	FILE *fp;
    const char *in = NULL;
    const char *dev = "/dev/null";
    char *out = NULL;

    char *tail = NULL;
    int sd = 0;
	unsigned char *buf;
	char *cmd_Buf;
    int opt;
    int ret = 0;
	int nbytes, len;
	int ddr_size = 512; /* default set 512MB */

	while ((opt = getopt_long(argc, argv, short_option,
            long_option, NULL)) != -1) {
        switch (opt) {
            case 'i':
                in = optarg;
                break;
            case 'd':
                dev = optarg;
                sd = 1;
                break;
            case 'm':
                ddr_size = strtol(optarg, NULL, 10);
                break;
            case 'V':
                fprintf(stdout, "I.MX6ULL download software V%s\n",
                    get_version());
                return 1;
            case 'h':
                help_display();
                return 1;
            default:
                usage();
                fprintf(stdout, "Use '-h' for more information.\n");
                return 1;
        }
    }

    if (!in || !sd) {
        usage();
        return 1;
    }

	/* DDR容量 */
	if(ddr_size != 256 && ddr_size != 512) {
		fprintf(stderr, "ERROR: DDR size setting ERROR!\n"
                        "Only supports 256MB and 512MB\n");
        return 1;
    }

    tail = strrchr(in, '.');
    if (tail == NULL) {
        out = (char*)malloc(strlen(in) + 5);
        memcpy(out, in, strlen(in));
        memcpy(out+strlen(in), ".imx", 5);
    } else {
        out = (char*)malloc(tail - in + 5);
        memcpy(out, in, tail - in);
        memcpy(out+(tail-in), ".imx", 5);
    }

	/* 打开bin文件 */
	fp = fopen(in, "rb"); /* 以二进制只读方式打开bin文件 */
	if(fp == NULL){
		printf("Can't Open %s\n", in);
		return -1;
	}
	
	/* 获取bin文件长度 */
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	printf("File %s size = %dBytes\n", in, len);
	
	/* 读取bin文件到缓冲区buf中 */
	buf = malloc(len + BIN_OFFSET);
	if(buf == NULL){
		printf("Malloc Failed!\n");
		fclose(fp);
		return -1;
	}
	memset(buf, 0, len + BIN_OFFSET); /* 清零 */
	/* 读取bin源码文件 */
	ret = fread(buf + BIN_OFFSET, 1, len, fp);
    if (ret != len) {
        printf("Read %s Error!\n", in);
        return 1;
    }

	/* 关闭文件 */
	fclose(fp);

#if PRINT_TAB
	printf("IVT DCD Table:\r\n");
	for(i = 0; i < 1024/32; i++){
		for(j = 0; j < 8; j++)
		{
			printf("0X%08X,",*(int *)(buf + BIN_OFFSET + (((i * 8) + j) * 4)));
		}
		printf("\r\n");
	}	
	free(buf);
	return 0;
#endif
	
	/* 添加IVT DCD等表信息到bin文件里面 */
	if(ddr_size == 512) {		/* 512MB */
		printf("DDR SIZE: 512MB\r\n");
		memcpy(buf, imx6_512mb_ivtdcd_table, sizeof(imx6_512mb_ivtdcd_table));
	}
	else if (ddr_size == 256) {	/* 256MB */
		printf("DDR SIZE: 256MB\r\n");
		memcpy(buf, imx6_256mb_ivtdcd_table, sizeof(imx6_256mb_ivtdcd_table));
	}

	/* 现在我们已经在buf中构建好了可以用于下载的bin文件，将buf中的数据保存到
	 * 到一个文件中，文件命名为.imx
	 */
    cmd_Buf = malloc(SHELLCMD_LEN);
    sprintf(cmd_Buf, "rm -rf %s", out);

	printf("Delete Old %s\n", out);
	ret = system(cmd_Buf);		/* 先删除旧的.imx文件	*/
    if (ret) {
        printf("Execution Error: %s\n", cmd_Buf);
    }
	
	fp = fopen(out, "wb");
	if(fp == NULL){
		printf("Cant't Open %s!!!\n", out);
		free(buf);
		return -1;
	}
	nbytes = fwrite(buf, 1, len + BIN_OFFSET, fp);
	if(nbytes != (len + BIN_OFFSET)){
		printf("File Write Error!\r\n");
		free(buf);
		fclose(fp);
		return -1;
	}
	free(buf);
	fclose(fp);
    printf("Create New %s\n", out);
	
	/* 构建烧写的shell命令 */
	sprintf(cmd_Buf,
        "sudo dd iflag=dsync oflag=dsync if=%s of=%s bs=512 seek=2",
        out, dev);
	printf("Download %s to %s\n", out, dev);
	
	/* 执行上面的shell命令 */
	ret = system(cmd_Buf);
    if (ret) {
        printf("Execution Error: %s\n", cmd_Buf);
    }

    free(out);
	free(cmd_Buf);
	return 0;	
}

