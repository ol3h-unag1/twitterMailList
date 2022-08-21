import <vector>;

import <iostream>;
import <fstream>;

import <string>;
import <format>;

import <algorithm>;

import <chrono>;

import <exception>;

import <filesystem>;

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

bool InitMailList()
{
    std::ifstream is{ UnparsedHandlersFilename };

    if (!is)
    {
        std::cout << std::format("Can't open {}", UnparsedHandlersFilename) << std::endl;
        return false;
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

bool ReadDB(std::vector< HandlerUsageInfoRT >& readTo)
{
    std::ifstream is{ HandlersUsageDataBase, std::ios::binary };
    std::cout << std::format("\t\tStream state:\n\t\t\tgood {}\n\t\t\tbad {}\n\t\t\tfail {}\n\t\t\teof {}", is.good(), is.bad(), is.fail(), is.eof()) << std::endl;

    if(readTo.capacity() < DefaultVectorCapacity)
        readTo.reserve(DefaultVectorCapacity);

    while (true)
    {
        HandlerFreqTimeBinInfo data;
        if (!is.read(reinterpret_cast<char*>(&data), sizeof data))
        {
            if (is.eof())
                return true;

            std::cout << std::format("\tError reading data-base entry.") << std::endl;
            std::cout << std::format("Stream state:\ngood {}\nbad {}\nfail {}\neof {}", is.good(), is.bad(), is.fail(), is.eof()) << std::endl;
            return false;
        }

        std::string const handler = data.handler;
        std::size_t const usageAmount = data.frequency;
        Clock_Type::time_point const timeLastUsed{ Clock_Type::duration{ data.lastTimeUsed } };

        readTo.push_back({ handler, usageAmount, timeLastUsed });
    }

    return true;
} 

int main()
{
   bool databaseInited = true;
   if (!std::filesystem::exists({ HandlersUsageDataBase }))
   {
       std::cout << std::format("Can't open {} data-base file.\nInitializaiton...", HandlersUsageDataBase) << std::endl;
       databaseInited = InitMailList();
   }

   if (!databaseInited)
   {
       std::cout << std::format("\tCan't initialize data-base. Exiting app.") << std::endl;
       return 1;
   }

   std::cout << std::format("\tInitializarion complete.") << std::endl;

   std::cout << std::format("Reading data-base.") << std::endl;
   std::vector< HandlerUsageInfoRT > huirt;
   if (!ReadDB(huirt))
   {
       std::cout << std::format("\tError reading DB. Entries read {}. Exiting app.", huirt.size()) << std::endl;
       return 3;
   };

   std::cout << std::format("\nDatabase read succeed. Entries read {}.", huirt.size()) << std::endl;

    // LS: show tags based on frequency of usage

}