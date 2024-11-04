/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 12:52:56 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/03 18:41:31 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>

// usefull shortcuts
typedef std::vector<std::string>			vec_str;
typedef vec_str::iterator					vec_str_ite;
typedef vec_str::const_iterator				vec_str_cite;

typedef std::map<std::string, std::string>	map_str;
typedef map_str::iterator					map_str_ite;
typedef map_str::const_iterator				map_str_cite;

#include "Array.hpp"
#include "../srcs/cgi/cgi.hpp"
#include "../srcs/config/config.hpp"
#include "../srcs/utils/timer.hpp"
#include "../srcs/utils/utils.hpp"
#include "../srcs/server/Server.hpp"
#include "../srcs/request/HttpRequest.hpp"
#include "../srcs/response/Response.hpp"
#include "../srcs/response/OutgoingData.hpp"
#include "../srcs/utils/Terminal.hpp"
