module DataModel;

import <optional>;
import <string>;

namespace Model
{
    class Data::DataImpl
    {
    public: 
        DataImpl() = default;
        DataImpl(DataImpl&&) = default;
        DataImpl& operator=(DataImpl&&) = default;

    private:
        std::optional< std::string > _value;
    };
}