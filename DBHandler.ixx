export module DBHandler;

import <vector>;

export namespace DataBase
{
    std::size_t const MinHandlerLen{ 4u + 1u };  // counting '@' as part of ID string
    std::size_t const MaxHandlerLen{ 15u + 1u }; // counting '@' as part of ID string
    std::size_t const DefaultVectorCapacity{ 1024u };

    class Data
    {
    private:
        char handler[MaxHandlerLen + 1]{ 0 }; // + '\0'
        std::size_t frequency{ 0u };
        std::size_t lastTimeUsed{ 0u };
    };

    // handles reads and writes to DB
    class Handler
    {
    public:
        std::vector< Data > Load() const;

        void Save(std::vector< Data > const&) const;
    };
};