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

    void TurnOff() {
        this->apa102->TurnOff();
    }

    void Push(PlayBackItem* item) {
        this->playBackItems.push(item);
    }

    void Clear() {
        while(this->playBackItems.empty()) {
            PlayBackItem* item = this->playBackItems.front();
            this->playBackItems.pop();
            delete item;
        }

        this->TurnOff();
    }

    void Update() {
        if (this->playBackItems.empty()) {
            this->TurnOff(); 
            return;   
        } 
        
        PlayBackItem* item = this->playBackItems.front();
        bool finished = true;
        item->UpdateLed(finished);
        if (finished) {
            this->playBackItems.pop();
            delete item;
        }
    }
};

#endif
