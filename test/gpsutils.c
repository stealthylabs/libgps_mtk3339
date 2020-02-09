/*
 * COPYRIGHT: 2015-2020 Stealthy Labs LLC
 * ORIGINAL DATE: 22nd April 2015
 * MODIFIED DATE: 16th Oct 2019
 * MODIFIED SOFTWARE: libgps_mtk3339
 */
#include <gpsutils.h>
#ifdef HAVE_CUNIT
    #include <CUnit/CUnit.h>
    #include <CUnit/Basic.h>
#endif

void test_hex_parse()
{
    const char buf[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };
    CU_ASSERT_EQUAL(sizeof(buf), 16);
    for (uint8_t i = 0; i < 16; ++i) {
        uint8_t c = gpsutils_hex_parse(buf[i]);
        CU_ASSERT_EQUAL(c, i);
    }
}

void test_str_toupper()
{
    char s[] = "this is a lower case string";
    const char *v = "THIS IS A LOWER CASE STRING";
    gpsutils_string_toupper((char *)s);
    CU_ASSERT_STRING_EQUAL(s, v);
}

void test_hex_dump()
{
    const uint8_t data[] = "This is binary \x01\x02\x7f data"; 
    char *buf = malloc(256 * sizeof(char));
    const char *expected =
"00000000 54 68 69 73 20 69 73 20 62 69 6E 61 72 79 20 01  |This is binary .|\n"
"00000010 02 7F 20 64 61 74 61 00                          |.. data.|\n";
    FILE *mem = NULL;
    CU_ASSERT_PTR_NOT_NULL(buf);
    memset(buf, 0, 256 * sizeof(char));
    mem = fmemopen(buf, 256, "w+");
    CU_ASSERT_PTR_NOT_NULL(mem);
    CU_ASSERT(sizeof(data) < 256);
    fprintf(stdout, "\n");
    gpsutils_hex_dump(data, sizeof(data), stdout);
    fprintf(stdout, "\n");
    gpsutils_hex_dump(data, sizeof(data), mem);
    fclose(mem);

    printf("expected len: %zd\n", strlen(expected));
    printf("received len: %zd\n", strlen(buf));
    CU_ASSERT_EQUAL(strlen(expected), strlen(buf));
    // for debugging
    for (int i = 0; i < strlen(expected) && i < strlen(buf); ++i) {
        if (expected[i] != buf[i]) {
            fprintf(stdout, "i: %d buf: %c expected: %c\n", i, buf[i], expected[i]);
        }
    }
    CU_ASSERT_STRING_EQUAL(buf, expected);
    free(buf);
    buf = NULL;
}

int main(int argc, char **argv)
{
    int err = 0;
    CU_pSuite suite = NULL;
#ifndef NDEBUG
    GPSUTILS_LOGLEVEL_SET(DEBUG);
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
        if (!CU_ADD_TEST(suite, test_hex_parse))
            break;
        if (!CU_ADD_TEST(suite, test_str_toupper))
            break;
        if (!CU_ADD_TEST(suite, test_hex_dump))
            break;
        /* set the mode of the test run in
         * debug/release mode*/
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    } while (0);
    err = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return err;
}
