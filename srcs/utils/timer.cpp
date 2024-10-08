/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tviejo <tviejo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 16:20:41 by tviejo            #+#    #+#             */
/*   Updated: 2024/10/08 16:28:14 by tviejo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "timer.hpp"

Timer::Timer(void)
{
    end.tv_sec = 0;
    end.tv_nsec = 0;
    start.tv_sec = 0;
    start.tv_nsec = 0;
}

Timer::~Timer(void)
{
}

Timer::Timer(const Timer &src)
{
    *this = src;
}

Timer &Timer::operator=(const Timer &src)
{
    if (this != &src)
    {
        start = src.start;
        end = src.end;
    }
    return (*this);
}

void Timer::startTimer(void)
{
    if (start.tv_sec != 0 || start.tv_nsec != 0)
        return;
    clock_gettime(CLOCK_REALTIME, &start);
}

void Timer::stopTimer(void)
{
    clock_gettime(CLOCK_REALTIME, &end);
}

double Timer::getElapsedTimeSeconds(void)
{
    Timer::stopTimer();
    return ((double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000);
}

double Timer::getElapsedTimeMilliseconds(void)
{
    Timer::stopTimer();
    return ((double)(end.tv_sec - start.tv_sec) * 1000 + (double)(end.tv_nsec - start.tv_nsec) / 1000000);
}

double Timer::getElapsedTimeMicroseconds(void)
{
    Timer::stopTimer();
    return ((double)(end.tv_sec - start.tv_sec) * 1000000 + (double)(end.tv_nsec - start.tv_nsec) / 1000);
}

double Timer::getElapsedTimeNanoseconds(void)
{
    Timer::stopTimer();
    return ((double)(end.tv_sec - start.tv_sec) * 1000000000 + (double)(end.tv_nsec - start.tv_nsec));
}

void    Timer::printElapsedTime(void)
{
    Timer::stopTimer();
    if (getElapsedTimeSeconds() >= 1)
        std::cout << round(getElapsedTimeSeconds() * 1000) / 1000 << " s" << std::endl;
    else if (getElapsedTimeMilliseconds() >= 1)
        std::cout << round(getElapsedTimeMilliseconds() * 1000) / 1000 << " ms" << std::endl;
    else if (getElapsedTimeMicroseconds() >= 1)
        std::cout << round(getElapsedTimeMicroseconds() * 1000) / 1000 << " µs" << std::endl;
    else
        std::cout << round(getElapsedTimeNanoseconds() * 1000) / 1000 << " ns" << std::endl;
}

bool    Timer::isTimeElapsedS(double seconds)
{
    Timer::stopTimer();
    return (getElapsedTimeSeconds() >= seconds);
}

bool    Timer::isTimeElapsedMs(double milliseconds)
{
    Timer::stopTimer();
    return (getElapsedTimeMilliseconds() >= milliseconds);
}

bool    Timer::isTimeElapsedUs(double microseconds)
{
    Timer::stopTimer();
    return (getElapsedTimeMicroseconds() >= microseconds);
}

bool    Timer::isTimeElapsedNs(double nanoseconds)
{
    Timer::stopTimer();
    return (getElapsedTimeNanoseconds() >= nanoseconds);
}