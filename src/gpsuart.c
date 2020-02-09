#include <gpsutils.h>
#ifdef HAVE_UV_H
    #include <uv.h>
#endif

int64_t __idle_counter = 0;
void gpsuart_default_idler(uv_idle_t *hndl) {
    __idle_counter++;
    if (__idle_counter >= 10e6) {
        uv_idle_stop(hndl);
    }
}

void gpsuart_default_fs(uv_fs_t *req) {
    GPSUTILS_INFO("fs req type: %d\n", req->fs_type);
    GPSUTILS_INFO("fs req result: %ld\n", req->result);
    if (req->fs_type == UV_FS_OPEN) {
        GPSUTILS_INFO("Opening the file: %s\n", req->path);
        uv_fs_t *req_c = req->data;
        GPSUTILS_INFO("fs req data: %p\n", req->data);
        GPSUTILS_INFO("fs req data type: %d\n", req->type);
        uv_fs_close(req->loop, req_c, (uv_file)req->result, gpsuart_default_fs);
    } else if (req->fs_type == UV_FS_CLOSE) {
        GPSUTILS_INFO("Closing the file: %s %ld\n", req->path, req->result);
    }
}

int main(int argc, char **argv)
{
    uv_loop_t *loop = malloc(sizeof(uv_loop_t));
    if (!loop) {
        GPSUTILS_ERROR_NOMEM(sizeof(uv_loop_t));
        return -ENOMEM;
    }
    uv_loop_init(loop);
    uv_idle_t idler;
    uv_idle_init(loop, &idler);
    uv_idle_start(&idler, gpsuart_default_idler);
    
    uv_fs_t req_o, req_c;
    req_o.data = &req_c;
    req_o.type = UV_FS;
    int rc = uv_fs_open(loop, &req_o, "/dev/ttyUSB0", //O_RDONLY, 0,
            O_NONBLOCK | O_RDWR | O_NOCTTY, 0, gpsuart_default_fs);
    GPSUTILS_INFO("rc: %d\n", rc);

    GPSUTILS_INFO("Idling...\n");
    uv_run(loop, UV_RUN_DEFAULT);


    uv_fs_req_cleanup(&req_o);
    uv_fs_req_cleanup(&req_c);
    uv_loop_close(loop);
    GPSUTILS_FREE(loop);    
    return 0;
}
