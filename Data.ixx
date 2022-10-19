export module DataModel;

import <cstddef>;

import <string>;

import <chrono>;

export namespace DataModel
{
    class Data
    {
    public:
        using clock_type = std::chrono::system_clock;
        using time_point_type = clock_type::time_point;
        using duration_type = clock_type::duration;

    public:
        Data() = default;
        Data(Data&&) noexcept = default;
        Data& operator=(Data&&) noexcept = default;

        Data(std::string id, std::size_t times, std::size_t last)
            : _twitterId(std::move(id))
            , _timesUsed(times)
            , _lastUsed(duration_type{ last })
        {}

    public:
        auto GetID() const { return _twitterId; }
        auto GetUsages() const noexcept { return _timesUsed; }
        auto GetLastTimeUsed() const noexcept { return _lastUsed; }

    private:
        std::string _twitterId;
        std::size_t _timesUsed{ 0u }; // times id was used already; amount
        time_point_type _lastUsed{ duration_type { 0u } }; // last time id was used; date
    };
}