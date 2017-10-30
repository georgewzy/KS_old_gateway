/*
  stm32flash - Open Source ST STM32 flash program for *nix
  Copyright 2010 Geoffrey McRae <geoff@spacevs.com>
  Copyright 2011 Steve Markgraf <steve@steve-m.de>
  Copyright 2012-2016 Tormod Volden <debian.tormod@gmail.com>
  Copyright 2013-2016 Antonio Borneo <borneo.antonio@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*

Song: noted. 2016-6-13 13:53:58

READ ME:
    This is a transplant for RTT. For now, just support .bin format files. you can active the 'ACT_WRITE' 
    'ACT_READ_PROTECT' and 'ACT_READ_UNPROTECT' action whit the function 
    ISP_stm32(struct port_options *port_opt, t_ISP_opt *opt). You can write
    to stm32 flash by function ISP_flash(filename, verify),enable the read protect
    by function ISP_readprot(void), disable the read protect by function
    ISP_unreadprot(void). 
    
    Demo using:
    
    ISP_unreadprot();
    ISP_flash("/boot/rtthread.bin", 1);
    ISP_readprot();

*/

//#include <sys/types.h>
//#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
//#include "getopt.h"

#include "init.h"
#include "utils.h"
#include "ISP_serial.h"
#include "stm32.h"
#include "parsers/parser.h"
#include "port.h"

#include "parsers/binary.h"
#include "parsers/hex.h"

#include "dfs_def.h"
#include <dfs_posix.h>


#define STM32_IAP_VERSION "0.5"


// Song: added at 2016-6-8 16:15:29.
extern char *optarg;
extern int optind, opterr, optopt;
int getopt(int argc, char * const argv[], const  char  *optstring);

/* device globals */
stm32_t		*stm		= NULL;

void		*p_st		= NULL;
parser_t	*parser		= NULL;

/* settings */
struct port_options port_opts = {
	.device = "uart1",
    .baudRate = SERIAL_BAUD_115200,
    .serial_mode = "8e1",
    .bus_addr = 0,
    .rx_frame_max = STM32_MAX_RX_FRAME,
    .tx_frame_max = STM32_MAX_TX_FRAME,
};

enum actions {
	ACT_NONE,
	ACT_READ,
	ACT_WRITE,
	ACT_WRITE_UNPROTECT,
	ACT_READ_PROTECT,
	ACT_READ_UNPROTECT,
	ACT_ERASE_ONLY,
	ACT_CRC
};

//enum actions	action		= ACT_NONE;
//int		npages		= 0;
//int             spage           = 0;
int             no_erase        = 0;
//char		verify		= 0;
//int		retry		= 10;
//char		exec_flag	= 0;
//uint32_t	execute		= 0;
//char		init_flag	= 1;
//char		force_binary	= 0;
//char		reset_flag	= 0;
//char		*filename;
//char		*gpio_seq	= NULL;
//uint32_t	start_addr	= 0;
//uint32_t	readwrite_len	= 0;

typedef struct
{
//struct port_options     port_opts;
enum actions    action;
int		        npages;
int             spage;
//int             no_erase;
char		    verify;
int		        retry;
char		    exec_flag;
uint32_t	    execute;
char		    init_flag;
char		    force_binary;
char		    reset_flag;
char		    filename[64];
char		    gpio_seq[48];
uint32_t	    start_addr;
uint32_t	    readwrite_len;
} t_ISP_opt;


t_ISP_opt ISP_opt = {
.action		= ACT_WRITE,
.npages		= 0,
.spage           = 0,
//.no_erase        = 0,
.verify		= 0,
.retry		= 10,
.exec_flag	= 0,
.execute		= 0,
.init_flag	= 1,
.force_binary	= 1,
.reset_flag	= 1,
.filename = {0},
.gpio_seq	= "boot,-reset,reset:-boot,-reset,reset",
.start_addr	= 0,
.readwrite_len	= 0
};

/* functions */
int  parse_options(int argc, char *argv[]);
void show_help(char *name);

