#ifndef BITS_H
#define BITS_H

#include <string>
#include "_config.h"
#include <iostream>

// Define OS anme
#if defined(unix) || defined(__unix) || defined(__unix__)
#    define __OSNAME UNIX
#elif defined(__APPLE__)
#    define __OSNAME APPLE
#elif defined(_WIN32) || defined(__CYGWIN__)
#    define __OSNAME WINDOWS
#endif

#define MIN(a,b) a<b ? a : b
#define MAX(a,b) a>b ? a : b

// Debug, error, and warning messages. Mainly for debugging purposes to help show where exactly in source an error occurred.
#define DMESG std::cout<<"\033[0;32m["<<__FILE__<<":"<<__LINE__<<"]\033[0m "
#define ERMSG std::cout<<"\033[0;31m["<<__FILE__<<":"<<__LINE__<<"]\033[0m "
#define WARNM std::cout<<"\033[0;35m["<<__FILE__<<":"<<__LINE__<<"]\033[0m "

// Stretch a range with a given value.
inline void stretch(int& min, int& max, int val)
{
    if(val < min) min = val;
    if(val > max) max = val;
}

// Find the Nth occurence of a character in a string.
int findNthOccur(const std::string &str, char chr, int num = 1){
    for(int i = 0; i < str.length(); ++i) {
        if(str[i] == chr) {
            if(--num == 0)
                return i;
        }
    }
    return -1;
}

// Basically getline() but with std::string and token variable instead of streams.
// pos is set to str.length() if no delim remains in the input string.
std::string advanceToken(const std::string &str, int &pos, char delim = '\n') {

    for(int i = pos; i < str.length(); ++i) {
        if(str[i] == delim) {
            int tmp = pos;
            pos = i + 1;
            return str.substr(tmp, i-tmp);
        }
    }
    if(pos == str.length())
    {
        return "";
    }
    int tmp = pos;
    pos = str.length();
    return str.substr(tmp);
}

std::string strip(const std::string &str, const std::string &whitespace = " \n\r\t\v\f") {
    std::string out;
    for(int i = 0; i < str.length(); ++i)
    {
        bool isWhite = false;
        for(int ws = 0; ws < whitespace.length(); ++ws)
        {
            if(whitespace[ws] == str[i])
            {
                isWhite = true;
                break;
            }
        }
        if(!isWhite)
        {
            out += str[i];
        }
    }

    return out;
}

// Integer logarithm. Returns floor(log(n)).
// Integer print length is 1 + this.
int ilog(long n, int base = 10)
{
    int out = 1;
    int c = 0;
    while(out <= n)
    {
        out *= base;
        c++; 
    }
    return c - 1;
}

// power function
long pow(long base, int power)
{
    long out = 1;
    int binCounter = 0;

    while(power > 0)
    {
        if(power & (1 << binCounter))
        {
            power -= 1 << binCounter;
            out *= base;
        }
        base *= base;
        binCounter++;
    }

    return out;
}

// Collapse to 2 or 3 sig figs and add metric prefix
std::string humanize(long l)
{
    int logv = ilog(l, 10);
    int unit = logv / 3;
    int digs[3];

    for(int i = 0; i < 3; ++i)
        digs[i] = l / pow(10, logv - i) % 10;

    std::string out;

    if(unit <= 5)
    {
        switch(logv % 3)
        {
            case 0:
                out += digs[0];
                out += ".";
                out += digs[1];
            break;
            case 1:
                out += " ";
                out += digs[0];
                out += digs[1];
            break;
            case 2:
                out += digs[0];
                out += digs[1];
                out += digs[2];
            break;
        }
        char suffixes[] = {'k', 'M', 'B', 'T', 'Q'};
        
        if(unit > 0)
            out += suffixes[unit - 1];
    }
    else
    {
        out += digs[0];
        out += ".";
        out += digs[1];
        out += "e";
        out += logv;
    }

    return out;
}

void clrln(int len, std::ostream& os = std::cout)
{
    for(int i = 0; i < len; ++i)
        os << ' ';
    os << '\r';
}

void printProgressBar(int cur, int max, int barLength = 30, std::ostream& os = std::cout)
{
    os << "[";
    
#ifdef TILEGRID_UNICODES
    int barFill = cur * barLength / max;
    int fracFill = cur * barLength * 4 / max - barFill * 4;

    for(int i = 0; i < barFill; ++i)
        os << "█";
    if(barLength - barFill >= 1)
    {
        switch(fracFill)
        {
            case 0:
                os << " ";
                break;
            case 1:
                os << "░";
                break;
            case 2:
                os << "▒";
                break;
            case 3:
                os << "▓";
                break;
        }
    }
    for(int i = 0; i < barLength - barFill - 1; ++i)
        os << " ";
#else
    int barFilled = cur * barLength / max;
    for(int i = 0; i < barFilled; ++i)
        os << "#";
    for(int i = 0; i < barLength - barFilled; ++i)
        os << " ";
#endif
    
    os << "] " << cur << " of " << max << " (" << (10000*cur/max)/(float)100 << "%)" << "   \r";
}

template <class T>
inline void optionalCmdlineParam(std::string flag, T &var, std::string fullName, T possibleValue, std::string abbrv, T defaultValue)
{
    if(flag == fullName)
        var = possibleValue;
    else if(flag == abbrv)
        var = defaultValue;
}
#endif