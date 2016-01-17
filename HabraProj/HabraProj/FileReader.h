#pragma once


class FileReader
{
    std::string mResult;
public:
    bool readAll(const std::string& path_in, std::string& content_out);
    bool getText(std::string& text_out);
};//FileReader
