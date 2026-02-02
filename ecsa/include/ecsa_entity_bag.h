#ifndef ECSA_VECTOR_H
#define ECSA_VECTOR_H

#include <cassert>

#include "ecsa.h"


namespace ecsa
{
    template<int MaxSize>
    class EntityBag
    {
        /**
         * @brief Actual array of entity IDs.
         * 
         */
        Entity _ids [ MaxSize == 0 ? 1 : MaxSize ];


        /**
         * @brief Current bag size.
         * 
         */
        int _size;


        public:


        /**
         * @brief Constructor.
         * 
         */
        EntityBag()
        {
            _size = 0;
        }


        /**
         * @brief Tells if the bag is full.
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool full()
        {
            return _size == MaxSize;
        }


        /**
         * @brief Tells if the bag is empty.
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool empty()
        {
            return _size == 0;
        }


        /**
         * @brief Add an entity ID to the back of the bag.
         * 
         */
        void push_back(Entity value)
        {
            assert(!full() && "ECSA ERROR: entity bag is full!");
            _ids[_size] = value;
            _size++;
        }


        /**
         * @brief Erase an entity ID from the bag at a certain index.
         * 
         * @param index The index of the entity ID to erase.
         */
        void erase(int i)
        {
            assert(!empty() && "ECSA ERROR: index is larger than current vector size!");
            _ids[i] = _ids[_size - 1];
            _size--;
        }


        /**
         * @brief Clears the bag from all entity IDs.
         * 
         */
        void clear()
        {
            _size = 0;
        }


        /**
         * @brief Get the first entity ID in the bag.
         * 
         * @return Type& 
         */
        [[nodiscard]] Entity front()
        {
            assert(!empty() && "ECSA ERROR: vector is empty!");
            return _ids[0];
        }


        /**
         * @brief Get the last entity ID in the bag.
         * 
         * @return Type& 
         */
        [[nodiscard]] Entity back()
        {
            assert(!empty() && "ECSA ERROR: vector is empty!");
            return _ids[_size - 1];
        }


        /**
         * @brief Beginning of bag (iterator).
         * 
         * @return Type* 
         */
        [[nodiscard]] Entity * begin()
        {
            return _ids;
        }


        /**
         * @brief End of bag (iterator).
         * 
         * @return Type* 
         */
        [[nodiscard]] Entity * end()
        {
            return _ids + _size;
        }


        /**
         * @brief Tells the current number of entity IDs in the bag.
         * 
         * @return int 
         */
        [[nodiscard]] int size()
        {
            return _size;
        }


        /**
         * @brief Returns a reference to an entiy ID of the bag at a certain index.
         * 
         * @param index
         * @return Type& 
         */
        [[nodiscard]] Entity & operator[](int i)
        {
            assert(i < _size && "ECSA ERROR: index of EntityBag out of range!");
            return _ids[i];
        }

    };
}


#endif