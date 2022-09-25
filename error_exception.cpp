module FileErrorException;

import <format>;

Error::Error(std::string what, std::source_location location)
    : _what(std::move(what))
    , _message(std::format("{} at {}:{} in module {}", _what, location.file_name(), location.line(), location.function_name()))
    , _location(std::move(location))
{}


FileError::FileError(std::string filename, std::string what, std::source_location location)
    : Error(std::move(filename + ": " + what), std::move(location))
    , _filename(std::move(filename))
{}
 