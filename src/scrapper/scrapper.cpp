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
uint32_t Scrapper::getLyrics(){
    getLyricsSong();
    return 0;
}

/**
*
*/
uint32_t Scrapper::getLyricsSong(){
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
            string googleInfo = "\"http://www.google.com/search?q=" + Constant::uencodeUTF8(artist)
            + Constant::uencodeUTF8(" " +  track) + "\"";

            ret = arr[i]->trackLyrics(info.at(0));

            if (ret == SINERROR){
                Traza::print("Letra obtenida correctamente", W_DEBUG);
                textElems->setFieldText("LetraCancion", info.at(0)->lyrics_body);
                //Actualizando la url de la info de la cancion
                if (!info.at(0)->urlInfo.empty())
                    ((TextElement *)textElems->getTextVector()->at(0))->setUrl(info.at(0)->urlInfo);
                else
                    ((TextElement *)textElems->getTextVector()->at(0))->setUrl(googleInfo);
                
                ((TextElement *)textElems->getTextVector()->at(0))->setIco(new_window);
                
            } else {
                textElems->setFieldText("LetraCancion", "");
				((TextElement *)textElems->getTextVector()->at(0))->setIco(-1);
            }
            textElems->setOffsetDesplazamiento(0);
            textElems->setImgDrawed(false);
        }
    }
    return 0;
}
