/**********************************************************
 *
 * Copyright (c) 2015 ZIH, TU Dresden
 * Olaf Krzikalla, olaf.krzikalla@tu-dresden.de
 * Marc Hartung, marc.hartung@tu-dresden.de
 *
 **********************************************************/

#ifndef INCLUDE_UTIL_VECTOR_HPP_
#define INCLUDE_UTIL_VECTOR_HPP_

#include <algorithm>

namespace Util
{
    template<typename value_type>
    class vector
    {
     public:
        typedef unsigned size_type;

        vector()
                : _num(0),
                  _array(NULL),
                  _ref(true)
        {
        }

        vector(size_type size)
                : _num(size), _ref(false)
        {
            init_alloc(size);
        }

        vector(size_type size, value_type initial)
                : _num(size), _ref(false)
        {
            init_alloc(size);
            init_assign(initial);
        }

        vector(const vector<value_type> & in)
                : _num(in._num),
                  _ref(false)
        {
            init_alloc(_num);
            *this = in;
        }

        ~vector()
        {
            if (!_ref)
                delete[] _array;
        }

        vector<value_type> getSub(size_type index_s, size_type index_e)
        {
            return vector<value_type>(*this, index_s, index_e);
        }

        value_type * data()
        {
            return _array;
        }

        void resize(size_type n)
        {
            if(_ref)
                throw std::runtime_error("Util::vector: cannot resize a reference of a vector");
            value_type * newArray = new value_type[n];
            std::copy(_array,_array+_num,newArray);
            delete[] _array;
            _array = newArray;
            _num = n;
        }

        value_type & operator[](size_type index)
        {
            return _array[index];
        }

        value_type operator[](size_type index) const
        {
            return _array[index];
        }

        void swap(vector<value_type> & in)
        {
            value_type * tmp = in._array;
            in._array = _array;
            _array = tmp;
        }

        size_type size() const
        {
            return _num;
        }

        vector<value_type> & operator=(const vector<value_type> & r)  //TODO what happens if ref is assigned?
        {
            if ((size() != r.size() && !r._ref))
            {
                if (size() != 0)
                    delete[] _array;
                init_alloc(r.size());
            }

            if(r._ref)
            {
                if(!_ref)
                    delete[] _array;
                _array = r._array;
                _num = r._num;
            }
            else
                std::copy(r._array, r._array+r.size(), _array);

            _ref = r._ref;
            return *this;
        }

        explicit operator value_type*()
        {
            return data();
        }

