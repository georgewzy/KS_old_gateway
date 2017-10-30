/* RT-Thread config file */
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/* RT_NAME_MAX*/
#define RT_NAME_MAX	   8

/* RT_ALIGN_SIZE*/
#define RT_ALIGN_SIZE	4

/* PRIORITY_MAX */
#define RT_THREAD_PRIORITY_MAX	32

/* Tick per Second */
#define RT_TICK_PER_SECOND	100

/* SECTION: RT_DEBUG */
/* Thread Debug */
#define RT_DEBUG
#define RT_USING_OVERFLOW_CHECK

/* Using Hook */
#define RT_USING_HOOK

#define IDLE_THREAD_STACK_SIZE     1024

/* Using Software Timer */
/* #define RT_USING_TIMER_SOFT */
#define RT_TIMER_THREAD_PRIO		4
#define RT_TIMER_THREAD_STACK_SIZE	512

/* SECTION: IPC */
/* Using Semaphore*/
#define RT_USING_SEMAPHORE

/* Using Mutex */
#define RT_USING_MUTEX

/* Using Event */
#define RT_USING_EVENT

/* Using MailBox */
#define RT_USING_MAILBOX

/* Using Message Queue */
#define RT_USING_MESSAGEQUEUE

/* SECTION: Memory Management */
/* Using Memory Pool Management*/
#define RT_USING_MEMPOOL

/* Using Dynamic Heap Management */
#define RT_USING_HEAP

/* Using Small MM */
#define RT_USING_SMALL_MEM

/* SECTION: Device System */
/* Using Device System */
#define RT_USING_DEVICE
#define RT_USING_DEVICE_IPC
/* Using serial framework */
#define RT_USING_SERIAL

// Song: added 2016-5-25 16:10:02
#define RT_USING_COMPONENTS_INIT

#define RT_USING_UART1
#define RT_USING_UART2
#define RT_USING_UART3
#define RT_USING_UART4
#define RT_USING_UART5
//#define RT_USING_UART6

// Song: added.
#define RT_USING_SPI
//#define RT_USING_I2C
//#define RT_USING_SDIO
//#define RT_USING_RTC

/* Using GPIO pin framework */
#define RT_USING_PIN

/* Using Hardware Timer framework */
//#define RT_USING_HWTIMER

/* SECTION: Console options */
#define RT_USING_CONSOLE
/* the buffer size of console*/
#define RT_CONSOLEBUF_SIZE	128
//#define RT_CONSOLE_DEVICE_NAME  "uart1"

/* SECTION: finsh, a C-Express shell */
#define RT_USING_FINSH
/* Using symbol table */
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION



/* SECTION: miscellaneous functions */
#define RT_USING_RYM
#define RT_USING_LOGTRACE

/* Using MODULE */
#define RT_USING_MODULE
#define RT_USING_LIBDL

#define RT_USING_INIT_RC


/* SECTION: device filesystem */
#define RT_USING_DFS 

//#define RT_USING_DFS_ROMFS

#define RT_USING_DFS_ELMFAT
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_DRIVES			1
#define RT_DFS_ELM_USE_LFN			2
#define RT_DFS_ELM_MAX_LFN			255
////#define RT_DFS_ELM_MAX_SECTOR_SIZE  512
#define RT_DFS_ELM_MAX_SECTOR_SIZE  4096

#define RT_DFS_ELM_CODE_PAGE	    437

/* the max number of mounted filesystem */
#define DFS_FILESYSTEMS_MAX			2
/* the max number of opened files 		*/
#define DFS_FD_MAX					6


/* SECTION: lwip, a lighwight TCP/IP protocol stack */
//#define RT_USING_LWIP 
/* LwIP uses RT-Thread Memory Management */
#define RT_LWIP_USING_RT_MEM
/* Enable ICMP protocol*/
#define RT_LWIP_ICMP
/* Enable UDP protocol*/
#define RT_LWIP_UDP
/* Enable TCP protocol*/
#define RT_LWIP_TCP
/* Enable DNS */
#define RT_LWIP_DNS
/* Enable DHCP */
#define RT_LWIP_DHCP

/* the number of simulatenously active TCP connections*/
#define RT_LWIP_TCP_PCB_NUM	5

/* ip address of target*/
#define RT_LWIP_IPADDR0	192
#define RT_LWIP_IPADDR1	168
#define RT_LWIP_IPADDR2	8
#define RT_LWIP_IPADDR3	201

/* gateway address of target*/
#define RT_LWIP_GWADDR0	192
#define RT_LWIP_GWADDR1	168
#define RT_LWIP_GWADDR2	8
#define RT_LWIP_GWADDR3	1

/* mask address of target*/
#define RT_LWIP_MSKADDR0	255
#define RT_LWIP_MSKADDR1	255
#define RT_LWIP_MSKADDR2	255
#define RT_LWIP_MSKADDR3	0

/* tcp thread options */
#define RT_LWIP_TCPTHREAD_PRIORITY		12
#define RT_LWIP_TCPTHREAD_MBOX_SIZE		4
#define RT_LWIP_TCPTHREAD_STACKSIZE		1024

/* ethernet if thread options */
#define RT_LWIP_ETHTHREAD_PRIORITY		15
#define RT_LWIP_ETHTHREAD_MBOX_SIZE		4
#define RT_LWIP_ETHTHREAD_STACKSIZE		512

/* TCP sender buffer space */
#define RT_LWIP_TCP_SND_BUF	8192
/* TCP receive window. */
#define RT_LWIP_TCP_WND		8192

#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0

#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0

/* RT_GDB_STUB */
//#define RT_USING_GDB

#endif
