#ifndef ECSA_ARRAY_H
#define ECSA_ARRAY_H

#include <cassert>
#include "ecsa.h"


namespace ecsa
{
    /**
     * @brief Base class for Array type.
     * 
     */
    class IArray
    {

    };
    
    template<typename Type, int Size>
    class Array : public IArray
    {
        /**
         * @brief Actual Array of data.
         * 
         */
        Type _data [ Size == 0 ? 1 : Size ];


        public:


        /**
         * @brief Constructor.
         * 
         */
        Array()
        {

        }


        /**
         * @brief Constructor. Allows to initialize the array elements to a specific value.
         * 
         * @param value The value to assign for initialization.
         */
        Array(Type value)
        {
            for (int i = 0; i < Size; i++)
                _data[i] = value;
        }


        /**
         * @brief Tells the capacity of the array.
         * 
         * @return int 
         */
        [[nodiscard]] int size()
        {
            return Size;
        }


        /**
         * @brief Returns a reference to the element at requested index.
         * 
         * @param index
         * @return Type& 
         */
        [[nodiscard]] Type & operator[](int i)
        {
            assert(i < Size && "ECSA ERROR: array index out of range!");
            return _data[i];
        }

    };
}


#endif