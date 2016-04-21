#include "jukebox.h"


const string filtroOGG = ".ogg";
const string musicDir = "Music";
const string fileMetadata = "metadata.txt";

bool Jukebox::canPlay;

/**
*
*/
Jukebox::Jukebox(){
    convertedFilesList = new listaSimple<string>();
    rutaInfoId3 = "";
    canPlay = false;

    this->serverDownloader = NULL;
    arrCloud[DROPBOXSERVER] = new Dropbox();
    arrCloud[GOOGLEDRIVESERVER] = new GoogleDrive();
}

/**
*
*/
Jukebox::~Jukebox()
{
    string rutaMetadata = Constant::getAppDir() + tempFileSep + fileMetadata;
    string tempFileDir = Constant::getAppDir() + tempFileSep + "temp.ogg";

    Dirutil dir;
    dir.borrarArchivo(rutaMetadata);
    dir.borrarArchivo(tempFileDir);

    Traza::print("Eliminando servidores", W_DEBUG);
    for (int i=0; i < MAXSERVERS; i++){
        delete arrCloud[i];
    }
    Traza::print("Servidores eliminados", W_DEBUG);
}

/**
*
*/
TID3Tags Jukebox::getSongInfo(string filepath){
    Launcher lanzador;
    FileLaunch emulInfo;
    Dirutil dir;
    TID3Tags songTags;
    const string tags = "title,album,artist,track,genre,publisher,composer";

    try{
        //Se especifica el fichero de musica a reproducir
        emulInfo.rutaroms = dir.getFolder(filepath);
        emulInfo.nombrerom = dir.getFileName(filepath);
        //Se especifica el ejecutable
        emulInfo.rutaexe = dir.GetShortUtilName(Constant::getAppDir()) + tempFileSep + "rsc";
        emulInfo.fileexe = "ffprobe.exe";

        emulInfo.parmsexe = "-i \"%ROMFULLPATH%\" -show_entries format_tags=";
        emulInfo.parmsexe.append(tags);
        emulInfo.parmsexe.append(" -show_entries stream_tags=");
        emulInfo.parmsexe.append(tags);
        emulInfo.parmsexe.append(" -show_entries format=size,duration");
        emulInfo.parmsexe.append(" -v quiet");
        //Lanzamos el programa
        bool resultado = lanzador.lanzarProgramaUNIXFork(&emulInfo);
//        if (resultado){
//            cout << "Lanzamiento OK" << endl;
//        } else {
//            cout << "Error en el lanzamiento" << endl;
//        }
        //Cargamos la salida de la ejecucion del programa anterior
        listaSimple<string> *lista = new listaSimple<string>();
        lista->loadStringsFromFile(Constant::getAppDir() + tempFileSep + "out.log");

        size_t  pos = 0;
        string line;
        string lineLower;
        string value;
        for (int i=0; i < lista->getSize(); i++){
            line = lista->get(i);
            lineLower = line;
            Constant::lowerCase(&lineLower);

            for (int j=0; j < tagMAX; j++){
                if ( lineLower.find(arrTags[j]) != string::npos
                    && (pos = line.find("=")) != string::npos){
                    value = line.substr(pos + 1);

                    Traza::print("Jukebox::getSongInfo. Tag " + value, W_DEBUG);
                    char *arr = new char[value.length()+1];
                    strcpy(arr, value.c_str());
                    Constant::utf8ascii(arr);
                    value = string(arr);
                    delete [] arr;
                    Traza::print("Jukebox::getSongInfo. Tag_utf8ascii " + value, W_DEBUG);

                    if (j == tagAlbum){
                        songTags.album = value;
                    } else if (j == tagTitle){
                        songTags.title = value;
                    } else if (j == tagDuration){
                        songTags.duration = value;
                    } else if (j == tagTrack){
                        songTags.track = value;
                    } else if (j == tagGenre){
                        songTags.genre = value;
                    } else if (j == tagPublisher){
                        songTags.publisher = value;
                    } else if (j == tagComposer){
                        songTags.composer = value;
                    } else if (j == tagArtist){
                        songTags.artist = value;
                    } else if (j == tagDate){
                        songTags.date = value;
                    }
                }
            }
        }
        delete lista;
    } catch (Excepcion &e){
        Traza::print("Excepcion en getSongInfo para " + filepath, W_ERROR);
    }
    return songTags;
}

