#ifndef PLAYBACKITEM_H
#define PLAYBACKITEM_H

class PlayBackItem {
public:
  virtual int updateLed(bool &finished) = 0;
   virtual ~PlayBackItem() {}
};
#endif