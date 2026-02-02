#ifndef CS_SYS_VISIBILITY_H
#define CS_SYS_VISIBILITY_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This updater takes care of toggling
     * the visibility of all entities with a GFX component.
     * 
     */
    class SysVisibility : public System<32>
    {
        Table& table;

        public:

        SysVisibility(Table& t);
        bool select(Entity e) override;
        void init() override;
        void update() override;
    };
}

#endif