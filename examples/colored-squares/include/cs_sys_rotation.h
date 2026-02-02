#ifndef CS_SYS_ROTATION_H
#define CS_SYS_ROTATION_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This system takes care of handling the
     * sprite rotation for entity with a TRANSFORM component.
     * 
     */
    class SysRotation : public System<96>
    {
        Table & table;
        
        public:

        SysRotation(Table& t);
        bool select(Entity e) override;
        void init() override;
        void update() override;
    };
}

#endif