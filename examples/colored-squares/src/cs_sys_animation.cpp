#include "cs_sys_animation.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"

cs::SysAnimation::SysAnimation(Table& t) :
    ecsa::System<64>(),
    table(t)
{
    
}

bool cs::SysAnimation::select(Entity e)
{
    return table.has<Ids::ANIMATION>(e);
}

void cs::SysAnimation::init()
{

}

void cs::SysAnimation::update()
{
    for (Entity e : this->subscribed())
    {
        // get the components for the entity
        Gfx & gfx    = table.get<Gfx, Ids::GFX>(e);
        Animation & anim = table.get<Animation, Ids::ANIMATION>(e);

        // update animation timer and current animation index
        if (anim.timer > 0)
            anim.timer--;
        else
        {
            if (anim.curr < anim.last)
                anim.curr++;
            else
                anim.curr = anim.first;
            anim.timer = 10;
        }

        // apply the correct animation to the sprite
        if (anim.timer == 0 && gfx.sprite.has_value())
            gfx.sprite.value().set_tiles(bn::sprite_items::squares.tiles_item(), anim.curr);
    }
}