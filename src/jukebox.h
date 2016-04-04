#ifndef JUKEBOX_H
#define JUKEBOX_H

#include "Launcher.h"
#include "Menuobject.h"
#include "dropbox.h"
#include "listaIni.h"
#include "thread.h"

using namespace std;

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

class Jukebox
{
    private:
        tmenu_gestor_objects *ObjectsMenu;
        string dirToUpload;
        string fileToDownload;
        string accessToken;
        void hashMapMetadatos(map<string, string> *metadatos, string ruta);
        Dropbox dropboxDownloader;
        string getMetadatos(map<string, string> *metadatos, string key);
        listaSimple<string> *convertedFilesList;

    public:
        /** Default constructor */
        Jukebox();
        /** Default destructor */
        virtual ~Jukebox();

//        double getSongTime(string filepath);
        TID3Tags getSongInfo(string filepath);
        void convertir(string ruta);
        DWORD convertir();
        DWORD uploadMusicToDropbox();
        DWORD refreshAlbumAndPlaylist();
        DWORD refreshPlaylist();
        DWORD downloadFile();
        DWORD refreshAlbum();

        void setObjectsMenu(tmenu_gestor_objects *var){ObjectsMenu = var;}
        void setDirToUpload(string var){dirToUpload = var;}
        void setFileToDownload(string var){fileToDownload = var;}
        void setAccessToken(string var){accessToken = var;}
        void uploadMusicToDropbox(string ruta, string accessToken);
        void refreshAlbumAndPlaylist(string accessToken);
        void refreshPlaylist(string rutaAlbumDropbox);
        void refreshAlbum(string accessToken);
        void downloadFile(string ruta);
        void abortDownload();
        DWORD refreshPlayListMetadata();

    protected:

};

#endif // JUKEBOX_H
