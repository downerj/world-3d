#ifndef IO_HXX
#define IO_HXX

#include <string>
#include <string_view>

auto readFile(std::string_view filePath) -> std::string;

#endif // IO_HXX
