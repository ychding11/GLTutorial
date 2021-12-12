//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//


#pragma once

#include <string>
#include <vector>
#include <map>
#include <Log.h>

inline std::vector<std::string> split(const std::string& text, char sep)
{
    std::vector<std::string> tokens;
    int start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos)
    {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

using int_c_array = int[];

#define MyEnum(enumType, ...)   \
    enum enumType             \
    {                         \
        __VA_ARGS__           \
    };                        \
    static const int enumType##Size = (sizeof(int_c_array{__VA_ARGS__})/sizeof(int));                              \
    static const std::vector<std::string> enumType##StrVector = split(#__VA_ARGS__, ',');                       \
    inline const std::vector<std::string>& enumType##ToStrVector(const enumType value)                          \
    {                                                                                                           \
        return enumType##StrVector;                                                                             \
    };                                                                                                          \
    inline const std::string enumType##ToString(const enumType value)                                           \
    {                                                                                                           \
        return enumType##StrVector.at(value);                                                                   \
    };                                                                                                          \
    inline std::map<std::string, enumType> enumType##ToStringMap(...)                                           \
    {                                                                                                           \
        std::map<std::string, enumType> m;                                                                      \
        enumType enumValue[enumType##Size] = { __VA_ARGS__ };                                                   \
        for(int i = 0; i < enumType##Size; ++i)                                                                 \
        {                                                                                                       \
            m[enumType##ToString(enumValue[i])] = enumValue[i];                                                 \
        }                                                                                                       \
        return m;                                                                                               \
    };                                                                                                          \
    static std::map<std::string, enumType> enumType##StrMap = enumType##ToStringMap(__VA_ARGS__);         \
    inline const enumType enumType##FromString(const std::string& str)                                           \
    {                                                                                                           \
        if(enumType##StrMap.count(str) == 0)                                                                    \
        {                                                                                                       \
            Err("No such enum {}", str);                                                                        \
        }                                                                                                       \
        return enumType##StrMap[str];                                                                         \
    };