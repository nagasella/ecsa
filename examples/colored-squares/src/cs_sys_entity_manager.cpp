#include "cs_sys_entity_manager.h"
#include "cs_entities.h"
#include "cs_queries.h"

#include "bn_keypad.h"
#include "bn_sprite_items_squares.h"

#include "bn_log.h"

cs::SysEntityManager::SysEntityManager(Table& t) :
    System<128, 0>(),
    table(t)
{
    
}

void cs::SysEntityManager::init()
{

}

void cs::SysEntityManager::update()
{
    // generate four new squares
    if (bn::keypad::a_pressed())
    {
        entities::red_square(table);
        entities::blue_square(table);
        entities::yellow_square(table);
        entities::flashing_square(table);
    }
    // delete all the red squares
    if (bn::keypad::up_pressed())
    {
        EntityBag<96> ids = table.query<96>(&queries::find_red_squares);
        for (Entity e : ids)
            table.destroy(e);
    }
    // delete all the squares
    else if (bn::keypad::down_pressed())
    {
        table.clear(); // clear the table (destroy all entities)
    }
    // delete all the rotating squares
    else if (bn::keypad::left_pressed())
    {
        EntityBag<96> ids = table.query<96, Ids::SYSROTATION>();
        for (Entity e : ids)
            table.destroy(e);
    }
    // freeze all the yellow squares with -64 < x < 64
    else if (bn::keypad::right_pressed())
    {
        // define the x boundary and run the query
        queries::XBoundary b = { .min = -64, .max = 64 };
        EntityBag<128> yellow_squares = table.query<128, queries::XBoundary>(&queries::find_yellow_squares_within, b);
        
        // stop movement for all entities returned by the query
        for (Entity e : yellow_squares)
        {
            Vector2 & vel = table.get<Vector2, Ids::VELOCITY>(e);
            vel.x = 0;
            vel.y = 0;
        }
    }
}