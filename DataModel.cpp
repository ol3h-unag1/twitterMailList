module DataModel;

import <optional>;
import <string>;
import <exception>;

namespace Model
{
    class Data::DataImpl
    {
    public: 
        DataImpl() = default;
        DataImpl(DataImpl&&) noexcept = default;
        DataImpl& operator=(DataImpl&&) noexcept = default;

        DataImpl(std::string);

    public:
        std::string GetValueImpl() const;
        bool HasValueImpl() const noexcept;

    private:
        std::optional< std::string > _value;
    };

    Data::DataImpl::DataImpl(std::string value)
        : _value{ std::move( value ) }
    {}

    std::string Data::DataImpl::GetValueImpl() const
    {
        return _value.value();;
    }

    bool Data::DataImpl::HasValueImpl() const noexcept
    {
        return _value.has_value();
    }
    
    /// 
    /// 
    /// 
    ///
    
    std::string Data::GetString() const
    {
        return _impl->GetValueImpl();
    }

    bool Data::HasValue() const
    {
        return _impl->HasValueImpl();
    }
}
