#ifndef IO_HXX
#define IO_HXX

#include <optional>
#include <string>
#include <string_view>

/*
 * Declarations.
 */

namespace my {

auto readFile(std::string_view filePath) -> std::optional<std::string>;

} // namespace my

#endif // IO_HXX
