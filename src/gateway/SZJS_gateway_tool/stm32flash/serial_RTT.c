//#include <fcntl.h>

#include "rtthread.h"
#include "board.h"
#include <rtdevice.h>
//#include "serial.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <termios.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
#include <stdio.h>

#include "ISP_serial.h"
#include "port.h"


struct serial {
	int fd;
    struct rt_device * device;
	char setup_str[11];
};

static serial_t serial_ISP = {0};



static serial_t *serial_open(const char *device)
{
	//serial_t *h = calloc(sizeof(serial_t), 1);

    serial_ISP.device = rt_device_find(device);
    if (serial_ISP.device == RT_NULL)
    {
        rt_kprintf("Not found the ISP device.\n");
    }
    else
    {
        rt_device_open(serial_ISP.device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    
    return &serial_ISP;
    
//	h->fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
//	if (h->fd < 0) {
//		free(h);
//		return NULL;
//	}
//	fcntl(h->fd, F_SETFL, 0);

//	tcgetattr(h->fd, &h->oldtio);
//	tcgetattr(h->fd, &h->newtio);

//	return h;
}

static void serial_flush(const serial_t *h)
{
	//tcflush(h->fd, TCIFLUSH);
}

static void serial_close(serial_t *h)
{
	//serial_flush(h);
	//tcsetattr(h->fd, TCSANOW, &h->oldtio);
	rt_device_close(h->device);
	//free(h);
}

static port_err_t serial_setup(serial_t *h, const serial_baud_t baud,
			       const serial_bits_t bits,
			       const serial_parity_t parity,
			       const serial_stopbit_t stopbit)
{
//	speed_t	port_baud;
//	tcflag_t port_bits;
//	tcflag_t port_parity;
//	tcflag_t port_stop;
//	struct termios settings;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

	switch (baud) {
		case SERIAL_BAUD_2400:    config.baud_rate = BAUD_RATE_2400; break;
		case SERIAL_BAUD_4800:    config.baud_rate = BAUD_RATE_4800; break;
		case SERIAL_BAUD_9600:    config.baud_rate = BAUD_RATE_9600; break;
		case SERIAL_BAUD_19200:   config.baud_rate = BAUD_RATE_19200; break;
		case SERIAL_BAUD_38400:   config.baud_rate = BAUD_RATE_38400; break;
		case SERIAL_BAUD_57600:   config.baud_rate = BAUD_RATE_57600; break;
		case SERIAL_BAUD_115200:  config.baud_rate = BAUD_RATE_115200; break;
		case SERIAL_BAUD_230400:  config.baud_rate = BAUD_RATE_230400; break;
		case SERIAL_BAUD_460800:  config.baud_rate = BAUD_RATE_460800; break;
		case SERIAL_BAUD_921600:  config.baud_rate = BAUD_RATE_921600; break;

		case SERIAL_BAUD_INVALID:
		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (bits) {
		case SERIAL_BITS_5: config.data_bits = DATA_BITS_5; break;
		case SERIAL_BITS_6: config.data_bits = DATA_BITS_6; break;
		case SERIAL_BITS_7: config.data_bits = DATA_BITS_7; break;
		case SERIAL_BITS_8: 
//                            if (parity != SERIAL_PARITY_NONE)
//                            {
//                                config.data_bits = DATA_BITS_9;
//                            }
//                            else
//                            {
                                config.data_bits = DATA_BITS_8;
//                            }    
                            break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (parity) {
		case SERIAL_PARITY_NONE: config.parity = PARITY_NONE; break;
		case SERIAL_PARITY_EVEN: config.parity = PARITY_EVEN; break;
		case SERIAL_PARITY_ODD:  config.parity = PARITY_ODD; break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (stopbit) {
		case SERIAL_STOPBIT_1: config.stop_bits = STOP_BITS_1; break;
		case SERIAL_STOPBIT_2: config.stop_bits = STOP_BITS_2; break;

		default:
			return PORT_ERR_UNKNOWN;
	}
    
    
    
    rt_device_control(h->device, RT_DEVICE_CTRL_CONFIG, &config);
    

//	/* reset the settings */
//#ifndef __sun		/* Used by GNU and BSD. Ignore __SVR4 in test. */
//	cfmakeraw(&h->newtio);
//#else /* __sun */
//	h->newtio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR
//			       | IGNCR | ICRNL | IXON);
//	h->newtio.c_oflag &= ~OPOST;
//	h->newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
//	h->newtio.c_cflag &= ~(CSIZE | PARENB);
//	h->newtio.c_cflag |= CS8;
//#endif /* __sun */
//#ifdef __QNXNTO__
//	h->newtio.c_cflag &= ~(CSIZE | IHFLOW | OHFLOW);
//#else
//	h->newtio.c_cflag &= ~(CSIZE | CRTSCTS);
//#endif
//	h->newtio.c_cflag &= ~(CSIZE | CRTSCTS);
//	h->newtio.c_iflag &= ~(IXON | IXOFF | IXANY | IGNPAR);
//	h->newtio.c_lflag &= ~(ECHOK | ECHOCTL | ECHOKE);
//	h->newtio.c_oflag &= ~(OPOST | ONLCR);

//	/* setup the new settings */
//	cfsetispeed(&h->newtio, port_baud);
//	cfsetospeed(&h->newtio, port_baud);
//	h->newtio.c_cflag |=
//		port_parity	|
//		port_bits	|
//		port_stop	|
//		CLOCAL		|
//		CREAD;
//	if ( port_parity != 0 )
//		h->newtio.c_iflag |= INPCK;

//	h->newtio.c_cc[VMIN] = 0;
//	h->newtio.c_cc[VTIME] = 5;	/* in units of 0.1 s */

//	/* set the settings */
//	serial_flush(h);
//	if (tcsetattr(h->fd, TCSANOW, &h->newtio) != 0)
//		return PORT_ERR_UNKNOWN;

//	/* confirm they were set */
//	tcgetattr(h->fd, &settings);
//	if (settings.c_iflag != h->newtio.c_iflag ||
//	    settings.c_oflag != h->newtio.c_oflag ||
//	    settings.c_cflag != h->newtio.c_cflag ||
//	    settings.c_lflag != h->newtio.c_lflag)
//		return PORT_ERR_UNKNOWN;

	snprintf(h->setup_str, sizeof(h->setup_str), "%u %d%c%d",
		 serial_get_baud_int(baud),
		 serial_get_bits_int(bits),
		 serial_get_parity_str(parity),
		 serial_get_stopbit_int(stopbit));
	return PORT_ERR_OK;
}

static port_err_t serial_RTT_open(struct port_interface *port,
				    struct port_options *ops)
{
	serial_t *h;

	/* 1. check options */
	if (ops->baudRate == SERIAL_BAUD_INVALID)
		return PORT_ERR_UNKNOWN;
	if (serial_get_bits(ops->serial_mode) == SERIAL_BITS_INVALID)
		return PORT_ERR_UNKNOWN;
	if (serial_get_parity(ops->serial_mode) == SERIAL_PARITY_INVALID)
		return PORT_ERR_UNKNOWN;
	if (serial_get_stopbit(ops->serial_mode) == SERIAL_STOPBIT_INVALID)
		return PORT_ERR_UNKNOWN;

	/* 2. open it */
	h = serial_open(ops->device);
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

//	/* 3. check for tty (but only warn) */
//	if (!isatty(h->fd))
//		rt_kprintf( "Warning: Not a tty: %s\n", ops->device);

	/* 4. set options */
	if (serial_setup(h, ops->baudRate,
			 serial_get_bits(ops->serial_mode),
			 serial_get_parity(ops->serial_mode),
			 serial_get_stopbit(ops->serial_mode)
			) != PORT_ERR_OK) {
		serial_close(h);
		return PORT_ERR_UNKNOWN;
	}

	port->private = h;
	return PORT_ERR_OK;
}

static port_err_t serial_RTT_close(struct port_interface *port)
{
	serial_t *h;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	serial_close(h);
	port->private = NULL;
	return PORT_ERR_OK;
}

static port_err_t serial_RTT_read(struct port_interface *port, void *buf,
				     size_t nbyte)
{
	serial_t *h;
	size_t r;
	uint8_t *pos = (uint8_t *)buf;
    uint32_t time = 5;  // waiting 50 ms

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	while (nbyte) {
		r = rt_device_read(h->device, 0, pos, nbyte);
//		if (r == 0)
//			return PORT_ERR_TIMEDOUT;
		if (r < 0)
			return PORT_ERR_UNKNOWN;

		nbyte -= r;
		pos += r;
        
        if (nbyte)
        {
            if (time == 0)
            {
                return PORT_ERR_TIMEDOUT;
            }
            else
            {
                rt_thread_delay(1);
                time --;
                if (r > 0)
                {
                    time = 5;
                }
            }
        }
	}
	return PORT_ERR_OK;
}

static port_err_t serial_RTT_write(struct port_interface *port, void *buf,
				      size_t nbyte)
{
	serial_t *h;
	size_t r;
	const uint8_t *pos = (const uint8_t *)buf;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	while (nbyte) {
		r = rt_device_write(h->device, 0, pos, nbyte);
		if (r < 1)
			return PORT_ERR_UNKNOWN;

		nbyte -= r;
		pos += r;
	}
	return PORT_ERR_OK;
}

static port_err_t serial_RTT_gpio(struct port_interface *port,
				    serial_gpio_t n, int level)
{
	serial_t *h;
	int bit, lines;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	switch (n) {
	case GPIO_RST:
		rt_pin_write(PIN_ISP_RESET, level);
		break;

	case GPIO_BOOT:
		rt_pin_write(PIN_ISP_BOOT, level);
		break;

	default:
		return PORT_ERR_UNKNOWN;
	}

    
    
//	switch (n) {
//	case GPIO_RTS:
//		bit = TIOCM_RTS;
//		break;

//	case GPIO_DTR:
//		bit = TIOCM_DTR;
//		break;

//	case GPIO_BRK:
//		if (level == 0)
//			return PORT_ERR_OK;
//		if (tcsendbreak(h->fd, 1))
//			return PORT_ERR_UNKNOWN;
//		return PORT_ERR_OK;

//	default:
//		return PORT_ERR_UNKNOWN;
//	}

//	/* handle RTS/DTR */
//	if (ioctl(h->fd, TIOCMGET, &lines))
//		return PORT_ERR_UNKNOWN;
//	lines = level ? lines | bit : lines & ~bit;
//	if (ioctl(h->fd, TIOCMSET, &lines))
//		return PORT_ERR_UNKNOWN;

	return PORT_ERR_OK;
}

static const char *serial_RTT_get_cfg_str(struct port_interface *port)
{
	serial_t *h;

	h = (serial_t *)port->private;
	return h ? h->setup_str : "INVALID";
}

struct port_interface port_serial = {
	.name	= "serial_RTT",
	.flags	= PORT_BYTE | PORT_GVR_ETX | PORT_CMD_INIT | PORT_RETRY,
	.open	= serial_RTT_open,
	.close	= serial_RTT_close,
	.read	= serial_RTT_read,
	.write	= serial_RTT_write,
	.gpio	= serial_RTT_gpio,
	.get_cfg_str	= serial_RTT_get_cfg_str,
};







