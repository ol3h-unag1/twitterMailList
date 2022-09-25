export module FileErrorException;

import <exception>;
import <string>;
import <source_location>;


export class Error : public std::exception
{
public:
    explicit Error(std::string what, std::source_location location = std::source_location::current());

public:
    virtual const char* what() const noexcept { return _what.c_str(); }  // ! noexcept
    virtual std::string const say() const noexcept { return _message; }

private:
    std::string const _what;
    std::string _message;
    std::source_location const _location;
};


export class FileError : public Error
{
public:
    explicit FileError(std::string filename, std::string what, std::source_location location = std::source_location::current());

public:
    virtual std::string const getFileName() const noexcept final { return _filename; } // ! final noexcept

private:
    std::string const _filename;
};