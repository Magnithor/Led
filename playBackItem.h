#ifndef PLAYBACKITEM_H
#define PLAYBACKITEM_H

#include "apa102.h"

class PlayBackItem {
public:
  virtual int updateLed(bool &finished) = 0;
   virtual ~PlayBackItem() {}
};
#endif