/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:59 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/13 16:07:38 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <cstdlib>
# include <vector>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>

class Cgi
{
    private:
        Cgi();
        Cgi(const Cgi &copy);
        Cgi &operator=(const Cgi &copy);
        char **getEnvp();
        void    deleteEnvp(char **envp);
        std::vector<std::string> _env;
        std::string _path;
        std::string _method;
        std::string _type;
    public:
        Cgi(std::string path, std::string method);
        ~Cgi();
        void execute();
        
};

#endif