#include "fmi/InputMapping.hpp"

namespace FMI
{

    InputMapping::InputMapping(vector<tuple<size_type, size_type> > realVarsMapping, vector<tuple<size_type, size_type> > intVarsMapping, vector<tuple<size_type, size_type> > bool_typeVarsMapping, vector<tuple<size_type, size_type> > stringVarsMapping)
    : _connectedVars({realVarsMapping, intVarsMapping, bool_typeVarsMapping,stringVarsMapping})
    {
        //_connectedVars[dataIndex<double>()] = realVarsMapping;
        //_connectedVars[dataIndex<int_type>()] = intVarsMapping;
        //_connectedVars[dataIndex<bool_type>()] = bool_typeVarsMapping;
        //_connectedVars[dataIndex<string_type >()] = stringVarsMapping;
    }

    ValueCollection InputMapping::pack(const ValueCollection & in) const
    {
        return ValueCollection(pack<real_type>(in), pack<int_type>(in), pack<bool_type>(in), pack<string_type>(in));
    }

    void InputMapping::unpack(ValueCollection & out, const ValueCollection & in) const
    {
        unpack<real_type>(out, in);
        unpack<int_type>(out, in);
        unpack<bool_type>(out, in);
        unpack<string_type>(out, in);
    }

    size_type InputMapping::size() const
    {
        size_type res = 0;
        for(size_t i=0;i<_connectedVars.size();++i)
            res += _connectedVars[i].size();
        return res;
    }

    ValueCollection InputMapping::getPackedValueCollection() const
    {
        return ValueCollection(getValues<real_type>().size(), getValues<int_type>().size(),
                               getValues<bool_type>().size(), getValues<string_type>().size());
    }

    std::ostream & operator<<(std::ostream & in, const FMI::InputMapping & im)
    {
        for (unsigned i = 0; i < 4; ++i)
        {
            if (i == 0)
                std::cout << "d\n";
            if (i == 1)
                std::cout << "i\n";
            if (i == 2)
                std::cout << "b\n";
            if (i == 3)
                std::cout << "s\n";
            for (unsigned j = 0; j < im._connectedVars[i].size(); ++j)
            {
                in << get<0>(im._connectedVars[i][j]) << " => " << get<1>(im._connectedVars[i][j]) << "\n";
            }
        }
        return in;
    }

} /* namespace FMI */
