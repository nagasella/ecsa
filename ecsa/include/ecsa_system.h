#ifndef ECSA_STATIC_QUERY_H
#define ECSA_STATIC_QUERY_H

#include "ecsa.h"

namespace ecsa
{
    template<int Size>
    class System : public ISystem
    {

        protected:
        
        /**
         * @brief A vector with the Ids of the subscribed entities.
         * 
         */
        Vector<Entity, Size == 0 ? 1 : Size> _subscribed;

        public:

        /**
         * @brief Subscribe an entity to the query.
         * 
         * @param e The Id of the entity.
         */
        void subscribe(Entity e) override
        {
            _subscribed.push_back(e);
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
            for (int i = 0; i < _subscribed.size(); i++)
            {
                if (_subscribed[i] == e)
                    return true;
            }
            return false;
        }

        
        /**
         * @brief Returns a vector with the Ids of the subscribed entities.
         * 
         * @return Vector<Entity, Size> 
         */
        Vector<Entity, Size> subscribed()
        {
            return _subscribed;
        }

    };
}

#endif