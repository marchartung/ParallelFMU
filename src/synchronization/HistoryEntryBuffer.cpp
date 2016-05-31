/*
 * HistoryEntryBuffer.cpp
 *
 *  Created on: 24.02.2016
 *      Author: hartung
 */

#include <synchronization/HistoryEntryBuffer.hpp>
#include <cstdint>

namespace Synchronization
{

    HistoryEntryBuffer::HistoryEntryBuffer(const HistoryEntry& in, bool init)
            : _free(init),
              _time(in.getTime()),
              _realSize(in.getValueCollection().getValues<real_type>().size()),
              _realVals(nullptr),
              _intSize(in.getValueCollection().getValues<int_type>().size()),
              _intVals(nullptr),
              _bool_typeSize(in.getValueCollection().getValues<bool_type>().size()),
              _bool_typeVals(nullptr),
              _dataSize((_realSize + 1) * sizeof(real_type) + (_intSize + 1) * sizeof(int) + (_bool_typeSize + 1) * sizeof(bool_type))
    {
        initialize(in);
    }

    HistoryEntryBuffer::~HistoryEntryBuffer()
    {
    }

    HistoryEntryBuffer::operator HistoryEntry() const
    {
        HistoryEntry res(
                ((real_type*)_data.get())[0],
                (getStartOfAdditionalIntValues()[0]),
                FMI::ValueCollection(_realSize,_intSize,_bool_typeSize,0ul),
                getStartOfAdditionalBoolValues()[0]
                );

        copy(_realVals,_realVals+_realSize,res.getValueCollection().getValues<real_type>().begin());
        copy(_intVals,_intVals+_intSize,res.getValueCollection().getValues<int_type>().begin());
        for(size_type i=0;i<_bool_typeSize;++i)
            res.getValueCollection().getValues<int_type>()[i] = _bool_typeVals[i];
        return res;
    }

    HistoryEntryBuffer& HistoryEntryBuffer::operator=(const HistoryEntry& in)
    {
        return (*this = HistoryEntryBuffer(in,true));
    }

    void* HistoryEntryBuffer::data()
    {
        return reinterpret_cast<void*>(_data.get());
    }

    size_type HistoryEntryBuffer::dataSize() const
    {
        return _dataSize;
    }

    void HistoryEntryBuffer::initialize(const HistoryEntry & in)
    {
        //setting up pointers
        _data = shared_ptr<char>(new char[_dataSize]);
        _realVals = getStartOfValueCollection();
        _intVals = reinterpret_cast<int*>(_realVals + _realSize);
        _bool_typeVals = reinterpret_cast<bool_type*>(_intVals + _intSize);

        //copy data
        ((real_type*)_data.get())[0] = in.getTime();
        getStartOfAdditionalIntValues()[0] = in.getSolverOrder();
        getStartOfAdditionalBoolValues()[0] = in.hasEvent();

        copy(in.getValueCollection().getValues<real_type>().begin(), in.getValueCollection().getValues<real_type>().end(), _realVals);
        copy(in.getValueCollection().getValues<int_type>().begin(), in.getValueCollection().getValues<int_type>().end(), _intVals);
        for(size_type i=0;i<_bool_typeSize;++i)
            _bool_typeVals[i] = in.getValueCollection().getValues<bool_type>()[i-1];
    }

    real_type* HistoryEntryBuffer::getStartOfValueCollection()
    {
        return reinterpret_cast<real_type*>(_data.get()+sizeof(real_type)+sizeof(int)+sizeof(bool_type));
    }

    int* HistoryEntryBuffer::getStartOfAdditionalIntValues()
    {
        return reinterpret_cast<int*>(_data.get()+sizeof(real_type));
    }

    bool_type* HistoryEntryBuffer::getStartOfAdditionalBoolValues()
    {
        return reinterpret_cast<bool_type*>(_data.get()+sizeof(real_type)+sizeof(int));
    }

    const int* HistoryEntryBuffer::getStartOfAdditionalIntValues() const
    {
        return reinterpret_cast<int*>(_data.get()+sizeof(real_type));
    }

    bool HistoryEntryBuffer::isFree() const
    {
        return _free;
    }

    void HistoryEntryBuffer::setFree(const bool& in)
    {
        _free = in;
    }

    const bool_type* HistoryEntryBuffer::getStartOfAdditionalBoolValues() const
    {
        return reinterpret_cast<bool_type*>(_data.get()+sizeof(real_type)+sizeof(int));
    }

} /* namespace Synchronization */

