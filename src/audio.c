#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

#include "asset_loader.h"
#include "audio.h"
#include "utils.h"

static ALLEGRO_AUDIO_STREAM *music = NULL;
static ALLEGRO_SAMPLE *samples[SFX_MAX] = {0};

ALLEGRO_AUDIO_STREAM *create_music(void)
{
    return al_load_audio_stream("./resources/audio/track_01.opus", 2, 2048);
}

static char *filename_to_load = NULL;

ALLEGRO_SAMPLE *create_sample(void)
{
    if (filename_to_load == NULL)
        return NULL;
    return al_load_sample(filename_to_load);
}

void audio_init(void)
{
    // if (music == NULL)
    // {
    //     music = asset_loader_load(A_AUDIO_STREAM, (AssetLoaderCallback)&create_music);
    //     must_init(music, "audio stream");

    //     al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
    //     audio_turn_music_down();
    //     al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
    // }

    char *filenames[SFX_MAX] = {
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

    for (int i = 0; i < SFX_MAX; i++)
    {
        if (filenames[i] == NULL || samples[i] != NULL)
        {
            continue;
        }
        filename_to_load = filenames[i];
        samples[i] = asset_loader_load(A_SAMPLE, (AssetLoaderCallback)&create_sample);
        must_init(samples[i], filenames[i]);
    }
}

void audio_play_sfx(SFX sfx)
{
    // printf("Play %d\n", (int)sfx);
    if (sfx < 0 || sfx >= SFX_MAX){
        safe_exit("SFX is out of range", 1);
        return;
    }
    if (samples[sfx] == NULL) {
        safe_exit("Sample is null", 1);
        return;
    }
    float gain = 0.4;
    if (sfx >= SFX_THREE && sfx <= SFX_GO) gain = 0.8;
    al_play_sample(samples[sfx], gain, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void audio_turn_music_down(void)
{
    if (music != NULL)
        al_set_audio_stream_gain(music, 0.05);
}

void audio_turn_music_up(void)
{
    if (music != NULL)
        al_set_audio_stream_gain(music, 0.15);
}