/**
*
*/
DWORD Jukebox::convertir(){
    convertir(dirToUpload);
    uploadMusicToDropbox(dirToUpload);

    return 0;
}

/**
*
*/
DWORD Jukebox::downloadFile(){
    downloadFile(fileToDownload);
    return 0;
}

/**
*
*/
DWORD Jukebox::uploadMusicToDropbox(){
    uploadMusicToDropbox(dirToUpload);
    return 0;
}
/**
*
*/
DWORD Jukebox::refreshAlbumAndPlaylist(){

    refreshAlbum();
    UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
    if (albumList->getSize() > 0){
        string albumSelec = albumList->getListValues()->get(0);
        refreshPlaylist(albumSelec);
    }

    return 0;
}


DWORD Jukebox::refreshPlaylist(){
    UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
    string albumSelected = albumList->getValue(albumList->getPosActualLista());
    refreshPlaylist(albumSelected);
    return 0;
}

/**
* Convierte al formato ogg todos los ficheros contenidos en el directorio
* especificado por parametro. Tambien genera el fichero de metadatos con la informacion
* de cada cancion
*/
void Jukebox::convertir(string ruta){
    Dirutil dir;
    listaSimple<FileProps> *filelist = new listaSimple<FileProps>();
    Traza::print("Jukebox::convertir", W_DEBUG);

    try{
        convertedFilesList->clear();
        Traza::print("Jukebox::clear", W_DEBUG);
        dir.listarDir(ruta.c_str(), filelist, filtroFicheros);
        FileProps file;
        Launcher lanzador;
        FileLaunch emulInfo;
        bool resultado = false;
        emulInfo.rutaexe = dir.GetShortUtilName(Constant::getAppDir()) + tempFileSep + "rsc";
        emulInfo.fileexe = "ffmpeg.exe";
        //Conversion OGG
        emulInfo.parmsexe = string("-loglevel quiet -y -i \"%ROMFULLPATH%\" -map_metadata 0 -acodec libvorbis ") +
                            string("-id3v2_version 3 -write_id3v1 1 -ac 2 -b:a 128k ") +
                            string("\"%ROMPATH%") + tempFileSep + string("%ROMNAME%.ogg\"");
        //Conversion MP3
    //    emulInfo.parmsexe = string("-y -i \"%ROMFULLPATH%\" -map_metadata 0 -acodec libmp3lame -id3v2_version 3 -write_id3v1 1 -ac 2 -b:a 128k ") +
    //                        string("\"%ROMPATH%\\%ROMNAME%.mp3\"");
        Fileio fichero;
        string metadata = "";
        string rutaMetadata = ruta + tempFileSep + fileMetadata;
        dir.borrarArchivo(rutaMetadata);
        Constant::setExecMethod(launch_create_process);

        int countFile = 0;
        Json::Value root;   // starts as "null"; will contain the root value after parsing
        string songFileName = "cancion";
        Json::StreamWriterBuilder wbuilder;
        //wbuilder.settings_["indentation"] = "";
        string rutaFicheroOgg;
        TID3Tags id3Tags;
        Traza::print("Codificando " + Constant::TipoToStr(filelist->getSize()) + " archivos de la ruta " + ruta, W_DEBUG);

        for (int i=0; i < filelist->getSize(); i++){
            file = filelist->get(i);
            if (filtroFicheros.find(dir.getExtension(file.filename)) != string::npos && file.filename.compare("..") != 0){
                rutaFicheroOgg = file.dir + tempFileSep + dir.getFileNameNoExt(file.filename) + ".ogg";

                //Counter for the codification progress message
                countFile++;
                int percent = (countFile/(float)(filelist->getSize()-1))*100;
                ObjectsMenu->getObjByName("statusMessage")->setLabel("Recodificando "
                            + Constant::TipoToStr(percent) + "% "
                            + " " + file.filename);

                //Si no existe fichero ogg, debemos recodificar
                if (!dir.existe(rutaFicheroOgg)){
                    id3Tags = getSongInfo(file.dir + tempFileSep + file.filename);
                    //Launching the coding in ogg format
                    emulInfo.rutaroms = file.dir;
                    emulInfo.nombrerom = file.filename;
                    //Como no es un fichero ogg, necesitamos recodificar
                    resultado = lanzador.lanzarProgramaUNIXFork(&emulInfo);
                    convertedFilesList->add(rutaFicheroOgg);
                } else {
                    //El fichero es ogg. No necesitamos recodificar y solo obtenemos info de la cancion
                    id3Tags = getSongInfo(rutaFicheroOgg);
                }
                Traza::print("Tags id3 obtenidos para: " + file.filename, W_DEBUG);
                //Generating metadata with time info for each song
                //songFileName = Constant::removeEspecialChars(dir.getFileNameNoExt(file.filename));
                songFileName = Constant::uencodeUTF8(dir.getFileNameNoExt(file.filename));
                Traza::print("songFileName", W_DEBUG);
                root[songFileName]["album"] = Constant::uencodeUTF8(id3Tags.album);
                Traza::print("album", W_DEBUG);
                root[songFileName]["title"] = Constant::uencodeUTF8(id3Tags.title);
                Traza::print("title", W_DEBUG);
                root[songFileName]["duration"] = id3Tags.duration;
                Traza::print("duration", W_DEBUG);
                Traza::print(id3Tags.track, W_DEBUG);
                root[songFileName]["track"] = id3Tags.track;
                Traza::print("track", W_DEBUG);
                root[songFileName]["genre"] = id3Tags.genre;
                Traza::print("genre", W_DEBUG);
                root[songFileName]["publisher"] = id3Tags.publisher;
                Traza::print("publisher", W_DEBUG);
                root[songFileName]["composer"] = id3Tags.composer;
                Traza::print("composer", W_DEBUG);
                root[songFileName]["artist"] = id3Tags.artist;
                Traza::print("Tags anyadidos al fichero de metadatos", W_DEBUG);
            }
        }
        convertedFilesList->sort();
        //Finalmente escribimos el fichero de metadata
        std::string outputConfig = Json::writeString(wbuilder, root);
        fichero.writeToFile(rutaMetadata.c_str(),(char *)outputConfig.c_str(),outputConfig.length(),true);
        ObjectsMenu->getObjByName("statusMessage")->setLabel("Subida terminada");
    } catch (Excepcion &e){
        Traza::print("Error Jukebox::convertir" + string(e.getMessage()), W_ERROR);
    }
}






