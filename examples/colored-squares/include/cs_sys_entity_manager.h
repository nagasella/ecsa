#ifndef CS_SYS_ENTITY_MANAGER_H
#define CS_SYS_ENTITY_MANAGER_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This system takes care of adding/removing/modifying
     * the entities on screen based on keypad input.
     * 
     */
    class SysEntityManager : public ecsa::System<0>
    {
        Table& table;
        
        public:

        SysEntityManager(Table& t);
        void init() override;
        void update() override;
    };
}

#endif