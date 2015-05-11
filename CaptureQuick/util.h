#pragma once

#include <string>

namespace util
{

std::string executable_path();
void setenv(std::string name, std::string value);

}
