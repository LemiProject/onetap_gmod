#include "globals.h"

#include "json.hpp"

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

void globals::on_save(nlohmann::json& out) {
    out["colors"]["colorfov"] = colorfov;
    out["colors"]["colortarger"] = colortarger;
    out["colors"]["colorespplayer"] = colorespplayer;
    out["colors"]["colorespentity"] = colorespentity;
    out["colors"]["colorfriend"] = colorfriend;
}

void savecolor(float* data, float* new_data) {
    data[0] = new_data[0];
    data[1] = new_data[1];
    data[2] = new_data[2];
    data[3] = new_data[3];
}

void globals::on_load(const nlohmann::json& in) {
    savecolor(colorfov, in["colors"]["colorfov"].get<std::array<float, 4>>().data());
    savecolor(colortarger, in["colors"]["colortarger"].get<std::array<float, 4>>().data());
    savecolor(colorespentity, in["colors"]["colorespentity"].get<std::array<float, 4>>().data());
    savecolor(colorfriend, in["colors"]["colorfriend"].get<std::array<float, 4>>().data());
    savecolor(colorespplayer, in["colors"]["colorespplayer"].get<std::array<float, 4>>().data());
}

