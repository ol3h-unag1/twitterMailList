export module FileErrorException;

import <exception>;
import <string>;
import <source_location>;

export class FileError : public std::exception
{
public:
    explicit FileError(std::string filename, std::string what, std::source_location location = std::source_location::current())
        : _filename(std::move(filename))
        , _what(std::move(what))
        , _location(std::move(location))
    {}

public:
    virtual std::string const getName() const noexcept final { return _filename; } // ! final noexcept
    virtual const char* what() const noexcept { return _what.c_str(); }  // ! noexcept

    std::string const say() const;

private:
    std::string const _filename;
    std::string const _what;

    std::source_location _location;
};