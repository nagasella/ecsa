#include "cs_queries.h"


bool cs::queries::find_red_squares(Table& table, Entity e)
{
    Color & col = table.get<Color, Ids::COLOR>(e);
    return col.color == Colors::RED;
}


bool cs::queries::find_yellow_squares_within(Table& table, Entity e, XBoundary& b)
{
    Color & col = table.get<Color, Ids::COLOR>(e);
    Vector2 & pos = table.get<Vector2, Ids::POSITION>(e);

    if (col.color == Colors::YELLOW && pos.x < b.max && pos.x > b.min)
        return true;

    return false;
}