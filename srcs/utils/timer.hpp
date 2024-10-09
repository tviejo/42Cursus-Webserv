/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 16:20:39 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 16:28:34 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include <ctime>
# include <math.h>

class Timer
{
    public:
        Timer(void);
        ~Timer(void);
        Timer(const Timer &src);
        Timer &operator=(const Timer &src);
        void startTimer(void);
        void stopTimer(void);
        double getElapsedTimeSeconds(void);
        double getElapsedTimeMilliseconds(void);
        double getElapsedTimeMicroseconds(void);
        double getElapsedTimeNanoseconds(void);
        void    printElapsedTime(void);
        bool    isTimeElapsedS(double seconds);
        bool    isTimeElapsedMs(double milliseconds);
        bool    isTimeElapsedUs(double microseconds);
        bool    isTimeElapsedNs(double nanoseconds);
    private:
        struct timespec start;
        struct timespec end;
};
