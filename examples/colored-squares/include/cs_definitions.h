#ifndef CS_DEFINITIONS_H
#define CS_DEFINITIONS_H

#include "ecsa.h"

#include "bn_optional.h"
#include "bn_sprite_ptr.h"


namespace cs
{
    // parametrization of an entity table and its updaters
    using Entity = ecsa::Entity;
    using Table  = ecsa::EntityTable<128, 8, 6>;

    // colors
    enum class Colors
    {
        RED, BLUE, YELLOW, FLASHING
    };

    // components definition
    struct Vector2 : public ecsa::Component
    {
        bn::fixed x, y;

        Vector2(bn::fixed x, bn::fixed y) : x(x), y(y)
        {

        }
    };

    struct Gfx : public ecsa::Component
    {
        bn::optional<bn::sprite_ptr> sprite;

        Gfx(bn::sprite_ptr sprite): sprite(sprite)
        {

        }

        ~Gfx()
        {
            sprite.reset();
        }
    };

    struct Animation : public ecsa::Component
    {
        int first, last, curr, timer;

        Animation(int first, int last) : first(first), last(last), curr(0), timer(0)
        {

        }
    };

    struct Transform : public ecsa::Component
    {
        int angle;
        bn::fixed scale;

        Transform(int angle, bn::fixed scale) : angle(angle), scale(scale)
        {

        }
    };

    struct Color : public ecsa::Component
    {
        Colors color;

        Color(Colors color) : color(color)
        {

        }
    };

    // tags for components and systems
    namespace Ids
    {
        enum
        {
            // components
            POSITION = 0,
            VELOCITY = 1,
            GFX = 2,
            COLOR = 3,
            TRANSFORM = 4,
            ANIMATION = 5,

            // systems
            SYSMOVEMENT = 0,
            SYSROTATION = 1,
            SYSSCALING = 2,
            SYSVISIBILITY = 3,
            SYSENTITYMANAGER = 4,
            SYSANIMATION = 5
        };
    }
    
}


#endif