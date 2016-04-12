#include "scrapper.h"

/**
*
*/
Scrapper::Scrapper(){
    ObjectsMenu = NULL;
    arr[0] = new LyricsWikia();
    arr[1] = new Darklyrics();
}

/**
*
*/
Scrapper::~Scrapper(){
    delete arr[0];
    delete arr[1];
}

/**
*
*/
DWORD Scrapper::getLyrics(){
    getLyricsSong();
    return 0;
}

/**
*
*/
DWORD Scrapper::getLyricsSong(){
    vector <TrackInfo *> info;
    int ret = NOTFOUND;
    UITextElementsArea *textElems = (UITextElementsArea *)ObjectsMenu->getObjByName("LetrasBox");
    textElems->setFieldText("LetraCancion", "Cargando...");
    textElems->setFieldText("TituloLetraCancion", track);

    for (int i=0; i < 2 && ret != SINERROR && ObjectsMenu != NULL; i++){
        Traza::print("Buscando en: " + arr[i]->getServiceName(), W_DEBUG);
        ret = arr[i]->trackSearch(track, artist, &info);
        if (ret == SINERROR){
            Traza::print("Url obtenida: " + info.at(0)->url, W_DEBUG);
            ret = arr[i]->trackLyrics(info.at(0));

            if (ret == SINERROR){
                Traza::print("Letra obtenida correctamente", W_DEBUG);
                textElems->setFieldText("LetraCancion", info.at(0)->lyrics_body);
            } else {
                textElems->setFieldText("LetraCancion", "");
            }
            textElems->setOffsetDesplazamiento(0);
            textElems->setImgDrawed(false);
        }
    }
    return 0;
}
