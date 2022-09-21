module FileErrorException;

import <format>;

FileError::FileError(std::string filename, std::string what, std::source_location location)
    : _filename(std::move(filename))
    , _what(std::move(what))
    , _message(std::format("{}: {} at {}:{} in module {}", _what, _filename, location.file_name(), location.line(), location.function_name()))
    , _location(std::move(location))
{}
 