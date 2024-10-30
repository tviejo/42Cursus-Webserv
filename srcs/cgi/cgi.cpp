/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:51:14 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/27 12:43:01 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "cgi.hpp"

volatile sig_atomic_t timeout_flag = 0;

void alarm_handler(int signum)
{
    (void)signum;
    timeout_flag = 1;
}

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
    this->_path = path;
    this->_method = method;
    if (info.empty())
        this->_env = "";
    else
        this->_env = "QueryString=" + info;
}

char **Cgi::getEnvp()
{
    char **envp = new char*[2];
    envp[0] = strdup(this->_env.c_str());
    envp[1] = NULL;
    return envp;
}

void Cgi::deleteEnvp(char **envp)
{
    if (envp)
    {
        free(envp[0]);
        delete[] envp;
    }
}

void Cgi::execute()
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        throw std::runtime_error("Failed to create pipe");
    }
    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
    timeout_flag = 0;
    alarm(1);
    int pid = fork();
    if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);

        char **envp = this->getEnvp();
        char *args[] = {strdup(this->_path.c_str()), NULL};
        execve(this->_path.c_str(), args, envp);
        deleteEnvp(envp);
        exit(1);
    }
    else if (pid > 0)
    {
        close(fd[1]);
        char buffer[4096];
        size_t size = 0;
        while ((size = read(fd[0], buffer, 4096)) > 0)
        {
            if (timeout_flag)
                break;
            this->_response += std::string(buffer, size);
            this->_contentLength += size;
        }
        close(fd[0]);
        int status;
        if (!timeout_flag)
        {
            waitpid(pid, &status, 0); 
        }
        else
        {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            alarm(0);
            sigaction(SIGALRM, NULL, NULL);
            throw TimeoutException();
        }
        alarm(0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            throw TimeoutException();
        }
        else if (!timeout_flag)
        {
            this->_header = this->createHeader(200, "OK", "text/html", this->_contentLength, _env);
            this->_isDone = true;
        }

        std::cerr << _header << std::endl;
        std::cerr << _response << std::endl;
    }
    else
    {
        close(fd[0]);
        close(fd[1]);
        throw std::runtime_error("Fork failed");
    }
    sigaction(SIGALRM, NULL, NULL);
}

void Cgi::CgiHandler()
{
    if (this->_path.empty())
    {
        throw std::runtime_error("Empty cgi path");
    }
    if (this->_path.find("/cgi-bin/") == std::string::npos)
    {
        throw std::runtime_error("Invalid cgi script");
    }
    if (access(this->_path.c_str(), F_OK) == -1)
    {
        throw std::runtime_error("Invalid cgi path");
    }
    if (this->_method == "GET" || this->_method == "POST" || this->_method == "DELETE")
    {
        this->execute();
    }
}

std::string Cgi::createHeader(size_t status, std::string message, std::string contentType, size_t contentLength, std::string cookie)
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
    if (!cookie.empty())
        header += "Set-Cookie: " + cookie + ";path=/; Max-Age=3600;\r\n";
    header += "Content-Type: " + contentType + "\r\n";
    header += "Content-Length: " + contentLength_string + "\r\n";
    header += "\r\n";
    return header;
}

class OutgoingData *Cgi::handleCgi(std::string root, std::string error, int clientSocket)
{
    try
    {
        this->CgiHandler();
    }
    catch (const TimeoutException& e)
    {
        std::cerr << e.what() << '\n';
        return Response::makeErrorResponse(504, "Gateway Timeout", root, error, clientSocket);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return Response::makeErrorResponse(500, "Internal Server Error", root, error, clientSocket);
    }
    return new OutgoingData(this->_header, this->_response);
}