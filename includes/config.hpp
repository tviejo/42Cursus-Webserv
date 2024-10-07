/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:57:38 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/07 13:24:38 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"

class Config
{
    private:
        std::vector<std::map<std::string, std::string>> _config;
        std::map<std::string, std::string> _server;

    public:
        Config(std::string file);
        ~Config();
};


#endif
