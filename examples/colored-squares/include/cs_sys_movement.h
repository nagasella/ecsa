#ifndef CS_SYS_MOVEMENT_H
#define CS_SYS_MOVEMENT_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This system takes care of the movement
     * of entities on screen.
     * 
     */
    class SysMovement : public System<128>
    {
        Table& table;

        public:

        SysMovement(Table& t);
        bool select(Entity e) override;
        void init() override;
        void update() override;
    };
}

#endif