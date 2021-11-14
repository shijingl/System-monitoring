#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(const long seconds);
std::string AppendZero(const long number);  
};                                    // namespace Format

#endif