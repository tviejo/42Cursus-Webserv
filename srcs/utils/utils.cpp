/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sarr <ade-sarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:50:17 by tviejo            #+#    #+#             */
/*   Updated: 2024/11/03 17:11:11 by ade-sarr         ###   ########.fr       */
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
void readDirectory(const std::string& name, vec_str& sv)
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

/* Read a directory and write all entries into an ostringstream
*/
void readDirectory(const std::string& name, std::ostringstream& oss, const std::string& separator, const std::string& quotes)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    bool firstEntry = true;
    while ((dp = readdir(dirp)) != NULL) {
        if (!firstEntry)
            oss << separator;
        oss << quotes[0] << dp->d_name << quotes[1];
        firstEntry = false;
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

/* returns the time as the number of milliseconds since the Epoch,
 * 1970-01-01 00:00:00 +0000 (UTC).
*/
t_time_ms	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/* renvoie la durée ecoulée depuis time0
*/
t_duree_ms	get_duree(t_time_ms time0)
{
	return (get_time() - time0);
}

/* renvoie la durée ecoulée depuis time0
 * et met à jour time0 avec le temps actuel
*/
t_duree_ms	get_duree_update_t0(t_time_ms *time0)
{
	t_duree_ms	duree;

	duree = get_time() - *time0;
	*time0 += duree;
	return (duree);
}

/* conversion entier long vers string
*/
std::string ltoa(long n)
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}