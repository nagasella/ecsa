#include "cs_sys_rotation.h"

cs::SysRotation::SysRotation(Table& t) :
    System<96>(),
    table(t)
{
    
}

bool cs::SysRotation::select(Entity e)
{
    return table.has<Ids::TRANSFORM>(e);
}

void cs::SysRotation::init()
{

}

void cs::SysRotation::update()
{
    for (Entity e : this->subscribed())
    {
        Gfx & gfx = table.get<Gfx, Ids::GFX>(e);
        Transform & transform = table.get<Transform, Ids::TRANSFORM>(e);

        transform.angle++;
        if (transform.angle == 360)
            transform.angle = 0;
        
        if (gfx.sprite.has_value())
            gfx.sprite.value().set_rotation_angle(transform.angle);
    }
}