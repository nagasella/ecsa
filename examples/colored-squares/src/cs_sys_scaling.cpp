#include "cs_sys_scaling.h"

#include "bn_keypad.h"


cs::SysScaling::SysScaling(Table& t) :
    System<64>(),
    table(t)
{
    
}

bool cs::SysScaling::select(Entity e)
{
    return table.has<Ids::TRANSFORM>(e);
}

void cs::SysScaling::init()
{

}

void cs::SysScaling::update()
{
    for (Entity e : this->subscribed())
    {
        Gfx & gfx = table.get<Gfx, Ids::GFX>(e);
        Transform & transform = table.get<Transform, Ids::TRANSFORM>(e);

        if (bn::keypad::r_pressed())
        {
            if (transform.scale == 1)
                transform.scale = 1.5;
            else
                transform.scale = 1;
        }

        if (gfx.sprite.has_value())
            gfx.sprite.value().set_scale(transform.scale);
    }
        
}