#include <iostream>
#include <cstdlib>
#include <string>
#include <stdio.h>

#define FOLDER "./www/html/upload-folder/"

std::string getEnvVar(std::string const &key)
{
    char *val = std::getenv(key.c_str());
    return val == NULL ? std::string() : std::string(val);
}

int main()
{
    bool isRemoved = true;

    std::string file = getEnvVar("QueryString");
    if (file != "")
    {
        isRemoved = false;
    }
    else if (std::remove((FOLDER + file).c_str()) != 0)
        isRemoved = false;

    std::cout << "<!DOCTYPE html>\n";
    std::cout << "<html lang=\"en\">\n";
    std::cout << "<head>\n";
    std::cout << "    <meta charset=\"UTF-8\">\n";
    std::cout << "    <title>CGI Response</title>\n";
    std::cout << "    <style>\n";
    std::cout << "        body {\n";
    std::cout << "            font-family: 'Arial', sans-serif;\n";
    std::cout << "            background-color: #f4f4f4;\n";
    std::cout << "            text-align: center;\n";
    std::cout << "            padding-top: 70px;\n";
    std::cout << "        }\n";
    std::cout << "        h1 {\n";
    std::cout << "            font-size: 2.5rem;\n";
    std::cout << "            color: #333;\n";
    std::cout << "        }\n";
    std::cout << "        p {\n";
    std::cout << "            font-size: 1.2rem;\n";
    std::cout << "            color: #666;\n";
    std::cout << "            margin-bottom: 20px;\n";
    std::cout << "        }\n";
    std::cout << "        a {\n";
    std::cout << "            color: #42b883;\n";
    std::cout << "            text-decoration: none;\n";
    std::cout << "            font-size: 1rem;\n";
    std::cout << "        }\n";
    std::cout << "        a:hover {\n";
    std::cout << "            text-decoration: underline;\n";
    std::cout << "        }\n";
    std::cout << "    </style>\n";
    std::cout << "</head>\n";
    std::cout << "<body>\n";

    if (isRemoved == true)
    {
        std::cout << "    <h1>File deleted successfully!</h1>\n";
    }
    else
    {
        std::cout << "    <h1>File not found!</h1>\n";
    }
    std::cout << "    <p><a href=\"../index.html\">Go back to Home</a></p>\n";


    std::cout << "</body>\n";
    std::cout << "</html>\n";

    return 0;
}