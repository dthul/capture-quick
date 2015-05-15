#pragma once

#include <string>

#include <QString>

namespace util
{

std::string executable_path();
void setenv(std::string name, std::string value);
uint64_t getId();
void showFile(const QString& filePath);

}
