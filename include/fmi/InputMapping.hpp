/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_FMI_INPUTMAPPING_HPP_
#define INCLUDE_FMI_INPUTMAPPING_HPP_

#include "fmi/ValueCollection.hpp"
#include "fmi/ValueSwitch.hpp"
#include "Stdafx.hpp"

namespace FMI
{
    /**
     * Class to connect the output variables of an FMU with the input variables of another FMU. The mappings are
     * separated by their type and defined as tuple '(index of output variable, index of input variable)'.
     * @note Only double, int, bool_type and string_type are valid types (T) for the template functions.
     *
     *
     * example:
     *
     *      FMU1             ->             FMU2
     *      r11                              r21
     *      r12  (pack)      r11  (unpack)   r22 = r11
     *      r13  ----->      i11   ----->    i21 = i11
     *      i11              b11             b21
     *      b11                              b22 = b11
     */
    class InputMapping : protected ValueSwitch
    {
     public:
        /**
         * Create a new connection mapping with the given mappings for the concrete types.
         * @param realVarsMapping Mapping tuples for real variables '(index of output real variable, index of input real variable)'.
         * @param intVarsMapping Mapping tuples for integer variables '(index of output integer variable, index of input integer variable)'.
         * @param bool_typeVarsMapping Mapping tuples for bool_typeean variables '(index of output bool_typeean variable, index of input bool_typeean variable)'.
         * @param stringVarsMapping Mapping tuples for string_type variables '(index of output string_type variable, index of input string_type variable)'.
         */
        InputMapping(vector<tuple<size_type, size_type> > realVarsMapping = vector<tuple<size_type, size_type> >(), vector<tuple<size_type, size_type> > intVarsMapping = vector<tuple<size_type, size_type> >(), vector<tuple<size_type, size_type> > bool_typeVarsMapping = vector<tuple<size_type, size_type> >(), vector<tuple<size_type, size_type> > stringVarsMapping =
                             vector<tuple<size_type, size_type> >());


        /**
         * Delete the mapping object including all stored mapping tuples.
         */
        virtual ~InputMapping() = default;

        template<typename T>
        void push_back(const tuple<size_type, size_type> & in)
        {
            _connectedVars[dataIndex<T>()].push_back(in);
        }

        /**
         * Set the mapping tuples for the given type T.
         * @note The values are copied.
         * @param values The new mapping values.
         */
        template<typename T>
        void setValues(const vector<tuple<size_type, size_type> >& values)
        {
        	T v;
            _connectedVars[dataIndex<T>()] = values;
        }

        /**
         * Get the stored mapping tuples for the given type T.
         * @return A reference to the internal mapping.
         */
        template<typename T>
        const vector<tuple<size_type, size_type> >& getValues() const
        {
            return _connectedVars[dataIndex<T>()];
        }

        template<typename T>
        size_type size() const
        {
            return _connectedVars[dataIndex<T>()].size();
        }

        /**
         * Takes a VC and reduces it according to the mapping stored. Does the opersite of unpack()
         * @return ValueCollection containing only the indexes stored in the InputMapping class
         */
        ValueCollection pack(const ValueCollection & in) const;

        void unpack(ValueCollection & out, const ValueCollection & in) const;

        size_type size() const
        {
            size_type res = 0;
            for(size_t i=0;i<_connectedVars.size();++i)
                res += _connectedVars[i].size();
            return res;
        }

        ValueCollection getPackedValueCollection() const
        {
            return ValueCollection(getValues<real_type>().size(),getValues<int_type>().size(),getValues<bool_type>().size(),getValues<string_type>().size());
        }


        friend std::ostream & operator<<(std::ostream & in, const InputMapping & im);

        template<typename T>
        const size_type * data() const
        {
            return reinterpret_cast<const size_type *>(_connectedVars[dataIndex<T>()].data());
        }

        template<typename T>
        size_type * data()
        {
            return reinterpret_cast<size_type *>(_connectedVars[dataIndex<T>()].data());
        }

     private:
        /// Output variables mapped to input variables
        vector<vector<tuple<size_type, size_type> > > _connectedVars;

        template<typename T>
        vector<T> pack(const ValueCollection & in) const
        {
            vector<T> res(_connectedVars[dataIndex<T>()].size());
            for (size_type i = 0; i < res.size(); ++i)
            {
                size_type cId = get<0>((_connectedVars[dataIndex<T>()][i]));
                res[i] = in.getValues<T>()[cId];
            }
            return res;
        }

        template<typename T>
        void unpack(ValueCollection & out, const ValueCollection & in) const
        {
            for (size_type i = 0; i < _connectedVars[dataIndex<T>()].size(); ++i)
            {
                out.getValues<T>()[get<1>((_connectedVars[dataIndex<T>()][i]))] = in.getValues<T>()[i];
            }
        }
    };

} /* namespace FMI */

#endif /* INCLUDE_FMI_INPUTMAPPING_HPP_ */
/**
 * @}
 */
