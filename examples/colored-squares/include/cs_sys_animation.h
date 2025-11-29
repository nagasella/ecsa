#ifndef CS_SYS_ANIMATION_H
#define CS_SYS_ANIMATION_H

#include "cs_definitions.h"

namespace cs
{
    /**
     * @brief This system takes care of udpating
     * the animation for entities with ANIM_SET component.
     * 
     */
    class SysAnimation : public ecsa::System<64>
    {
        Table & table;

        public:

        SysAnimation(Table& t);
        bool select(Entity e) override;
        void init() override;
        void update() override;
    };
}

#endif