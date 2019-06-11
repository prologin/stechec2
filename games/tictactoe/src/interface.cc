// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include <iostream>
#include <sstream>
#include <vector>

#include "api.hh"

// from api.cc
extern Api* api;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr)
{
    os << "[";
    typename std::vector<T>::const_iterator it;
    for (it = arr.begin(); it != arr.end(); ++it)
    {
        if (it != arr.begin())
            os << ", ";
        os << *it;
    }
    os << "]";
    return os;
}

// todo avoir un ostringstream a la place de std::string

std::string convert_to_string(int i)
{
    std::ostringstream s;
    s << i;
    std::string result = s.str();
    return result;
}
std::string convert_to_string(double i)
{
    std::ostringstream s;
    s << i;
    std::string result = s.str();
    return result;
}
std::string convert_to_string(std::string i)
{
    return i;
}
std::string convert_to_string(bool i)
{
    return i ? "true" : "false";
}
std::string convert_to_string(std::vector<int> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
std::string convert_to_string(error in)
{
    switch (in)
    {
    case OK:
        return "\"ok\"";
    case OUT_OF_BOUNDS:
        return "\"out_of_bounds\"";
    case ALREADY_OCCUPIED:
        return "\"already_occupied\"";
    case ALREADY_PLAYED:
        return "\"already_played\"";
    }
    return "bad value";
}
std::string convert_to_string(std::vector<error> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
std::string convert_to_string(position in)
{
    std::string x = convert_to_string(in.x);
    std::string y = convert_to_string(in.y);
    std::string out = "{";
    out += "x:" + x;
    out += ", ";
    out += "y:" + y;
    return out + "}";
}

std::string convert_to_string(std::vector<position> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
/// Play at the given position
extern "C" error api_play(position pos)
{
    return api->play(pos);
}

/// Returns your team number
extern "C" int api_my_team()
{
    return api->my_team();
}

/// Returns the TicTacToe board
extern "C" std::vector<int> api_board()
{
    return api->board();
}

/// Cancels the last played action
extern "C" bool api_cancel()
{
    return api->cancel();
}

/// Affiche le contenu d'une valeur de type error
std::ostream& operator<<(std::ostream& os, error v)
{
    switch (v)
    {
    case OK:
        os << "OK";
        break;
    case OUT_OF_BOUNDS:
        os << "OUT_OF_BOUNDS";
        break;
    case ALREADY_OCCUPIED:
        os << "ALREADY_OCCUPIED";
        break;
    case ALREADY_PLAYED:
        os << "ALREADY_PLAYED";
        break;
    }
    return os;
}
extern "C" void api_afficher_error(error v)
{
    std::cerr << v << std::endl;
}

/// Affiche le contenu d'une valeur de type position
std::ostream& operator<<(std::ostream& os, position v)
{
    os << "{ ";
    os << "x"
       << "=" << v.x;
    os << ", ";
    os << "y"
       << "=" << v.y;
    os << " }";
    return os;
}
extern "C" void api_afficher_position(position v)
{
    std::cerr << v << std::endl;
}
