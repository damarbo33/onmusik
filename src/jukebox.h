#ifndef JUKEBOX_H
#define JUKEBOX_H

#include "Launcher.h"
#include "Menuobject.h"
#include "servers/dropbox.h"
#include "servers/googledrive.h"
#include "servers/onedrive.h"
#include "listaIni.h"
#include "thread.h"
#include "audiocd/CAudioCD.h"
#include "cddb/freedb.h"

using namespace std;

static const string filtroFicheros = ".mp3,.mid,.wav,.wma,.cda,.aac,.ac3,.flac,.mp4,.ogg";
static const string filtroFicherosReproducibles = ".mp3,.mid,.wav,.ogg,.flac";

struct TID3Tags{
    string album;
    string title;
    string duration;
    string track;
    string genre;
    string publisher;
    string composer;
    string artist;
    string date;
};

const char arrTags[9][20] = {{"album"},{"title"},
{"duration"},{"track"},{"genre"},{"publisher"},
{"composer"},{"artist"},{"date"}};

typedef enum{tagAlbum, tagTitle, tagDuration,tagTrack,tagGenre,tagPublisher,tagComposer,tagArtist,tagDate,tagMAX} id3Pos;
typedef enum{DROPBOXSERVER, GOOGLEDRIVESERVER, ONEDRIVESERVER, MAXSERVERS} cloudServers;
const char arrNameServers[3][20] = {{"Drobox"},{"Google"},{"undefined"}};
const char DIRCLOUD[] = "ONMUSIK";

class Jukebox{

    public:
        Jukebox();
        virtual ~Jukebox();

        TID3Tags getSongInfo(string filepath);
        void convertir(string ruta, CdTrackInfo *cddbTrack);
        uint32_t convertir();
        uint32_t uploadMusicToServer();
        uint32_t refreshAlbumAndPlaylist();
        uint32_t refreshPlaylist();
        uint32_t downloadFile();
        uint32_t refreshAlbum();
        uint32_t refreshPlayListMetadata();
        uint32_t refreshPlayListMetadataFromId3Dir();
        uint32_t authenticateServers();
        uint32_t extraerCD();
        uint32_t searchCddbAlbums();

        void setObjectsMenu(tmenu_gestor_objects *var){ObjectsMenu = var;}
        void setDirToUpload(string var){dirToUpload = var;}
        void setFileToDownload(string var){fileToDownload = var;}
        void uploadMusicToServer(string ruta);

        void setCdDrive(string var){cdDrive = var;}
        void setExtractionPath(string var){extractionPath = var;}

//        void refreshPlaylist(string rutaAlbumDropbox);
        void downloadFile(string ruta);
        void abortDownload();
        void abortServers();
        void addLocalAlbum(string ruta);
        void setRutaInfoId3(string var){rutaInfoId3 = var;}
        bool isCanPlay(){return canPlay;}
        void setCanPlay(bool var){canPlay = var;}
        void setConcatNameFolder(bool var){concatNameFolder = var;}

        void setServerSelected(int var){serverSelected = var;}
        int getServerSelected(){return serverSelected;}

        void setAlbumSelected(string var){albumSelected = var;}
        string getAlbumSelected(){return albumSelected;}

        IOauth2 *getServerCloud(int idServer){
            if (idServer >= 0 && idServer < MAXSERVERS)
                return arrCloud[idServer];
            else
                return NULL;
        }

        int extraerCD(string cdDrive, string extractionPath, CdTrackInfo *cdTrack);
        int getCddb(CAudioCD *audioCD, CdTrackInfo *cdTrack);

        int searchCddbAlbums(string cdDrive, vector<CdTrackInfo *> *cdTrackList);
        vector<CdTrackInfo *> *cdTrackList;

        void setCdTrackList(vector<CdTrackInfo *> *cdTrackList){this->cdTrackList = cdTrackList;}
        vector<CdTrackInfo *> * getCdTrackList(){return this->cdTrackList;}

        void setIdSelected(string idSelected) { this->idSelected = idSelected; }
        string getIdSelected() { return this->idSelected; }
        void setCategSelected(string categSelected) { this->categSelected = categSelected; }
        string getCategSelected() { return this->categSelected; }
        void loadConfigCDDB(FreedbQuery *query);

    protected:

    private:
        tmenu_gestor_objects *ObjectsMenu;
        string dirToUpload;
        string fileToDownload;
        int serverSelected;
        string albumSelected;
        bool concatNameFolder;
        string cdDrive;
        string extractionPath;
        string categSelected;
        string idSelected;

        void hashMapMetadatos(map<string, string> *metadatos, string ruta);
//        Dropbox dropboxDownloader;
        IOauth2 *serverDownloader;

        string getMetadatos(map<string, string> *metadatos, string key);
        listaSimple<string> *convertedFilesList;
        string rutaInfoId3;
        static bool canPlay;
        IOauth2 * arrCloud[MAXSERVERS];
        string generarNombreAlbum(FileProps *file, string ruta);
        string generarDirGoogleDrive(string nombreAlbum);
        void subirMetadatos(string nombreAlbum, string rutaUpload, string rutaMetadata);
        bool aborted;
        bool isDir(string ruta);
        bool existe(string ruta);
        bool borrarArchivo(string ruta);


};

#endif // JUKEBOX_H
