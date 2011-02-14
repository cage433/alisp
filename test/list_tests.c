#include "list.h"
#include "check.h"
#include "test_suites.h"
#include "stdlib.h"
#include "boxed_value.h"

START_TEST(test_list_length)
{
    List *l = NULL;
    fail_unless(listlen(l) == 0);
    l = cons("foo", l);
    fail_unless(listlen(l) == 1);
    l = cons("bar", l);
    fail_unless(listlen(l) == 2);
}
END_TEST

static void *list_test_inc(const void *v){
    boxed_value *bv = (boxed_value *)v;
    return make_boxed_int(bv->int_value + 1);
}

START_TEST(test_map)
{
    List *l = make_list(2, make_boxed_int(10), make_boxed_int(20));
    List *l2 = list_map(l, list_test_inc);
    List *expected = make_list(2, make_boxed_int(11), make_boxed_int(21));
    fail_unless(lists_equal(l2, expected, boxed_values_equal));
}
END_TEST

static int test_for_each_cnt = 0;
static void inc_test_for_each_cnt(void *v){
    const boxed_value *bv = v;
    test_for_each_cnt += bv->int_value;
}
START_TEST(test_for_each){
    test_for_each_cnt = 0;
    List *l = make_list(3, make_boxed_int(3), make_boxed_int(4), make_boxed_int(5));
    list_for_each(l, inc_test_for_each_cnt);
    fail_unless(test_for_each_cnt == 12);
}
END_TEST

static void* fold_test_times(void *v1, void *v2){
    const boxed_value *bv1 = v1;
    const boxed_value *bv2 = v2;
    boxed_value *result = make_boxed_int(bv1->int_value * bv2->int_value);
    my_free(v1);
    return result;
}

START_TEST(test_fold)
{
    boxed_value *init = make_boxed_int(10);
    List *l = make_list(2, make_boxed_int(5), make_boxed_int(12));
    boxed_value *result = list_fold(l, init, fold_test_times);
    fail_unless(boxed_values_equal(result, make_boxed_int(600)));
}
END_TEST


Suite *test_list_suite ()
{
    Suite *s = suite_create ("list");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_list_length);
    tcase_add_test (tc_core, test_map);
    tcase_add_test (tc_core, test_for_each);
    tcase_add_test (tc_core, test_fold);
    suite_add_tcase (s, tc_core);

    return s;
}

