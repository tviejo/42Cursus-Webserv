#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

int main()
{
    time_t currentTime = time(0);
    char* dt = ctime(&currentTime);

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
    std::cout << "    <p>The current server time is: " << dt << "</p>\n";
    std::cout << "    <p><a href=\"../index.html\">Go back to Home</a></p>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";

    return 0;
}
