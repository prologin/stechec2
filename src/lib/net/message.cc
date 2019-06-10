// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2019 Association Prologin <association@prologin.org>
#include "message.hh"

#include <cstring>
#include <ostream>
#include <sstream>

namespace net {

Message::Message(uint32_t type_, uint32_t client_id_)
    : type(type_), client_id(client_id_)
{}

std::string Message::str() const
{
    std::stringstream ss;

    ss << "type      : " << type << "\n"
       << "client_id : " << client_id;

    return ss.str();
}

} // namespace net

std::ostream& operator<<(std::ostream& os, const net::Message& msg)
{
    os << msg.str() << std::endl;

    return os;
}
