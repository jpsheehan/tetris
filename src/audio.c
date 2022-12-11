#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include "audio.h"
#include "utils.h"

static ALLEGRO_SAMPLE* samples[SFX_MAX];

void audio_init(void)
{
    must_init(al_install_audio(), "install audio");
    must_init(al_init_acodec_addon(), "init audio codec");
    must_init(al_reserve_samples(16), "reserve samples");

    char* filenames[SFX_MAX] = {
        "./resources/audio/sfx_rotate_cw.ogg",
        "./resources/audio/sfx_rotate_ccw.ogg",
        "./resources/audio/sfx_lock_down.ogg",
        "./resources/audio/sfx_hard_drop.ogg",
        "./resources/audio/sfx_line_clear.ogg",
    };

    for (int i = 0; i < SFX_MAX; i++) {
        samples[i] = al_load_sample(filenames[i]);
        must_init(samples[i], filenames[i]);
    }
}

void audio_deinit(void)
{
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
    al_play_sample(samples[sfx], 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}