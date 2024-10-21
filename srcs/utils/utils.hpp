/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:05 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/20 03:27:09 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

typedef std::vector<std::string> stringvec;

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
void readDirectory(const std::string& name, stringvec& sv);
void readDirectory(const std::string& name, std::ostringstream& oss);
std::streampos getFileSize(const std::string & filename);

#endif