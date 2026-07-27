#ifndef HOTCHLIB_API_PAIR_H
#define HOTCHLIB_API_PAIR_H

#include "hlib/Allocator.h"

namespace hlib {
    template <typename key_t, typename value_t> 
    struct CPair {
        key_t Key;
        value_t Value;
        
        CPair() = default;
        CPair(const key_t& k, const value_t& v) : Key(k), Value(v) {}
        CPair(key_t&& k, value_t&& v) : Key(std::move(k)), Value(std::move(v)) {}

        CPair(CPair&& other) : Key(std::move(other.Key)), Value(std::move(other.Value)) {}
        
        bool operator==(const CPair& other) const { return Key == other.Key; }
        bool operator<(const CPair& other) const { return Key < other.Key; }

        CPair& operator=(const CPair& other) {
            Key = other.Key;
            Value = other.Value;
            return *this;
        }

        /*CPair& operator=(CPair&& other) {
            

            memset(&other, 0, sizeof(other));
            return *this;
        }*/
    };
}

#endif
