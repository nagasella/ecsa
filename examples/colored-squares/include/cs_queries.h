#ifndef CS_QUERIES_H
#define CS_QUERIES_H

#include "cs_definitions.h"


/**
 * @brief Queries definition.
 * 
 */
namespace cs::queries
{
    bool find_red_squares(Table& table, Entity e);

    struct XBoundary
    {
        bn::fixed min, max;
    };
    bool find_yellow_squares_within(Table& table, Entity e, XBoundary& b);
    
}

#endif