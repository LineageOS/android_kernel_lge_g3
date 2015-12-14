#ifndef __ATMF04_EFLASH_H__
#define __ATMF04_EFLASH_H__

#define SZ_PAGE_DATA                64
#ifdef CONFIG_LGE_T1_CAP
#define FW_DATA_PAGE               	115
#else
#define FW_DATA_PAGE               	96
#endif

#define ADDR_EFLA_STS               0xFF	//eflash status register
#define ADDR_EFLA_PAGE_L            0xFD	//eflash page
#define ADDR_EFLA_PAGE_H            0xFE	//eflash page
#define ADDR_EFLA_CTRL              0xFC	//eflash control register

#define CMD_EFL_L_WR                0x01	//Eflash Write
#define CMD_EFL_RD                  0x03	//Eflash Read
#define CMD_EFL_ERASE_ALL           0x07	//Eflash All Page Erase

#define CMD_EUM_WR                  0x21	//Extra user memory write
#define CMD_EUM_RD                  0x23	//Extra user memory read
#define CMD_EUM_ERASE               0x25	//Extra user memory erase

#define FLAG_DONE                   0x03
#define FLAG_DONE_ERASE             0x02

#define FLAG_FUSE                   1
#define FLAG_FW                     2

#define FL_EFLA_TIMEOUT_CNT         200

#define RTN_FAIL                    0
#define RTN_SUCC                    1
#define RTN_TIMEOUT                 2

#define ON                          1
#define OFF                         2

#endif

