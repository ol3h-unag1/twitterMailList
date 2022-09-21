module Log;

import <string>;
import <iostream>;

Log::Logger::~Logger()
{
    bool flush{ !_messages.empty()};
    while (_messages.size())
    {
        std::cout << _messages.front() << "\n";
        _messages.pop();
    }
    if (flush)
        std::cout << std::endl;
}

void Log::Logger::log(std::string msg)
{
    _messages.emplace(std::move(msg));
}
