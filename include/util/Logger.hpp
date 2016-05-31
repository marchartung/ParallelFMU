/** @addtogroup Util
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_UTIL_LOGGER_HPP_
#define INCLUDE_UTIL_LOGGER_HPP_

#ifdef USE_LOGGER
#define LOGGER_WRITE(message,category,level) Util::Logger::write(message,category,level)
#define LOGGER_WRITE_TUPLE(message,categoryLevel) Util::Logger::write(message,categoryLevel)
#else
#define LOGGER_WRITE(x,y,z)
#define LOGGER_WRITE_TUPLE(x,y)
#endif //USE_LOGGER

#include "Stdafx.hpp"

namespace Util
{
    enum LogCategory
    {
        LC_LOADER = 0,
        LC_SYS = 1,
        LC_SOLVER = 2,
        LC_EVT = 3,
        LC_OTHER = 4
    };
    enum LogLevel
    {
        LL_ERROR = 0,
        LL_WARNING = 1,
        LL_INFO = 2,
        LL_DEBUG = 3
    };

    /**
     * A container to store the configuration settings of the logger.
     */
    struct LogSettings
    {
        /**
         * An array that stores the level of the logger for all known categories.
         */
        vector<LogLevel> modes;

        /**
         * Create a new object for logger settings. All categories are set to the warning level by default.
         */
        LogSettings()
        {
            modes = vector<LogLevel>(5, LL_DEBUG);
        }
        /**
         * Set all categories at once to the given level.
         * @param level The new level for all categories.
         */
        void setAll(LogLevel level)
        {
            for (unsigned i = 0; i < modes.size(); ++i)
                modes[i] = level;
        }
    };

    /**
     * This is the base class for all logger instances. It is implemented as singleton and will write all messages
     * to the error stream.
     */
    class Logger
    {
     public:
        /**
         * Destroy the logger object.
         */
        virtual ~Logger();

        /**
         * Get the singleton instance.
         * @return The internal instance of the logger that should be used.
         */
        static Logger& getInstance()
        {
            if (instance == nullptr)
                initialize(LogSettings());

            return *instance;
        }

        /**
         * Initialize a new logger instance configured by the given settings.
         * @param settings
         */
        static void initialize(LogSettings settings)
        {
            if (instance != nullptr)
                instance.reset();

            instance = std::shared_ptr<Logger>(new Logger(settings, true));
        }

        /**
         * Initialize an instance of the default logger and store it as internal instance.
         */
        static void initialize()
        {
            initialize(LogSettings());
        }

        /**
         * This function will write the given message with the category and level information to the output of the
         * logger-instance.
         * @param message The message that should be written.
         * @param category The category of the message.
         * @param level The significance of the given message.
         */
        static inline void write(string_type  message, LogCategory category, LogLevel level)
        {
            Logger& instance = getInstance();
            if (instance.isEnabled())
                instance.writeInternal(message, category, level);
        }

        /**
         * This function will write the given message with the category and level information to the output of the
         * logger-instance.
         * @param message The message that should be written.
         * @param mode The category and significance indicator encapsulated as one pair.
         */
        static inline void write(string_type  message, std::pair<LogCategory, LogLevel> mode)
        {
            write(message, mode.first, mode.second);
        }

        /**
         * Enable or disable the logger.
         * @param enabled True if the logger should be enabled.
         */
        static void setEnabled(bool_type enabled)
        {
            getInstance().setEnabledInternal(enabled);
        }

        /**
         * Check if the logger is enabled.
         * @return True if the logger is enabled.
         */
        static bool_type isEnabled()
        {
            return getInstance().isEnabledInternal();
        }

        /**
         * Check if a message with the given level, belonging the give category will be written through the logger.
         * @param category The category that should be checked.
         * @param level The level that should be checked.
         * @return True if the message would not be filtered and written to the output.
         */
        bool_type isOutput(LogCategory category, LogLevel level) const;
        /**
         * Check if a message with the given level, belonging the give category will be written through the logger.
         * @param mode The category and level that should be checked.
         * @return True if the message would not be filtered and written to the output.
         */
        bool_type isOutput(std::pair<LogCategory, LogLevel> mode) const;

     protected:
        /**
         * Create a new logger instance with the given settings.
         * @attention There should be just one logger-instance, so take care about the singleton.
         * @param settings The settings of the logger.
         * @param enabled True if the logger should be enabled.
         */
        Logger(LogSettings settings, bool_type enabled);
        /**
         * Create a new logger instance with default settings.
         * @attention There should be just one logger-instance, so take care about the singleton.
         * @param enabled True if the logger should be enabled.
         */
        Logger(bool_type enabled);
        /**
         * Write the message with the given category and level information to the output.
         * @note This function should be overwritten by concrete logger implementation.
         * @param message The message that should be written.
         * @param category The category of the message.
         * @param level The significance of the given message.
         */
        virtual void writeInternal(string_type  message, LogCategory category, LogLevel level);
        /**
         * Enable or disable the logger.
         * @param enabled True if the logger should be enabled.
         * @note This function can be overwritten by concrete logger implementations.
         */
        virtual void setEnabledInternal(bool_type enabled);
        /**
         * Check if the logger is enabled.
         * @return True if the logger is enabled.
         * @note This function can be overwritten by concrete logger implementations.
         */
        virtual bool_type isEnabledInternal();
        /**
         * Get a prefix text decoration for a message of the given category and level.
         * @param category The category of the message.
         * @param level The level of the message.
         * @return The prefix that contains color information.
         */
        string_type  getPrefix(LogCategory category, LogLevel level) const;
        /**
         * Get a suffix text decoration for a message of the given category and level.
         * @param category The category of the message.
         * @param level The level of the message.
         * @return The suffix that contains color and category information.
         */
        string_type  getSuffix(LogCategory category, LogLevel level) const;
        /**
         * Convert the given log category to string.
         * @param category The category that should be converted.
         * @return The result string.
         */
        string_type  logCategoryToString(LogCategory category) const;
        /**
         * Convert the given log level to string.
         * @param level The level that should be converted.
         * @return The result string.
         */
        string_type  logLevelToString(LogLevel level) const;
        /**
         * The static logger singleton instance. This should be set by the initialize method of the concrete logger
         * implementation.
         */
        static std::shared_ptr<Logger> instance;

     private:
        LogSettings _settings;
        vector<std::tuple<string_type , string_type > > _textDecorator;
        bool_type _isEnabled;
    };

} /* namespace Util */

#endif /* INCLUDE_UTIL_LOGGER_HPP_ */
/**
 * @}
 */
