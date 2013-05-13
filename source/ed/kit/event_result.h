#ifndef _ED_KIT_EVENT_RESULT_H_
#define _ED_KIT_EVENT_RESULT_H_

namespace ed
{
  class module;
  class event_result
  {
    mutable bool deactivated;

    module &m;
    const int local_id;
    bool result;

    friend class module;
    event_result( module &, int local_id, bool result );
  public:
    event_result( const event_result & );

    operator bool &(); // change result commit or cancel
    operator bool() const;

    virtual ~event_result();
  };
};

#include "module.h"

#endif