#ifndef ECSA_IQUERY_H
#define ECSA_IQUERY_H

#include "ecsa.h"

namespace ecsa
{
    class ISystem
    {

        /**
         * @brief Tells if the system is corrently being processed.
         * 
         */
        bool _active;


        public:


        /**
         * @brief Tells if the system is currently active.
         * If it is, it `update` function will be processed by ECSA each frame.
         * 
         */
        bool active()
        {
            return _active;
        }


        /**
         * @brief Activate the system.
         * 
         */
        void activate()
        {
            _active = true;
        }


        /**
         * @brief Deactivate the system.
         * 
         */
        void deactivate()
        {
            _active = false;
        }


        /**
         * @brief Used to filter entities based on some condition.
         * This function is ran every time an entity is subscribed to an entity table.
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
         * @brief Initialization logic the system.
         * 
         */
        virtual void init()
        {

        }


        /**
         * @brief Update logic for the system.
         * 
         */
        virtual void update()
        {

        }

        
        virtual void subscribe(Entity e);
        virtual void unsubscribe(Entity e);
        virtual bool subscribed(Entity e);

        virtual ~ISystem() = default;
    };  
}

#endif