/**
*
*/
string Jukebox::getMetadatos(map<string, string> *metadatos, string key){
    if (metadatos->count(key) > 0){
        return metadatos->at(key);
    } else {
        return "";
    }
}

/**
*
*/
void Jukebox::hashMapMetadatos(map<string, string> *metadatos, string ruta){
     Traza::print("Jukebox::hashMapMetadatos", W_DEBUG);
     try{
        Json::Value root;   // will contains the root value after parsing.
        Json::Reader reader;
        Fileio file;
        Traza::print("Jukebox::hashMapMetadatos. Cargando de ruta: " + ruta, W_DEBUG);
        file.loadFromFile(ruta);
        Traza::print("Jukebox::hashMapMetadatos. Parseando...", W_DEBUG);
        bool parsingSuccessful = reader.parse( file.getFile(), root );

        if ( !parsingSuccessful ){
            Traza::print("Jukebox::hashMapMetadatos. Failed to parse configuration: " + reader.getFormattedErrorMessages(), W_ERROR);
        } else {
            string songFileName;
            string atributeName;
            string atributeValue;
            Traza::print("Jukebox::hashMapMetadatos. Buscando atributos...", W_DEBUG);
            for (int i=0; i < root.getMemberNames().size(); i++){
                songFileName = root.getMemberNames().at(i);
                Traza::print("Jukebox::hashMapMetadatos. cancion: " + songFileName, W_DEBUG);
                if (songFileName.compare("error") != 0){
                    songFileName = Constant::udecodeUTF8(songFileName);
                    Traza::print("Jukebox::hashMapMetadatos. decodificada: " + songFileName, W_DEBUG);
                    Json::Value value;
                    value = root[root.getMemberNames().at(i)];
                    for (int j=0; j < value.getMemberNames().size(); j++){
                        atributeName = value.getMemberNames().at(j);
                        atributeValue = value[atributeName].asString();
                        metadatos->insert( make_pair(songFileName + atributeName, atributeValue));
                    }
                }
            }
            Traza::print("Jukebox::hashMapMetadatos. END", W_DEBUG);
        }
    }catch (Excepcion &e){
        Traza::print("No se ha podido obtener el fichero de metadatos", W_ERROR);
    }
}

