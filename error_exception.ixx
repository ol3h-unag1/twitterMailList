export module FileErrorException;

import <exception>;

import <string>;
import <string_view>;

import <source_location>;


export class Error : public std::exception
{
public:
    explicit Error(std::string_view what, std::source_location location = std::source_location::current());

public:
    char const* what() const override { return say().data(); }
    virtual std::string_view say() const noexcept { return _message; }

private:
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