#include <gpsconfig.h>
#include <gpsdata.h>
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

typedef struct {
    gpsdata_parser_t *parser;
    gpsdata_data_t *datalistp;
    int log_fd;
    char log_file[PATH_MAX];
} mygps_t;

void device_io_cb(EV_P_ ev_io *w, int revents)
{
    if (w && revents & EV_READ) {
        if (w->fd >= 0) {
            char buf[80];
            memset(buf, 0, sizeof(buf));
            ssize_t nb = read(w->fd, buf, sizeof(buf));
            if (nb < 0) {//error
                int err = errno;
                GPSUTILS_ERROR("Error reading device fd: %d. Error: %s(%d)\n",
                        w->fd, strerror(err), err);
                ev_io_stop(EV_A_ w);
                gpsdevice_close(w->fd);
            } else if (nb == 0) {
                GPSUTILS_WARN("no data received from device. waiting...\n");
            } else { // valid read
                mygps_t *mydata = (mygps_t *)(w->data);
                if (!mydata || !mydata->parser) {
                    GPSUTILS_ERROR("Invalid parser pointer. closing I/O\n");
                    ev_io_stop(EV_A_ w);
                    gpsdevice_close(w->fd);
                } else {
                    if (mydata->log_fd > 0) {
                        write(mydata->log_fd, buf, nb);
                    }
                    gpsutils_timer_t timer;
                    size_t onum = 0;
                    gpsutils_timer_start(&timer);
                    int rc = gpsdata_parser_parse(mydata->parser, buf, nb,
                                                &(mydata->datalistp), &onum);
                    gpsutils_timer_stop(&timer);
                    GPSUTILS_DEBUG("Time Taken to parse %zd bytes: %0.08lf\n",
                                    nb, timer.time_taken);
                    if (rc < 0) {
                        GPSUTILS_WARN("Failed to parse data %zd bytes:\n", nb);
                        gpsutils_hex_dump(buf, (size_t)nb, GPSUTILS_LOG_PTR);
                        gpsdata_parser_reset(mydata->parser);
                    } else {
                        GPSUTILS_INFO("Parsed %zu packets\n", onum);
                        gpsdata_list_dump(mydata->datalistp, GPSUTILS_LOG_PTR);
                        // do more things here like add the data to a database
                        // free list here to save memory growth
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
#ifndef NDEBUG
    GPSUTILS_LOGLEVEL_SET(DEBUG);
#else
    GPSUTILS_LOGLEVEL_SET(INFO);
#endif
    mygps_t mydata;
    memset(&mydata, 0, sizeof(mydata));
    snprintf(mydata.log_file, sizeof(mydata.log_file) - 1, "/tmp/gps_%d.log", time(NULL));
    // we log the data to a file for debugging
    mydata.log_fd = open(mydata.log_file, O_RDWR | O_CLOEXEC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (mydata.log_fd < 0) {
        int err = errno;
        GPSUTILS_ERROR("Failed to open %s: %s(%d)\n", mydata.log_file, strerror(err), err);
        return -1;
    }
    GPSUTILS_INFO("Logging all received data to %s\n", mydata.log_file);
    mydata.parser = gpsdata_parser_create();
    if (!mydata.parser) {
        GPSUTILS_ERROR("Failed to create gps parser\n");
        close(mydata.log_fd);
        return -1;
    }

    ev_io device_watcher = { 0 };
    ev_timer timeout_watcher = { 0 };
    // use the default event loop
    struct ev_loop *loop = EV_DEFAULT;
    const char *dev = "/dev/ttyUSB0";
    if (argc > 1) {
        dev = argv[1];
    }
    GPSUTILS_INFO("Using %s as device\n", dev);
    int rc = 0;
    int dev_fd = gpsdevice_open(dev, true);
    if (dev_fd < 0) {
        rc = -1;
    } else {
        gpsdevice_request_antenna_status(dev_fd, true, false);
        gpsdevice_request_firmware_info(dev_fd);
        ev_io_init(&device_watcher, device_io_cb, dev_fd, EV_READ);
        //set the data pointer so we can access the parser in the callback
        device_watcher.data = (void *)&mydata;
        ev_io_start(loop, &device_watcher);

        const char *no_timeout = getenv("NO_TIMEOUT");
        if (no_timeout) {
            GPSUTILS_INFO("No timeout set, press Ctrl+C to exit loop\n");
        } else {
            ev_timer_init(&timeout_watcher, timeout_cb, 10 /* seconds */, 0.);
            ev_timer_start(loop, &timeout_watcher);
        }
        ev_run(loop, 0);
        rc = 0;
    }
    gpsdevice_close(dev_fd);
    // free memory
    gpsdata_parser_free(mydata.parser);
    gpsdata_list_free(&(mydata.datalistp));
    close(mydata.log_fd);
    return rc;
}
