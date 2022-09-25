module DBHandler;

import <vector>;

import <string>;
import <format>;

import <fstream>;
import <filesystem>;

import <exception>;

import <source_location>;

import FileErrorException;

auto WORKAROUND1630686_DB_HANDLER_CPP{ std::source_location::current() }; // https://developercommunity.visualstudio.com/t/internal-compiler-error-in-msvc-compiler-in-visual/1630686

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

        std::vector< std::string > handlers;
        handlers.reserve(HandlersContainerDefaultCapacity);


        //_log << std::format("\tRead {} handlers. Preparing to create {}", handlers.size(), HandlersUsageDataBase);

    }

    std::vector<std::string> Handler::GetSpecialHandlers() const
    {
        return { "@VP", "@UN", "@u2", "@jlo", "@bbc", "@cnn" }; // special handlers;
    }
};