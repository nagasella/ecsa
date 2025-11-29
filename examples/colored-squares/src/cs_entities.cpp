#include "cs_entities.h"

#include "bn_sprite_items_squares.h"


void cs::entities::red_square(Table& table)
{
    Entity e = table.create();
    table.add<Ids::POSITION>(e, new Vector2(0, 0));
    table.add<Ids::VELOCITY>(e, new Vector2(0.5, 0.5));
    table.add<Ids::COLOR>(e, new Color(Colors::RED));
    table.add<Ids::GFX>(e, new Gfx(bn::sprite_items::squares.create_sprite(0, 0)));
    table.subscribe(e);
}

void cs::entities::blue_square(Table& table)
{
    Entity e = table.create();
    table.add<Ids::POSITION>(e, new Vector2(0, 0));
    table.add<Ids::VELOCITY>(e, new Vector2(-0.5, 0.5));
    table.add<Ids::COLOR>(e, new Color(Colors::BLUE));
    table.add<Ids::TRANSFORM>(e, new Transform(0, 1));
    table.add<Ids::GFX>(e, new Gfx(bn::sprite_items::squares.create_sprite(0, 0)));
    table.subscribe(e);

    table.get<Gfx, Ids::GFX>(e).sprite.value().set_tiles(bn::sprite_items::squares.tiles_item(), 1);
}

void cs::entities::yellow_square(Table& table)
{
    Entity e = table.create();
    table.add<Ids::POSITION>(e, new Vector2(0, 0));
    table.add<Ids::VELOCITY>(e, new Vector2(-0.5, -0.5));
    table.add<Ids::COLOR>(e, new Color(Colors::YELLOW));
    table.add<Ids::TRANSFORM>(e, new Transform(0, 1));
    table.add<Ids::GFX>(e, new Gfx(bn::sprite_items::squares.create_sprite(0, 0)));
    table.subscribe(e);

    table.get<Gfx, Ids::GFX>(e).sprite.value().set_tiles(bn::sprite_items::squares.tiles_item(), 2);
}

void cs::entities::flashing_square(Table& table)
{
    Entity e = table.create();
    table.add<Ids::POSITION>(e, new Vector2(0, 0));
    table.add<Ids::VELOCITY>(e, new Vector2(0.5, -0.5));
    table.add<Ids::COLOR>(e, new Color(Colors::FLASHING));
    table.add<Ids::ANIMATION>(e, new Animation(0, 2));
    table.add<Ids::GFX>(e, new Gfx(bn::sprite_items::squares.create_sprite(0, 0)));
    table.subscribe(e);
}