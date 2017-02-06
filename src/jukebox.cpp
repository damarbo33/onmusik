#include "jukebox.h"


const string filtroOGG = ".ogg";
const string musicDir = "Music";
const string fileMetadata = "metadata.txt";

bool Jukebox::canPlay;

/**
*
*/
Jukebox::Jukebox(){

    int pos = Constant::getAppDir().rfind(Constant::getFileSep());
    if (pos == string::npos){
        FILE_SEPARATOR = FILE_SEPARATOR_UNIX;
        tempFileSep[0] = FILE_SEPARATOR;
    }

    convertedFilesList = new listaSimple<string>();
    rutaInfoId3 = "";
    canPlay = false;
    concatNameFolder = true;

    this->serverDownloader = NULL;
    arrCloud[DROPBOXSERVER] = new Dropbox();
    arrCloud[GOOGLEDRIVESERVER] = new GoogleDrive();
    serverSelected = DROPBOXSERVER;
    aborted = false;
    cdDrive = "";
    extractionPath = "";
}

/**
*
*/
Jukebox::~Jukebox()
{
    string rutaMetadata = Constant::getAppDir() + FILE_SEPARATOR + fileMetadata;
    string tempFileDir = Constant::getAppDir() + FILE_SEPARATOR + "temp.ogg";

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
        emulInfo.rutaexe = dir.GetShortUtilName(Constant::getAppDir()) + FILE_SEPARATOR + "rsc";
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
        lista->loadStringsFromFile(Constant::getAppDir() + FILE_SEPARATOR + "out.log");

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
    convertir(dirToUpload, NULL);
    uploadMusicToServer(dirToUpload);
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
DWORD Jukebox::uploadMusicToServer(){
    uploadMusicToServer(dirToUpload);
    return 0;
}

/**
*
*/
DWORD Jukebox::extraerCD(){
    if (!cdDrive.empty() && !extractionPath.empty()){
        CdTrackInfo cddbTrack;
        int res = extraerCD(cdDrive, extractionPath, &cddbTrack);

        if (res > 0 && !extractionPath.empty()){
            string ruta = extractionPath + FILE_SEPARATOR + "rip";
            Dirutil dir;

            listaSimple<FileProps> *filelist = new listaSimple<FileProps>();
            dir.listarDir(ruta.c_str(), filelist);

            if (filelist->getSize() > 0){
                int i=0;
                bool dirFound = false;
                string rutaRip;
                while (i < filelist->getSize() && !dirFound){
                    if (filelist->get(i).filetype == TIPODIRECTORIO && filelist->get(i).filename.compare(".") != 0
                        && filelist->get(i).filename.compare("..") != 0)
                    {
                            dirFound = true;
                            rutaRip = filelist->get(i).dir + FILE_SEPARATOR + filelist->get(i).filename;
                    } else {
                        i++;
                    }
                }

                Traza::print("Jukebox::extraerCD. Subiendo desde: " + rutaRip, W_DEBUG);
                setDirToUpload(rutaRip);
                convertir(rutaRip, &cddbTrack);
                uploadMusicToServer(rutaRip);
                if (dirFound && !rutaRip.empty()){
                    dir.borrarDir(rutaRip);
                }
            }
            delete filelist;
        }
    }

    return 0;
}

/**
*
*/
DWORD Jukebox::refreshAlbumAndPlaylist(){
    aborted = false;
    refreshAlbum();
    UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
    if (albumList->getSize() > 0){
        this->setAlbumSelected(albumList->getListValues()->get(0));
        refreshPlaylist();
    }

    return 0;
}

/**
*
*/
DWORD Jukebox::refreshPlaylist(){
    Traza::print("Jukebox::refreshPlaylist", W_DEBUG);
    Dirutil dir;
    CloudFiles files;
    string ruta;
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu->getObjByName("playLists"));
    IOauth2 *server = this->getServerCloud(this->getServerSelected());

    if (!this->albumSelected.empty()){
        playList->clearLista();
        string albumSelec = this->albumSelected;
        Traza::print("albumSelec: " + albumSelec, W_DEBUG);
        //Obtenemos los ficheros que nos devuelve dropbox
        Traza::print("Jukebox::refreshPlaylist. ListFiles: " + albumSelec, W_DEBUG);
        server->listFiles(albumSelec, server->getAccessToken(), &files);
        string filename;
        string metadataLocal = Constant::getAppDir() + FILE_SEPARATOR + "metadata.txt";

        Traza::print("Jukebox::refreshPlaylist. Descargando Metadatos: " + metadataLocal, W_DEBUG);
        //Descargando fichero con metadatos
        string metadataCloud;

        if (!aborted){
            if (this->getServerSelected() == GOOGLEDRIVESERVER){
                metadataCloud = ((GoogleDrive *)server)->fileExist("metadata.txt",albumSelec, server->getAccessToken());
            } else {
                metadataCloud = albumSelec + "/" + "metadata.txt";
            }

            Traza::print("Jukebox::refreshPlaylist. Descargando metadatos: " + metadataCloud, W_DEBUG);
            server->getFile(metadataLocal,
                            metadataCloud,
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
            string idFile;
            for (int i=0; i < files.fileList.size(); i++){
                ruta = files.fileList.at(i)->path;
                filename = ruta.substr(ruta.find_last_of("/")+1);

                if (this->getServerSelected() == GOOGLEDRIVESERVER){
                    idFile = files.fileList.at(i)->strHash;
                } else {
                    idFile = ruta;
                }

    //            cout << filename  << "; " << dir.getExtension(filename) << endl;
                if (!files.fileList.at(i)->isDir && filtroOGG.find(dir.getExtension(filename)) != string::npos){
                    fichero = dir.getFileNameNoExt(filename);
                    Traza::print("fichero: " + fichero, W_DEBUG);
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
                    miFila.push_back(new ListGroupCol(metaKeyTitle.empty() ? dir.getFileNameNoExt(filename) : metaKeyTitle, idFile));
                    miFila.push_back(new ListGroupCol(metaKeyArtist, metaKeyArtist));
                    miFila.push_back(new ListGroupCol(metaKeyAlbum, metaKeyAlbum));
                    miFila.push_back(new ListGroupCol(Constant::timeFormat(ulongSeconds), strSeconds));
                    playList->addElemLista(miFila);
                }
            }
        }
        playList->setImgDrawed(false);
    }
    Traza::print("Jukebox::refreshPlaylist END", W_DEBUG);


    return 0;
}

/**
* Convierte al formato ogg todos los ficheros contenidos en el directorio
* especificado por parametro. Tambien genera el fichero de metadatos con la informacion
* de cada cancion
*/
void Jukebox::convertir(string ruta, CdTrackInfo *cddbTrack){
    Dirutil dir;
    vector <FileProps> *filelist = new vector <FileProps>();
    Traza::print("Jukebox::convertir", W_DEBUG);
    //Codec alternativo alpha
    //string codecType = " -vn -acodec vorbis -strict -2 -b:a 128k ";
    //Basando en calidad
    //string codecType = " -acodec vorbis -vn -strict -2 -q:a 4 ";
    //Codec recomendado
    string codecType = " -vn -acodec libvorbis -b:a 128k ";

    try{
        convertedFilesList->clear();
        Traza::print("Jukebox::clear", W_DEBUG);
        dir.listarDirRecursivo(ruta, filelist, filtroFicheros);
        FileProps file;
        Launcher lanzador;
        FileLaunch emulInfo;
        bool resultado = false;
        emulInfo.rutaexe = dir.GetShortUtilName(Constant::getAppDir()) + FILE_SEPARATOR + "rsc";
        emulInfo.fileexe = "ffmpeg.exe";
        //Conversion OGG
        emulInfo.parmsexe = string("-loglevel quiet -y -i \"%ROMFULLPATH%\" -map_metadata 0 ") +
                            string("-id3v2_version 3 -write_id3v1 1 ") + codecType +
                            string("\"%ROMPATH%") + FILE_SEPARATOR + string("%ROMNAME%.ogg\"");
        //Conversion MP3
    //    emulInfo.parmsexe = string("-y -i \"%ROMFULLPATH%\" -map_metadata 0 -acodec libmp3lame -id3v2_version 3 -write_id3v1 1 -ac 2 -b:a 128k ") +
    //                        string("\"%ROMPATH%\\%ROMNAME%.mp3\"");
        Fileio fichero;
        string metadata = "";
        string rutaMetadata = ruta + FILE_SEPARATOR + fileMetadata;
        dir.borrarArchivo(rutaMetadata);
        Constant::setExecMethod(launch_create_process);

        int countFile = 0;
        Json::Value root;   // starts as "null"; will contain the root value after parsing
        string songFileName = "cancion";
        Json::StreamWriterBuilder wbuilder;
        //wbuilder.settings_["indentation"] = "";
        string rutaFicheroOgg;
        TID3Tags id3Tags;
        Traza::print("Codificando " + Constant::TipoToStr(filelist->size()) + " archivos de la ruta " + ruta, W_DEBUG);

        for (int i=0; i < filelist->size(); i++){
            file = filelist->at(i);
            if (filtroFicheros.find(dir.getExtension(file.filename)) != string::npos && file.filename.compare("..") != 0){
                rutaFicheroOgg = file.dir + FILE_SEPARATOR + dir.getFileNameNoExt(file.filename) + ".ogg";

                //Counter for the codification progress message
                countFile++;
                int percent = (countFile/(float)(filelist->size()))*100;
                ObjectsMenu->getObjByName("statusMessage")->setLabel("Recodificando " + Constant::TipoToStr(percent) + "% " + " " + file.filename);

                //Si no existe fichero ogg, debemos recodificar
                if (!dir.existe(rutaFicheroOgg)){
                    //Launching the coding in ogg format
                    emulInfo.rutaroms = file.dir;
                    emulInfo.nombrerom = file.filename;
                    //Si tenemos informacion del cd, generamos sus id3_tags
                    if (cddbTrack != NULL && i < cddbTrack->titleList.size() ){

                        int pos;
                        string album = cddbTrack->albumName;
                        string artist = cddbTrack->albumName;
                        if ((pos = cddbTrack->albumName.find("/")) != string::npos ){
                            artist = cddbTrack->albumName.substr(0, pos);
                            album = cddbTrack->albumName.substr(pos + 1);
                        }

                        emulInfo.parmsexe = string("-loglevel quiet -y -i \"%ROMFULLPATH%\"") +
                            + " -metadata title=\"" + cddbTrack->titleList.at(i) + "\""
                            + " -metadata album=\"" + album + "\""
                            + " -metadata artist=\"" + artist + "\""
                            + " -metadata album_artist=\"" + artist + "\""
                            + " -metadata date=\"" + cddbTrack->year + "\""
                            + " -metadata genre=\"" + cddbTrack->genre + "\""
                            + " -metadata track=\"" + Constant::TipoToStr(i+1) + "/" + Constant::TipoToStr(cddbTrack->titleList.size()) + "\"" +
                            string(" -id3v2_version 3 -write_id3v1 1 ") + codecType +
                            string("\"%ROMPATH%") + FILE_SEPARATOR + string("%ROMNAME%.ogg\"");
                    }
                    //Como no es un fichero ogg, necesitamos recodificar
                    resultado = lanzador.lanzarProgramaUNIXFork(&emulInfo);
                    //Anyadimos el fichero convertido
                    convertedFilesList->add(rutaFicheroOgg);

                    if (cddbTrack != NULL){
                        if (!dir.isDir(file.dir + FILE_SEPARATOR + file.filename)){
                            dir.borrarArchivo(file.dir + FILE_SEPARATOR + file.filename);
                        }
                        id3Tags = getSongInfo(rutaFicheroOgg);
                    } else {
                        id3Tags = getSongInfo(file.dir + FILE_SEPARATOR + file.filename);
                    }

                } else {
                    //El fichero es ogg. No necesitamos recodificar y solo obtenemos info de la cancion
                    id3Tags = getSongInfo(rutaFicheroOgg);
                }
                Traza::print("Tags id3 obtenidos para: " + file.filename, W_DEBUG);
                //Generating metadata with time info for each song
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
        ObjectsMenu->getObjByName("statusMessage")->setLabel("Conversión terminada");
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
    string file = Constant::getAppDir() + FILE_SEPARATOR + "temp.ogg";
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
    string tempFileDir = Constant::getAppDir() + FILE_SEPARATOR + "temp.ogg";
    Traza::print("Jukebox::downloadFile. Descargando fichero " + ruta, W_DEBUG);

    //Creamos el servidor de descarga y damos valor a sus propiedades
    if (this->getServerSelected() == GOOGLEDRIVESERVER){
        this->serverDownloader = new GoogleDrive(this->getServerCloud(this->getServerSelected()));
    } else if (this->getServerSelected() == DROPBOXSERVER){
        this->serverDownloader = new Dropbox(this->getServerCloud(this->getServerSelected()));
    }
    //Realizamos la descarga del fichero
    this->serverDownloader->getFile(tempFileDir, ruta, this->serverDownloader->getAccessToken());
    Traza::print("Jukebox::downloadFile. Fichero " + ruta + " descargado", W_DEBUG);
    //Actualizamos las propiedades por si ha habido un cambio en el token por un refreshtoken de oauth
    // (Por ahora solo en Google drive)
//    this->getServerCloud(this->getServerSelected())->setProperties(this->serverDownloader);

    if (serverDownloader != NULL){
        //Liberamos los recursos
        delete this->serverDownloader;
        this->serverDownloader = NULL;
    }

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
void Jukebox::abortServers(){
    for (int i=0; i < MAXSERVERS; i++){
        arrCloud[i]->abortDownload();
    }
    aborted = true;
}

/**
*
*/
void Jukebox::addLocalAlbum(string ruta){
    Traza::print("Jukebox::addLocalAlbum", W_DEBUG);
    UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
    Dirutil dir;
    string nombreAlbum = dir.getFolder(ruta);
    nombreAlbum = nombreAlbum.substr(nombreAlbum.find_last_of(FILE_SEPARATOR)+1);

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
                miFila.push_back(new ListGroupCol(file.filename, file.dir + FILE_SEPARATOR + file.filename));
                miFila.push_back(new ListGroupCol("",""));
                miFila.push_back(new ListGroupCol("",""));
                miFila.push_back(new ListGroupCol(Constant::timeFormat(0), "0"));
                playList->addElemLista(miFila);
                if (rutaInfoId3.compare(file.dir + FILE_SEPARATOR + file.filename) == 0){
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
        try{
            int error = arrCloud[i]->authenticate();
            if (error == ERRORREFRESHTOKEN){
                arrCloud[i]->storeAccessToken(arrCloud[i]->getClientid(), arrCloud[i]->getSecret(), arrCloud[i]->getRefreshToken(), true);
            } else if (error != NOERROR){
                salida = error;
            }
        } catch (Excepcion &e){
            Traza::print("Excepcion capturada", W_DEBUG);
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
    string ruta;
    string idRuta;

    for (int serverID=0; serverID < MAXSERVERS && !aborted; serverID++){
        IOauth2 *server = this->getServerCloud(serverID);

        if (serverID == GOOGLEDRIVESERVER){
            idRuta = ((GoogleDrive *)server)->fileExist("Music","", server->getAccessToken());
            server->listFiles(idRuta, server->getAccessToken(), &files);
        } else {
            server->listFiles("/" + musicDir, server->getAccessToken(), &files);
        }

        string discName;
        for (int i=0; i < files.fileList.size(); i++){
            if (files.fileList.at(i)->isDir){
                ruta = files.fileList.at(i)->path;
                discName = ruta.substr(ruta.find_last_of("/")+1);

                if (serverID == GOOGLEDRIVESERVER){
                    albumList->addElemLista(discName, files.fileList.at(i)->strHash, music, serverID);
                } else {
                    albumList->addElemLista(discName, ruta, music, serverID);
                }
                Traza::print("Jukebox::refreshAlbum anyadido: " + ruta, W_DEBUG);
            }
        }
        files.clear();
    }

    Traza::print("Jukebox::refreshAlbum END", W_DEBUG);
    return 0;
}

/**
*
*/
void Jukebox::uploadMusicToServer(string ruta){
    string rutaMetadata = ruta + FILE_SEPARATOR + fileMetadata;
    Dirutil dir;
    vector<FileProps> *filelist = new vector<FileProps>();
    dir.listarDirRecursivo(ruta, filelist, filtroOGG);
    FileProps file;
    int countFile = 0;
    string rutaLocal;
    string nombreAlbum;
    string lastNombreAlbum;
    string rutaUpload;
    IOauth2 *server = this->getServerCloud(this->getServerSelected());

    if (!server->getAccessToken().empty() && filelist->size() > 0){

        for (int i=0; i < filelist->size(); i++){
            file = filelist->at(i);
            rutaLocal = file.dir + FILE_SEPARATOR + file.filename;
            nombreAlbum = generarNombreAlbum(&file, ruta);

            if (lastNombreAlbum.compare(nombreAlbum) != 0 && !lastNombreAlbum.empty()){
                subirMetadatos(lastNombreAlbum, rutaUpload, rutaMetadata);
            }

            if (this->getServerSelected() == GOOGLEDRIVESERVER){
                rutaUpload = generarDirGoogleDrive(nombreAlbum);
            } else {
                rutaUpload = musicDir + "/" + nombreAlbum + "/" + file.filename;
            }

            if (filtroOGG.find(dir.getExtension(file.filename)) != string::npos && file.filename.compare("..") != 0){
                countFile++;
                int percent = countFile/(float)filelist->size()*100;
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
            lastNombreAlbum = nombreAlbum;
        }

        subirMetadatos(nombreAlbum, rutaUpload, rutaMetadata);
        dir.borrarArchivo(rutaMetadata);
        refreshAlbum();
        UIList *albumList = ((UIList *)ObjectsMenu->getObjByName("albumList"));
        albumList->setImgDrawed(false);
        ObjectsMenu->getObjByName("statusMessage")->setLabel("Album " + nombreAlbum + " subido");
    }

}
/**
*
*/
string Jukebox::generarNombreAlbum(FileProps *file, string ruta){
    string nombreAlbum;
    IOauth2 *server = this->getServerCloud(this->getServerSelected());
    nombreAlbum = file->dir.substr(ruta.length());
    //Comprobamos si la ruta indicada tiene subdirectorios
    if (nombreAlbum.empty() || !concatNameFolder){
        //No hay subdirectorios. Suponemos que el nombre del disco esta indicado en la carpeta
        nombreAlbum = file->dir.substr(file->dir.find_last_of(FILE_SEPARATOR) + 1);
    } else {
        //Hay subdirectorios, obtenemos el nombre del directorio de la ruta actual
        //y le concatenamos el subdirectorio entero
        nombreAlbum = ruta.substr(ruta.find_last_of(FILE_SEPARATOR) + 1);
        string subdir = file->dir.substr(file->dir.find_last_of(FILE_SEPARATOR) + 1);
        subdir = Constant::replaceAll(subdir, tempFileSep, "_");
        nombreAlbum += " " + subdir;
    }
    return nombreAlbum;
}

/**
*
*/
string Jukebox::generarDirGoogleDrive(string nombreAlbum){
    IOauth2 *server = this->getServerCloud(this->getServerSelected());
    string idMusic = ((GoogleDrive *)server)->fileExist("Music","", server->getAccessToken());
    if (idMusic.empty()){
        string idDir = ((GoogleDrive *)server)->mkdir("ONMUSIK", "", server->getAccessToken());
        idMusic = ((GoogleDrive *)server)->mkdir("Music", idDir, server->getAccessToken());
    }
    string idRutaUpload = ((GoogleDrive *)server)->fileExist(nombreAlbum,idMusic, server->getAccessToken());
    if (idRutaUpload.empty()){
        idRutaUpload = ((GoogleDrive *)server)->mkdir(nombreAlbum, idMusic, server->getAccessToken());
    }
    return idRutaUpload;
}

/**
*
*/
void Jukebox::subirMetadatos(string nombreAlbum, string rutaUpload, string rutaMetadata){
    IOauth2 *server = this->getServerCloud(this->getServerSelected());
    ObjectsMenu->getObjByName("statusMessage")->setLabel("Subiendo " + fileMetadata);
    Traza::print("Subiendo metadatos...", W_DEBUG);
    if (this->getServerSelected() != GOOGLEDRIVESERVER){
        rutaUpload = musicDir + "/" + nombreAlbum + "/" + fileMetadata;
    }
    Traza::print("Confirmando metadatos " + rutaUpload + "...", W_DEBUG);
    server->chunckedUpload(rutaMetadata, rutaUpload, server->getAccessToken());
}

/**
*
*/
int Jukebox::extraerCD(string cdDrive, string extractionPath, CdTrackInfo *cdTrack){
    Dirutil dir;
    string msg;
    int i=0;
    int padSize = 0;
    CAudioCD audioCD;

    //Abrimos el CD de audio
    if ( ! audioCD.Open( cdDrive.at(0) ) ){
        Traza::print("Jukebox::extraerCD No se puede abrir la unidad " + cdDrive, W_DEBUG);
        ObjectsMenu->getObjByName("statusMessage")->setLabel("No se puede abrir la unidad " + cdDrive);
        return 0;
    }

    //Obtenemos el numero de pistas
    ULONG TrackCount = audioCD.GetTrackCount();
    Traza::print("Jukebox::extraerCD Disc ID: " + audioCD.getCddbID() + ", Numero de pistas: " + Constant::TipoToStr(TrackCount), W_DEBUG);
    ObjectsMenu->getObjByName("statusMessage")->setLabel("Extrayendo " + Constant::TipoToStr(TrackCount) + " pistas");
    //Obtenemos informacion del cd
    this->getCddb(&audioCD, cdTrack);

    if (!extractionPath.empty()){
        string albumName = Constant::removeEspecialChars(
                                Constant::replaceAll(
                                Constant::toAnsiString(cdTrack->albumName), "/", "-"));

        padSize = Constant::TipoToStr(TrackCount).length();

        if (albumName.empty()){
            albumName = "rip_" + Constant::replaceAll(Constant::fecha(), ":", "_");
        }

        string rutaRip = extractionPath + FILE_SEPARATOR + "rip"
                        + FILE_SEPARATOR
                        + albumName;

        Traza::print("Iofrontend::extraerCD. Extrayendo CD a la ruta: "+ rutaRip, W_DEBUG);
        if (!existe(extractionPath)){
            ObjectsMenu->getObjByName("statusMessage")->setLabel("No se puede acceder al directorio " + extractionPath);
            return 0;
        }
        dir.mkpath(string(extractionPath + FILE_SEPARATOR + "rip").c_str(), 0777);
        dir.mkpath(rutaRip.c_str(), 0777);

        for (i=0; i<TrackCount; i++ ){
            ULONG Time = audioCD.GetTrackTime( i );

            msg = "Track " + Constant::TipoToStr(i+1) + " Tiempo: " + Constant::TipoToStr(Time/60) + ":"
                 + Constant::TipoToStr(Time%60) + " Tamaño: "
                 + Constant::TipoToStr(ceil (audioCD.GetTrackSize(i) / double( pow(1024, 2)))) + " MB";

            ObjectsMenu->getObjByName("statusMessage")->setLabel(msg);

            string songName;

            if (cdTrack->titleList.size() > 0 && i < cdTrack->titleList.size()){
                songName = rutaRip + FILE_SEPARATOR + Constant::pad(Constant::TipoToStr(i+1), padSize, '0')
                           + " - " + cdTrack->titleList.at(i) + ".wav";
            } else {
                songName = rutaRip + FILE_SEPARATOR +  "Track "
                        + Constant::pad(Constant::TipoToStr(i+1), padSize, '0') + ".wav";
            }

            // Save track-data to file...
            if ( ! audioCD.ExtractTrack( i, songName.c_str() ) ){
                ObjectsMenu->getObjByName("statusMessage")->setLabel("No se puede extraer la pista " + Constant::TipoToStr(i));
                Traza::print("No se puede extraer la pista", i, W_DEBUG);
            }
            // ... or just get the data into memory
    //        CBuf<char> Buf;
    //        if ( ! AudioCD.ReadTrack(i, &Buf) )
    //            printf( "Cannot read track!\n" );
        }

    }

    audioCD.Close();
    return i;
}

/**
* Obtiene la informacion exacta de cada cancion del disco. Es necesario que
* ya se sepa el id del disco y su categoria. Ver la llamada a Jukebox::searchCddbAlbums
*/
int Jukebox::getCddb(CAudioCD *audioCD, CdTrackInfo *cdTrack){
    Freedb cddb;
    FreedbQuery query;
    vector<CdTrackInfo *> cdTrackList;
    query.discId = this->getIdSelected();
    query.categ = this->getCategSelected();
    loadConfigCDDB(&query);
    int code = cddb.getCdInfo(&query, cdTrack);
    return code;
}

/**
*
*/
DWORD Jukebox::searchCddbAlbums(){
    if (this->cdTrackList != NULL && !this->cdDrive.empty()){
        this->cdTrackList->clear();
        searchCddbAlbums(this->cdDrive, this->cdTrackList);
    }
    return 0;
}

/**
*
*/
int Jukebox::searchCddbAlbums(string cdDrive, vector<CdTrackInfo *> *cdTrackList){

    CAudioCD audioCD;
    //Abrimos el CD de audio
    if ( ! audioCD.Open( cdDrive.at(0) ) ){
        return 0;
    }
    CdTrackInfo cddbTrack;

    Freedb cddb;
    FreedbQuery query;
    query.discId = audioCD.getCddbID();
    loadConfigCDDB(&query);
    query.totalSeconds = audioCD.getDiscSeconds();

    std::vector<CDTRACK> *cdInfo = audioCD.getCdInfo();
    for (int i=0; i < cdInfo->size(); i++){
        query.offsets.push_back(cdInfo->at(i).Offset);
    }

    //Listamos todos los posibles albums
    int code = cddb.searchCd(&query, cdTrackList);
    Traza::print("Codigo", code, W_DEBUG);
    audioCD.Close();
    return cdTrackList->size();
}


/**
* Comprueba si existe el directorio o fichero pasado por parámetro
*/
bool Jukebox::existe(string ruta){
    Traza::print("Jukebox::existe " + ruta, W_DEBUG);
    if(isDir(ruta)){
         Traza::print("El directorio existe", W_DEBUG);
        return true;
    } else {
        FILE *archivo = fopen(ruta.c_str(), "r");
        if (archivo != NULL) {
            fclose(archivo);
            Traza::print("El fichero existe", W_DEBUG);
            return true; //TRUE
        } else {
            Traza::print("El fichero no existe", W_DEBUG);
            return false; //FALSE
        }
    }
}

/**
*
*/
bool Jukebox::isDir(string ruta){
    struct stat info;
    stat(ruta.c_str(), &info);

    if(S_ISDIR(info.st_mode)){
        Traza::print("Fichero es un directorio", W_DEBUG);
        return true;
    } else {
        Traza::print("Fichero no es un directorio", W_DEBUG);
        return false;
    }

}

void Jukebox::loadConfigCDDB(FreedbQuery *query){
    string configIniFile = Constant::getAppDir() + Constant::getFileSep() + "config.ini";
    ListaIni<Data> *config = new ListaIni<Data>();
    config->loadFromFile(configIniFile);
    config->sort();

    query->username =     config->find("cddbuser") >= 0 ? config->get(config->find("cddbuser")).getValue() : "";
    query->hostname = config->find("cddbhostname") >= 0 ? config->get(config->find("cddbhostname")).getValue() : "";
    query->clientname =     config->find("cddbclientname") >= 0 ? config->get(config->find("cddbclientname")).getValue() : "";
    query->version =  config->find("cddbclientversion") >= 0 ? config->get(config->find("cddbclientversion")).getValue() : "";
    delete config;
}

