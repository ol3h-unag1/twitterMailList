export module DBHandler;

import <vector>;
import <string>;

import DataModel;
import Log;

export namespace DataBase
{
    // handles reads and writes to DB
    class Handler
    {
        Handler() noexcept;
    public:
        std::vector< DataModel::Data > Load();

        void Save(std::vector< DataModel::Data > const&) const;

        // checks if database file exists
        bool IsInitialized() const noexcept(false);

    private:
        // parse twitter handlers from file and create bin file of database
        // handlers are written fully, times/lastTime is set to 0
        void InitDatabase() noexcept(false);

        std::vector< std::string > GetSpecialHandlers() const;

    private:
        std::string _dbFileName;
        std::string _unparsedHandlersFileName;

        std::vector< DataModel::Data > _loadedData;

        Log::Logger _log;
    };
};