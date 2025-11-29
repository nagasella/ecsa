#ifndef ECSA_ENTITY_MASK_H
#define ECSA_ENTITY_MASK_H

#include <cassert>
#include "ecsa.h"
#include "bn_log.h"

namespace ecsa
{
    template<int Entities>
    class EntityMask
    {
        /**
         * @brief The entity mask.
         * 
         */
        int _mask [ (Entities - 1) / 32 + 1 ];
        

        public:


        /**
         * @brief Constructor.
         * 
         */
        EntityMask()
        {
            for (int i = 0; i < ((Entities - 1) >> 5) + 1; i++)
                _mask[i] = 0;
        }


        /**
         * @brief Creates in an entity in the first free slot.
         * 
         */
        Entity create()
        {
            for (int j = 0; j < Entities / 32 + 1; j++)
            {
                for (Entity i = 0; i < 32; i++)
                {
                    if (((_mask[j] >> i) & 1) == 0)
                    {
                        add(j * 32 + i);
                        return j * 32 + i;
                    }
                }
            }
            return 0xffff;
        }


        /**
         * @brief Marks a certain entity as present.
         * 
         * @param e The ID of the entity.
         */
        void add(Entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            _mask[ e >> 5 ] |= (1 << (e & 31));
        }


        /**
         * @brief Marks a certain entity as absent.
         * 
         * @param e The ID of the entity.
         */
        void destroy(Entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            _mask[e >> 5] &= ~(1 << (e & 31));
        }


        /**
         * @brief Tells if the entity is present in the mask.
         * 
         * @param e The ID of the entity.
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool contains(Entity e)
        {
            assert(e < Entities && "ESA ERROR: entity index is out of range!");
            return ( (_mask[e >> 5] >> (e & 31)) & 1 ) == 1;
        }

    };
}

#endif