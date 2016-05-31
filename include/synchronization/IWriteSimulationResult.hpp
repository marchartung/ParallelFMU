/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_IWRITESIMULATIONRESULT_HPP_
#define INCLUDE_SYNCHRONIZATION_IWRITESIMULATIONRESULT_HPP_

#include "BasicTypedefs.hpp"

namespace Synchronization
{
    class IWriteSimulationResult
    {
     public:
        IWriteSimulationResult();

        virtual ~IWriteSimulationResult();

        value_type getCurrentWriteTime() const;

        void writeResults();
    };

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_IWRITESIMULATIONRESULT_HPP_ */
/**
 * @}
 */
