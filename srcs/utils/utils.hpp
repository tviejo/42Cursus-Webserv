/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:05 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/03 17:12:29 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <sys/time.h>

inline std::string &trimRef(std::string &str, const std::string &charToTrim = " \t\n\r\f\v")
{
	str.erase(str.find_last_not_of(charToTrim) + 1);
	str.erase(0, str.find_first_not_of(charToTrim));
	return str;
}

inline std::string trim(std::string str, const std::string &charToTrim = " \t\n\r\f\v")
{
	str.erase(str.find_last_not_of(charToTrim) + 1);
	str.erase(0, str.find_first_not_of(charToTrim));
	return str;
}

bool isDirectory(const std::string& name);
void readDirectory(const std::string& name, vec_str& sv);
void readDirectory(const std::string& name, std::ostringstream& oss);
void readDirectory(const std::string& name, std::ostringstream& oss, const std::string& separator, const std::string& quotes);
std::streampos getFileSize(const std::string & filename);

typedef long t_time_ms;
typedef long t_duree_ms;

t_time_ms	get_time(void);
t_duree_ms	get_duree(t_time_ms time0);
t_duree_ms	get_duree_update_t0(t_time_ms *time0);

std::string ltoa(long n);

#endif