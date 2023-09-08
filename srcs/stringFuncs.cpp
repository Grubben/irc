#include <stringFuncs.hpp>

std::string ltrim(std::string str)
{
    std::size_t found = str.find_first_not_of(" \t");

    if (found == std::string::npos)
        return str;

    str = str.substr(found);

    return str;
}

std::string trim(std::string str)
{
    str = ltrim(str);

    std::size_t found = str.find_last_not_of(" \t");
    str = str.substr(0, found + 1);

    return str;
}

std::string trim(std::string str, std::string undesired)
{
    std::size_t found = str.find_first_not_of(undesired);

    str = str.substr(found);

    found = str.find_last_not_of(undesired);
    str = str.substr(0, found + 1);

    return str;
}


std::vector<std::string>    split(std::string str)
{
    std::vector<std::string>    divided;

    str = trim(str);

    while (std::size_t found = str.find(" "))
    {
        if (found == std::string::npos)
            break;

        divided.push_back(str.substr(0, found));

        str = str.substr(found);
        str = ltrim(str);
    }
    divided.push_back(str);
    return divided;
}
