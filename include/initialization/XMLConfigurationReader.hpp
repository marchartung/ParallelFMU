/** @addtogroup Initialization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_INITIALIZATION_XMLCONFIGURATIONREADER_HPP_
#define INCLUDE_INITIALIZATION_XMLCONFIGURATIONREADER_HPP_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <typeinfo>

#include "Stdafx.hpp"
#include "initialization/IConfigurationReader.hpp"
#include "initialization/DefaultValues.hpp"

#include "Plans.hpp"

using boost::property_tree::ptree;

namespace Initialization
{
    /**
     * Class that parses the XML configuration file.
     */
    class XMLConfigurationReader : public IConfigurationReader
    {
     public:
        /**
         * Create reader to parse XML configuration file.
         * @param configurationFilePath Path to configuration file.
         */
        XMLConfigurationReader(string_type configurationFilePath);

        /**
         * Destroy the reader object.
         */
        virtual ~XMLConfigurationReader() = default;

        /**
         * @return The program plan which holds the simulation plans.
         */
        ProgramPlan getProgramPlan();

     private:
        vector<vector<SimulationPlan>> getSimulationPlans(const list<SolverPlan> & solverPlans, const SchedulePlan & schedPlan,
                                                          const SimulationPlan simPlan);

        FmuPlan getFmuPlan(const ptree::value_type & fmuElem, const SimulationPlan & simPlan, const size_type id);

        list<SolverPlan> getSolverPlanFromMultipleFmu(const ptree::value_type & firstElem, const size_type & startId,
                                                      const SimulationPlan & simPlan);

        list<SolverPlan> getSolverPlanFromFmu(const ptree::value_type & firstElem, const size_type & id,
                                              const SimulationPlan & simPlan, const bool & single);

        list<SolverPlan> getSolverPlans(const SimulationPlan & simPlan);

        SolverPlan getSolverPlan(const ptree::value_type & firstElem, const SimulationPlan & simPlan,
                                 size_type id = std::numeric_limits<size_type>::max());

        ConnectionPlan getConnectionPlan(ptree::value_type & mapElem);

        list<ConnectionPlan> getConnectionPlans();

        vector<vector<size_type>> getNodeSchedule(ptree::value_type & solverElem);

        SchedulePlan getSchedulePlan();

        void createMapping(const list<SolverPlan> & solverPlans, SchedulePlan & schedPlan);

        SimulationPlan getDefaultSimulationPlan();

        void appendConnectionInformation(list<SolverPlan> & solverPlans, list<ConnectionPlan> & connPlans, const SchedulePlan & schedPlan);

        WriterPlan getWriterPlan();

     private:
        /// Path to the configuration file to parse.
        string_type _configurationFilePath;
        ptree _propertyTree;

        template<typename T, typename ...Remain>
        void checkForUndefinedValues(const T & first, const Remain & ... remainer)
        {
            checkForUndefinedValues<T>(first);
            checkForUndefinedValues<Remain...>(remainer ...);
        }

        template<typename T>
        void checkForUndefinedValues(const T & first)
        {
            if (DefaultValues::isUndefinedValue<T>(first))
                throw runtime_error("Uninitialized value.");
        }
    };

} /* namespace Initialization */

#endif /* INCLUDE_INITIALIZATION_XMLCONFIGURATIONREADER_HPP_ */
/**
 * @}
 */

