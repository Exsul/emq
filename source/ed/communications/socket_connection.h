/* This file protected by faith_base_00 license
 * No part of this file could be copyed or changed without Kirill Berezin agreement
 * No part of this file could be used or viewed without Kirill Berezin agreement
 */

#ifndef _ED_COMMUNITCATIONS_SOCKET_CONNECTION_H_
#define _ED_COMMUNITCATIONS_SOCKET_CONNECTION_H_

#include "abstract_connection.h"
#include "../notifications/event_notification.h"
#include <string>

namespace ed
{
  namespace com
  {
    class socket_connection : public abstract_connection
    {
      unsigned int desc;
      bool connected;

      void ConnectAttempt();
      void ConnectAttempt( unsigned int ip, int port );
    public:
      socket_connection( std::string addr, int port );
      socket_connection( int icoming_descriptor );

      virtual void Notify( const ed::event_notification & );
      virtual int Incoming( );
      
      ~socket_connection();
    };
  };
};

#endif