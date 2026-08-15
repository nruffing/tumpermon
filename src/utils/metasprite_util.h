#ifndef METASPRITE_UTIL_H
#define METASPRITE_UTIL_H

#include "../sprites.h"

// Builds a MetaspriteAnimationFrames with every slot set to `idle_ref` — the
// "pad to max with idle" convention documented on MetaspriteAnimationFrames
// (sprites.h), for directions/entities that don't have (or don't yet have)
// distinct walk-cycle frames.
MetaspriteAnimationFrames pad_metasprite_animation_frames(MetaspriteRef idle_ref);

#endif
