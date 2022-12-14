#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include "audio.h"
#include "utils.h"

static ALLEGRO_AUDIO_STREAM* music = NULL;
static ALLEGRO_SAMPLE* samples[SFX_MAX] = { 0 };

void audio_init(void)
{
    must_init(al_install_audio(), "install audio");
    must_init(al_init_acodec_addon(), "init audio codec");
    must_init(al_reserve_samples(16), "reserve samples");

    music = al_load_audio_stream("./resources/audio/track_01.opus", 2, 2048);
    must_init(music, "audio stream");
    al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
    al_attach_audio_stream_to_mixer(music, al_get_default_mixer());

    char* filenames[SFX_MAX] = {
        "./resources/audio/sfx_rotate_cw.ogg",
        "./resources/audio/sfx_rotate_ccw.ogg",
        "./resources/audio/sfx_lock_down.ogg",
        "./resources/audio/sfx_hard_drop.ogg",
        "./resources/audio/sfx_line_clear.ogg",
        NULL,
        "./resources/audio/sfx_three.ogg",
        "./resources/audio/sfx_two.ogg",
        "./resources/audio/sfx_one.ogg",
        "./resources/audio/sfx_go.ogg",
    };

    for (int i = 0; i < SFX_MAX; i++) {
        if (filenames[i] == NULL) {
            continue;
        }
        samples[i] = al_load_sample(filenames[i]);
        must_init(samples[i], filenames[i]);
    }
}

void audio_deinit(void)
{
    if (music != NULL)
    {
        al_destroy_audio_stream(music);
        music = NULL;
    }

    for (int i = 0; i < SFX_MAX; i++)
    {
        if (samples[i] != NULL) {
            al_destroy_sample(samples[i]);
            samples[i] = NULL;
        }
    }

    if (al_is_audio_installed())
        al_uninstall_audio();
}

void audio_play_sfx(SFX sfx)
{
    if (sfx < 0 || sfx >= SFX_MAX)
        return;
    if (samples[sfx] == NULL)
        return;
    al_play_sample(samples[sfx], 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void audio_play_music(void)
{

}