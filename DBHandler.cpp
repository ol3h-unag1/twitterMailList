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

// https://developercommunity.visualstudio.com/t/internal-compiler-error-in-msvc-compiler-in-visual/1630686
auto WORKAROUND1630686_DB_HANDLER_CPP{ std::source_location::current() }; 

using std::begin;
using std::end;

namespace DataBase
{
    std::string const UnparsedHandlersFilename{ "messages.html" };
    std::string const HandlersUsageDataBaseFileName{ "hudb.bin" };
    std::size_t const HandlersContainerDefaultCapacity{ 1024u };

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

        _loadedData.clear();
        _loadedData.reserve(HandlersContainerDefaultCapacity);

        return _loadedData;
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
        // - - - - - - - - -
        // check invariant
        if (IsInitialized())
            throw FileError{ 
            _dbFileName, 
            "File already exists"
        };

        // - - - - - - - - -
        // parse handlers from html
        std::ifstream is{ UnparsedHandlersFilename };

        if (not is)
            throw FileError{ 
            UnparsedHandlersFilename, 
            "std::ifstream can't open" 
        };

        _log << std::format(
            "InitDatabase: Opened {} for reading.", 
            UnparsedHandlersFilename
        );

        std::vector< std::string > handlers{ GetSpecialHandlers() };
        handlers.reserve(HandlersContainerDefaultCapacity);

        // 
        std::string stringToParse;
        while (is >> stringToParse)
        {
            // find @ in string that marks id beginning
            auto const idStartIter{ 
                std::find(stringToParse.begin(), 
                stringToParse.end(), 
                DataModel::HandlerStartSymbol) 
            };

            std::size_t const idStart{
                idStartIter != stringToParse.end() ?
                std::size_t(idStartIter - stringToParse.begin()) :
                std::string::npos 
            };

            if (std::string::npos == idStart)
                continue;

            // find end of id position @abc123!
            auto const idEndIter{
                std::find_if_not(
                    idStartIter, 
                    stringToParse.end(), 
                    DataModel::isAllowedHandlerSymbol)
            };

            std::size_t const idEnd{ 
                idStart + (idEndIter - idStartIter) - 1 
            };

            // check id lenght
            if (not DataModel::isCorrectHandlerLen(idEnd - idStart))
                continue;

            handlers.push_back(
                stringToParse.substr(idStart, idEnd - idStart + 1)
            );
        } 

        is.close();
        is.clear();

        _log << std::format(
            "\tRead {} handlers. Preparing to create {}", 
            handlers.size(), 
            HandlersUsageDataBaseFileName
        );

        std::vector< DataModel::Raw > dataToWrite;
        dataToWrite.reserve(handlers.size());

        auto initRaw{
            [](auto const& handlerString)
        {
                DataModel::Raw data;

                if (handlerString.size() > DataModel::MaxHandlerLen)
                    throw std::out_of_range(
                        "Handler size is to big!"
                    );

                std::copy(
                    begin(handlerString), 
                    end(handlerString), 
                    data.handler);

                return data;
        } };

        std::transform(
            begin(handlers),
            end(handlers), 
            std::back_inserter( dataToWrite ), 
            initRaw);
        
        
        // - - - - - - - - -
        // create binary files and write parsed data
        std::ofstream os{ 
            HandlersUsageDataBaseFileName,
            std::ios::binary 
        };

        if (not os.is_open())
            throw FileError{ 
            HandlersUsageDataBaseFileName,
            "std::ofstream can't open file (std::ios::binary)"
        };

        for (auto const& data : dataToWrite)
            os.write(reinterpret_cast<char const*>(&data), sizeof data);

        _log << std::format(
            "Writing to binary ended {}.",
            (os ? "sucessfully" : "with error")
        );

        os.close();
        os.clear();

        _log << std::format(
            "\t{} records saved to {}", 
            dataToWrite.size(), 
            HandlersUsageDataBaseFileName
        );

    }

    std::vector<std::string> Handler::GetSpecialHandlers() const
    {
        return { "@VP", "@UN", "@u2", "@jlo", "@bbc", "@cnn" }; // special handlers;
    }
};