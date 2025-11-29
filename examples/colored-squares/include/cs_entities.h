#ifndef CS_ENTITIES_H
#define CS_ENTITIES_H

#include "cs_definitions.h"


/**
 * @brief This namespace contains functions that generate entities
 * of different type.
 * 
 */
namespace cs::entities
{
    void red_square(Table& table);
    void blue_square(Table& table);
    void yellow_square(Table& table);
    void flashing_square(Table& table);
}

#endif