#include <gpsconfig.h>
#ifdef HAVE_ERRNO_H
    #include <errno.h>
#endif
#ifdef HAVE_STDIO_H
    #include <stdio.h>
#endif
#ifdef HAVE_FCNTL_H
    #include <fcntl.h>
#endif
#ifdef HAVE_EV_H
    #include <ev.h>
#endif
#include <gpsdata.h>

typedef struct {
    gpsdata_parser_t *parser;
    gpsdata_data_t *datalistp;
} mygps_t;

void device_io_cb(EV_P_ ev_io *w, int revents)
{
    if (w && revents & EV_READ) {
        if (w->fd >= 0) {
            char buf[256];
            ssize_t nb = read(w->fd, buf, sizeof(buf));
            if (nb < 0) {//error
                int err = errno;
                GPSUTILS_ERROR("Error reading device fd: %d. Error: %s(%d)\n",
                        w->fd, strerror(err), err);
                ev_io_stop(EV_A_ w);
                close(w->fd);
            } else if (nb == 0) {
                GPSUTILS_ERROR("EOF reached on device. No bytes read. shutting down I/O\n");
                ev_io_stop(EV_A_ w);
                close(w->fd);
            } else { // valid read
                mygps_t *mydata = (mygps_t *)(w->data);
                if (!mydata || !mydata->parser) {
                    GPSUTILS_ERROR("Invalid parser pointer. closing I/O\n");
                    ev_io_stop(EV_A_ w);
                    close(w->fd);
                } else {
                    size_t onum = 0;
                    int rc = gpsdata_parser_parse(mydata->parser, buf,
                                    sizeof(buf), &(mydata->datalistp), &onum);
                    if (rc < 0) {
                        GPSUTILS_WARN("Failed to parse data:\n");
                        gpsutils_hex_dump(buf, sizeof(buf), GPSUTILS_LOG_PTR);
                    } else {
                        GPSUTILS_INFO("Parsed %zu packets\n", onum);
                        gpsdata_list_dump(mydata->datalistp, GPSUTILS_LOG_PTR);
                        /* do more things here like add the data to a database */
                        // free it to save memory
                        gpsdata_list_free(&(mydata->datalistp));
                        mydata->datalistp = NULL;
                    }
                }
            }
        }
    } else {
        GPSUTILS_ERROR("Invalid callback invocation, exiting loop\n");
        ev_break(EV_A_ EVBREAK_ALL);// stop all loops
    }
}

void timeout_cb(EV_P_ ev_timer *w, int revents)
{
    GPSUTILS_INFO("Timeout called\n");
    ev_break(EV_A_ EVBREAK_ALL);// stop all loops
}

int main(int argc, char **argv)
{
    mygps_t mydata = { NULL, NULL };
    mydata.parser = gpsdata_parser_create();
    if (!mydata.parser) {
        GPSUTILS_ERROR("Failed to create gps parser\n");
        return -1;
    }
    ev_io device_watcher = { 0 };
    ev_timer timeout_watcher = { 0 };
    // use the default event loop
    struct ev_loop *loop = EV_DEFAULT;
    const char *dev = "/dev/ttyUSB0";
    int rc = 0;
    int dev_fd = open(dev, O_NONBLOCK | O_RDONLY | O_NOCTTY | O_CLOEXEC);
    if (dev_fd < 0) {
        int err = errno;
        GPSUTILS_ERROR("Failed to open %s: %s(%d)\n", dev, strerror(err), err);
        rc = -1;
    } else {
        ev_io_init(&device_watcher, device_io_cb, dev_fd, EV_READ);
        //set the data pointer so we can access the parser in the callback
        device_watcher.data = (void *)&mydata;
        ev_io_start(loop, &device_watcher);

        ev_timer_init(&timeout_watcher, timeout_cb, 10 /* seconds */, 0.);
        ev_timer_start(loop, &timeout_watcher);
        ev_run(loop, 0);
        rc = 0;
    }
    // free memory
    gpsdata_parser_free(mydata.parser);
    gpsdata_list_free(&(mydata.datalistp));
    return rc;
}
