/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:17 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/20 03:32:38 by ade-sarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "utils.hpp"
#include <sys/types.h>
#include <dirent.h>

bool isDirectory(const std::string& name)
{
	DIR *dir = opendir(name.c_str());
	if (dir)
		closedir(dir);
	return (dir != NULL);
}

/* Read a directory and store all entries in the 'sv' string vector
*/
void readDirectory(const std::string& name, stringvec& sv)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        sv.push_back(dp->d_name);
    }
    closedir(dirp);
}

/* Read a directory and write all entries (ended with '\n' separator) into an ostringstream
*/
void readDirectory(const std::string& name, std::ostringstream& oss)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        oss << dp->d_name << "\n";
    }
    closedir(dirp);
}

/* Return -1 if filename doesn't exist or can't be open
*/
std::streampos getFileSize(const std::string & filename)
{
	std::ifstream ifs(filename.c_str(), std::ifstream::in | std::ifstream::ate | std::ifstream::binary);
	return ifs.tellg();
}
