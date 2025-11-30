#ifndef ECSA_ENTITY_TABLE_H
#define ECSA_ENTITY_TABLE_H

#include "ecsa.h"
#include "bn_log.h"

namespace ecsa
{
    template<int Entities, int Components, int Systems>
    class EntityTable
    {
        EntityMask<Entities> _entities;
        Array<Array<Component *, Entities>, Components> * _table;
        Array<ISystem *, Systems> * _systems;

        public:

        
        /**
         * @brief Constructor.
         * 
         */
        EntityTable()
        {
            _table = new Array<Array<Component *, Entities>, Components>(nullptr);
            _systems = new Array<ISystem *, Systems>(nullptr);
        }


        /**
         * @brief Create a new entity and return its Id.
         * 
         * @return Entity 
         */
        Entity create()
        {
            return _entities.create();
        }


        /**
         * @brief Subscribe an entity to all the relevant systems in the table.
         * 
         * @param e The Id of the entity to subscribe.
         */
        void subscribe(Entity e)
        {
            for (int i = 0; i < Systems; i++)
            {
                ISystem * s = (*_systems)[i];
                if (s != nullptr && s->select(e))
                    s->subscribe(e);
            }
        }

        
        /**
         * @brief Remove an entity from the table, 
         * and unsubscribe it from all the relevant systems.
         * 
         * @param e The Id of the entity.
         */
        void destroy(Entity e)
        {
            for (int c = 0; c < Components; c++)
            {
                delete (*_table)[c][e];
                (*_table)[c][e] = nullptr;
            }
            for (int i = 0; i < Systems; i++)
            {
                ISystem * s = (*_systems)[i];
                if (s != nullptr && s->subscribed(e))
                    s->unsubscribe(e);
            }
            _entities.destroy(e);
        }


        /**
         * @brief Destroy all the entities in the table.
         * 
         */
        void clear()
        {
            for (int e = 0; e < Entities; e++)
            {
                if (_entities.contains(e))
                    destroy(e);
            }
        }


        /**
         * @brief Tells if the table contains a certain entity.
         * 
         * @param e The Id of the entity.
         * @return true 
         * @return false 
         */
        bool contains(Entity e)
        {
            return _entities.contains(e);
        }


        /**
         * @brief Add a component to an entity.
         * 
         * @tparam Id The Id of the component.
         * @param e The Id of the entity.
         * @param c A pointer to the component object, created with `new`.
         */
        template<int Id>
        void add(Entity e, Component * c)
        {
            (*_table)[Id][e] = c;
        }


        /**
         * @brief Get a reference to the component of an entity.
         * 
         * @tparam Type The type of the component.
         * @tparam Id The Id of the component.
         * @param e The Id of the entity.
         * @return Type& 
         */
        template<typename Type, int Id>
        Type & get(Entity e)
        {
            return (Type &) *((*_table)[Id][e]);
        }


        /**
         * @brief Tells if an entity has a certain component.
         * 
         * @tparam Id The Id of the component.
         * @param e The Id of the entity.
         * @return true 
         * @return false 
         */
        template<int Id>
        bool has(Entity e)
        {
            return (*_table)[Id][e] != nullptr;
        }


        /**
         * @brief Add a system to the table.
         * 
         * @tparam Id The Id to assign to the system.
         * @param s A pointer to the system, created with `new`.
         */
        template<int Id>
        void add(ISystem * s)
        {
            s->activate();
            (*_systems)[Id] = s;
        }


        /**
         * @brief Get a system by its Id.
         * 
         * @tparam Id The Id of the system.
         * @return ISystem* 
         */
        template<int Id>
        ISystem * get()
        {
            return (*_systems)[Id];
        }


        /**
         * @brief Activate a system. (Its `update` function will be executed when `EntityTable::update()` is called)
         * 
         * @tparam Id The Id of the system to activate.
         */
        template<int Id>
        void activate()
        {
            ISystem * s = (*_systems)[Id];
            s->activate();
        }


        /**
         * @brief Dectivate a system. (Its `update` function will not be executed when `EntityTable::update()` is called)
         * 
         * @tparam Id The Id of the system to activate.
         */
        template<int Id>
        void deactivate()
        {
            ISystem * s = (*_systems)[Id];
            s->deactivate();
        }


        /**
         * @brief Activate all systems associated to the table. (Their `update` function will be executed when `EntityTable::update()` is called)
         * 
         */
        void activate_all()
        {
            for (int i = 0; i < Systems; i++)
            {
                ISystem * s = (*_systems)[i];
                if (s != nullptr)
                    s->activate();
            }
        }


        /**
         * @brief Deactivate all systems associated to the table. (Their `update` function will not be executed when `EntityTable::update()` is called)
         * 
         */
        void deactivate_all()
        {
            for (int i = 0; i < Systems; i++)
            {
                ISystem * s = (*_systems)[i];
                if (s != nullptr)
                    s->deactivate();
            }
        }


