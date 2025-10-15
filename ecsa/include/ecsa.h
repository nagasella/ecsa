#ifndef ECSA_H
#define ECSA_H


namespace ecsa
{

    /**
     * @brief `Entity` alias, used for entity IDs.
     * 
     */
    using Entity = unsigned short;


    /**
     * @brief `Id` alias, used for components and queries IDs.
     * 
     */
    using Id = unsigned short;


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
     * @brief A minimal, custom implementation of `std::array`.
     * 
     * @tparam Type the data type of the array elements.
     * @tparam Size The size of the array.
     */
    template<typename Type, int Size>
    class Array;


    /**
     * @brief A minimal, custom implementation of `std::vector`.
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
     * @brief A bitmask used for registering entities.
     * 
     * @tparam Entities 
     */
    template<int Entities>
    class EntityMask;


    /**
     * @brief The main data structure of ECSA, allows to organize entities (game objects) and their components.
     * Each table can also have some queries associated, which are used to process the components of each entity.
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
     * @brief A system processes entities that own certain compoennts.
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