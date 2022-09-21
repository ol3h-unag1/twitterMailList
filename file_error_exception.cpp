module FileErrorException;

FileError::FileError(std::string filename, std::string what, std::source_location location)
: _filename(std::move(filename))
, _what(std::move(what))
, _location(std::move(location))
{}