/**
*
*/
DWORD Jukebox::refreshPlayListMetadata(){
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu->getObjByName("playLists"));
    string file = Constant::getAppDir() + tempFileSep + "temp.ogg";
    unsigned int posSongSelected = playList->getLastSelectedPos();
    Traza::print("Obteniendo datos de la cancion: " + file, W_DEBUG);
    TID3Tags songTags = getSongInfo(file);
    Traza::print("Datos obtenidos", W_DEBUG);
    if (!songTags.title.empty()) playList->getCol(posSongSelected, 0)->setTexto(songTags.title);
    if (!songTags.artist.empty()) playList->getCol(posSongSelected, 1)->setValor(songTags.artist);
    if (!songTags.artist.empty()) playList->getCol(posSongSelected, 1)->setTexto(songTags.artist);
    if (!songTags.album.empty()) playList->getCol(posSongSelected, 2)->setValor(songTags.album);
    if (!songTags.album.empty()) playList->getCol(posSongSelected, 2)->setTexto(songTags.album);
    if (!songTags.duration.empty()) playList->getCol(posSongSelected, 3)->setValor(songTags.duration);
    if (!songTags.duration.empty()) playList->getCol(posSongSelected, 3)
                        ->setTexto(Constant::timeFormat(floor(Constant::strToTipo<double>(songTags.duration))));
    Traza::print("Playlist actualizado", W_DEBUG);
    UIProgressBar *objProg = (UIProgressBar *)ObjectsMenu->getObjByName("progressBarMedia");
    int max_ = objProg->getProgressMax();
    Traza::print("Actualizando barra de progreso con el valor", max_, W_DEBUG);
    //Actualizamos la barra de progreso en el caso de que no tuvieramos informacion del maximo de duracion
    if (max_ == 0){
        max_ = floor(Constant::strToTipo<double>(songTags.duration));
        objProg->setProgressMax(max_);
        ObjectsMenu->getObjByName("mediaTimerTotal")->setLabel(Constant::timeFormat(max_));
    }
    Traza::print("Redibujar playlist", W_DEBUG);
    playList->setImgDrawed(false);
    return 0;
}

/**
*
*/
void Jukebox::downloadFile(string ruta){
    Dirutil dir;
    string tempFileDir = Constant::getAppDir() + tempFileSep + "temp.ogg";
    //Creamos el servidor de descarga y damos valor a sus propiedades
    this->serverDownloader = new Dropbox(this->getServerCloud(DROPBOXSERVER));
    //Realizamos la descarga del fichero
    this->serverDownloader->getFile(tempFileDir, ruta, this->serverDownloader->getAccessToken());
    //Actualizamos las propiedades por si ha habido un cambio en el token por un refreshtoken de oauth
    // (Por ahora solo en Google drive)
    Traza::print("Fichero " + ruta + " descargado", W_DEBUG);
    this->getServerCloud(DROPBOXSERVER)->setProperties(this->serverDownloader);
    //Liberamos los recursos
    delete this->serverDownloader;
    this->serverDownloader = NULL;
}

/**
*
*/
void Jukebox::abortDownload(){
    if (this->serverDownloader != NULL){
        this->serverDownloader->abortDownload();
        delete this->serverDownloader;
        this->serverDownloader = NULL;
    }
}

