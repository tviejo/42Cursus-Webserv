/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:51:14 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/24 17:55:02 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "cgi.hpp"

Cgi::Cgi()
{
}

Cgi::~Cgi()
{

}

Cgi::Cgi(const Cgi &copy)
{
    *this = copy;
}

Cgi &Cgi::operator=(const Cgi &copy)
{
    if (this != &copy)
    {
        this->_env = copy._env;
    }
    return (*this);
}

Cgi::Cgi(std::string path, std::string method, std::string info)
{
    this->_header = "";
    this->_contentLength = 0;
    this->_isDone = false;
    this->_type = "python3";
    this->_path = path;
    this->_method = method;
    this->_env = "name="+ info;
}



char    **Cgi::getEnvp()
{
    char **envp = new char*[2];
    envp[0] = strdup(this->_env.c_str());
    envp[1] = NULL;
    return (envp);
}

void    Cgi::deleteEnvp(char **envp)
{
    delete[] envp;
}

void    Cgi::execute()
{
    int fd[2];
    pipe(fd);
    int pid = fork();
    if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        char **envp = this->getEnvp();
        char *args[] = {strdup(this->_path.c_str()), NULL};
        execve(this->_path.c_str(), args, envp);
        exit(0);
    }
    else
    {
        close(fd[1]);
        char buffer[4096];
        size_t size = 0;
        while ((size = read(fd[0], buffer, 4096)) > 0)
        {
            this->_response += std::string(buffer, size);
            this->_contentLength += size;
        }
        close(fd[0]);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            throw std::runtime_error("Cgi script failed");
        else
        {
            this->_header = this->createHeader(200, "OK", "text/html", this->_contentLength);
            this->_isDone = true;
        }
    }
}

void    Cgi::CgiHandler()
{
    if (this->_path.empty())
    {
        throw std::runtime_error("Empty cgi path");
    }
    if (this->_path.find("/cgi-bin/") == std::string::npos)
    {
        throw std::runtime_error("Invalid cgi script");
    }
    if (std::strncmp(this->_path.c_str() + this->_path.size() - 3, ".py", 3) != 0)
    {
        throw std::runtime_error("Invalid cgi type");
    }
    if (access(this->_path.c_str(), F_OK) == -1)
    {
        throw std::runtime_error("Invalid cgi path");
    }
    if (this->_method == "GET")
    {
        this->execute();
    }
}

std::string Cgi::createHeader(size_t status, std::string message, std::string contentType, size_t contentLength)
{
    std::string header;
    std::string status_string;
    std::string contentLength_string;
    std::stringstream ssContentLength;
    std::stringstream ssStatus;
    ssContentLength << contentLength;
    contentLength_string = ssContentLength.str();
    ssStatus << status;
    status_string = ssStatus.str();
    header += "HTTP/1.1 " + status_string + " " + message + "\r\n";
    header += "Content-Type: " + contentType + "\r\n";
    header += "Content-Length: " + contentLength_string + "\r\n";
    header += "\r\n";
    return (header);
}

// int main()
// {
//     Cgi cgi("./cgi-bin/test.py", "GET", "name=thomas");
//     try
//     {
//         cgi.CgiHandler();
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
//     std::cout << cgi.GetHeader() << std::endl;
//     std::cout << cgi.GetResponse() << std::endl;
//     return (0);
// }