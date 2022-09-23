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

        DataImpl(std::string);

    private:
        std::optional< std::string > _value;
    };

    Data::DataImpl::DataImpl(std::string)
    {
    }
}