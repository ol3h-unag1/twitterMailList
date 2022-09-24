export module DataModel;

import <cstddef>;

import <string>;

export namespace DataModel
{
    class Data
    {
    public:
        Data() = default;
        Data(Data&&) noexcept = default;
        Data& operator=(Data&&) noexcept = default;

        Data(std::string id, std::size_t times, std::size_t last)
            : _twitterId(std::move(id))
            , _timesUsed(times)
            , _lastUsed(last)
        {}

    public:
        auto GetID() const { return _twitterId; }
        auto GetUsages() const noexcept { return _timesUsed; }
        auto GetLastTimeUsed() const noexcept { return _lastUsed; }

    private:
        std::string _twitterId;
        std::size_t _timesUsed{ 0u }; // times id was used already; amount
        std::size_t _lastUsed{ 0u }; // last time id was used; date
    };
}