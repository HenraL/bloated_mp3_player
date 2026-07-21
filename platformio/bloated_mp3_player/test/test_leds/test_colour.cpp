#include <unity.h>
#include <leds.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_colour_default_ctor_all_zero(void)
{
    My::LED::Colour c;
    TEST_ASSERT_EQUAL_UINT8(0, c.r);
    TEST_ASSERT_EQUAL_UINT8(0, c.g);
    TEST_ASSERT_EQUAL_UINT8(0, c.b);
    TEST_ASSERT_EQUAL_UINT8(0, c.w);
}

void test_colour_param_ctor_stores_channels(void)
{
    My::LED::Colour c(12, 34, 56, 78);
    TEST_ASSERT_EQUAL_UINT8(12, c.r);
    TEST_ASSERT_EQUAL_UINT8(34, c.g);
    TEST_ASSERT_EQUAL_UINT8(56, c.b);
    TEST_ASSERT_EQUAL_UINT8(78, c.w);
}

void test_colour_max_values(void)
{
    My::LED::Colour c(255, 255, 255, 255);
    TEST_ASSERT_EQUAL_UINT8(255, c.r);
    TEST_ASSERT_EQUAL_UINT8(255, c.g);
    TEST_ASSERT_EQUAL_UINT8(255, c.b);
    TEST_ASSERT_EQUAL_UINT8(255, c.w);
}

void test_colour_red_only(void)
{
    My::LED::Colour c(200, 0, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(200, c.r);
    TEST_ASSERT_EQUAL_UINT8(0, c.g);
    TEST_ASSERT_EQUAL_UINT8(0, c.b);
    TEST_ASSERT_EQUAL_UINT8(0, c.w);
}

void test_colour_green_only(void)
{
    My::LED::Colour c(0, 150, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(0, c.r);
    TEST_ASSERT_EQUAL_UINT8(150, c.g);
    TEST_ASSERT_EQUAL_UINT8(0, c.b);
    TEST_ASSERT_EQUAL_UINT8(0, c.w);
}

void test_colour_blue_only(void)
{
    My::LED::Colour c(0, 0, 100, 0);
    TEST_ASSERT_EQUAL_UINT8(0, c.r);
    TEST_ASSERT_EQUAL_UINT8(0, c.g);
    TEST_ASSERT_EQUAL_UINT8(100, c.b);
    TEST_ASSERT_EQUAL_UINT8(0, c.w);
}

void test_colour_white_only(void)
{
    My::LED::Colour c(0, 0, 0, 50);
    TEST_ASSERT_EQUAL_UINT8(0, c.r);
    TEST_ASSERT_EQUAL_UINT8(0, c.g);
    TEST_ASSERT_EQUAL_UINT8(0, c.b);
    TEST_ASSERT_EQUAL_UINT8(50, c.w);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_colour_default_ctor_all_zero);
    RUN_TEST(test_colour_param_ctor_stores_channels);
    RUN_TEST(test_colour_max_values);
    RUN_TEST(test_colour_red_only);
    RUN_TEST(test_colour_green_only);
    RUN_TEST(test_colour_blue_only);
    RUN_TEST(test_colour_white_only);
    return UNITY_END();
}
