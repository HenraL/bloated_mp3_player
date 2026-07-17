#include <unity.h>
#include <internal/sdcard.hpp>

void setUp(void) {}
void tearDown(void) {}

void test_is_audio_file_wav(void)
{
    TEST_ASSERT_TRUE(SDCard::is_audio_file("song.wav"));
}

void test_is_audio_file_mp3(void)
{
    TEST_ASSERT_TRUE(SDCard::is_audio_file("track.mp3"));
}

void test_is_audio_file_caps_WAV(void)
{
    TEST_ASSERT_TRUE(SDCard::is_audio_file("SONG.WAV"));
}

void test_is_audio_file_caps_MP3(void)
{
    TEST_ASSERT_TRUE(SDCard::is_audio_file("TRACK.MP3"));
}

void test_is_audio_file_with_path(void)
{
    TEST_ASSERT_TRUE(SDCard::is_audio_file("/music/song.wav"));
}

void test_is_audio_file_txt_fails(void)
{
    TEST_ASSERT_FALSE(SDCard::is_audio_file("notes.txt"));
}

void test_is_audio_file_jpg_fails(void)
{
    TEST_ASSERT_FALSE(SDCard::is_audio_file("cover.jpg"));
}

void test_is_audio_file_no_ext_fails(void)
{
    TEST_ASSERT_FALSE(SDCard::is_audio_file("README"));
}

void test_is_audio_file_dot_in_middle_fails(void)
{
    TEST_ASSERT_FALSE(SDCard::is_audio_file("my.wav.file.txt"));
}

void test_begin_without_hw_returns_false(void)
{
    TEST_ASSERT_FALSE(SDCard::begin(13, 11, 12, 14));
}

void test_is_mounted_after_fail(void)
{
    SDCard::begin(13, 11, 12, 14);
    TEST_ASSERT_FALSE(SDCard::is_mounted());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_is_audio_file_wav);
    RUN_TEST(test_is_audio_file_mp3);
    RUN_TEST(test_is_audio_file_caps_WAV);
    RUN_TEST(test_is_audio_file_caps_MP3);
    RUN_TEST(test_is_audio_file_with_path);
    RUN_TEST(test_is_audio_file_txt_fails);
    RUN_TEST(test_is_audio_file_jpg_fails);
    RUN_TEST(test_is_audio_file_no_ext_fails);
    RUN_TEST(test_is_audio_file_dot_in_middle_fails);
    RUN_TEST(test_begin_without_hw_returns_false);
    RUN_TEST(test_is_mounted_after_fail);
    return UNITY_END();
}
