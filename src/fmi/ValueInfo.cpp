#include "fmi/ValueInfo.hpp"

namespace FMI
{

    ValueInfo::ValueInfo()
            : ValueSwitch(),
              _valueReferenceToNamesMapping(4, std::map<size_type, vector<string_type> >()),
              _valueInputReferenceToNamesMapping(4, std::map<size_type, string_type>()),
              _valueReferenceToDescriptionMapping(4, std::map<size_type, vector<string_type> >())
    {
    }

    ValueInfo::ValueInfo(const ValueInfo& obj)
        : ValueSwitch(),
          _valueReferenceToNamesMapping(obj._valueReferenceToNamesMapping)
    {
    }

    ValueInfo::~ValueInfo()
    {
    }

    size_type ValueInfo::size() const
    {
        size_t counter = 0;
        for (size_t i = 0; i < _valueReferenceToNamesMapping.size(); ++i)
            counter += _valueReferenceToNamesMapping[i].size();

        return counter;
    }

    std::vector<string_type> ValueInfo::getAllValueNames() const
    {
        std::vector<string_type> valueNames;
        std::vector<string_type> valueNamesTmp;
        std::vector<string_type>::iterator it;

        valueNamesTmp = getValueNames<double>();
        it = valueNames.end();
        valueNames.insert(it, valueNamesTmp.begin(), valueNamesTmp.end());
        valueNamesTmp = getValueNames<int_type>();
        it = valueNames.end();
        valueNames.insert(it, valueNamesTmp.begin(), valueNamesTmp.end());
        valueNamesTmp = getValueNames<bool_type>();
        it = valueNames.end();
        valueNames.insert(it, valueNamesTmp.begin(), valueNamesTmp.end());
        valueNamesTmp = getValueNames<string_type>();
        it = valueNames.end();
        valueNames.insert(it, valueNamesTmp.begin(), valueNamesTmp.end());

        return valueNames;
    }

    list<std::pair<string_type , size_type> > ValueInfo::getAllReferences() const
    {
        list<std::pair<string_type , size_type> > res;
        for(const auto& maps : _valueReferenceToNamesMapping)
            for(const auto& p : maps)
                for(const string_type & str : p.second)
                    res.push_back(std::pair<string_type ,size_type>(str,p.first));
        return res;
    }

    void ValueInfo::getAllValueReferencesUnrolled(list<size_type>& realValueIndices, list<size_type>& bool_typeValueIndices, list<size_type>& intValueIndices,
                                                  list<size_type>& stringValueIndices) const
    {
        list<size_type> valueReferencesTmp;
        list<size_type>::iterator it;

        valueReferencesTmp = getAllValueReferencesUnrolled<double>();
        it = realValueIndices.end();
        realValueIndices.insert(it, valueReferencesTmp.begin(), valueReferencesTmp.end());

        valueReferencesTmp = getAllValueReferencesUnrolled<int_type>();
        it = intValueIndices.end();
        intValueIndices.insert(it, valueReferencesTmp.begin(), valueReferencesTmp.end());

        valueReferencesTmp = getAllValueReferencesUnrolled<bool_type>();
        it = bool_typeValueIndices.end();
        bool_typeValueIndices.insert(it, valueReferencesTmp.begin(), valueReferencesTmp.end());

        valueReferencesTmp = getAllValueReferencesUnrolled<string_type>();
        it = stringValueIndices.end();
        stringValueIndices.insert(it, valueReferencesTmp.begin(), valueReferencesTmp.end());
    }

} /* namespace FMI */
