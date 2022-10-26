export module DBHandler;

import <vector>;
import <string>;

import <optional>;

import DataModel;
import Log;

export namespace DataBase
{
    // handles reads and writes to DB
    class Handler
    {
        Handler() noexcept;

    public:
        [[nodiscard]] std::vector< DataModel::Data > load();

        void save(std::vector< DataModel::Data > const&) const;

        // checks if database file exists
        bool isBinaryExists() const noexcept(false);

        bool isOk() const { return isBinaryExists() and _cache.size(); }

    private:
        // parse twitter handlers from file and create bin file of database
        // handlers are written fully, times/lastTime is set to 0
        void initDatabase() noexcept(false);

        std::vector< std::string > getSpecialHandlers() const;
        
    private:
        // clears loaded data
        void clearCache();

        bool writeToBinary(std::vector< DataModel::Raw > const& raw);

        std::optional< std::vector< DataModel::Data > > readFromBinary();

    private:
        std::string _dbFileName;
        std::string _unparsedHandlersFileName;

        std::vector< DataModel::Data > _cache;

        Log::Logger _log;
    };
};