#include "util/Logger.hpp"

namespace Util
{

    std::shared_ptr<Logger> Logger::instance;

    Logger::Logger(LogSettings settings, bool_type enabled)
            : _settings(settings),
              _textDecorator(4),
              _isEnabled(enabled)
    {
        _textDecorator[LL_ERROR] = tuple<string_type, string_type>("\e[31m\e[1m", "\e[0m");
        _textDecorator[LL_WARNING] = tuple<string_type, string_type>("\e[33m", "\e[0m");
        _textDecorator[LL_INFO] = tuple<string_type, string_type>("\e[39m", "\e[0m");
        _textDecorator[LL_DEBUG] = tuple<string_type, string_type>("\e[90m", "\e[0m");
    }

    Logger::Logger(bool_type enabled)
            : _settings(LogSettings()),
              _isEnabled(enabled)
    {
    }

    Logger::~Logger()
    {
    }

    void Logger::writeInternal(string_type message, LogCategory category, LogLevel level)
    {
        if (isOutput(category, level))
        {
            cerr << getPrefix(category, level) << "  " << message << getSuffix(category, level) << endl;
        }
    }

    void Logger::setEnabledInternal(bool_type enabled)
    {
        _isEnabled = enabled;
    }

    bool_type Logger::isEnabledInternal()
    {
        return _isEnabled;
    }

    bool_type Logger::isOutput(LogCategory category, LogLevel level) const
    {
        return _settings.modes[category] >= level && _isEnabled;
    }

    bool_type Logger::isOutput(pair<LogCategory, LogLevel> mode) const
    {
        return isOutput(mode.first, mode.second);
    }

    string_type Logger::getPrefix(LogCategory category, LogLevel level) const
    {
        string_type prefix = std::get<0>(_textDecorator[level]);
        prefix += string_type("[") + logCategoryToString(category) + string_type("]");
        return prefix;
    }

    string_type Logger::getSuffix(LogCategory category, LogLevel level) const
    {
        string_type suffix = std::get<1>(_textDecorator[level]);
        return suffix;
    }

    string_type Logger::logCategoryToString(LogCategory category) const
    {
        switch (category)
        {
            case LC_LOADER:
                return "LOADER";
            case LC_SYS:
                return "SYSTEM";
            case LC_SOLVER:
                return "SOLVER";
            case LC_EVT:
                return "EVENT";
            case LC_OTHER:
                return "OTHER";
            default:
                return "";
        }
    }

    string_type Logger::logLevelToString(LogLevel level) const
    {
        switch (level)
        {
            case (LL_DEBUG):
                return "DEBUG";
            case (LL_ERROR):
                return "ERROR";
            case (LL_INFO):
                return "INFO";
            case (LL_WARNING):
                return "WARNING";
            default:
                return "";
        }
    }

} /* namespace Util */
