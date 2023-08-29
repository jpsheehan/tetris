#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

#include "asset_loader.h"
#include "audio.h"
#include "utils.h"

static int music = 0;
static int samples[SFX_MAX] = {0};

static char *filenames[SFX_MAX] = {
    "./resources/audio/sfx_rotate_cw.ogg",
    "./resources/audio/sfx_rotate_ccw.ogg",
    "./resources/audio/sfx_lock_down.ogg",
    "./resources/audio/sfx_hard_drop.ogg",
    "./resources/audio/sfx_line_clear.ogg",
    "./resources/audio/sfx_tetris.ogg",
    "./resources/audio/sfx_perfect_clear.ogg",
    "./resources/audio/sfx_tspin.ogg",
    "./resources/audio/sfx_tspin.ogg",
    "./resources/audio/sfx_pause.ogg",
    "./resources/audio/sfx_three.ogg",
    "./resources/audio/sfx_two.ogg",
    "./resources/audio/sfx_one.ogg",
    "./resources/audio/sfx_go.ogg",
    "./resources/audio/sfx_prespawn_rotate.ogg",
};

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
     if (music == 0)
     {
         music = asset_loader_load("audio stream", A_AUDIO_STREAM, (AssetLoaderCallback)&create_music);

         al_set_audio_stream_playmode(A(music), ALLEGRO_PLAYMODE_LOOP);
         audio_turn_music_down();
         al_attach_audio_stream_to_mixer(A(music), al_get_default_mixer());
     }

    for (int i = 0; i < SFX_MAX; i++)
    {
        if (filenames[i] == NULL || samples[i] != 0)
        {
            continue;
        }
        filename_to_load = filenames[i];
        samples[i] = asset_loader_load(filename_to_load, A_SAMPLE, (AssetLoaderCallback)&create_sample);
    }
}

void audio_play_sfx(SFX sfx)
{
    // printf("Play %d\n", (int)sfx);
    if (sfx < 0 || sfx >= SFX_MAX)
    {
        safe_exit("SFX is out of range", 1);
        return;
    }
    if (samples[sfx] == 0)
    {
        if (filenames[sfx] != NULL)
            safe_exit("Sample is null", 1);
        return;
    }
    float gain = 0.4;
    if (sfx >= SFX_THREE && sfx <= SFX_GO)
        gain = 0.5;
    al_play_sample(A(samples[sfx]), gain, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void audio_turn_music_down(void)
{
    if (music != 0)
        al_set_audio_stream_gain(A(music), 0.05);
}

void audio_turn_music_up(void)
{
    if (music != 0)
        al_set_audio_stream_gain(A(music), 0.15);
}