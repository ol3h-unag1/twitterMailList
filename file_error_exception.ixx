export module FileErrorException;

import <exception>;
import <string>;
import <source_location>;

export class FileError : public std::exception
{
public:
    explicit FileError(std::string filename, std::string what, std::source_location location = std::source_location::current());

public:
    virtual std::string const getName() const noexcept final { return _filename; } // ! final noexcept
    virtual const char* what() const noexcept { return _what.c_str(); }  // ! noexcept

    virtual std::string const say() const noexcept { return _message; }

private:
    std::string const _filename;
    std::string const _what;

    std::string const _message;

    std::source_location _location;
};