/* This file protected by faith_base_00 license
 * No part of this file could be copyed or changed without Kirill Berezin agreement
 * No part of this file could be used or viewed without Kirill Berezin agreement
 */

#ifndef _ED_SERVER_CONTROLLER_H_
#define _ED_SERVER_CONTROLLER_H_

#include "../names/name_server.h"
#include "../notifications/slot.h"
#include <vector>

namespace ed
{
  template<class connection>
  struct server_controller
  {
    typedef name_server::id_type id_type;
    typedef name_server::word_type word_type;
  private:
    typedef slot<connection> client_type;

    name_server names;
    std::vector<client_type> clients;
  public:
    id_type RegisterName( NAME_TYPE nt, word_type name );
  };
};

#ifndef _ED_SERVER_CONTROLLER_IMPL_
namespace ed
{
  #include "server_controller.cpp"
};
#endif

#endif