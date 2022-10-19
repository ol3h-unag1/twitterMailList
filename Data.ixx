export module DataModel;

import <cstddef>;

import <string_view>;

import <chrono>;

import <unordered_set>;

using std::begin;
using std::end;

export namespace DataModel
{
    auto const HandlerStartSymbol = '@';

    // counting '@' as part of ID string
    std::size_t const MinHandlerLen{ 4u + 1u }; 

    // counting '@' and as part of ID string
    std::size_t const MaxHandlerLen{ 15u + 1u }; 

    auto isCorrectHandlerLen = 
        [](auto const& len)
    {
        return 
            MinHandlerLen <= len 
            && 
            len <= MaxHandlerLen;
    };

    auto isAllowedHandlerSymbol = 
        [](auto const& symbol)
    {
        // big/small letters, digits and underscore with @
        static std::string_view const config{ 
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_@"
        };

        static std::unordered_set< std::string_view::value_type > const allowed {
            begin(config),
            end(config)
        };

        return allowed.contains(symbol);
    };

    struct Raw
    {
        char handler[MaxHandlerLen + 1]{ 0 }; // + '\0'
        std::size_t timesUsed{ 0u };
        std::size_t lastTimeUsed{ 0u };
    };

    class Data
    {
    public:
        using clock_type = std::chrono::system_clock;
        using time_point_type = clock_type::time_point;
        using duration_type = clock_type::duration;

    public:
        Data() = default;

        Data(Data const&) = default;
        Data& operator=(Data const&) noexcept = default;

        Data(Data&&) noexcept = default;
        Data& operator=(Data&&) noexcept = default;

    public:
        Data(std::string_view id, std::size_t times, std::size_t last)
            : _twitterId(std::move(id))
            , _usagesCount(times)
            , _lastUsed(duration_type{ last })
        {}

        Data(Raw const& raw)
            : Data(
                raw.handler,
                raw.timesUsed,
                raw.lastTimeUsed
            )
        {}

    public:
        std::string_view getID() const noexcept { return _twitterId; }
        auto getUsagesCount() const noexcept { return _usagesCount; }
        auto getLastTimeUsed() const noexcept { return _lastUsed; }

    private:
        std::string _twitterId;

        // times id was used already
        std::size_t _usagesCount{ 0u }; 

        // last time id was used
        time_point_type _lastUsed{ duration_type { 0u } }; 
    };
}