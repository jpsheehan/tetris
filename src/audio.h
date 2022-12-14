#ifndef AUDIO_H_
#define AUDIO_H_

typedef enum SFX {
    SFX_ROTATE_CW,
    SFX_ROTATE_CCW,
    SFX_LOCK_DOWN,
    SFX_HARD_DROP,
    SFX_LINE_CLEAR,
    SFX_MAX
} SFX;

void audio_init(void);
void audio_deinit(void);
void audio_play_sfx(SFX sfx);
void audio_play_music(void);

#endif