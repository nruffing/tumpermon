#include "metasprite_util.h"

MetaspriteAnimationFrames pad_metasprite_animation_frames(MetaspriteRef idle_ref)
{
    MetaspriteAnimationFrames frames;
    for (uint8_t i = 0; i < METASPRITE_MAX_ANIMATION_FRAMES; i++) {
        frames.refs[i] = idle_ref;
    }
    return frames;
}
