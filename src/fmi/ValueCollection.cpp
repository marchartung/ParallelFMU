#include "fmi/ValueCollection.hpp"

namespace FMI
{

    // Use of constructor delegation.
    ValueCollection::ValueCollection()
            : ValueCollection(0ul, 0ul, 0ul, 0ul)
    {
    }

    // Use of constructor delegation.
    ValueCollection::ValueCollection(const ValueCollection & in)
            : ValueCollection(in._realValues, in._intValues, in._bool_typeValues, in._stringValues)
    {
    }

    ValueCollection::ValueCollection(size_type realSize, size_type intSize, size_type bool_typeSize, size_type stringSize)
            : _realValues(vector<real_type>(realSize)),
              _intValues(vector<int_type>(intSize)),
              _bool_typeValues(vector<bool_type>(bool_typeSize)),
              _stringValues(vector<string_type>(stringSize))
    {
    }

    ValueCollection::ValueCollection(const vector<real_type> & real, const vector<int_type> & integer, const vector<bool_type> & bool_typeean, const vector<string_type> & str)
            : _realValues(real),
              _intValues(integer),
              _bool_typeValues(bool_typeean),
              _stringValues(str)
    {
    }

    template<>
    vector<real_type>& ValueCollection::getValues<real_type>()
    {
        return _realValues;
    }

    template<>
    vector<int_type>& ValueCollection::getValues<int_type>()
    {
        return _intValues;
    }

    template<>
    vector<bool_type>& ValueCollection::getValues<bool_type>()
    {
        return _bool_typeValues;
    }

    template<>
    vector<string_type>& ValueCollection::getValues<string_type>()
    {
        return _stringValues;
    }

    template<>
    const vector<string_type>& ValueCollection::getValues<string_type>() const
    {
        return _stringValues;
    }

    template<>
    const vector<bool_type>& ValueCollection::getValues<bool_type>() const
    {
        return _bool_typeValues;
    }

    template<>
    const vector<int_type>& ValueCollection::getValues<int_type>() const
    {
        return _intValues;
    }

    template<>
    const vector<real_type>& ValueCollection::getValues<real_type>() const
    {
        return _realValues;
    }

    void ValueCollection::assign(const ValueCollection & in)
    {
        assignInternal<real_type>(in);
        assignInternal<int_type>(in);
        assignInternal<bool_type>(in);
        assignInternal<string_type>(in);
    }

    ValueCollection & ValueCollection::operator=(const ValueCollection & in)
    {
        this->_bool_typeValues = in._bool_typeValues;
        this->_intValues = in._intValues;
        this->_stringValues = in._stringValues;
        this->_realValues = in._realValues;
        return *this;
    }

    size_t ValueCollection::size() const
    {
        return _bool_typeValues.size() + _intValues.size() + _stringValues.size() + _realValues.size();
    }


    bool_type ValueCollection::empty() const
    {
    	return _bool_typeValues.empty() && _intValues.empty() && _stringValues.empty() && _realValues.empty();
    }

    ostream & operator<<(ostream & out, const ValueCollection & in)
    {
        out << "[Real:|";
        for (auto val : in.getValues<real_type>())
        {
            out << "" << val << "|";
        }
        out << ",Int:|";
        for (auto val : in.getValues<int_type>())
        {
            out << "" << val << "|";
        }
        out << ",Bool:|";
        for (auto val : in.getValues<bool_type>())
        {
            out << "" << ((val == 0) ? false : true) << "|";
        }
        out << ",String:|";
        for (auto val : in.getValues<string_type>())
        {
            out << "" << val << "|";
        }
        out << "]";
        return out;
    }

} /* namespace FMI */

string_type  to_string(const FMI::ValueCollection & in)
{
    std::stringstream res;
    res << in;
    return res.str();
}
