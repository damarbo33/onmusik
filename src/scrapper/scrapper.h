#ifndef SCRAPPER_H
#define SCRAPPER_H

#include "lyrics/lyricswikia.h"
#include "lyrics/darklyrics.h"
#include "Menuobject.h"

class Scrapper
{
    public:
        Scrapper();
        virtual ~Scrapper();
        void setTrack(string var){track = var;}
        void setArtist(string var){artist = var;}
        void setObjectsMenu(tmenu_gestor_objects *var){ObjectsMenu = var;}
        DWORD getLyrics();

    protected:

    private:
        tmenu_gestor_objects *ObjectsMenu;
        string track;
        string artist;
        vector <TrackInfo *> info;

        DWORD getLyricsSong();
        LyricsBase *arr[2];
};

#endif // SCRAPPER_H
