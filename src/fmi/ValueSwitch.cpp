#include "fmi/ValueSwitch.hpp"

namespace FMI
{

    template<>
    size_type ValueSwitch::dataIndex<double>() const
    {
        return static_cast<size_type>(0u);
    }

    template<>
    size_type ValueSwitch::dataIndex<int_type>() const
    {
        return static_cast<size_type>(1u);
    }

    template<>
    size_type ValueSwitch::dataIndex<bool_type>() const
    {
        return static_cast<size_type>(2u);
    }

    template<>
    size_type ValueSwitch::dataIndex<string_type>() const
    {
        return static_cast<size_type>(3u);
    }


} /* namespace FMI */
