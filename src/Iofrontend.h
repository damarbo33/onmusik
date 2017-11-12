/**
* Clase que sirve para dibujar los menus o cualquier pantalla
* de la aplicacion
*/

#ifndef Iofrontend_H
#define Iofrontend_H

#include <string>
#include <vector>
#include "Ioutil.h"
#include "Colorutil.h"
#include "Menuobject.h"
#include "ImagenGestor.h"
#include "Icogestor.h"
#include "Launcher.h"
#include "thread.h"

#include "uipicture.h"
#include "uiinput.h"
#include "uipicture.h"
#include "uiart.h"
#include "uibutton.h"
#include "uiprogressbar.h"
#include "uipopupmenu.h"
#include "jukebox.h"
#include "servers/dropbox.h"
#include "audioplayer.h"
#include "scrapper/scrapper.h"
#include "updater/updater.h"
#include "uilistgroup.h"
#include "beans/listgroupcol.h"
#include "uilistcommon.h"
#include "UITreeListBox.h"


const unsigned int MAXSONG_REPEAT_AVOID = 20;
const unsigned int ALPHABACKGROUND = 235;



//*********************************************************************************/
//Cuidado con no sobrepasar el numero de elementos que define la constante MAXMENU
//*********************************************************************************/
 typedef enum { PANTALLAREPRODUCTOR = 0,     //0
        PANTALLALISTADODIR,
        PANTALLACONFIRMAR,
        PANTALLABROWSER2,
        PANTALLAOPENMEDIA,
        MENUPOPUPS,
        PANTALLAPREGUNTA,
        PANTALLABIENVENIDA,
        PANTALLALOGIN,
        PANTALLACDDB,
        PANTALLACDDBDATA,
        PANTALLAFOO,
        MAXMENU
} enumMenus;

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



class Iofrontend : public Ioutil{

     private :
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
        Colorutil *convColor;
        int selMenu;
        //Funciones para anyadir punteros a funciones
        typedef int (Iofrontend::*typept2Func)(tEvento *); //Se declara el puntero a funcion
        typept2Func pt2Func[MAXMENU*MAXOBJECTS]; //Se declara un array de punteros a funcion

        struct tprops{
            vector<string> name;
            vector<string> parms;
            int size;
        } propertiesPt2Func;

        struct tscreenobj{
            bool drawComponents;
            bool execFunctions;
        };


        void playMedia(tEvento *evento);
        void obtenerImgCateg(tEvento *);
        void setDinamicSizeObjects();
        void mensajeCompilando(tEvento, string, bool);
        bool procesarBoton(Object *, tmenu_gestor_objects *);
        string casoJOYBUTTONS(tEvento *evento);
        long waitMedia();
        void setPanelMediaVisible(bool );
        bool bucleReproductor();
        int calculaPosPanelMedia();
        void addEvent(string, typept2Func);
        void addEvent(string, typept2Func, int);
        int findEventPos(string);
        void setEvent(string, typept2Func);
        void setEvent(string nombre, typept2Func funcion, int parms);
        //Eventos asignados a los botones
        int simularEscape(tEvento *);
        int simularIntro(tEvento *);
        int marcarBotonSeleccionado(tEvento *);
        int casoDEFAULT(tEvento );
        int accionesMenu(tEvento *);
        int accionesGotoPantalla(tEvento *);
        int accionesCargaPantalla(tEvento *);
        int loadDirFromExplorer(tEvento *);
        int accionesBtnAceptarDir(tEvento *);
        int accionesMediaAvanzar(tEvento *);
        int accionesMediaRetroceder(tEvento *);
        int accionesMediaPause(tEvento *);
        int accionesMediaStop(tEvento *);
        int mediaClicked(tEvento *);
        int accionesListaExplorador(tEvento *);
        int accionesPlaylist(tEvento *evento);
        int accionVolumen(tEvento *evento);
        int accionVolumenMute(tEvento *evento);
        int accionRepeat(tEvento *evento);
        int accionRandom(tEvento *evento);
        int accionCombo(tEvento *evento);
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

        string showExplorador(tEvento *);
        void cargaMenuFromLista(UIListCommon *, tEvento *);
        void cargaMenu(int, string, tEvento *);
        bool cargarDatosEmulador(string );
        bool lanzarPrograma(string);
        bool browser(int, string, int, string, string);
        void comprobarUnicode(int);
        void setTextFromExplorador(tEvento *, UIInput *);
        void clearEmuFields();
        UIPopupMenu * addPopup(int pantalla, string popupName, string callerName);
        bool procesarPopups(tmenu_gestor_objects *objMenu, tEvento *evento);
        void comprobarFixesSO(FileLaunch *emulInfo);
        bool procesarMenuActual(tmenu_gestor_objects *objMenu, tEvento *evento);
        DWORD setInfoRomValues();
        //Muestra el menu especificado como si fuera emergente
        void showMenuEmergente(int menu, string objImagenFondo);
        bool cargarOpcionesGenerales();
        int showPopupUpload(tEvento *evento);
        int showPopupUploadCD(tEvento *evento);
        int startSongPlaylist(tEvento *evento);
        int selectAlbum(tEvento *evento);
        int accionAlbumPopup(tEvento *evento);
        void refreshSpectrum(AudioPlayer *player);

        void loadComboUnidades(string objName, int pantalla,  int types);
        void reloadSong(int posAlbumSelected, int posSongSelected);
        void getLyricsFromActualSong();
        int casoPANTALLALOGIN(string titulo, string txtDetalle, bool allButtonsOn);
        int accionUploadPopup(tEvento *evento);
        int accionUploadCDPopup(tEvento *evento);
        int uploadToServer(tEvento *evento, int idServer);
        int comprobarTokenServidores();
        void waitJukebox( Thread<Jukebox> *var, int pantalla);
        bool waitAceptCancel(string btnAceptar, string btnCancelar, int pantalla);
        int accionesCddbAceptar(tEvento *evento);
        void showCDDBMenuData();
        int accionesLetrasBox(tEvento *evento);
        int accionesAlbumSelec(tEvento *evento);
        int selectTreeAlbum(tEvento *evento);

    public :
        Iofrontend();
        ~Iofrontend();
        bool drawMenu(tEvento );
        void resizeMenu();
        void popUpMenu(tEvento);
        void initUIObjs();
        ImagenGestor *imgGestor ;
        bool isMenuLeftVisible();
        string casoPANTALLAPREGUNTA(string, string);
        bool casoPANTALLACONFIRMAR(string, string);
        //DEBERIAN SER PRIVATE!!!!!!!!!!!!!!!!!!!!!!!!!!!
        tmenu_gestor_objects *ObjectsMenu[MAXMENU];  //Creo tantos punteros a tmenu_gestor_objects como menus dispongo
                                                    //De esta forma podre asignar objetos a cada menu

        bool procesarControles(tmenu_gestor_objects *, tEvento *, tscreenobj *);
        void setSelMenu(int var){comprobarUnicode(var); selMenu = var;}
        int getSelMenu(){ return selMenu;}
        int accionCopiarTextoPopup(tEvento *evento);
        void autenticateAndRefresh();
        int autenticarServicios();
        int fillAlbumLocal(string ruta, bool updateId3tags);
        void bienvenida();
        void actualizaciones();
        Jukebox * getJuke(){return juke;}
        void addLocalAlbum(string ruta);
        int accionesCddbCancelar(tEvento *evento);

};
#endif //Iofrontend_H
