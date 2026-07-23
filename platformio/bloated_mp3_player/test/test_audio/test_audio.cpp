#include <unity.h>
#include <internal/audio.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_volume_default(void)
{
    Audio::Audio audio(17, 18, 8, 256);
    TEST_ASSERT_EQUAL_UINT8(128, audio.getVolume());
}

void test_volume_set_and_get(void)
{
    Audio::Audio audio(17, 18, 8, 256);
    audio.setVolume(200);
    TEST_ASSERT_EQUAL_UINT8(200, audio.getVolume());
}

void test_volume_zero(void)
{
    Audio::Audio audio(17, 18, 8, 256);
    audio.setVolume(0);
    TEST_ASSERT_EQUAL_UINT8(0, audio.getVolume());
}

void test_volume_max(void)
{
    Audio::Audio audio(17, 18, 8, 256);
    audio.setVolume(255);
    TEST_ASSERT_EQUAL_UINT8(255, audio.getVolume());
}

void test_initial_state_stopped(void)
{
    Audio::Audio audio(17, 18, 8, 256);
    TEST_ASSERT_EQUAL_UINT8(0, (uint8_t)audio.getStatus());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_volume_default);
    RUN_TEST(test_volume_set_and_get);
    RUN_TEST(test_volume_zero);
    RUN_TEST(test_volume_max);
    RUN_TEST(test_initial_state_stopped);
    return UNITY_END();
}
