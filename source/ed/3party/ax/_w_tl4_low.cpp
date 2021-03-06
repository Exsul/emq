/* This file protected by faith_base_00 license
 * No part of this file could be copyed or changed without Develop Project agreement
 * No part of this file could be used or viewed without Develop Project agreement
 */

#include "../../exceptions/exception.h"
#pragma comment(lib, "ws2_32.lib")

#include <stdlib.h>
#include <WinSock2.h>
#include <fcntl.h>
#include "tl4_low.h"

using namespace ax::tl4;

namespace
{

#if 1
  class WSA
  {
  public:
    int Success;
    WSA( void )
    {
      WSADATA Data;

      if (WSAStartup(MAKEWORD(1, 0), &Data) != 0)
        Success = 0;
      else
        Success = 1;
    }
    ~WSA( void )
    {
      WSACleanup();
    }
  } aaa;
#endif
  void CloseSocket( unsafe_dword &s )
  {
    shutdown(s, SD_BOTH);
    closesocket(s);
  }

  bool CreateSocket( unsafe_dword &s )
  {
    if (s != INVALID_SOCKET)
      return true;
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
      return false;
    unsigned long a = 1;
    if (ioctlsocket(s, FIONBIO, &a) == SOCKET_ERROR)
    {
      CloseSocket(s);
      return false;
    }
    return true;
  }
}

LOW_STATUSES low::GetIp( const char * const addr, unsafe_dword &ip )
{
  hostent *a = gethostbyname(reinterpret_cast<const char *>(addr));
  if (a == NULL)
  {
    ip = _TL4_NOT_IP_;
    return CANT_RESOLVE_IP;
  }
  ip = *(dword *)a->h_addr_list[0];
  return SUCCESS;
}

LOW_STATUSES low::Connect( unsafe_dword &s, 
                          unsafe_dword addr, const unsafe_word port )
{
  if (!CreateSocket(s))
    return CANT_CREATE_SOCKET;
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.S_un.S_addr = addr;
  memset(address.sin_zero, 0, 8);
  if (connect(s, (sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
  {
    int res = WSAGetLastError();
    if (res == WSAEISCONN)
      return SUCCESS;
    if (res == WSAEWOULDBLOCK || res == WSAEINVAL)
      return TRYING_CONNECT;
    if (res == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
    return LOW_CONNECTION_REFUSED;
  }
  return SUCCESS;
}

LOW_STATUSES low::Send( const unsafe_dword s, const unsafe_byte *buff, const word size )
{
  if (s == _TL4_NOT_SOCKET_ || buff == NULL)
    throw_message("Low level protect");
  int a;
  if ((a = send(s, (char *)buff, size, 0)) == -1)
  {
    a = WSAGetLastError();
    if (a == WSAEWOULDBLOCK)
      return PLEASE_WAIT;
    if (a == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
    return DISCONNECT;
  }
  /*
  FILE *F = fopen("outc.txt", "at");
  unsafe_byte hex[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  fprintf(F, "\n[%4i]", a);
  for (int i = 0; i < a; i++)
    fprintf(F, "%c%c ", hex[buff[i]>>4], hex[buff[i]&15]);
  fclose(F);
  */
  throw_assert(a == size);
  return SUCCESS;
}

LOW_STATUSES low::Recieve( const unsafe_dword s, byte *const buff, word &readed, word size )
{
  if (s == _TL4_NOT_SOCKET_ || buff == NULL)
    throw_message("Low level protect");
  readed = 0;
  short res = recv(s, (char *)buff, size, 0);
  if (res == -1)
  {
    word r = WSAGetLastError();
    if (r == WSAECONNRESET)
      return DISCONNECT;
    if (res == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
    return NO_MESSAGES;
  }
  if (res == 0)
    return DISCONNECT;
  /*
  FILE *F = fopen("inc.txt", "at");
  unsafe_byte hex[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  fprintf(F, "\n[%4i]", res);
  for (int i = 0; i < res; i++)
    fprintf(F, "%c%c ", hex[buff[i]>>4], hex[buff[i]&15]);
  fclose(F);
  /*
  static bword recieved = 0;
  recieved += res;
  static byte q = -1;
  static word pos = 0;
  for (int i = 0; i < res; ++i)
    if (pos == 0)
      throw_assert(buff[i] == 1 && buff[i+1] == 255)
      i += 1, pos += 2;
    else
      throw_assert(++q == buff[i])
      if (++pos == 257)
        pos = 0;
        /**/
  readed = res;
  return SUCCESS;
}

int low::Incoming( const unsafe_dword s, word size )
{
  if (s == _TL4_NOT_SOCKET_)
    throw_message("Low level protect");
  throw_assert(size <= _TL4_DATA_SEGMENT_SIZE);
  char buffer[_TL4_DATA_SEGMENT_SIZE];
  short res = recv(s, buffer, size, MSG_PEEK);
  if (res == -1)
  {
    word r = WSAGetLastError();
    if (r == WSAECONNRESET)
      return -1;
    if (r == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
    if (r == WSAEWOULDBLOCK)
      return 0;
    dead_space();
  }
  if (res == 0)
    return -1;
  return res;
}


LOW_STATUSES low::Close( unsafe_dword &s )
{
  if (s == _TL4_NOT_SOCKET_)
    throw_message("Low level protect");
  shutdown(s, SD_BOTH);
  closesocket(s);
  s = _TL4_NOT_SOCKET_;
  return SUCCESS;
}

LOW_STATUSES low::Open( unsafe_dword &s, 
                       const unsafe_word port, const unsafe_word max_connections )
{
  if (!CreateSocket(s))
    return CANT_CREATE_SOCKET;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)))
  {
    word res = WSAGetLastError();
    if (res == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
    return CANT_BIND;
  }
  if (listen(s, 0))
    return CANT_LISTEN;
  Sleep(1);
  return SUCCESS;
}

udw low::Accept( const unsafe_dword s, unsafe_dword &ip, unsafe_word &port )
{
  struct sockaddr_in a;
  int size = sizeof(a);
  udw res = (udw)accept(s, (struct sockaddr *)&a, &size);
  if (res != INVALID_SOCKET)
  {
    ip = a.sin_addr.S_un.S_addr;
    port = ntohs(a.sin_port);
    return res;
  }
  else
    if (WSAGetLastError() == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
  ip = _TL4_NOT_IP_;
  port = _TL4_NOT_PORT_;
  return _TL4_NOT_SOCKET_;
}

LOW_STATUSES low::SetBacklog( const unsafe_dword socket, 
                             const unsafe_word max_connections )
{
  if (listen(socket, max_connections))
  {
    word res = WSAGetLastError();
    if (res == WSAENOTSOCK)
      throw_message("Low level protect. Random socket descriptor");
    return CANT_LISTEN;
  }
  return SUCCESS;
}