/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:51:14 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/04 03:20:15 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "cgi.hpp"

volatile sig_atomic_t timeout_flag = 0;

void alarm_handler(int signum)
{
	(void)signum;
	timeout_flag = 1;
}

/*Cgi::Cgi()
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
}*/

std::string &replaceChar(std::string& str, char toFind, char toReplace)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == toFind)
			str[i] = toReplace;
	}
	return str;
}

Cgi::Cgi(std::string execPath, const HTTPRequest &req, int clientId) : _request(req)
{
	this->_header = "";
	this->_contentLength = 0;
	this->_isDone = false;
	this->_execPath = execPath;
	_clientId = clientId;
	_isUserNameCgi = execPath.find("name.py") != std::string::npos;
	
	std::string Qstring;
	if (req.get_method() == "GET") {
		Qstring = req.getFirstQueryString();
	}
	else if (req.get_method() == "POST") {
		Qstring = req.getFirstFormField();
	}
	if (Qstring.empty())
		_env = "";
	else {
		if (_isUserNameCgi)
			_env = "UserName=" + replaceChar(Qstring, '+', ' ');
		else
			_env = "QueryString=" + Qstring;
	}
}

char **Cgi::getEnvp()
{
	const map_str &map = (_request.get_method() == "GET") ?
		  _request.getQueryStrings()
		: _request.getForm();

	_nbVarsEnv = map.size() + 1;
	char **envp = new char*[_nbVarsEnv + 1];
	envp[0] = strdup(this->_env.c_str());
	int i = 1;
	for (map_str_cite it = map.begin(); it != map.end(); it++, i++)
	{
		envp[i] = strdup((it->first + "=" + it->second).c_str());
	}
	envp[i] = NULL;
	return envp;
}

void Cgi::deleteEnvp(char **envp)
{
	if (envp)
	{
		for (int i = 0; i < _nbVarsEnv; i++)
			free(envp[i]);
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
		char *args[] = {strdup(_execPath.c_str()), NULL};	// is strup() needed for args[0] ?
		execve(_execPath.c_str(), args, envp);				// and why not strdup() here ?
		free(args[0]);										// added missing free (isn't it ?)
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
			_header = this->createHeader(200, "OK", "text/html", this->_contentLength, _isUserNameCgi ? _env : "");
			_isDone = true;
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
	if (this->_execPath.empty())
	{
		throw std::runtime_error("Empty cgi path");
	}
	if (this->_execPath.find("/cgi-bin/") == std::string::npos)
	{
		throw std::runtime_error("Invalid cgi script");
	}
	if (access(this->_execPath.c_str(), F_OK) == -1)
	{
		throw std::runtime_error("Invalid cgi path");
	}
	const std::string method = _request.get_method();
	if (method == "GET" || method == "POST" /*|| method == "DELETE"*/)
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

class OutgoingData *Cgi::handleCgi()
{
	try
	{
		this->CgiHandler();
	}
	catch (const TimeoutException& e)
	{
		std::cerr << e.what() << '\n';
		return Response::makeErrorResponse(504, "Gateway Timeout", _request.getServer(), _clientId);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return Response::makeErrorResponse(500, "Internal Server Error", _request.getServer(), _clientId);
	}
	return new OutgoingData(this->_header, this->_response);
}