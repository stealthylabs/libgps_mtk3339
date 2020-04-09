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


static const char *g_filename = NULL;

void test_parse_file()
{
    gpsutils_timer_t tt;
    CU_ASSERT_PTR_NOT_NULL(g_filename);
    if (!g_filename) {
        GPSUTILS_ERROR("Filename is NULL\n");
        return;
    }
    FILE *fp = fopen(g_filename, "rb");
    CU_ASSERT_PTR_NOT_NULL(fp);
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);
    char buf[80];
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
    GPSUTILS_INFO("Time taken to parse input file %s: %lfs\n", g_filename, tt.time_taken);
    CU_ASSERT_PTR_NOT_NULL(outp);
    GPSUTILS_INFO("No. of objects in file: %zd\n", gpsdata_list_count(outp));
    gpsdata_list_dump(outp, stdout);
    gpsdata_list_free(&outp);

    fclose(fp);
    gpsdata_parser_free(fsm);
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
            g_filename = argv[1];
            GPSUTILS_INFO("Will try to parse file %s\n", argv[1]);
            if (!CU_ADD_TEST(suite, test_parse_file))
                break;
        } else {
            g_filename = "sample_gpsdata_usbttl.txt";
            if (access(g_filename, F_OK | R_OK) != -1) {
                GPSUTILS_INFO("Will try to parse file %s\n", g_filename);
                if (!CU_ADD_TEST(suite, test_parse_file))
                    break;
            } else {
                GPSUTILS_WARN("File %s not found or cannot be read, skipping test\n", g_filename);
                g_filename = NULL;
            }
        }
        /* set the mode of
         * the test run in
         * debug/release
         * mode*/
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    } while (0);
    err = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return err;
}
