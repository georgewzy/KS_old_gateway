#include <rtthread.h>
#include <ymodem.h>
#include <dfs_posix.h>
#include <stdlib.h>

#include <board.h>

struct custom_ctx
{
    struct rym_ctx parent;
    int fd;
    int flen;
    char fpath[256];
};

static enum rym_code _rym_bg(
        struct rym_ctx *ctx,
        rt_uint8_t *buf,
        rt_size_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;
    cctx->fpath[0] = '/';
    /* the buf should be the file name */
    strcpy(&(cctx->fpath[1]), (const char*)buf);
    cctx->fd = open(cctx->fpath, O_CREAT | O_WRONLY | O_TRUNC, 0);
    if (cctx->fd < 0)
    {
        rt_err_t err = rt_get_errno();
        rt_kprintf("error creating file: %d\n", err);
        rt_kprintf("abort transmission\n");
        return RYM_CODE_CAN;
    }

    cctx->flen = atoi((const char*)buf+strlen((const char*)buf)+1);
    if (cctx->flen == 0)
        cctx->flen = -1;
    return RYM_CODE_ACK;
}

static enum rym_code _rym_tof(
        struct rym_ctx *ctx,
        rt_uint8_t *buf,
        rt_size_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;
    RT_ASSERT(cctx->fd >= 0);
    if (cctx->flen == -1)
    {
        write(cctx->fd, buf, len);
    }
    else
    {
        int wlen = len > cctx->flen ? cctx->flen : len;
        write(cctx->fd, buf, wlen);
        cctx->flen -= wlen;
    }
    return RYM_CODE_ACK;
}

static enum rym_code _rym_end(
        struct rym_ctx *ctx,
        rt_uint8_t *buf,
        rt_size_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;

    RT_ASSERT(cctx->fd >= 0);
    close(cctx->fd);
    cctx->fd = -1;

    return RYM_CODE_ACK;
}

rt_err_t rym_write_to_file(rt_device_t idev, char *dir)
{
    rt_err_t res;
    int fd;
    char dir_buf[128] = {0};
    struct custom_ctx *ctx = rt_malloc(sizeof(*ctx));

    RT_ASSERT(idev);

    rt_kprintf("entering RYM mode\n");

    res = rym_recv_on_device(&ctx->parent, idev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
            _rym_bg, _rym_tof, _rym_end, 1000);

    /* there is no Ymodem traffic on the line so print out info. */
    rt_kprintf("leaving RYM mode with code %d\n", res);
    rt_kprintf("file %s has been created.\n", ctx->fpath);

    if (res == 0)
    {
        if (dir != NULL)
        {
            if ((strlen(dir) == 1) && (dir[0] == '/'))
            {
                
            }
            else
            {
                strcpy(&dir_buf[0],dir);
                strcpy(&dir_buf[strlen(dir)],ctx->fpath);
                dir_buf[strlen(dir) + strlen(ctx->fpath)] = 0;
                
                fd = open(dir_buf, O_RDONLY, 0);
                if (fd >= 0)
                {
                    close(fd);
                    rm(dir_buf);
                }
                else
                {
                    close(fd);
                }
                    
                copy(ctx->fpath, dir_buf);
                rm(ctx->fpath);
            }
        }
    }
    
    rt_free(ctx);

    return res;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
rt_err_t ry(char *dname)
{
    rt_err_t res;

    rt_device_t dev = rt_device_find(dname);
    if (!dev)
    {
        rt_kprintf("could not find device:%s\n", dname);
        return -RT_ERROR;
    }

    res = rym_write_to_file(dev, NULL);

    return res;
}
FINSH_FUNCTION_EXPORT(ry, receive files by ymodem protocol);
#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
rt_err_t rf(char *dir)
{
    rt_err_t res;

    rt_device_t dev = rt_console_get_device();
    if (!dev)
    {
        rt_kprintf("could not find console.\n");
        return -RT_ERROR;
    }

    res = rym_write_to_file(dev, dir);

    return res;
}
FINSH_FUNCTION_EXPORT(rf, receive file to special directory);
#endif

