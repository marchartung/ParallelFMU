/*
 * matrix.hpp
 *
 *  Created on: 02.10.2015
 *      Author: hartung
 */

#ifndef INCLUDE_UTIL_MATRIX_HPP_
#define INCLUDE_UTIL_MATRIX_HPP_

#include "util/Vector.hpp"

namespace Util
{
    template<typename value_type>
    class matrix
    {
     public:

        matrix()
                : dim1(0),
                  dim2(0),
                  mat(vector<value_type>(0))
        {
        }

        matrix(size_type dim1, size_type dim2)
                : dim1(dim1),
                  dim2(dim2),
                  mat(vector<value_type>(dim1 * dim2))
        {
        }

        virtual ~matrix()
        {
        }

        vector<value_type> operator[](size_type index)
        {
            return mat.getSub(index * dim1, index * dim1 + dim2);
        }

        explicit operator value_type*() const
        {
            return mat.data();
        }

        value_type * data()
        {
            return mat.data();
        }

        pair<size_type,size_type> size() const
        {
            return pair<size_type,size_type>(dim1,dim2);
        }

        matrix<value_type> & operator*=(value_type sk)
        {
            mat *= sk;
            return *this;
        }

     private:
        size_type dim1, dim2;
        vector<value_type> mat;

    };

} /* namespace Util */

#endif /* INCLUDE_UTIL_MATRIX_HPP_ */
