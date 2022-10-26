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
    std::vector< DataModel::Data > Handler::load()
    {
        if (!isBinaryExists())
        {
            initDatabase();
        }
        else
        {
            
        }

        return _cache;
    }

    void Handler::save(std::vector< DataModel::Data > const&) const 
    {
        
    }

    bool Handler::isBinaryExists() const
    {
        return std::filesystem::exists(_dbFileName);
    }

    void Handler::initDatabase() noexcept(false)
    {
        clearCache();

        // - - - - - - - - -
        // check invariant
        if (isBinaryExists())
            throw FileError{ 
            _dbFileName, 
            "File already exists"
        };

        // - - - - - - - - -
        // parse handlers from html
        std::ifstream is{ _unparsedHandlersFileName };

        if (not is)
            throw FileError{ 
            _unparsedHandlersFileName, 
            "std::ifstream can't open" 
        };

        _log << std::format(
            "InitDatabase: Opened {} for reading.", 
            _unparsedHandlersFileName
        );

        std::vector< std::string > handlers{ getSpecialHandlers() };
        handlers.reserve(HandlersContainerDefaultCapacity);

        std::string stringToParse;
        while (is >> stringToParse)
        {
            // find @ in string that marks id beginning
            auto const idStartIter{ 
                std::find(
                    begin(stringToParse),
                    end(stringToParse),
                    DataModel::HandlerStartSymbol
                ) 
            };

            std::size_t const idStart{
                idStartIter != stringToParse.end() ?
                std::size_t(idStartIter - stringToParse.begin()) :
                std::string::npos 
            };

            if (std::string::npos == idStart)
                continue;

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
            _dbFileName
        );

        std::vector< DataModel::Raw > raw;
        raw.reserve(handlers.size());
       
        // populating data to write to binary
        std::transform(
            begin(handlers),
            end(handlers), 
            std::back_inserter( raw ), 
            [](auto const& handler)
            {
                if (handler.size() > DataModel::MaxHandlerLen)
                    throw std::out_of_range(
                        "Handler size is to big!"
                    );

                DataModel::Raw data{};

                std::copy(
                    begin(handler),
                    end(handler),
                    data.handler);

                return data;
            });
        
        // cache data 
        std::transform(
            begin(raw),
            end(raw),
            std::back_inserter(_cache),
            [](auto const& raw)
            {
                return DataModel::Data(raw);
            });
        
        // - - - - - - - - -
        // create binary files and write parsed data
        std::ofstream os{ 
            _dbFileName,
            std::ios::binary 
        };

        if (not os.is_open())
            throw FileError{ 
            _dbFileName,
            "std::ofstream can't open file (std::ios::binary)"
        };

        for (auto const& bin : raw)
            os.write(reinterpret_cast<char const*>(&bin), sizeof bin);

        _log << std::format(
            "Writing to binary ended {}.",
            (os ? "sucessfully" : "with error")
        );

        os.close();
        os.clear();

        _log << std::format(
            "\t{} records saved to {}", 
            raw.size(), 
            _dbFileName
        );

    }

    std::vector<std::string> Handler::getSpecialHandlers() const
    {
        return { "@VP", "@UN", "@u2", "@jlo", "@bbc", "@cnn" }; // special handlers;
    }

    void Handler::clearCache()
    {
        _cache.clear();
    }

    bool Handler::writeToBinary(std::vector<DataModel::Raw> const& raw)
    {
        return false;
    }

    std::optional< std::vector< DataModel::Data > > Handler::readFromBinary()
    {
        return std::optional<std::vector<DataModel::Data>>();
    }
};