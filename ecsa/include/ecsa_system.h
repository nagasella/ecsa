#ifndef ECSA_STATIC_QUERY_H
#define ECSA_STATIC_QUERY_H

#include "ecsa.h"

namespace ecsa
{
    template<int TableEntities, int SystemEntities>
    class System : public ISystem
    {

        protected:

        /**
         * @brief A mask tracking subscribed entities.
         * 
         */
        EntityMask<TableEntities> _mask_subscribed;
        
        /**
         * @brief An entity bag with the Ids of the subscribed entities.
         * 
         */
        EntityBag<SystemEntities> _subscribed;

        public:

        /**
         * @brief Subscribe an entity to the query.
         * 
         * @param e The Id of the entity.
         */
        void subscribe(Entity e) override
        {
            _subscribed.push_back(e);
            _mask_subscribed.add(e);
        }


        /**
         * @brief Unsubscribe an entity from the query.
         * 
         * @param e The Id of the entity.
         */
        void unsubscribe(Entity e) override
        {
            for (int i = 0; i < _subscribed.size(); i++)
            {
                if (_subscribed[i] == e)
                {
                    _subscribed.erase(i);
                    _mask_subscribed.destroy(e);
                    break;
                }
            }
        }


        /**
         * @brief Tells whether an entity is subscribed to the query or not.
         * 
         * @param e The Id of the entity.
         * @return true 
         * @return false 
         */
        bool subscribed(Entity e) override
        {
            return _mask_subscribed.contains(e);
        }

        
        /**
         * @brief Returns an entity bag with the Ids of the subscribed entities.
         * 
         * @return EntityBag<SystemEntities> 
         */
        EntityBag<SystemEntities> subscribed()
        {
            return _subscribed;
        }

        virtual ~System() = default;

    };
}

#endif