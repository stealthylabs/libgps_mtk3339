/*
 * COPYRIGHT: 2015-2020 Stealthy Labs LLC
 * ORIGINAL DATE: 22nd April 2015
 * MODIFIED DATE: 16th Oct 2019
 * MODIFIED SOFTWARE: libgps_mtk3339
 */
#include <gpsdata.h>
#ifdef HAVE_FCNTL_H
    #include <fcntl.h>
#endif
#ifdef HAVE_CUNIT
    #include <CUnit/CUnit.h>
    #include <CUnit/Basic.h>
#endif

typedef struct filename_t {
    char *name;
    struct filename_t *next;
} filename_t;

filename_t *g_filelist = NULL;

void test_parse_file()
{
    gpsutils_timer_t tt;
    CU_ASSERT_PTR_NOT_NULL(g_filelist);
    if (!g_filelist) {
        GPSUTILS_ERROR("Filename list is NULL\n");
        return;
    }
    filename_t *el = NULL;
    LL_FOREACH(g_filelist, el) {
        CU_ASSERT_PTR_NOT_NULL(el);
        CU_ASSERT_PTR_NOT_NULL(el->name);
        if (!el->name)
            continue;
        FILE *fp = fopen(el->name, "rb");
        CU_ASSERT_PTR_NOT_NULL(fp);
        gpsdata_parser_t *fsm = gpsdata_parser_create();
        CU_ASSERT_PTR_NOT_NULL(fsm);
        char buf[256];
        gpsdata_data_t *outp = NULL;
        gpsutils_timer_start(&tt);
        while (!feof(fp)) {
            size_t nb = fread(buf, sizeof(char), sizeof(buf) / sizeof(char), fp);
            if (nb > 0) {
                size_t onum = 0;
                int rc = gpsdata_parser_parse(fsm, buf, nb, &outp, &onum);
                CU_ASSERT(rc >= 0);
                gpsdata_parser_dump_state(fsm, stdout);
                memset(buf, 0, sizeof(buf) / sizeof(char));
                if (rc < 0)
                    break;
            } else {
                GPSUTILS_WARN("Looks like end of file reached\n");
                break;
            }
        }
        gpsutils_timer_stop(&tt);
        GPSUTILS_INFO("Time taken to parse input file %s: %lfs\n", el->name, tt.time_taken);
        CU_ASSERT_PTR_NOT_NULL(outp);
        GPSUTILS_INFO("No. of objects in file: %zd\n", gpsdata_list_count(outp));
        gpsdata_list_dump(outp, stdout);
        gpsdata_list_free(&outp);

        fclose(fp);
        gpsdata_parser_free(fsm);
    }
}

int main(int argc, char **argv)
{
    int err = 0;
    CU_pSuite suite = NULL;
#ifndef NDEBUG
    GPSUTILS_LOGLEVEL_SET(DEBUG);
#else
    GPSUTILS_LOGLEVEL_SET(INFO);
#endif
    if (CU_initialize_registry() != CUE_SUCCESS) {
        GPSUTILS_ERROR("%s\n", CU_get_error_msg());
        return CU_get_error();
    }
    do {
        suite = CU_add_suite(argv[0], NULL, NULL);
        if (suite == NULL) {
            GPSUTILS_ERROR("%s\n",
                    CU_get_error_msg());
            break;
        }
        if (argc > 1) {
            for (int i = 1; i < argc; i++) {
                if (argv[1]) {
                    filename_t *afile = calloc(1, sizeof(filename_t));
                    if (!afile) {
                        GPSUTILS_ERROR_NOMEM(sizeof(filename_t));
                        return -1;
                    } else {
                        afile->name = strdup(argv[1]);
                        LL_APPEND(g_filelist, afile);
                        GPSUTILS_INFO("Will try to parse file %s\n", afile->name);
                    }
                }
            }
        } else {
            char buf[PATH_MAX];
            for (int i = 1; i < 9; i++) {
                memset(buf, 0, sizeof(buf));
                snprintf(buf, sizeof(buf) - 1, "sample_gpsdata_usbttl_%d.txt", i);
                if (access(buf, F_OK | R_OK) != -1) {
                    filename_t *afile = calloc(1, sizeof(filename_t));
                    if (!afile) {
                        GPSUTILS_ERROR_NOMEM(sizeof(filename_t));
                        return -1;
                    } else {
                        afile->name = strdup(buf);
                        LL_APPEND(g_filelist, afile);
                        GPSUTILS_INFO("Will try to parse file %s\n", afile->name);
                    }
                }
            }
        }
        size_t count = 0;
        filename_t *el = NULL, *el_tmp = NULL;
        LL_COUNT(g_filelist, el, count);
        if (count == 0) {
            GPSUTILS_WARN("No files found for testing, skipping tests\n");
        } else {
            if (!CU_ADD_TEST(suite, test_parse_file))
                break;
        }
        /* set the mode of the test run in
         * debug/release mode*/
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
        LL_FOREACH_SAFE(g_filelist, el, el_tmp) {
            LL_DELETE(g_filelist, el);
            GPSUTILS_FREE(el->name);
            GPSUTILS_FREE(el);
        }
    } while (0);
    err = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return err;
}
