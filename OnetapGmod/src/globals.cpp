#include "globals.h"

constexpr int bool_idx = 0;
constexpr int int_idx = 1;

void globals::c_entity_list::push_back(const std::string& c)
{
    //std::unique_lock<std::mutex> l(mutex);
    if (!exist(c))
        classes.push_back(c);
}

void globals::c_entity_list::remove(int idx)
{
    //std::unique_lock<std::mutex> l(mutex);
    classes.erase(classes.begin() + idx);
}

bool globals::c_entity_list::exist(const std::string& c)
{
    //std::lock_guard<std::mutex> l(mutex);
    return std::find(classes.begin(), classes.end(), c) != classes.end();
}

int globals::c_entity_list::find(const std::string& c)
{
    //std::lock_guard<std::mutex> l(mutex);
    if (!classes.empty())
        for (auto i = 0; i < classes.size(); ++i)
            if (classes.at(i) == c)
                return i;

    return -1;
}

void globals::c_entity_list::exchange(const std::vector<std::string>& c)
{
    //std::unique_lock<std::mutex> l(mutex);
    classes = c;
}

bool globals::c_entity_list::empty()
{
    //std::lock_guard<std::mutex> l(mutex);
    return classes.empty();
}

void globals::c_entity_list::clear()
{
    classes.clear();
}

size_t globals::c_entity_list::size()
{
    return classes.size();
}

std::vector<std::string> globals::c_entity_list::data()
{
    return classes;
}