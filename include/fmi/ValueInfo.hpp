/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_FMI_FMUVALUEINFO_HPP_
#define INCLUDE_FMI_FMUVALUEINFO_HPP_

#include "fmi/ValueSwitch.hpp"
#include "Stdafx.hpp"

namespace FMI
{
    /**
     * A container class to store additional information like name and description for all variables.
     */
    class ValueInfo : protected ValueSwitch
    {
     public:
        /**
         * Create a new container for value informations.
         */
        ValueInfo();

        /**
         * Deep copy the given value information into a new object.
         * @param valueInfo The information you want to copy.
         */
        ValueInfo(const ValueInfo& valueInfo);

        /**
         * Delete the value information including all names and descriptions.
         */
        virtual ~ValueInfo();

        list<string_type> getAllValueNames() const;

        list<std::pair<string_type ,size_type> > getAllReferences() const;

        size_type size() const;

        template<typename T>
        size_type getReference(const std::string & varName) const
        {
            const map<size_type, vector<string_type> >& values = _valueReferenceToNamesMapping[dataIndex<T>()];
            for(const auto & p : values)
            {
                for(const auto name : p.second)
                    if(name == varName)
                        return p.first;
            }
            return std::numeric_limits<size_type>::max();
        }

        template<typename T>
        void addNameReferencePair(const string_type & name, size_type valueReference)
        {
            if (name.length() == 0)
                throw std::runtime_error("Cannot add empty name to reference mapping");

            map<size_type, vector<string_type> >& values = _valueReferenceToNamesMapping[dataIndex<T>()];

            auto iter = values.find(valueReference);
            if (iter == values.end())
            {
                values.insert(pair<size_type, vector<string_type> >(valueReference, vector<string_type >(1, name)));
            }
            else
                iter->second.push_back(name);
        }

        template<typename T>
        void addInputNameReferencePair(const string_type & name, size_type valueReference)
        {
            if (name.length() == 0)
                throw std::runtime_error("Cannot add empty name to reference mapping");

            map<size_type, string_type >& values = _valueInputReferenceToNamesMapping[dataIndex<T>()];

            auto iter = values.find(valueReference);
            if (iter == values.end())
            {
                values.insert(pair<size_type, string_type >(valueReference, name));
            }
        }

        template<typename T>
        list<size_type> getAllValueReferencesUnrolled() const
        {
            const map<size_type, vector<string_type> >& values = _valueReferenceToNamesMapping[dataIndex<T>()];
            std::list<size_type> res;
            for (const auto & p : values)
            {
                res.push_back(p.first);
            }
            return res;
        }

        void getAllValueReferencesUnrolled(list<size_type>& realValueIndices, list<size_type>& bool_typeValueIndices, list<size_type>& intValueIndices,
                                           list<size_type>& stringValueIndices) const;

        template<typename T>
        list<string_type> getValueNames() const
        {
            const map<size_type, vector<string_type> >& values = _valueReferenceToNamesMapping[dataIndex<T>()];
            list<string_type> res;
            for (map<size_type, vector<string_type> >::const_iterator it = values.begin();it!=values.end();++it)
            {
                for (size_type i=0;i<it->second.size();++i)
                {
                    res.push_back(it->second[i]);
                }
            }
            return res;
        }

        template<typename T>
        std::vector<string_type> getInputValueNames() const
        {
            const map<size_type, string_type >& values = _valueInputReferenceToNamesMapping[dataIndex<T>()];
            std::vector<string_type> res;
            for (map<size_type,string_type >::const_iterator it = values.begin();it!=values.end();++it)
            {
                    res.push_back(it->second);
            }
            return res;
        }

        template<typename T>
        const vector<string_type> & getAllValueNamesByReference(size_type valueReference) const
        {
            const map<size_type, vector<string_type> >& values = _valueReferenceToNamesMapping[dataIndex<T>()];

            return values.find(valueReference)->second;
        }

     private:
        vector<map<size_type, vector<string_type> > > _valueReferenceToNamesMapping;
        vector<map<size_type, string_type> > _valueInputReferenceToNamesMapping;
        vector<map<size_type, vector<string_type> > > _valueReferenceToDescriptionMapping;
    };
} /* namespace FMI */

#endif /* INCLUDE_FMI_FMUVALUEINFO_HPP_ */
/**
 * @}
 */
