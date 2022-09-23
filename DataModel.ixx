export module DataModel;

import <memory>;
import <string>;

export namespace Model
{
    class Data
    {
    public:
        // throws std::bad_optional_access if there's no value
        std::string GetString() const;

        // usually called before GetString()
        bool HasValue() const;

    private:
        class DataImpl;

    private:
        std::unique_ptr< DataImpl > _impl;
    };

}