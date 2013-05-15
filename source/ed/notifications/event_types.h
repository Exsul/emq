#ifndef _ED_NOTIFICATIONS_EVENT_TYPES_H_
#define _ED_NOTIFICATIONS_EVENT_TYPES_H_

namespace ed
{
  enum EVENT_STATE
  {
    PRE_QUERY,
    PRE_REPLY,
    POST_COMMIT,
    POST_CANCEL
  };

  enum EVENT_RING
  {
    RING0_THREAD,
    RING1_MACHINE,
    RING2_NETWORK,
    RING3_WORLD
  };
};

#endif