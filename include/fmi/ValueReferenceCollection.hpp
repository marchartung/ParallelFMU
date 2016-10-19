/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_FMI_FMUVALUEREFERENCECOLLECTION_HPP_
#define INCLUDE_FMI_FMUVALUEREFERENCECOLLECTION_HPP_

#include "fmi/ValueSwitch.hpp"
#include "Stdafx.hpp"

namespace FMI
{

    /**
     * A class to store references (i.e., IDs from XML model description) of FMU values. It is often used
     * together with \ref Data::ValueCollection to keep a mapping between the concrete values and
     * the corresponding references.
     * @note Only double, int, bool_type and string_type are valid types (T) for the template functions.
     */
    class ValueReferenceCollection : protected ValueSwitch
    {
     public:
        /**
         * Create a collection of reference values either from the given arguments or as empty collection.
         * @param realVars References to the variables of type real.
         * @param intVars References to the variables of type int.
         * @param bool_typeVars References to the variables of type bool_type.
         * @param stringVars References to the variables of type string.
         */
        ValueReferenceCollection(const vector<size_type> & realVars = vector<size_type>(),
                                 const vector<size_type> & intVars = vector<size_type>(),
                                 const vector<size_type> & boolVars = vector<size_type>(),
                                 const vector<size_type> & stringVars = vector<size_type>());

        ValueReferenceCollection(const size_type & numReals, const size_type & numInts, const size_type & numBools,
                                 const size_type & numStrings);

        ValueReferenceCollection(const ValueReferenceCollection & in);

        /**
         * Destroy reference collection.
         */
        virtual ~ValueReferenceCollection() = default;

        /**
         * Set reference values.
         * @param values
         */
        template<typename T>
        void setValues(const vector<size_type> & values)
        {
            _values[dataIndex<T>()] = values;
        }

        /**
         * Return constant reference to the reference values.
         * @return Reference to the internal storage vector.
         */
        template<typename T>
        const vector<size_type> & getValues() const
        {
            return _values[dataIndex<T>()];
        }

        template<typename T>
        vector<size_type> & getValues()
        {
            return _values[dataIndex<T>()];
        }

        /**
         * @return Return the number of reference values stored in this collection.
         */
        size_type size() const;

        ValueReferenceCollection & operator=(const ValueReferenceCollection & in);

        friend std::ostream & operator<<(std::ostream & s, const ValueReferenceCollection & vrc);

     protected:
        /// Storage location for the reference values. This are integral type.
        vector<vector<size_type>> _values;
    };

} /* namespace FMI */

#endif /* INCLUDE_FMI_FMUVALUEREFERENCECOLLECTION_HPP_ */
/**
 * @}
 */
