export module DataModel;

import <memory>;

export namespace Model
{
    class Data
    {
    public:


    private:
        class DataImpl;

    private:
        std::unique_ptr< DataImpl > _impl;
    };
}