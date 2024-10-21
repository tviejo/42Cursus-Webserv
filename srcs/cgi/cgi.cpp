/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:51:14 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/21 11:35:00 by tviejo           ###   ########.fr       */
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
    this->_type = "python3";
    this->_path = path;
    this->_method = method;
    this->_env = info;
}



char    **Cgi::getEnvp()
{
    char **envp = new char*[1];
    envp[0] = strdup(this->_env.c_str());
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
        char **envp = this->getEnvp();
        char *args[] = {strdup(this->_path.c_str()), NULL};
        execve(this->_path.c_str(), args, envp);
        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
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

int main()
{
    Cgi cgi("./cgi-bin/test.py", "GET", "NAME=thomas");
    try
    {
        cgi.CgiHandler();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return (0);
}