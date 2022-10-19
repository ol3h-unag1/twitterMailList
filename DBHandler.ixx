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
        std::vector< DataModel::Data > load();

        void save(std::vector< DataModel::Data > const&) const;

        // checks if database file exists
        bool isInitialized() const noexcept(false);

        bool isOk() const { return isInitialized() and _cache.size(); }

    private:
        // parse twitter handlers from file and create bin file of database
        // handlers are written fully, times/lastTime is set to 0
        void initDatabase() noexcept(false);

        std::vector< std::string > getSpecialHandlers() const;
        
    private:
        // clears loaded data
        void reset();

        // writes raw data to binary
        bool write(std::vector< DataModel::Raw > const& raw);

        // reads raw data from binary
        std::optional< std::vector< DataModel::Data > > read();

    private:
        std::string _dbFileName;
        std::string _unparsedHandlersFileName;

        std::vector< DataModel::Data > _cache;

        Log::Logger _log;
    };
};