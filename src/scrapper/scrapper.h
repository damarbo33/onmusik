#ifndef SCRAPPER_H
#define SCRAPPER_H

#include "lyrics/lyricswikia.h"
#include "lyrics/darklyrics.h"
#include "lyrics/MusixMatch.h"
#include "lyrics/chartLyrics.h"
#include "Menuobject.h"

class Scrapper
{
    public:
        Scrapper();
        virtual ~Scrapper();
        void setTrack(string var){track = var;}
        void setArtist(string var){artist = var;}
        void setObjectsMenu(tmenu_gestor_objects *var){ObjectsMenu = var;}
        uint32_t getLyrics();

    protected:

    private:
        tmenu_gestor_objects *ObjectsMenu;
        string track;
        string artist;
        vector <TrackInfo *> info;

        uint32_t getLyricsSong();
        LyricsBase *arr[2];
};

#endif // SCRAPPER_H
