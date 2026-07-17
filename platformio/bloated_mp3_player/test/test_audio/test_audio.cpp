#include <unity.h>
#include <internal/audio.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_volume_default(void)
{
    TEST_ASSERT_EQUAL_UINT8(128, Audio::get_volume());
}

void test_volume_set_and_get(void)
{
    Audio::set_volume(200);
    TEST_ASSERT_EQUAL_UINT8(200, Audio::get_volume());
}

void test_volume_zero(void)
{
    Audio::set_volume(0);
    TEST_ASSERT_EQUAL_UINT8(0, Audio::get_volume());
}

void test_volume_max(void)
{
    Audio::set_volume(255);
    TEST_ASSERT_EQUAL_UINT8(255, Audio::get_volume());
}

void test_initial_state_stopped(void)
{
    TEST_ASSERT_FALSE(Audio::is_playing());
    TEST_ASSERT_EQUAL_UINT8(0, (uint8_t)Audio::State::Stopped);
}

void test_play_raw_null_returns_false(void)
{
    TEST_ASSERT_FALSE(Audio::play_raw(nullptr, 0));
}

void test_begin_no_hw_returns_false(void)
{
    TEST_ASSERT_FALSE(Audio::begin(16, 17, 18));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_volume_default);
    RUN_TEST(test_volume_set_and_get);
    RUN_TEST(test_volume_zero);
    RUN_TEST(test_volume_max);
    RUN_TEST(test_initial_state_stopped);
    RUN_TEST(test_play_raw_null_returns_false);
    RUN_TEST(test_begin_no_hw_returns_false);
    return UNITY_END();
}
