import <vector>;

import <iostream>;
import <fstream>;

import <string>;
import <string_view>;

import <format>;

import <algorithm>;

import <chrono>;

import <exception>;

import <filesystem>;

import <source_location>;

using Clock_Type = std::chrono::system_clock;

std::string const UnparsedHandlersFilename{ "messages.html" }; 
std::string const HandlersUsageDataBase{ "hudb.bin" };

auto const HandlerStartSymbol = '@';

std::size_t const MinHandlerLen{ 4u + 1u };  // counting '@' as part of ID string
std::size_t const MaxHandlerLen{ 15u + 1u }; // counting '@' as part of ID string
std::size_t const DefaultVectorCapacity{ 1024u };

auto isCorrectHandlerLen = [](auto const& len)
{
    return MinHandlerLen <= len && len <= MaxHandlerLen;
};

auto isAllowedHandlerSymbol = [](std::string::value_type const& s)
{
    // big/small letter, digits and underscore with @
    static std::string const allowedSymbols{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_@" };
    return std::find(allowedSymbols.begin(), allowedSymbols.end(), s) != allowedSymbols.end();
};

struct HandlerFreqTimeBinInfo
{
    char handler[MaxHandlerLen + 1] { 0 }; // + '\0'
    std::size_t frequency{ 0u };
    std::size_t lastTimeUsed{ 0u };
};

struct HandlerUsageInfoRT
{
    std::string const handler;
    std::size_t const usageAmount;
    Clock_Type::time_point const lastTimeUsed;
};

class FileError : public std::exception
{
public:
    FileError(std::string filename, std::string what, const std::source_location location = std::source_location::current())
        : _filename(std::move(filename))
        , _what(std::move(what))
        , _location(std::move(location))
    {}

public:
    virtual std::string const getName() const noexcept final { return _filename; } // ! final noexcept
    virtual const char* what() const noexcept { return _what.c_str(); }  // ! noexcept

    std::string const say() const;

private:
    std::string const _filename;
    std::string const _what;

    std::source_location _location;
};

bool InitDatabase()
{
    std::ifstream is{ UnparsedHandlersFilename };

    if (!is)
    {
        throw FileError{UnparsedHandlersFilename, "std::ifstream can't open"};
    }

    std::cout << std::format("Opened {} for reading.", UnparsedHandlersFilename) << std::endl;

    // holding those @twats
    std::vector< std::string > handlers;
    handlers.reserve(DefaultVectorCapacity);

    handlers.push_back("@VP"); // special handler;
    handlers.push_back("@UN"); // special handler;
    handlers.push_back("@u2"); // special handler;
    handlers.push_back("@jlo"); // special handler;
    handlers.push_back("@bbc"); // special handler;
    handlers.push_back("@cnn"); // special handler;

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
    }

    std::vector< HandlerFreqTimeBinInfo > dataToWrite;
    dataToWrite.reserve(handlers.size());

    for (auto const& handler : handlers)
    {
        HandlerFreqTimeBinInfo data;
        for (std::size_t i = 0u; i < handler.size() + 1; ++i)
            data.handler[i] = handler[i];
        data.frequency = 0u;
        data.lastTimeUsed = 0u;

        dataToWrite.push_back(data);
    }

    std::ofstream os{ HandlersUsageDataBase, std::ios::binary };
    if (!os.is_open()) 
        return false;

    for (auto const& data : dataToWrite)
        os.write(reinterpret_cast<char const*>(&data), sizeof data);

    os.close();

    return true;
}

// return pair <bool, container> where bool is a flag is DB read was ok, and contaier holds DB records
auto ReadDB()
{
    std::ifstream is{ HandlersUsageDataBase, std::ios::binary };
    std::cout << std::format("\t\tStream state:\n\t\t\tgood {}\n\t\t\tbad {}\n\t\t\tfail {}\n\t\t\teof {}", is.good(), is.bad(), is.fail(), is.eof()) << std::endl;

    std::pair< bool, std::vector< HandlerUsageInfoRT > > result{ true, {} };
    auto& dbReadOk = result.first;
    auto& records = result.second;
    records.reserve(DefaultVectorCapacity);

    while (true)
    {
        HandlerFreqTimeBinInfo data;
        if (!is.read(reinterpret_cast<char*>(&data), sizeof data))
        {
            if (is.eof())
                return result;

            std::cout << std::format("\tError reading data-base entry.") << std::endl;
            std::cout << std::format("Stream state:\ngood {}\nbad {}\nfail {}\neof {}", is.good(), is.bad(), is.fail(), is.eof()) << std::endl;
            dbReadOk = false;
            return result;
        }

        std::string const handler = data.handler;
        std::size_t const usageAmount = data.frequency;
        Clock_Type::time_point const timeLastUsed{ Clock_Type::duration{ data.lastTimeUsed } };

        records.push_back({ handler, usageAmount, timeLastUsed });
    }

    return result;
} 

int app()
{
    if (!std::filesystem::exists( HandlersUsageDataBase ))
    {
        try {
            InitDatabase();
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {

        }
    }

    try {
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {

    }

    bool databaseInited = true;
    if (!std::filesystem::exists( HandlersUsageDataBase ))
    {
        std::cout << std::format("Can't open {} database file.\n Will try to init new database.", HandlersUsageDataBase) << std::endl;
        databaseInited = InitDatabase();
    }

    if (!databaseInited)
    {
        std::cout << std::format("\tCan't initialize data-base. Exiting app.") << std::endl;
        return 1;
    }

    std::cout << std::format("DB is ready.") << std::endl;
    std::cout << std::format("Reading data-base.") << std::endl;

    auto [result, records] = ReadDB();
    if (!result)
    {
        std::cout << std::format("\tError reading DB. Entries read {}. Exiting app.", records.size()) << std::endl;
        return 3;
    };

    std::cout << std::format("\nDatabase read succeed. Entries read {}.", records.size()) << std::endl;

    return 0;
}

int main()
{
    try {
        return app();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unhandled exception!" << std::endl;
    }


    // LS: show tags based on frequency of usage

}