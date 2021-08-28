#pragma once
#include <vector>
#include <algorithm>
#include <string>


inline bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


__forceinline std::string replace_all(std::string str, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = str.find(search, pos)) != std::string::npos)
		str.replace(pos, search.length(), replace), pos += search.length();
	return str;
}


__forceinline void lowercase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::tolower(c); });
}