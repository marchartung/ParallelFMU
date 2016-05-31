/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */
#ifndef INCLUDE_SYNCHRONIZATION_SUBHISTORY_HPP_
#define INCLUDE_SYNCHRONIZATION_SUBHISTORY_HPP_

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "synchronization/HistoryEntry.hpp"
#include "synchronization/Interpolation.hpp"

namespace Synchronization {
class SubHistory {
public:
	~SubHistory() {
	}
	;

	size_type size() const;

	real_type getNewestTime() const;

	FMI::ValueCollection interpolate(const real_type & time) const;

	FMI::ValueCollection operator[](const real_type & time) const;

	const Interpolation& getInterpolation() const;

	friend class AbstractDataHistory;

private:
	std::set<HistoryEntry> _entries;
	Interpolation _interpolation;

	SubHistory() = delete;

	SubHistory(const Interpolation & interpolation,
			const FMI::ValueCollection & bufferScheme);

	bool_type insert(const HistoryEntry & in);

	size_type deleteOlderThan(const HistoryEntry & in);
};

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_SUBHISTORY_HPP_ */
/**
 * @}
 */
