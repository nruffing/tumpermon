#include "player.h"

#include <gb/gb.h>

#include <res/player.h>

#include "kinematics.h"
#include "utils/metasprite_util.h"

AnimatedMetasprite create_player_metasprite(void)
{
    MetaspriteRef down_ref = { .metasprite = player_metasprites[0], .flip_x = false };
    MetaspriteRef up_ref = { .metasprite = player_metasprites[1], .flip_x = false };
    MetaspriteRef right_ref = { .metasprite = player_metasprites[2], .flip_x = true };
    MetaspriteRef left_ref = { .metasprite = player_metasprites[2], .flip_x = false };

    AnimatedMetasprite metasprite = {
        .metadata = {
            .start_tile_index = PLAYER_SPRITE_TILE_START_INDEX,
            .sprite_num = PLAYER_SPRITE_SLOT,
            .base_props = DEFAULT_METASPRITE_BASE_PROP,
            .max_sprite_count = PLAYER_SPRITE_MAX_COUNT,
        },
    };

    // No walk-cycle art yet — every animation-frame slot for a direction
    // repeats that direction's single pose (see sprites.h's "pad to max
    // with idle" convention on MetaspriteAnimationFrames). Once walk frames
    // exist, build each direction's frames directly instead of padding.
    metasprite.frames[DIRECTION_DOWN] = pad_metasprite_animation_frames(down_ref);
    metasprite.frames[DIRECTION_UP] = pad_metasprite_animation_frames(up_ref);
    metasprite.frames[DIRECTION_LEFT] = pad_metasprite_animation_frames(left_ref);
    metasprite.frames[DIRECTION_RIGHT] = pad_metasprite_animation_frames(right_ref);

    return metasprite;
}

Player initialize_player(Position position, Direction direction, AnimatedMetasprite metasprite)
{
    PlayerPreviousFrame previous_frame = { .hit_points = 0 };
    Player player = { .metasprite = metasprite,
                      .position = position,
                      .direction = direction,
                      .hit_points = INITIAL_PLAYER_HIT_POINT,
                      .previous_frame = previous_frame };
    update_player_sprite(&player);
    return player;
}

void apply_player_velocity(Player *player, Velocity velocity)
{
    player->position.x += velocity.x;
    player->position.y += velocity.y;

    // Prefer facing vertically over horizontally when moving diagonally.
    // Standing still (both zero) keeps whatever direction was last faced.
    if (velocity.y < 0) {
        player->direction = DIRECTION_UP;
    } else if (velocity.y > 0) {
        player->direction = DIRECTION_DOWN;
    } else if (velocity.x < 0) {
        player->direction = DIRECTION_LEFT;
    } else if (velocity.x > 0) {
        player->direction = DIRECTION_RIGHT;
    }
}

void update_player_sprite(Player *player)
{
    move_animated_metasprite_to_position(player->metasprite, player->position, player->direction);
}

void update_player_previous_frame_state(Player *player)
{
    PlayerPreviousFrame previous_frame = { .hit_points = player->hit_points };
    player->previous_frame = previous_frame;
}
