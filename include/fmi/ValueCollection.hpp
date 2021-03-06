/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_FMI_VALUECOLLECTION_HPP_
#define INCLUDE_FMI_VALUECOLLECTION_HPP_

#include "Stdafx.hpp"

namespace FMI
{

    /**
     * A class to store concrete values of an FMU. It is often used together with \ref Data::ValueReferenceCollection
     * to keep a mapping between the concrete values and the corresponding references.
     * @note Only value_type, int, bool_type and string_type are valid types (T) for the template functions.
     */
    class ValueCollection
    {
     public:
        /**
         * Create a new and empty value collection for an FMU.
         */
        ValueCollection();

        ValueCollection(const ValueCollection & in);

        /**
         * Create a new and value collection for an FMU with container size.
         */
        ValueCollection(size_type realSize, size_type intSize, size_type bool_typeSize, size_type stringSize);

        /**
         * Create a new ValueCollection filled with args for an FMU.
         */
        ValueCollection(const vector<real_type> & real, const vector<int_type> & integer,
                        const vector<bool_type> & bool_typeean, const vector<string_type> & str);

        /**
         * Delete the value collection including all stored values.
         */
        virtual ~ValueCollection() = default;

        /**
         * Get a reference to the vector of all stored values. This function can be used to modify the values.
         * @return Reference to the internal storage vector.
         */
        template<typename T>
        vector<T> & getValues()
        {
            throw runtime_error("ValueCollection: Type was not collected.");
        }

        template<typename T>
        void getValues(vector<T> & out, const vector<size_type> & references) const
        {
            for (size_type i = 0; i < out.size(); ++i)
                out[i] = getValues<T>()[references[i]];
        }

        /**
         * Get a constant reference to the vector of all stored values.
         * @return Reference to the internal storage vector.
         */
        template<typename T>
        const vector<T> & getValues() const
        {
            throw runtime_error("ValueCollection: Type was not collected.");
        }

        template<typename T>
        void setValues(const vector<T> & out, const vector<size_type> & references)
        {
            for (size_type i = 0; i < out.size(); ++i)
                getValues<T>()[references[i]] = out[i];
        }

        /**
         * Operator that checks for equality of two value collections.
         * @param in Right hand side value collection.
         * @return True, if the all values are equal, false otherwise.
         */
        bool_type operator==(const ValueCollection & in) const
        {
            return (_realValues == in._realValues && _intValues == in._intValues && _boolValues == in._boolValues
                    && _stringValues == in._stringValues);
        }

        /**
         * Overloaded operator that checks for inequality of two value collections.
         * @param in Right hand side value collection.
         * @return True, if the all values are not equal, false if the values are equal.
         */
        bool_type operator!=(const ValueCollection & in) const
        {
            return !(*this == in);
        }

        friend ostream & operator<<(ostream & out, const ValueCollection & in);

        /**
         * Assigns values of one collection to this one, without checking if the container sizes fit
         * @param ValueCollection which values should be assigned
         */
        void assign(const ValueCollection & in);

        /**
         * Overloaded assign operator. Resizes the vectors if necessary .
         */
        ValueCollection & operator=(const ValueCollection & in);

        /**
         * Get the overall size of all stored values.
         * @return The size of real, int, bool_type and string_type values as sum.
         */
        size_t size() const;

        bool_type empty() const;

        template<typename T>
        T * data()
        {
            return getValues<T>().data();
        }

        template<typename T>
        const T * data() const
        {
            return getValues<T>().data();
        }

     private:
        /// Storage of values of type real.
        vector<real_type> _realValues;

        /// Storage of values of type int.
        vector<int_type> _intValues;

        /// Storage of values of type bool_type.
        vector<bool_type> _boolValues;

        /// Storage of values of type string.
        vector<string_type> _stringValues;

        template<typename T>
        void resizeInternal(const ValueCollection & in)
        {
            if (getValues<T>().size() != in.getValues<T>().size())
                getValues<T>().resize(in.getValues<T>().size());
        }

        template<typename T>
        void assignInternal(const ValueCollection & in)
        {
            std::copy(in.getValues<T>().begin(), in.getValues<T>().end(), getValues<T>().begin());
        }

    };

    template<> vector<double>& ValueCollection::getValues();
    template<> vector<int_type>& ValueCollection::getValues();
    template<> vector<bool_type>& ValueCollection::getValues();
    template<> vector<string_type>& ValueCollection::getValues();

    template<> const vector<double>& ValueCollection::getValues() const;
    template<> const vector<int_type>& ValueCollection::getValues() const;
    template<> const vector<bool_type>& ValueCollection::getValues() const;
    template<> const vector<string_type>& ValueCollection::getValues() const;

} /* namespace FMI */
string_type to_string(const FMI::ValueCollection & in);

#endif /* INCLUDE_FMI_VALUECOLLECTION_HPP_ */
/**
 * @}
 */
