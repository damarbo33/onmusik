/**
* Clase que sirve para dibujar los menus o cualquier pantalla
* de la aplicacion
*/

#ifndef Iofrontend_H
#define Iofrontend_H

#include "common/BaseFrontend.h"

#include "jukebox.h"
#include "servers/dropbox.h"
#include "audioplayer.h"
#include "scrapper/scrapper.h"
#include "updater/updater.h"
#include <chrono>
#include <thread>
#include "IofrontendDefines.h"


const unsigned int MAXSONG_REPEAT_AVOID = 20;
const int MAX_STOP_TIMEOUT = 8000;

class SongsReproduced{
    public:
        SongsReproduced();
        ~SongsReproduced();
        void addSongReproduced(unsigned int pos, unsigned int max_);
        bool isReproduced(unsigned int pos);
        unsigned int getNSongsReproduced(){return nSongsReproduced;}
    private:

        bool arraySongReproduced[MAXSONG_REPEAT_AVOID];
        unsigned int nSongsReproduced;
        void clear();
};

class Iofrontend : public BaseFrontend {
    public :
        Iofrontend();
        ~Iofrontend();
        
        void autenticateAndRefresh();
        void bienvenida();
        void actualizaciones();
        void addLocalAlbum(string ruta);
        
     private :
        //Override from parent class BaseFrontend
        void setDinamicSizeObjects();
        void initUIObjs();
        
        //Rest of methods for the specific program
        Thread<AudioPlayer> *threadPlayer;
        Thread<Jukebox> *threadDownloader;
        Thread<Scrapper> *threadLyrics;
        int posAlbumSelected;
        int posSongSelected;
        AudioPlayer *player;
        Jukebox *juke;
        Scrapper *scrapper;
        vector <TrackInfo *> info;
        SongsReproduced mySongsReproduced;
        void playMedia(tEvento *evento);
        long waitMedia();
        void setPanelMediaVisible(bool );
        bool bucleReproductor();
        int calculaPosPanelMedia();
        int accionesMediaAvanzar(tEvento *);
        int accionesMediaRetroceder(tEvento *);
        int accionesMediaPause(tEvento *);
        int accionesMediaStop(tEvento *);
        int mediaClicked(tEvento *);
        int accionesPlaylist(tEvento *evento);
        int accionVolumen(tEvento *evento);
        int accionVolumenMute(tEvento *evento);
        int accionRepeat(tEvento *evento);
        int accionRandom(tEvento *evento);
        int accionesEqualizer(tEvento *evento);
        int accionesfiltroAudio0(tEvento *evento);
        int accionesfiltroAudio1(tEvento *evento);
        int accionesfiltroAudio2(tEvento *evento);
        int accionesfiltroAudio3(tEvento *evento);
        int accionesfiltroAudio4(tEvento *evento);
        int accionesfiltroAudio5(tEvento *evento);
        int accionesfiltroAudio6(tEvento *evento);
        int accionesfiltroAudio7(tEvento *evento);
        int accionesfiltroAudio8(tEvento *evento);
        int accionesResetFiltros(tEvento *evento);
        int accionesSwitchFiltros(tEvento *evento);
        int accionesLetras(tEvento *evento);
        int openLocalDisc(tEvento *evento);
        int AddServer(tEvento *evento);
        int btnActionAddServer(tEvento *evento);
        bool errorTokenServidor(int servidor);
        void refrescarAlbums();
        int showPopupUpload(tEvento *evento);
        int showPopupUploadCD(tEvento *evento);
        int startSongPlaylist(tEvento *evento);
        int selectAlbum(tEvento *evento);
        int accionAlbumPopup(tEvento *evento);
        void refreshSpectrum(AudioPlayer *player);
        void reloadSong(int posAlbumSelected, int posSongSelected);
        void getLyricsFromActualSong();
        int casoPANTALLALOGIN(string titulo, string txtDetalle, bool enableServersUp);
        int accionUploadPopup(tEvento *evento);
        int accionUploadCDPopup(tEvento *evento);
        int uploadToServer(tEvento *evento, int idServer);
        int comprobarTokenServidores();
        void waitJukebox(string pantalla);
        int accionesCddbAceptar(tEvento *evento);
        void showCDDBMenuData();
        int accionesLetrasBox(tEvento *evento);
        int accionesAlbumSelec(tEvento *evento);
        int selectTreeAlbum(tEvento *evento);
        bool waitFinishThreadPlayer(int timeout);
        bool waitFinishThreadDownloader(int timeout);
        bool isMenuLeftVisible();
        int autenticarServicios();
        int fillAlbumLocal(string ruta, bool updateId3tags);
        Jukebox * getJuke(){return juke;}
        int accionesCddbCancelar(tEvento *evento);
        
    
};
#endif //Iofrontend_H
