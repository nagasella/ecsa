#ifndef CS_SYS_SCALING_H
#define CS_SYS_SCALING_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This udpater takes care of handling the scale
     * of the sprites of entities with the SCALE compoennt.
     * 
     */
    class SysScaling : public ecsa::System<64>
    {
        Table& table;
        
        public:

        SysScaling(Table& t);
        bool select(Entity e) override;
        void init() override;
        void update() override;
    };
}

#endif