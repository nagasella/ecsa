#ifndef ECSA_IQUERY_H
#define ECSA_IQUERY_H

#include "ecsa.h"

namespace ecsa
{
    class ISystem
    {
        public:


        /**
         * @brief `select` caluse of the query. Used to filter entities based on some condition.
         * This clause is ran every time an entity is created in ECSA.
         * 
         * @param e 
         * @return true 
         * @return false 
         */
        virtual bool select(Entity e)
        {
            return false;
        }


        /**
         * @brief Initialize the query object.
         * 
         */
        virtual void init()
        {

        }


        /**
         * @brief Update logic for the query object.
         * 
         */
        virtual void update()
        {

        }
        
        virtual void subscribe(Entity e);
        virtual void unsubscribe(Entity e);
        virtual bool subscribed(Entity e);
    };  
}

#endif