/**
*
*/
void Jukebox::addLocalAlbum(string ruta){
    Traza::print("Jukebox::addLocalAlbum", W_DEBUG);
    UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
    Dirutil dir;
    string nombreAlbum = dir.getFolder(ruta);
    nombreAlbum = nombreAlbum.substr(nombreAlbum.find_last_of(Constant::getFileSep())+1);

    if (dir.existe(ruta)){
        Traza::print("Jukebox::addLocalAlbum anyadiendo: " + ruta, W_DEBUG);
        albumList->addElemLista(nombreAlbum, dir.getFolder(ruta), music);
    }
    Traza::print("Jukebox::addLocalAlbum END", W_DEBUG);
}

/**
*
*/
DWORD Jukebox::refreshPlayListMetadataFromId3Dir(){
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu->getObjByName("playLists"));
    double duration = 0.0;
    canPlay = false;

    Dirutil dir;
//    listaSimple<FileProps> *filelist = new listaSimple<FileProps>();
    vector<FileProps> *filelist = new vector<FileProps>();
    FileProps file;
    string nombreCancion;
    string ruta = rutaInfoId3;
    ruta = dir.getFolder(ruta);

    playList->clearLista();
    //dir.listarDir(ruta.c_str(), filelist, filtroFicheros);
    dir.listarDirRecursivo(ruta, filelist);
    int posFound = 0;
    int pos = 0;


    for (int i=0; i < filelist->size(); i++){
            file = filelist->at(i);
            if (filtroFicherosReproducibles.find(dir.getExtension(file.filename)) != string::npos ){
                vector <ListGroupCol *> miFila;
                miFila.push_back(new ListGroupCol(file.filename, file.dir + Constant::getFileSep() + file.filename));
                miFila.push_back(new ListGroupCol("",""));
                miFila.push_back(new ListGroupCol("",""));
                miFila.push_back(new ListGroupCol(Constant::timeFormat(0), "0"));
                playList->addElemLista(miFila);
                if (rutaInfoId3.compare(file.dir + Constant::getFileSep() + file.filename) == 0){
                    posFound = pos;
                }
                pos++;
            }
    }
    playList->calcularScrPos();
    playList->setPosActualLista(posFound);
    playList->refreshLastSelectedPos();
    playList->calcularScrPos();

    delete filelist;
    playList->setImgDrawed(false);
    canPlay = true;

    for (int i=0; i < playList->getSize(); i++){
        string file = playList->getValue(i);
        Traza::print("Obteniendo datos de la cancion: " + file, W_DEBUG);
        TID3Tags songTags = getSongInfo(file);
        Traza::print("Datos obtenidos", W_DEBUG);
        if (rutaInfoId3.compare(playList->getValue(i)) == 0){
            duration = floor(Constant::strToTipo<double>(songTags.duration));
        }
        if (!songTags.title.empty()) playList->getCol(i, 0)->setTexto(songTags.title);
        if (!songTags.artist.empty()) playList->getCol(i, 1)->setValor(songTags.artist);
        if (!songTags.artist.empty()) playList->getCol(i, 1)->setTexto(songTags.artist);
        if (!songTags.album.empty()) playList->getCol(i, 2)->setValor(songTags.album);
        if (!songTags.album.empty()) playList->getCol(i, 2)->setTexto(songTags.album);
        if (!songTags.duration.empty()) playList->getCol(i, 3)->setValor(songTags.duration);
        if (!songTags.duration.empty()) playList->getCol(i, 3)
                            ->setTexto(Constant::timeFormat(floor(Constant::strToTipo<double>(songTags.duration))));
        playList->setImgDrawed(false);
    }

    Traza::print("Playlist actualizado", W_DEBUG);
    UIProgressBar *objProg = (UIProgressBar *)ObjectsMenu->getObjByName("progressBarMedia");
    int max_ = objProg->getProgressMax();
    Traza::print("Actualizando barra de progreso con el valor", max_, W_DEBUG);
    //Actualizamos la barra de progreso en el caso de que no tuvieramos informacion del maximo de duracion
    if (max_ == 0){
        max_ = duration;
        objProg->setProgressMax(max_);
        ObjectsMenu->getObjByName("mediaTimerTotal")->setLabel(Constant::timeFormat(max_));
    }
    Traza::print("Redibujar playlist", W_DEBUG);

    return 0;
}

