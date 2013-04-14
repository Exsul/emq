/* This file protected by faith_base_00 license
 * No part of this file could be copyed or changed without Kirill Berezin agreement
 * No part of this file could be used or viewed without Kirill Berezin agreement
 */

#ifndef _ED_MESSAGES_MESSAGE_H_
#define _ED_MESSAGES_MESSAGE_H_

namespace ed
{
  enum MESSAGE_TYPE
  {
    REGISTER,
    LISTEN,
    NOTIFY
  };
  struct message
  {
    const int len;
    const unsigned char *buffer;
    
    message() : len(0), buffer(NULL)
    {
    }
    
    MESSAGE_TYPE GetType() const
    {
      throw_assert(len > 0);
      throw_assert(buffer);
      int t = buffer[0];
      throw_assert(t >= REGISTER && t <= NOTIFY);
      return (MESSAGE_TYPE)t;
    }
    
    ~message()
    {
      if (buffer)
        delete buffer;
    }
  };
};

#endif