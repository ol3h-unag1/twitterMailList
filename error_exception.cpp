module FileErrorException;

import <format>;

Error::Error(std::string_view what, std::source_location location)
    : std::exception( what.data() )
    , _message(std::format("{} at {}:{} in module {}", what, location.file_name(), location.line(), location.function_name()))
    , _location(std::move(location))
{}

FileError::FileError(std::string filename, std::string what, std::source_location location)
    : Error(std::move(filename + ": " + what), std::move(location))
    , _filename(std::move(filename))
{}
 