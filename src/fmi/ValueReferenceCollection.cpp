#include <stdexcept>
#include "fmi/ValueReferenceCollection.hpp"

namespace FMI
{

    ValueReferenceCollection::ValueReferenceCollection(const vector<size_type> & realVars, const vector<size_type> & intVars, const vector<size_type> & boolVars, const vector<size_type> & stringVars)
            : _values( { realVars, intVars, boolVars, stringVars })

    {
        /*_values = vector<vector<size_type> >(4, vector<size_type>());
         _values[dataIndex<double>()] = realVars;
         _values[dataIndex<int_type>()] = intVars;
         _values[dataIndex<bool_type>()] = bool_typeVars;
         _values[dataIndex<string_type>()] = stringVars;*/
    }

    size_type ValueReferenceCollection::size() const
    {
        size_type sum = 0;
        for (unsigned i = 0; i < _values.size(); ++i)
            sum += _values[i].size();
        return sum;
    }


    std::ostream & operator<<(std::ostream & s, const ValueReferenceCollection & vrc)
    {
        s << "[Real: [";
        for(size_type i=0;i<vrc._values[0].size();++i)
            s << "\'" << i << " -> " << vrc._values[0][i] << "\' ";
        s << "], Int: [";
        for(size_type i=0;i<vrc._values[1].size();++i)
            s << "\'" << i << " -> " << vrc._values[1][i] << "\' ";

        s << "], Bool: [";
        for(size_type i=0;i<vrc._values[2].size();++i)
            s << "\'" << i << " -> " << vrc._values[2][i] << "\' ";

        s << "], String: [";
        for(size_type i=0;i<vrc._values[3].size();++i)
            s << "\'" << i << " -> " << vrc._values[3][i] << "\' ";
        s << "]";
        return s;
    }

} /* namespace FMI */