static const char *action2str(enum actions act)
{
	switch (act) {
		case ACT_READ:
			return "memory read";
		case ACT_WRITE:
			return "memory write";
		case ACT_WRITE_UNPROTECT:
			return "write unprotect";
		case ACT_READ_PROTECT:
			return "read protect";
		case ACT_READ_UNPROTECT:
			return "read unprotect";
		case ACT_ERASE_ONLY:
			return "flash erase";
		case ACT_CRC:
			return "memory crc";
		default:
			return "";
	};
}

static void err_multi_action(enum actions new)
{
	rt_kprintf(
		"ERROR: Invalid options !\n"
		"\tCan't execute \"%s\" and \"%s\" at the same time.\n",
		action2str(ISP_opt.action), action2str(new));
}

static int is_addr_in_ram(uint32_t addr)
{
	return addr >= stm->dev->ram_start && addr < stm->dev->ram_end;
}

static int is_addr_in_flash(uint32_t addr)
{
	return addr >= stm->dev->fl_start && addr < stm->dev->fl_end;
}

/* returns the page that contains address "addr" */
static int flash_addr_to_page_floor(uint32_t addr)
{
	int page;
	uint32_t *psize;

	if (!is_addr_in_flash(addr))
		return 0;

	page = 0;
	addr -= stm->dev->fl_start;
	psize = stm->dev->fl_ps;

	while (addr >= psize[0]) {
		addr -= psize[0];
		page++;
		if (psize[1])
			psize++;
	}

	return page;
}

/* returns the first page whose start addr is >= "addr" */
int flash_addr_to_page_ceil(uint32_t addr)
{
	int page;
	uint32_t *psize;

	if (!(addr >= stm->dev->fl_start && addr <= stm->dev->fl_end))
		return 0;

	page = 0;
	addr -= stm->dev->fl_start;
	psize = stm->dev->fl_ps;

	while (addr >= psize[0]) {
		addr -= psize[0];
		page++;
		if (psize[1])
			psize++;
	}

	return addr ? page + 1 : page;
}

/* returns the lower address of flash page "page" */
static uint32_t flash_page_to_addr(int page)
{
	int i;
	uint32_t addr, *psize;

	addr = stm->dev->fl_start;
	psize = stm->dev->fl_ps;

	for (i = 0; i < page; i++) {
		addr += psize[0];
		if (psize[1])
			psize++;
	}

	return addr;
}




