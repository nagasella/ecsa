#ifndef ECSA_H
#define ECSA_H


namespace ecsa
{

    /**
     * @brief `Entity` alias, used for entity Ids.
     * 
     */
    using Entity = unsigned int;


    /**  
     * @brief `Id` alias, used for components and systems Ids.
     * 
     */
    using Id = unsigned int;


    /**
     * @brief Base struct for every component.
     * 
     */
    struct Component
    {
        virtual ~Component()
        {

        }
    };


    /**
     * @brief An array-like data structure.
     * 
     * @tparam Type The data type of the array elements.
     * @tparam Size The capacity of the array.
     */
    template<typename Type, int Size>
    class Array;


    /**
     * @brief A vector-like data structure.
     * Its maximum capacity is defined at compile time.
     * It mostly relies on integer indexes rather than iterators,
     * however it allows for range-based for loops.
     * 
     * @tparam Type The data type of the vector elements.
     * @tparam MaxSize The maximum number of elements the vector can contain.
     */
    template<typename Type, int MaxSize>
    class Vector;


    /**
     * @brief A bitmask-like data structure used for keeping track of entities.
     * 
     * @tparam Entities 
     */
    template<int Entities>
    class EntityMask;


    /**
     * @brief The main data structure of ECSA, allows to organize entities (game objects) and their components.
     * Each table can also have some systems associated, which are used to process the components of each entity.
     * A table can also be queried to obtain the Ids of specific entities.
     * 
     * @tparam Entities The maximum number of entities that can be allocated.
     * @tparam Components The maximum number of components each entity can have.
     * @tparam Systems The maximum number of systems that can be associated to the table.
     */
    template<int Entities, int Components, int Systems>
    class EntityTable;


    /**
     * @brief Base class for System type.
     * 
     */
    class ISystem;


    /**
     * @brief A system processes entities that satsify a certain condition, like owning certain components.
     * 
     * @tparam Size The maximum number of entities the system is expected to process.
     */
    template<int Size>
    class System;

}

#include "ecsa_array.h"
#include "ecsa_vector.h"
#include "ecsa_entity_mask.h"
#include "ecsa_isystem.h"
#include "ecsa_system.h"
#include "ecsa_entity_table.h"

#endif