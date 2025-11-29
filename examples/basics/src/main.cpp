#include "bn_core.h"
#include "bn_display.h"
#include "bn_optional.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_log.h"

#include "bn_sprite_items_squares.h"

#include "ecsa.h"

// components ids
#define VELOCITY 0
#define GFX 1

// systems ids
#define SYSMOVEMENT 0

// components
struct Vector2 : ecsa::Component
{
    bn::fixed x, y;

    Vector2(bn::fixed x, bn::fixed y) : x(x), y(y)
    {

    }
};

struct Gfx : ecsa::Component
{
    bn::sprite_ptr sprite;

    Gfx(bn::sprite_ptr sprite) : sprite(sprite)
    {

    }
};

// parametrization of an entity table
using Table = ecsa::EntityTable<2, 2, 1>;

// this updater changes the the (x, y) on-screen coordinates 
// of each entity's sprite, based on the entity's velocity
class SysMovement : public ecsa::System<2>
{
    Table & table;

    public:

    SysMovement(Table& t) : ecsa::System<2>(), table(t)
    {

    }

    // select only entities that have both a sprite and velocity component
    bool select(ecsa::Entity e) override
    {
        return table.has<GFX>(e) && table.has<VELOCITY>(e);
    }

    // initialization (if needed...)
    void init() override
    {

    }

    // update each entity processed by this updater
    void update() override
    {
        for (ecsa::Entity e : this->subscribed())
        {
            // read the entity's components from the table
            Gfx & gfx = table.get<Gfx, GFX>(e);
            Vector2 & vel = table.get<Vector2, VELOCITY>(e);

            // get the sprite position
            bn::fixed x = gfx.sprite.x();
            bn::fixed y = gfx.sprite.y();

            // bounce off screen edges
            if (x < -bn::display::width() / 2)
            {
                x = -bn::display::width() / 2;
                vel.x *= -1;
            }
            else if (x > bn::display::width() / 2)
            {
                x = bn::display::width() / 2;
                vel.x *= -1;
            }
            if (y < -bn::display::height() / 2)
            {
                y = -bn::display::height() / 2;
                vel.y *= -1;
            }
            else if (y > bn::display::height() / 2)
            {
                y = bn::display::height() / 2;
                vel.y *= -1;
            }

            // update the sprite position
            gfx.sprite.set_x(x + vel.x);
            gfx.sprite.set_y(y + vel.y);
        }
    }

};


// implements a query that finds all the entities moving towards the right
bool find_entities_moving_right(Table & table, ecsa::Entity e)
{
    Vector2 & vel = table.get<Vector2, VELOCITY>(e);
    if (vel.x > 0)
        return true;
    return false;
}


int main()
{
    bn::core::init();

    // define an entity table
    Table table;

    // add systems and initialize them
    table.add<SYSMOVEMENT>(new SysMovement(table));
    table.init();

    // add an entity to the table, and give it some components
    ecsa::Entity e = table.create();
    BN_LOG(e);
    table.add<VELOCITY>(e, new Vector2(0.5, 0.5));
    table.add<GFX>(e, new Gfx(bn::sprite_items::squares.create_sprite(0, 0)));
    table.subscribe(e); // REQUIRED to subscribe the entitiy to the relevant systems

    // add another entity to the table, and give it some components
    e = table.create();
    table.add<VELOCITY>(e, new Vector2(-0.5, -0.5));
    table.add<GFX>(e, new Gfx(bn::sprite_items::squares.create_sprite(0, 0)));
    table.subscribe(e); // REQUIRED to subscribe the entitiy to the relevant systems
    
    while (true)
    {
        // update all the systems previously added to this table
        table.update();

        // when A is pressed, run a query that finds all entities moving 
        // towards the right, and reverse their x direction
        if (bn::keypad::a_pressed())
        {
            ecsa::Vector<ecsa::Entity, 2> ids = table.query<2>(&find_entities_moving_right);
            for (ecsa::Entity e : ids)
            {
                Vector2 & vel = table.get<Vector2, VELOCITY>(e);
                vel.x *= -1;
            }       
        }

        bn::core::update();
    }

}