        /**
         * @brief Perform a query that returns the Ids of all the entities
         * subscribed to a certain system.
         * 
         * @tparam Size The maximum number of entities processed by the system.
         * @tparam SystemId The Id of the system.
         * @return Vector<Entity, Size> 
         */
        template<int Size, int SystemId>
        Vector<Entity, Size> query()
        {
            return ((System<Size> *) get<SystemId>())->subscribed();
        }


        /**
         * @brief Perform a query on the whole table, using a `bool` function for filtering.
         * Returns a vector with the Ids of the entities that satisfy the filtering condition.
         * 
         * @tparam Size The expected maximum number of entites the query will find.
         * @param func A pointer to the function used as a fltering condition.
         * @return Vector<Entity, Size> 
         */
        template<int Size>
        Vector<Entity, Size> query(bool (* func) (EntityTable<Entities, Components, Systems> &, Entity))
        {
            Vector<Entity, Size> result;
            for (Entity e = 0; e < Entities; e++)
            {
                if ((*func)(*this, e))
                    result.push_back(e);
            }
            return result;
        }


        /**
         * @brief Perform a query on the whole table, using a `bool` function for filtering.
         * Allows also to pass a parameter of any type for dynamic filtering.
         * Returns a vector with the Ids of the entities that satisfy the filtering condition.
         * 
         * @tparam Size The expected maximum number of entites the query will find.
         * @tparam ParamType The type of the parameter used for filtering.
         * @param func A pointer to the function used as a filtering condition.
         * @param param A refernece to the parameter used for dynamic filtering.
         * @return Vector<Entity, Size> 
         */
        template<int Size, typename ParamType>
        Vector<Entity, Size> query(bool (* func) (EntityTable<Entities, Components, Systems> &, Entity, ParamType &), ParamType & param)
        {
            Vector<Entity, Size> result;
            for (Entity e = 0; e < Entities; e++)
            {
                if ((*func)(*this, e, param))
                    result.push_back(e);
            }
            return result;
        }


        /**
         * @brief Perform a query on the subset of entities processed by a certain system, 
         * using a `bool` function for filtering.
         * Returns a vector with the Ids of the entities that satisfy the filtering condition.
         * 
         * @tparam Size The expected maximum number of entites the query will find.
         * @tparam SystemId The Id of the system.
         * @param func A pointer to the function used as a filtering condition.
         * @return Vector<Entity, Size> 
         */
        template<int Size, int SystemId>
        Vector<Entity, Size> query(bool (* func) (EntityTable<Entities, Components, Systems> &, Entity))
        {
            Vector<Entity, Size> result;
            Vector<Entity, Size> ids = ((System<Size> *) get<SystemId>())->subscribed();
            for (Entity e : ids)
            {
                if ((*func)(*this, e))
                    result.push_back(e);
            }
            return result;
        }

        
        /**
         * @brief Perform a query on the subset of entities processed by a certain system, 
         * using a `bool` function for filtering.
         * Allows also to pass a parameter of any type for dynamic filtering.
         * Returns a vector with the ids of the Ids of the entities that satisfy the filtering condition.
         * 
         * @tparam Size The expected maximum number of entites the query will find.
         * @tparam SystemId The Id of the system.
         * @tparam ParamType The type of the parameter used for filtering.
         * @param func A pointer to the function used as a filtering condition.
         * @param param A refernece to the parameter used for dynamic filtering.
         * @return Vector<Entity, Size> 
         */
        template<int Size, int SystemId, typename ParamType>
        Vector<Entity, Size> query(bool (* func) (EntityTable<Entities, Components, Systems> &, Entity, ParamType &), ParamType & param)
        {
            Vector<Entity, Size> result;
            Vector<Entity, Size> ids = ((System<Size> *) get<SystemId>())->subscribed();
            for (Entity e : ids)
            {
                if ((*func)(*this, e, param))
                    result.push_back(e);
            }
            return result;
        }


        /**
         * @brief Initialize all the systems in the table.
         * 
         */
        void init()
        {
            for (int i = 0; i < Systems; i++)
            {
                ISystem * s = (*_systems)[i];
                if (s != nullptr)
                    s->init();
            }
        }


        /**
         * @brief Update all the (active) systems in the table.
         * 
         */
        void update()
        {
            for (int i = 0; i < Systems; i++)
            {
                ISystem * s = (*_systems)[i];
                if (s != nullptr && s->active())
                    s->update();
            }
        }


        /**
         * @brief Destructor.
         * 
         */
        ~EntityTable()
        {
            for (int c = 0; c < Components; c++)
            {
                for (int e = 0; e < Entities; e++)
                {                    
                    delete (*_table)[c][e];
                    (*_table)[c][e] = nullptr;
                }
            }
            delete _table;
            for (int s = 0; s < Systems; s++)
            {
                delete (*_systems)[s];
                (*_systems)[s] = nullptr;
            }
            delete _systems;
        }


    };
}

#endif