/**
*
*/
DWORD Jukebox::authenticateServers(){
    DWORD salida = NOERROR;
    for (int i=0; i < MAXSERVERS; i++){
        int error = arrCloud[i]->authenticate();
        if (error == ERRORREFRESHTOKEN){
            arrCloud[i]->storeAccessToken(arrCloud[i]->getClientid(), arrCloud[i]->getSecret(), arrCloud[i]->getRefreshToken(), true);
        } else if (error != NOERROR){
            salida = error;
        }
    }
    return salida;
}

/**
*
*/
DWORD Jukebox::refreshAlbum(){
    Traza::print("Jukebox::refreshAlbum", W_DEBUG);
    UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
    albumList->clearLista();
    CloudFiles files;

    IOauth2 *server = this->getServerCloud(DROPBOXSERVER);
    server->listFiles("/" + musicDir, server->getAccessToken(), &files);

    string ruta;
    for (int i=0; i < files.fileList.size(); i++){
        if (files.fileList.at(i)->isDir){
            ruta = files.fileList.at(i)->path;
            Traza::print("Jukebox::refreshAlbum anyadiendo: " + ruta, W_DEBUG);
            albumList->addElemLista(ruta.substr(ruta.find_last_of("/")+1),ruta,music);
        }
    }
    Traza::print("Jukebox::refreshAlbum END", W_DEBUG);
    return 0;
}

/**
*
*/
void Jukebox::uploadMusicToDropbox(string ruta){
    string rutaMetadata = ruta + tempFileSep + fileMetadata;
    Dirutil dir;
    listaSimple<FileProps> *filelist = new listaSimple<FileProps>();
    dir.listarDir(ruta.c_str(), filelist, filtroOGG);
    FileProps file;
    int countFile = 0;
    string rutaLocal;
    string nombreAlbum;
    string rutaUpload;
    IOauth2 *server = this->getServerCloud(DROPBOXSERVER);

    if (!server->getAccessToken().empty() && filelist->getSize() > 0){
        for (int i=0; i < filelist->getSize(); i++){
            file = filelist->get(i);
            rutaLocal = file.dir + tempFileSep + file.filename;
            nombreAlbum = file.dir.substr(file.dir.find_last_of(tempFileSep) + 1);
            rutaUpload = musicDir + "/" + nombreAlbum + "/" + file.filename;

            if (filtroOGG.find(dir.getExtension(file.filename)) != string::npos && file.filename.compare("..") != 0){
                countFile++;
                int percent = (countFile/(float)(filelist->getSize()-1))*100;
                ObjectsMenu->getObjByName("statusMessage")->setLabel("Subiendo "
                            + Constant::TipoToStr(percent) + "% "
                            + " " + file.filename);

                Traza::print("Subiendo fichero...", W_DEBUG);
                Traza::print("Confirmando subida del album " + rutaUpload + "...", W_DEBUG);
                server->chunckedUpload(rutaLocal, rutaUpload, server->getAccessToken());
                //Si habiamos convertido el fichero, lo borramos
                if (convertedFilesList->find(rutaLocal) != -1){
                    dir.borrarArchivo(rutaLocal);
                }
            }
        }

        ObjectsMenu->getObjByName("statusMessage")->setLabel("Subiendo " + fileMetadata);
        Traza::print("Subiendo metadatos...", W_DEBUG);
        rutaUpload = musicDir + "/" + nombreAlbum + "/" + fileMetadata;
        Traza::print("Confirmando metadatos " + rutaUpload + "...", W_DEBUG);
        server->chunckedUpload(rutaMetadata, rutaUpload, server->getAccessToken());
        dir.borrarArchivo(rutaMetadata);
        UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
        albumList->addElemLista(nombreAlbum, "/" + musicDir + "/" + nombreAlbum, music);
        albumList->setImgDrawed(false);
        ObjectsMenu->getObjByName("statusMessage")->setLabel("Album " + nombreAlbum + " subido");
    }

}

