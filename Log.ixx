export module Log;

import <exception>;
import <queue>;
import <string>;

export namespace Log {

    class Logger
    {
    public:
        Logger() = default;
        Logger(Logger&) = delete;

        ~Logger();

        void log(std::string msg);
        Logger& operator<<(std::string msg);

        void flush();

    private:
        std::queue< std::string > _messages;
    };
}