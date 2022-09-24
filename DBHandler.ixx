export module DBHandler;

import <vector>;

import DataModel;

export namespace DataBase
{
    // handles reads and writes to DB
    class Handler
    {
    public:
        std::vector< DataModel::Data > Load() const;

        void Save(std::vector< DataModel::Data > const&) const;
    };
};