/**
*
*/
void Jukebox::refreshPlaylist(string rutaAlbumDropbox){
    Traza::print("Jukebox::refreshPlaylist", W_DEBUG);
    Dirutil dir;
    CloudFiles files;
    string ruta;
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu->getObjByName("playLists"));
    IOauth2 *server = this->getServerCloud(DROPBOXSERVER);

    if (!rutaAlbumDropbox.empty()){
        playList->clearLista();
        string albumSelec = rutaAlbumDropbox;
        Traza::print("albumSelec: " + albumSelec, W_DEBUG);
        //Obtenemos los ficheros que nos devuelve dropbox
        Traza::print("Jukebox::refreshPlaylist. ListFiles: " + albumSelec, W_DEBUG);
        server->listFiles(albumSelec, server->getAccessToken(), &files);
        string filename;
        string metadataLocal = Constant::getAppDir() + tempFileSep + "metadata.txt";

        Traza::print("Jukebox::refreshPlaylist. Descargando Metadatos: " + metadataLocal, W_DEBUG);
        //Descargando fichero con metadatos
        server->getFile(metadataLocal,
                        albumSelec + "/" + "metadata.txt",
                        server->getAccessToken());

        map<string, string> metadatos;
        Traza::print("Jukebox::refreshPlaylist. Generando Hashmap", W_DEBUG);
        hashMapMetadatos(&metadatos, metadataLocal);
        string strSeconds;
        unsigned long ulongSeconds = 0;
        string metaKeyTime;
        string metaKeyArtist;
        string metaKeyAlbum;
        string metaKeyTitle;
        string fichero;

        Traza::print("Jukebox::refreshPlaylist. Rellenando metadatos para cada cancion", W_DEBUG);
        for (int i=0; i < files.fileList.size(); i++){
            ruta = files.fileList.at(i)->path;
            filename = ruta.substr(ruta.find_last_of("/")+1);
//            cout << filename  << "; " << dir.getExtension(filename) << endl;
            if (!files.fileList.at(i)->isDir && filtroOGG.find(dir.getExtension(filename)) != string::npos){
                fichero = dir.getFileNameNoExt(filename);
                Traza::print(ruta, W_PARANOIC);
                //Miramos en la hashmap si existe nuestra clave
                metaKeyTime = fichero + arrTags[tagDuration];
                if (metadatos.count(metaKeyTime) > 0){
                    //cout << "Buscando tiempo para: " << metaKeyTime << endl;
                    strSeconds = metadatos.at(metaKeyTime);
                    ulongSeconds = floor(Constant::strToTipo<double>(strSeconds));
                } else {
                    strSeconds = "";
                    ulongSeconds = 0;
                }

                metaKeyArtist = getMetadatos(&metadatos, fichero + arrTags[tagArtist]);
                metaKeyAlbum = getMetadatos(&metadatos, fichero + arrTags[tagAlbum]);
                metaKeyTitle = getMetadatos(&metadatos, fichero + arrTags[tagTitle]);

                Traza::print("Jukebox::refreshPlaylist. Album: " + metaKeyAlbum, W_DEBUG);
                Traza::print("Jukebox::refreshPlaylist. Title: " + metaKeyTitle, W_DEBUG);

                metaKeyAlbum = Constant::udecodeUTF8(metaKeyAlbum);
                metaKeyTitle = Constant::udecodeUTF8(metaKeyTitle);

                Traza::print("Jukebox::refreshPlaylist. Album: " + metaKeyAlbum, W_DEBUG);
                Traza::print("Jukebox::refreshPlaylist. Title: " + metaKeyTitle, W_DEBUG);

                vector <ListGroupCol *> miFila;
                miFila.push_back(new ListGroupCol(metaKeyTitle.empty() ? dir.getFileNameNoExt(filename) : metaKeyTitle,ruta));
                miFila.push_back(new ListGroupCol(metaKeyArtist, metaKeyArtist));
                miFila.push_back(new ListGroupCol(metaKeyAlbum, metaKeyAlbum));
                miFila.push_back(new ListGroupCol(Constant::timeFormat(ulongSeconds), strSeconds));
                playList->addElemLista(miFila);
            }
        }
        playList->setImgDrawed(false);
    }
    Traza::print("Jukebox::refreshPlaylist END", W_DEBUG);
}









