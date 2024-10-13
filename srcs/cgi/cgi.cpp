/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:51:14 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/13 16:12:58 by tviejo           ###   ########.fr       */
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

Cgi::Cgi(std::string path, std::string method)
{
    this->_type = "python3";
    this->_path = path;
    this->_method = method;
}


char    **Cgi::getEnvp()
{
    char **envp = new char*[100];
    for (size_t i = 0; i < 1; i++)
    {
        envp[i] = strdup("name=thomas");
    }
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
        char *args[] = {strdup(this->_path.c_str()), NULL};
        execve(this->_path.c_str(), args, envp);
        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
    this->deleteEnvp(envp);
}

// int main()
// {
//     Cgi cgi("./cgi-bin/test.py", "GET");
//     cgi.execute();
//     return (0);
// }