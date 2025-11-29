#include "cs_sys_movement.h"

cs::SysMovement::SysMovement(Table& t) :
    ecsa::System<128>(),
    table(t)
{
    
}

bool cs::SysMovement::select(Entity e)
{
    return table.has<Ids::POSITION>(e)
        && table.has<Ids::VELOCITY>(e);
}

void cs::SysMovement::init()
{

}

void cs::SysMovement::update()
{
    for (Entity e : this->subscribed())
    {
        Gfx & gfx     = table.get<Gfx, Ids::GFX>(e);
        Vector2 & pos = table.get<Vector2, Ids::POSITION>(e);
        Vector2 & vel = table.get<Vector2, Ids::VELOCITY>(e);

        pos.x += vel.x;
        pos.y += vel.y;

        if (pos.x < -120)
        {
            pos.x = -120;
            vel.x *= -1;
        }
        else if (pos.x > 120)
        {
            pos.x = 120;
            vel.x *= -1;
        }
        
        if (pos.y < -80)
        {
            pos.y = -80;
            vel.y *= -1;
        }
        else if (pos.y > 80)
        {
            pos.y = 80;
            vel.y *= -1;
        }

        if (gfx.sprite.has_value())
        {
            gfx.sprite.value().set_x(pos.x);
            gfx.sprite.value().set_y(pos.y);
        }
    }
    
}