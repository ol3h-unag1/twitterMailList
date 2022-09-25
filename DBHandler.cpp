module DBHandler;

import <vector>;
import <iterator>;

import <string>;
import <format>;

import <fstream>;

import <fstream>;
import <filesystem>;

import <exception>;

import <source_location>;

import <algorithm>;

import FileErrorException;

auto WORKAROUND1630686_DB_HANDLER_CPP{ std::source_location::current() }; // https://developercommunity.visualstudio.com/t/internal-compiler-error-in-msvc-compiler-in-visual/1630686

using std::begin;
using std::end;

namespace DataBase
{
    auto const HandlerStartSymbol = '@';

    std::string const UnparsedHandlersFilename{ "messages.html" };
    std::string const HandlersUsageDataBaseFileName{ "hudb.bin" };
    std::size_t const HandlersContainerDefaultCapacity{ 1024u };

    std::size_t const MinHandlerLen{ 4u + 1u };  // counting '@' as part of ID string
    std::size_t const MaxHandlerLen{ 15u + 1u }; // counting '@' and as part of ID string

    auto isCorrectHandlerLen = [](auto const& len)
    {
        return MinHandlerLen <= len && len <= MaxHandlerLen;
    };

    auto isAllowedHandlerSymbol = [](std::string::value_type const& s)
    {
        // big/small letters, digits and underscore with @
        static std::string const allowedSymbols{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_@" };
        return std::find(allowedSymbols.begin(), allowedSymbols.end(), s) != allowedSymbols.end();
    };

    struct BinPOD
    {
        char handler[MaxHandlerLen + 1]{ 0 }; // + '\0'
        std::size_t timesUsed{ 0u };
        std::size_t lastTimeUsed{ 0u };
    };

    Handler::Handler() noexcept
        : _dbFileName{ HandlersUsageDataBaseFileName }
        , _unparsedHandlersFileName{ UnparsedHandlersFilename }
    {}

    // handles reads and writes to DB
    std::vector< DataModel::Data > Handler::Load()
    {
        if (!IsInitialized())
        {
            InitDatabase();
        }

        std::vector< DataModel::Data > data;

        return data;
    }

    void Handler::Save(std::vector< DataModel::Data > const&) const 
    {
        
    }

    bool Handler::IsInitialized() const
    {
        return std::filesystem::exists(_dbFileName);
    }

    void Handler::InitDatabase() noexcept(false)
    {
        if (IsInitialized())
            throw FileError{ _dbFileName, "File already exists"};

        std::ifstream is{ UnparsedHandlersFilename };

        if (!is)
            throw FileError{ UnparsedHandlersFilename, "std::ifstream can't open" };

        _log << std::format("InitDatabase: Opened {} for reading.", UnparsedHandlersFilename);

        std::vector< std::string > handlers{ GetSpecialHandlers() }; // first adding special handlers
        handlers.reserve(HandlersContainerDefaultCapacity);

        // parse handlers
        std::string stringToParse;
        while (is >> stringToParse)
        {
            // find @ in string that marks id beginning
            auto const idStartIter{ std::find(stringToParse.begin(), stringToParse.end(), HandlerStartSymbol) };
            std::size_t const idStart{
                idStartIter != stringToParse.end() ?
                std::size_t(idStartIter - stringToParse.begin()) :
                std::string::npos };

            if (std::string::npos == idStart)
                continue;

            // find end of id position @abc123!
            auto const idEndIter = std::find_if_not(idStartIter, stringToParse.end(), isAllowedHandlerSymbol);
            std::size_t const idEnd{ idStart + std::size_t(idEndIter - idStartIter) - 1 };

            // check id lenght
            if (!isCorrectHandlerLen(idEnd - idStart))
                continue;

            handlers.push_back(stringToParse.substr(idStart, idEnd - idStart + 1));
        } // check input stream state after the loop?

        _log << std::format("\tRead {} handlers. Preparing to create {}", handlers.size(), HandlersUsageDataBaseFileName);

        std::vector< BinPOD > dataToWrite;
        dataToWrite.reserve(handlers.size());

        auto str2BinPOD{
            [](auto const& handlerString)
        {
                BinPOD data;
                data.timesUsed = 0;
                data.lastTimeUsed = 0;

                if (handlerString.size() > MaxHandlerLen)
                    throw std::out_of_range(std::format("Handler size is to big!")); //change to use mu exception

                std::copy(begin(handlerString), end(handlerString), data.handler);
                return data;
        } };

        std::transform(begin(handlers), end(handlers), std::back_inserter( dataToWrite ), str2BinPOD);
    }

    std::vector<std::string> Handler::GetSpecialHandlers() const
    {
        return { "@VP", "@UN", "@u2", "@jlo", "@bbc", "@cnn" }; // special handlers;
    }
};