#include "cs_sys_visibility.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"


cs::SysVisibility::SysVisibility(Table& t) :
    ecsa::System<32>(),
    table(t)
{
    
}

bool cs::SysVisibility::select(Entity e)
{
    return table.has<Ids::GFX>(e);
}

void cs::SysVisibility::init()
{

}

void cs::SysVisibility::update()
{
    for (Entity e : this->subscribed())
    {
        Gfx & gfx = table.get<Gfx, Ids::GFX>(e);

        if (bn::keypad::l_pressed())
        {
            if (gfx.sprite.value().visible())
                gfx.sprite.value().set_visible(false);
            else
                gfx.sprite.value().set_visible(true);
        }
    }
}