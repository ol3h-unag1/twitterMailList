export module FileErrorException;

import <exception>;
import <string>;
import <source_location>;

export class FileError : public std::exception
{
public:
    FileError();

public:
    virtual std::string const& getName() const noexcept final { return _filename; } // ! final noexcept
    virtual const char* what() const noexcept { return _what.c_str(); }  // ! noexcept

    virtual std::string const& say() const noexcept { return _say; }  // ! noexcept

private:
    std::string const _filename;
    std::string const _what;

    std::source_location _location;
    std::string const _say;
};