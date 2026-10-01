#pragma once
#include <cstddef>
#include <cstring>
#include <vector>
class Preferences {
 public:
    inline static bool available = true, failWrite = false, corruptRead = false;
    inline static std::vector<unsigned char> bytes;
    bool begin(const char*, bool) { return available; }
    void end() {}
    size_t getBytesLength(const char*) { return bytes.size(); }
    size_t putBytes(const char*, const void* data, size_t n) {
        if (failWrite) return 0;
        auto p = static_cast<const unsigned char*>(data);
        bytes.assign(p, p+n); return n;
    }
    size_t getBytes(const char*, void* data, size_t n) {
        if (bytes.size() != n) return 0;
        std::memcpy(data, bytes.data(), n);
        if (corruptRead) static_cast<unsigned char*>(data)[0] ^= 1;
        return n;
    }
};
