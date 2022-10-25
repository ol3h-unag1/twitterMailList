module Log;

import <string>;
import <iostream>;

Log::Logger::~Logger()
{
    flush();
}

void Log::Logger::log(std::string msg)
{
    _messages.emplace(std::move(msg));
}

Log::Logger& Log::Logger::operator<<(std::string msg)
{
    log(std::move(msg));
    return *this;;
}

void Log::Logger::flush()
{
    bool flush{ not _messages.empty() };
    while (_messages.size())
    {
        std::cout << _messages.front() << "\n";
        _messages.pop();
    }
    if (flush)
        std::cout << std::endl;
}
