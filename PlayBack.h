#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <queue>

#include "apa102.h"
#include "playBackItem.h"

class PlayBack {
private:
    APA102 *apa102;
    std::queue<PlayBackItem*> playBackItems;
public:
    PlayBack(APA102 *apa102) {
        this->apa102 = apa102;
    }

    void turnOff() {
        this->apa102->turnOff();
    }

    void push(PlayBackItem* item) {
        this->playBackItems.push(item);
    }

    void clear() {
        while(!this->playBackItems.empty()) {
            PlayBackItem* item = this->playBackItems.front();
            this->playBackItems.pop();
            delete item;
        }

        this->turnOff();
    }

    int update() {
        if (this->playBackItems.empty()) {
            this->turnOff(); 
            return -1;   
        } 
        
        PlayBackItem* item = this->playBackItems.front();
        bool finished = true;
        int requestWait = item->updateLed(finished);
        if (finished) {
            this->playBackItems.pop();
            delete item;
        }
        return requestWait;
    }
};

#endif
