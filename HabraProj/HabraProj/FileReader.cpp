#include "stdafx.h"
#include "FileReader.h"

bool FileReader::readAll(const std::string& path_in, std::string& content_out)
{
    std::ifstream file;
    if (!content_out.empty())
    {
        content_out.clear();
    }
    file.open(path_in.c_str(), std::ios::out);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        content_out += line + "\n";
    }

    file.close();
    return true;
}

bool FileReader::getText(std::string& text_out)
{
    if (mResult.empty())
    {
        std::cout << "class does not contain any data";
        return false;
    }
    
    text_out = mResult;
    return true;
}
