#include "util.h"

#include <atomic>
#include <iostream>
#include <stdexcept>

#include <QProcess>
#include <QStringList>

#ifdef _WIN32
    //define something for Windows (32-bit and 64-bit, this part is common)
    #include <stdlib.h>
    #ifdef _WIN64
        //define something for Windows (64-bit only)
    #endif
#elif __APPLE__
    #include <mach-o/dyld.h>
    #include <stdlib.h>
#elif __linux
    // linux
#elif __unix // all unices not caught above
    // Unix
#elif __posix
    // POSIX
#endif

namespace util
{

std::string executable_path() {
#ifdef __APPLE__
    uint32_t buf_size = 0;
    if (_NSGetExecutablePath(nullptr, &buf_size) != -1)
        throw new std::runtime_error("Could not query the buffer size needed to query the executable path");
    char buf[buf_size];
    if (_NSGetExecutablePath(buf, &buf_size) != 0)
        throw new std::runtime_error("Could not query the executable path");
    char* const real_path_c = realpath(buf, nullptr);
    if (!real_path_c)
        throw new std::runtime_error("Could not canonicalize the executable path");
    const std::string real_path(real_path_c);
    free(real_path_c);
    return real_path;
#else
#error "util::executable_path() is not implemented for this operating system"
#endif
}

void setenv(std::string name, std::string value) {
#if defined(__APPLE__) || defined(__unix)
    if (::setenv(name.c_str(), value.c_str(), 1) != 0)
        throw new std::runtime_error("Could not set environment variable");
#elif defined(_WIN32)
    if (_putenv_s(name.c_str(), value.c_str()) != 0)
        throw new std::runtime_error("Could not set environment variable");
#else
#error "util::setenv() is not implemented for this operating system"
#endif
}

// see: http://lynxline.com/show-in-finder-show-in-explorer/
void showFile(const QString &filePath) {
#if defined(__APPLE__)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \""+filePath+"\"";
    args << "-e";
    args << "end tell";
    QProcess::startDetached("osascript", args);
#elif defined(_WIN32)
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(filePath);
    QProcess::startDetached("explorer", args);
#else
#warning "util::showFile() not implemented for this operating system"
    std::cout << "util::showFile() not implemented for this operating system" << std::endl;
#endif

}

std::atomic<uint64_t> gId(0);

uint64_t getId() {
    return std::atomic_fetch_add(&gId, 1ull);
}

}