        vector<value_type> & operator=(value_type sk)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] = sk;
            return *this;
        }

        vector<value_type> & operator*=(vector<value_type> const & r)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] *= r._array[i];
            return *this;
        }

        vector<value_type> & operator/=(vector<value_type> const & r)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] /= r._array[i];
            return *this;
        }

        vector<value_type> & operator+=(vector<value_type> const & r)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] += r._array[i];
            return *this;
        }

        vector<value_type> & operator-=(vector<value_type> const & r)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] -= r._array[i];
            return *this;
        }

        vector<value_type> & operator*=(value_type sk)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] *= sk;
            return *this;
        }

        vector<value_type> & operator/=(value_type sk)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] /= sk;
            return *this;
        }

        vector<value_type> & operator+=(value_type sk)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] += sk;
            return *this;
        }

        vector<value_type> & operator-=(value_type sk)
        {
            for (size_type i = 0; i < _num; ++i)
                _array[i] -= sk;
            return *this;
        }

        template<typename T>
        friend T min(vector<T> const & in);
        template<typename T>
        friend T max(vector<T> const & in);
        template<typename T>
        friend vector<T> abs(vector<T> const & in);

        template<typename T>
        friend vector<T> operator*(vector<T> const & l, vector<T> const & r);
        template<typename T>
        friend vector<T> operator/(vector<T> const & l, vector<T> const & r);
        template<typename T>
        friend vector<T> operator+(vector<T> const & l, vector<T> const & r);
        template<typename T>
        friend vector<T> operator-(vector<T> const & l, vector<T> const & r);

        template<typename T>
        friend vector<T> operator*(T sk, vector<T> const & r);
        template<typename T>
        friend vector<T> operator/(T sk, vector<T> const & r);
        template<typename T>
        friend vector<T> operator+(T sk, vector<T> const & r);
        template<typename T>
        friend vector<T> operator-(T sk, vector<T> const & r);

        template<typename T>
        friend vector<T> operator*(vector<T> const & r, T sk);
        template<typename T>
        friend vector<T> operator/(vector<T> const & r, T sk);
        template<typename T>
        friend vector<T> operator+(vector<T> const & r, T sk);
        template<typename T>
        friend vector<T> operator-(vector<T> const & r, T sk);

        template<typename T>
        friend std::ostream & operator<<(std::ostream & os, vector<T> const & in);

     private:
        size_type _num;
        value_type * _array;
        bool_type _ref;

        vector(vector<value_type> & in, size_type index_s, size_type index_e)
        {
            _num = index_e - index_s;
            _array = &in._array[index_s];
            _ref = true;
        }

        void init_alloc(size_type size)
        {
            _num = size;
            _array = new value_type[_num];
            _ref = false;
        }

        void init_assign(value_type val)
        {
            *this = val;
        }
    };

    template<typename T>
    T max(vector<T> const & in)
    {
        T res = std::numeric_limits<T>::min();
        for (typename vector<T>::size_type i = 0; i < in._num; ++i)
            if (res < in[i])
                res = in[i];
        return res;
    }

    template<typename T>
    T min(vector<T> const & in)
    {
        T res = std::numeric_limits<T>::max();
        for (typename vector<T>::size_type i = 0; i < in._num; ++i)
            if (res > in[i])
                res = in[i];
        return res;
    }

    template<typename T>
    vector<T> abs(vector<T> const & in)
    {
        vector<T> res(in);
        for (typename vector<T>::size_type i = 0; i < in._num; ++i)
            res[i] = std::abs(res[i]);
        return res;
    }

    template<typename value_type>
    vector<value_type> operator*(vector<value_type> const & l, vector<value_type> const & r)
    {
        vector<value_type> res(l);
        res *= r;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator/(vector<value_type> const & l, vector<value_type> const & r)
    {
        vector<value_type> res(l);
        res /= r;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator+(vector<value_type> const & l, vector<value_type> const & r)
    {
        vector<value_type> res(l);
        for (typename vector<value_type>::size_type i = 0; i < res.size(); ++i)
            res += r;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator-(vector<value_type> const & l, vector<value_type> const & r)
    {
        vector<value_type> res(l);
        res -= r;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator*(value_type sk, vector<value_type> const & r)
    {
        vector<value_type> res(r);
        res *= sk;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator/(value_type sk, vector<value_type> const & r)
    {
        vector<value_type> res(r);
        res /= sk;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator+(value_type sk, vector<value_type> const & r)
    {
        vector<value_type> res(r);
        res += sk;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator-(value_type sk, vector<value_type> const & r)
    {
        vector<value_type> res(r);
        res -= sk;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator*(vector<value_type> const & r, value_type sk)
    {
        vector<value_type> res(r);
        res *= sk;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator/(vector<value_type> const & r, value_type sk)
    {
        vector<value_type> res(r);
        res /= sk;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator+(vector<value_type> const & r, value_type sk)
    {
        vector<value_type> res(r);
        res += sk;
        return res;
    }

    template<typename value_type>
    vector<value_type> operator-(vector<value_type> const & r, value_type sk)
    {
        vector<value_type> res(r);
        res -= sk;
        return res;
    }

    template<typename T>
    std::ostream & operator<<(std::ostream & os, vector<T> const & in)
    {
        for (typename vector<T>::size_type i = 0; i < in.size() - 1; ++i)
            os << in[i] << ", ";
        os << in[in.size() - 1] << "\n";
        return os;
    }

} /* namespace Util */

#endif /* INCLUDE_UTIL_VECTOR_HPP_ */
