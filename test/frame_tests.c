#include "frame.h"
#include "check.h"
#include "boxed_value.h"
#include "stdio.h"

START_TEST(test_ref_count){
    boxed_value *v = make_boxed_int(2);
    fail_unless(v->ref_count == 0);
    Hash *frame = create_empty_frame();
    char *key = "key";
    frame_add(frame, key, v);
    fail_unless(v->ref_count == 1);
    frame_add(frame, key, v);
    fail_unless(v->ref_count == 1);
}
END_TEST

START_TEST(test_ref_count2){
    boxed_value *v1 = make_boxed_int(2);
    boxed_value *v2 = make_boxed_int(2);
    Hash *frame1 = create_empty_frame();
    Hash *frame2 = create_empty_frame();
    char *key = "key";
    frame_add(frame1, key, v1);
    frame_add(frame2, key, v1);
    fail_unless(v1->ref_count == 2);
    frame_add(frame2, key, v1);
    fail_unless(v1->ref_count == 2);
    frame_add(frame2, key, v2);
    fail_unless(v1->ref_count == 1);
}
END_TEST



Suite *test_frame_suite ()
{
    Suite *s = suite_create ("frame");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_ref_count);
    tcase_add_test (tc_core, test_ref_count2);
    suite_add_tcase (s, tc_core);

    return s;
}
