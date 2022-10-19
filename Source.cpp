import <vector>;

import <iostream>;
import <fstream>;

import <string>;
import <string_view>;

import <memory>;

import <format>;

import <algorithm>;

import <chrono>;

import <exception>;

import <filesystem>;

import <source_location>;

import FileErrorException;
import Log;

Log::Logger* gLog{ nullptr };

using Clock_Type = std::chrono::system_clock;

std::string const UnparsedHandlersFilename{ "messages.html" }; 
std::string const HandlersUsageDataBase{ "hudb.bin" };

auto const HandlerStartSymbol = '@';

std::size_t const MinHandlerLen{ 4u + 1u };  // counting '@' as part of ID string
std::size_t const MaxHandlerLen{ 15u + 1u }; // counting '@' as part of ID string
std::size_t const DefaultVectorCapacity{ 1024u };

auto WORKAROUND1630686_SOURCE_CPP{ std::source_location::current() }; // https://developercommunity.visualstudio.com/t/internal-compiler-error-in-msvc-compiler-in-visual/1630686

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

// parses handlers from text file
// creates binary database file 
// 
// throws FileError:    
//      - if can't read unparsed handlers file
//      - can't open/create binary
void InitDatabase()
{
    std::ifstream is{ UnparsedHandlersFilename };

    if (!is)
        throw FileError{ UnparsedHandlersFilename, "std::ifstream can't open" };

    *gLog << std::format("InitDatabase: Opened {} for reading.", UnparsedHandlersFilename);

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
    } // check input stream state after the loop?

    *gLog << std::format("\tRead {} handlers. Preparing to create {}", handlers.size(), HandlersUsageDataBase);

    std::vector< HandlerFreqTimeBinInfo > dataToWrite;
    dataToWrite.reserve(handlers.size());

    for (auto const& h : handlers)
    {
        HandlerFreqTimeBinInfo data;
        for (std::size_t i = 0u; i < h.size() + 1; ++i)
        {
            data.handler[i] = h[i];
        }

        data.frequency = 0u;
        data.lastTimeUsed = 0u;

        dataToWrite.push_back(data);
    }

    std::ofstream os{ HandlersUsageDataBase, std::ios::binary };
    if (!os.is_open())
        throw FileError{ HandlersUsageDataBase, "std::ofstream can't open file (std::ios::binary)" };

    for (auto const& data : dataToWrite)
        os.write(reinterpret_cast<char const*>(&data), sizeof data);

    // check output stream state after the loop?
    os.close(); // unnecessary

    *gLog << std::format("\t{} records saved to {}", dataToWrite.size(), HandlersUsageDataBase);
}

// return pair <bool, container> where bool is a flag is DB read was ok, and contaier holds DB records
auto ReadDB()
{
    std::ifstream is{ HandlersUsageDataBase, std::ios::binary };
    if (!is)
        throw FileError{ HandlersUsageDataBase, "std::ifstream can't open file (std::ios::binary)" };

    *gLog << std::format("ReadDB {}: Stream state:\n\tgood {}\n\tbad {}\n\tfail {}\n\tteof {}", HandlersUsageDataBase, is.good(), is.bad(), is.fail(), is.eof());

    std::vector< HandlerUsageInfoRT > records{};
    records.reserve(DefaultVectorCapacity);

    while (is)
    {
        HandlerFreqTimeBinInfo data;
        if (!is.read(reinterpret_cast<char*>(&data), sizeof data))
        {
            if (is.eof())
                return records;

            *gLog << std::format("\tError reading data-base entry. Stream state:\n\tgood {}\n\tbad {}\n\tfail {}\n\teof {}", is.good(), is.bad(), is.fail(), is.eof());
            throw FileError{ HandlersUsageDataBase, "std::ifstream error reading data"};
        }

        std::string const handler = data.handler;
        std::size_t const usageAmount = data.frequency;
        Clock_Type::time_point const timeLastUsed{ Clock_Type::duration{ data.lastTimeUsed } };

        records.push_back({ handler, usageAmount, timeLastUsed });
    } // check input stream state after the loop?


    return records;
} 

int app()
{
    Log::Logger log;
    gLog = &log;

    if (!std::filesystem::exists( HandlersUsageDataBase ))
    {
        try {
            InitDatabase();
        }
        catch (FileError& e) {
            std::cout << e.say() << std::endl;
            return 1;
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            return 1;
        }
        catch (...) {
            std::cout << "Unhandled exception!" << std::endl;
            return 1;
        }
    }

    auto records{ ReadDB()};
    *gLog << std::format("\nDatabase read succeed. Entries read {}.\n", records.size());

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


    // Designing app
    // 
    // LS: show tags based on frequency of usage   

}