int ISP_stm32(struct port_options *port_opt, t_ISP_opt *opt)
{

	struct port_interface *port = NULL;
	int ret = 1;
	stm32_err_t s_err;
	parser_err_t perr;
	FILE *diag = stdout;
	uint8_t		buffer[256];
	uint32_t	addr, start, end;
	unsigned int	len;
	int		failed = 0;
	int		first_page, num_pages;

    off_t 	offset = 0;
    size_t r;
    unsigned int size;
    unsigned int max_wlen, max_rlen;
    
    //opt->action = ACT_WRITE;
    
    
    
	rt_kprintf( "stm32flash " STM32_IAP_VERSION "\n\n");
//	rt_kprintf( "http://stm32flash.sourceforge.net/\n\n");
//	if (parse_options(argc, argv) != 0)
//		goto close;

//	if ((action == ACT_READ) && filename[0] == '-') {
//		diag = stderr;
//	}

	if (opt->action == ACT_WRITE) {
		/* first try hex */
		if (!opt->force_binary) {
			parser = &PARSER_HEX;
			p_st = parser->init();
			if (!p_st) {
				rt_kprintf( "%s Parser failed to initialize\n", parser->name);
				goto close;
			}
		}

		if (opt->force_binary || (perr = parser->open(p_st, opt->filename, 0)) != PARSER_ERR_OK) {
			if (opt->force_binary || perr == PARSER_ERR_INVALID_FILE) {
				if (!opt->force_binary) {
					parser->close(p_st);
					p_st = NULL;
				}

				/* now try binary */
				parser = &PARSER_BINARY;
				p_st = parser->init();
				if (!p_st) {
					rt_kprintf( "%s Parser failed to initialize\n", parser->name);
					goto close;
				}
				perr = parser->open(p_st, opt->filename, 0);
			}

			/* if still have an error, fail */
			if (perr != PARSER_ERR_OK) {
				rt_kprintf( "%s ERROR: %s\n", parser->name, parser_errstr(perr));
				if (perr == PARSER_ERR_SYSTEM) perror(opt->filename);
				goto close;
			}
		}

		rt_kprintf( "Using Parser : %s\n", parser->name);
	} else {
		parser = &PARSER_BINARY;
		p_st = parser->init();
		if (!p_st) {
			rt_kprintf( "%s Parser failed to initialize\n", parser->name);
			goto close;
		}
	}

	if (port_open(port_opt, &port) != PORT_ERR_OK) {
		rt_kprintf( "Failed to open port: %s\n", port_opt->device);
		goto close;
	}

	rt_kprintf( "Interface %s: %s\n", port->name, port->get_cfg_str(port));
	if (opt->init_flag && init_bl_entry(port, opt->gpio_seq) == 0)
		goto close;
	stm = stm32_init(port, opt->init_flag);
	if (!stm)
		goto close;

	rt_kprintf( "Version      : 0x%02x\n", stm->bl_version);
	if (port->flags & PORT_GVR_ETX) {
		rt_kprintf( "Option 1     : 0x%02x\n", stm->option1);
		rt_kprintf( "Option 2     : 0x%02x\n", stm->option2);
	}
	rt_kprintf( "Device ID    : 0x%04x (%s)\n", stm->pid, stm->dev->name);
	rt_kprintf( "- RAM        : %dKiB  (%db reserved by bootloader)\n", (stm->dev->ram_end - 0x20000000) / 1024, stm->dev->ram_start - 0x20000000);
	rt_kprintf( "- Flash      : %dKiB (size first sector: %dx%d)\n", (stm->dev->fl_end - stm->dev->fl_start ) / 1024, stm->dev->fl_pps, stm->dev->fl_ps[0]);
	rt_kprintf( "- Option RAM : %db\n", stm->dev->opt_end - stm->dev->opt_start + 1);
	rt_kprintf( "- System RAM : %dKiB\n", (stm->dev->mem_end - stm->dev->mem_start) / 1024);


	/*
	 * Cleanup addresses:
	 *
	 * Starting from options
	 *	start_addr, readwrite_len, spage, npages
	 * and using device memory size, compute
	 *	start, end, first_page, num_pages
	 */
	if (opt->start_addr || opt->readwrite_len) {
		start = opt->start_addr;

		if (is_addr_in_flash(start))
			end = stm->dev->fl_end;
		else {
			no_erase = 1;
			if (is_addr_in_ram(start))
				end = stm->dev->ram_end;
			else
				end = start + sizeof(uint32_t);
		}

		if (opt->readwrite_len && (end > start + opt->readwrite_len))
			end = start + opt->readwrite_len;

		first_page = flash_addr_to_page_floor(start);
		if (!first_page && end == stm->dev->fl_end)
			num_pages = STM32_MASS_ERASE;
		else
			num_pages = flash_addr_to_page_ceil(end) - first_page;
	} else if (!opt->spage && !opt->npages) {
		start = stm->dev->fl_start;
		end = stm->dev->fl_end;
		first_page = 0;
		num_pages = STM32_MASS_ERASE;
	} else {
		first_page = opt->spage;
		start = flash_page_to_addr(first_page);
		if (start > stm->dev->fl_end) {
			rt_kprintf( "Address range exceeds flash size.\n");
			goto close;
		}

		if (opt->npages) {
			num_pages = opt->npages;
			end = flash_page_to_addr(first_page + num_pages);
			if (end > stm->dev->fl_end)
				end = stm->dev->fl_end;
		} else {
			end = stm->dev->fl_end;
			num_pages = flash_addr_to_page_ceil(end) - first_page;
		}

		if (!first_page && end == stm->dev->fl_end)
			num_pages = STM32_MASS_ERASE;
	}

	if (opt->action == ACT_READ) {
		unsigned int max_len = port_opt->rx_frame_max;

		rt_kprintf( "Memory read\n");

		perr = parser->open(p_st, opt->filename, 1);
		if (perr != PARSER_ERR_OK) {
			rt_kprintf( "%s ERROR: %s\n", parser->name, parser_errstr(perr));
			if (perr == PARSER_ERR_SYSTEM)
				perror(opt->filename);
			goto close;
		}

		//fflush(diag);
		addr = start;
		while(addr < end) {
			uint32_t left	= end - addr;
			len		= max_len > left ? left : max_len;
			s_err = stm32_read_memory(stm, addr, buffer, len);
			if (s_err != STM32_ERR_OK) {
				rt_kprintf( "Failed to read memory at address 0x%08x, target write-protected?\n", addr);
				goto close;
			}
			if (parser->write(p_st, buffer, len) != PARSER_ERR_OK)
			{
				rt_kprintf( "Failed to write data to file\n");
				goto close;
			}
			addr += len;

			rt_kprintf(
				"\rRead address 0x%08x (%.2f%%) ",
				addr,
				(100.0f / (float)(end - start)) * (float)(addr - start)
			);
			//fflush(diag);
		}
		rt_kprintf(	"Done.\n");
		ret = 0;
		goto close;
	} else if (opt->action == ACT_READ_PROTECT) {
		rt_kprintf( "Read-Protecting flash\n");
		/* the device automatically performs a reset after the sending the ACK */
		opt->reset_flag = 0;
		stm32_readprot_memory(stm);
		rt_kprintf(	"Done.\n");
	} else if (opt->action == ACT_READ_UNPROTECT) {
		rt_kprintf( "Read-UnProtecting flash\n");
		/* the device automatically performs a reset after the sending the ACK */
		opt->reset_flag = 0;
		stm32_runprot_memory(stm);
		rt_kprintf(	"Done.\n");
	} else if (opt->action == ACT_ERASE_ONLY) {
		ret = 0;
		rt_kprintf( "Erasing flash\n");

		if (num_pages != STM32_MASS_ERASE &&
		    (start != flash_page_to_addr(first_page)
		     || end != flash_page_to_addr(first_page + num_pages))) {
			rt_kprintf( "Specified start & length are invalid (must be page aligned)\n");
			ret = 1;
			goto close;
		}

		s_err = stm32_erase_memory(stm, first_page, num_pages);
		if (s_err != STM32_ERR_OK) {
			rt_kprintf( "Failed to erase memory\n");
			ret = 1;
			goto close;
		}
	} else if (opt->action == ACT_WRITE_UNPROTECT) {
		rt_kprintf( "Write-unprotecting flash\n");
		/* the device automatically performs a reset after the sending the ACK */
		opt->reset_flag = 0;
		stm32_wunprot_memory(stm);
		rt_kprintf(	"Done.\n");

	} else if (opt->action == ACT_WRITE) {
		rt_kprintf( "Write to memory\n");


		max_wlen = port_opt->tx_frame_max - 2;	/* skip len and crc */
		max_wlen &= ~3;	/* 32 bit aligned */

		max_rlen = port_opt->rx_frame_max;
		max_rlen = max_rlen < max_wlen ? max_rlen : max_wlen;

		/* Assume data from stdin is whole device */
		if (opt->filename[0] == '-' && opt->filename[1] == '\0')
			size = end - start;
		else
			size = parser->size(p_st);

		// TODO: It is possible to write to non-page boundaries, by reading out flash
		//       from partial pages and combining with the input data
		// if ((start % stm->dev->fl_ps[i]) != 0 || (end % stm->dev->fl_ps[i]) != 0) {
		//	rt_kprintf( "Specified start & length are invalid (must be page aligned)\n");
		//	goto close;
		// } 

		// TODO: If writes are not page aligned, we should probably read out existing flash
		//       contents first, so it can be preserved and combined with new data
		if (!no_erase && num_pages) {
			rt_kprintf( "Erasing memory\n");
			s_err = stm32_erase_memory(stm, first_page, num_pages);
			if (s_err != STM32_ERR_OK) {
				rt_kprintf( "Failed to erase memory\n");
				goto close;
			}
		}

		//fflush(diag);
		addr = start;
		while(addr < end && offset < size) {
			uint32_t left	= end - addr;
			len		= max_wlen > left ? left : max_wlen;
			len		= len > size - offset ? size - offset : len;

			if (parser->read(p_st, buffer, &len) != PARSER_ERR_OK)
				goto close;

			if (len == 0) {
				if (opt->filename[0] == '-') {
					break;
				} else {
					rt_kprintf( "Failed to read input file\n");
					goto close;
				}
			}
	
			again:
			s_err = stm32_write_memory(stm, addr, buffer, len);
			if (s_err != STM32_ERR_OK) {
				rt_kprintf( "Failed to write memory at address 0x%08x\n", addr);
				goto close;
			}

			if (opt->verify) {
				uint8_t compare[len];
				unsigned int offset, rlen;

				offset = 0;
				while (offset < len) {
					rlen = len - offset;
					rlen = rlen < max_rlen ? rlen : max_rlen;
					s_err = stm32_read_memory(stm, addr + offset, compare + offset, rlen);
					if (s_err != STM32_ERR_OK) {
						rt_kprintf( "Failed to read memory at address 0x%08x\n", addr + offset);
						goto close;
					}
					offset += rlen;
				}

				for(r = 0; r < len; ++r)
					if (buffer[r] != compare[r]) {
						if (failed == opt->retry) {
							rt_kprintf( "Failed to verify at address 0x%08x, expected 0x%02x and found 0x%02x\n",
								(uint32_t)(addr + r),
								buffer [r],
								compare[r]
							);
							goto close;
						}
						++failed;
						goto again;
					}

				failed = 0;
			}

			addr	+= len;
			offset	+= len;

			rt_kprintf(
				"\rWrote %saddress 0x%08x (%.2f%%) ",
				opt->verify ? "and verified " : "",
				addr,
				(100.0f / size) * offset
			);
			//fflush(diag);

		}

		rt_kprintf(	"Done.\n");
		ret = 0;
		goto close;
	} else if (opt->action == ACT_CRC) {
		uint32_t crc_val = 0;

		rt_kprintf( "CRC computation\n");

		s_err = stm32_crc_wrapper(stm, start, end - start, &crc_val);
		if (s_err != STM32_ERR_OK) {
			rt_kprintf( "Failed to read CRC\n");
			goto close;
		}
		rt_kprintf( "CRC(0x%08x-0x%08x) = 0x%08x\n", start, end,
			crc_val);
		ret = 0;
		goto close;
	} else
		ret = 0;

close:
	if (stm && opt->exec_flag && ret == 0) {
		if (opt->execute == 0)
			opt->execute = stm->dev->fl_start;

		rt_kprintf( "\nStarting execution at address 0x%08x... ", opt->execute);
		//fflush(diag);
		if (stm32_go(stm, opt->execute) == STM32_ERR_OK) {
			opt->reset_flag = 0;
			rt_kprintf( "done.\n");
		} else
			rt_kprintf( "failed.\n");
	}

	if (stm && opt->reset_flag) {
		rt_kprintf( "\nResetting device... ");
		//fflush(diag);
		if (init_bl_exit(stm, port, opt->gpio_seq))
			rt_kprintf( "done.\n");
		else	rt_kprintf( "failed.\n");
	}

	if (p_st  ) parser->close(p_st);
	if (stm   ) stm32_close  (stm);
	if (port)
		port->close(port);

	rt_kprintf( "\n");
	return ret;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
int ISP_flash(char *filename, uint8_t verify, uint32_t  address, uint8_t if_reset)
{
    int ret = 0;
    uint32_t addr = address;
    uint8_t verify_flag = verify;
    
    rt_kprintf(" verify : %d\n address : 0x%08X \n", verify_flag, addr);
    
    strcpy(ISP_opt.filename, filename);
    ISP_opt.action = ACT_WRITE;
    ISP_opt.verify = verify_flag;
    ISP_opt.reset_flag = if_reset;
    ISP_opt.start_addr = addr;
    
    ret = ISP_stm32(&port_opts, &ISP_opt);

    ISP_opt.verify = 0;
    ISP_opt.start_addr = 0;
    
    return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(ISP_flash, ISP_flash, ISP flash for stm32 --filename  --verify  --start_address --if_reset.);

int ISP_readprot(void)
{
    ISP_opt.action = ACT_READ_PROTECT;
    //ISP_opt.reset_flag = 1;
    return ISP_stm32(&port_opts, &ISP_opt);
}
FINSH_FUNCTION_EXPORT_ALIAS(ISP_readprot, ISP_readprot, ISP enable the read protect.);

int ISP_unreadprot(void)
{
    ISP_opt.action = ACT_READ_UNPROTECT;
    //ISP_opt.reset_flag = 1;
    return ISP_stm32(&port_opts, &ISP_opt);
}
FINSH_FUNCTION_EXPORT_ALIAS(ISP_unreadprot, ISP_unreadprot, ISP disable the read protect.);
#endif 



void show_help(char *name) {
	rt_kprintf(
		"Usage: %s [-bvngfhc] [-[rw] filename] [tty_device | i2c_device]\n"
		"	-a bus_address	Bus address (e.g. for I2C port)\n"
		"	-b rate		Baud rate (default 57600)\n"
		"	-m mode		Serial port mode (default 8e1)\n"
		"	-r filename	Read flash to file (or - stdout)\n"
		"	-w filename	Write flash from file (or - stdout)\n"
		"	-C		Compute CRC of flash content\n"
		"	-u		Disable the flash write-protection\n"
		"	-j		Enable the flash read-protection\n"
		"	-k		Disable the flash read-protection\n"
		"	-o		Erase only\n"
		"	-e n		Only erase n pages before writing the flash\n"
		"	-v		Verify writes\n"
		"	-n count	Retry failed writes up to count times (default 10)\n"
		"	-g address	Start execution at specified address (0 = flash start)\n"
		"	-S address[:length]	Specify start address and optionally length for\n"
		"	                   	read/write/erase operations\n"
		"	-F RX_length[:TX_length]  Specify the max length of RX and TX frame\n"
		"	-s start_page	Flash at specified page (0 = flash start)\n"
		"	-f		Force binary parser\n"
		"	-h		Show this help\n"
		"	-c		Resume the connection (don't send initial INIT)\n"
		"			*Baud rate must be kept the same as the first init*\n"
		"			This is useful if the reset fails\n"
		"	-i GPIO_string	GPIO sequence to enter/exit bootloader mode\n"
		"			GPIO_string=[entry_seq][:[exit_seq]]\n"
		"			sequence=[-]n[,sequence]\n"
		"	-R		Reset device at exit.\n"
		"\n"
		"Examples:\n"
		"	Get device information:\n"
		"		%s /dev/ttyS0\n"
		"	  or:\n"
		"		%s /dev/i2c-0\n"
		"\n"
		"	Write with verify and then start execution:\n"
		"		%s -w filename -v -g 0x0 /dev/ttyS0\n"
		"\n"
		"	Read flash to file:\n"
		"		%s -r filename /dev/ttyS0\n"
		"\n"
		"	Read 100 bytes of flash from 0x1000 to stdout:\n"
		"		%s -r - -S 0x1000:100 /dev/ttyS0\n"
		"\n"
		"	Start execution:\n"
		"		%s -g 0x0 /dev/ttyS0\n"
		"\n"
		"	GPIO sequence:\n"
		"	- entry sequence: GPIO_3=low, GPIO_2=low, GPIO_2=high\n"
		"	- exit sequence: GPIO_3=high, GPIO_2=low, GPIO_2=high\n"
		"		%s -R -i -3,-2,2:3,-2,2 /dev/ttyS0\n",
		name,
		name,
		name,
		name,
		name,
		name,
		name,
		name
	);
}


