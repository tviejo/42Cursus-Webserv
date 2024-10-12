/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:51:14 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/12 16:18:18 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"
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

Cgi::Cgi(std::string path, std::string method)
{
    this->_type = "python3";
    this->_path = path;
    this->_method = method;
    this->setEnv(path, method);
}

void    Cgi::setEnv(std::string path, std::string method)
{
    this->_env.push_back("REDIRECT_STATUS=200");
    this->_env.push_back("REQUEST_METHOD=" + method);
    this->_env.push_back("SCRIPT_FILENAME=" + path);
    this->_env.push_back("SCRIPT_NAME=" + path);
    this->_env.push_back("PATH_INFO=" + path);
    this->_env.push_back("PATH_TRANSLATED=" + path);
    this->_env.push_back("QUERY_STRING=");
    this->_env.push_back("REMOTE_ADDR=");
}

char    **Cgi::getEnvp()
{
    char **envp = new char*[this->_env.size() + 1];
    for (size_t i = 0; i < this->_env.size(); i++)
    {
        envp[i] = ft_strdup(this->_env[i].c_str());
    }
    envp[this->_env.size()] = NULL;
    return (envp);
}

void    Cgi::deleteEnvp(char **envp)
{
    for (size_t i = 0; i < this->_env.size(); i++)
    {
        delete [] envp[i];
    }
    delete [] envp;
}

void    Cgi::execute()
{
    char **envp = this->getEnvp();
    int pid = fork();
    if (pid == 0)
    {
        if (this->_type == "python3")
            execl("/usr/bin/python3", "/usr/bin/python3", this->_path.c_str(), NULL);
        else
            std::cerr << "Error: unknown type" << std::endl;
    }
    else
    {
        waitpid(pid, NULL, 0);
    }