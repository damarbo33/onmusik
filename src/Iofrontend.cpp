#include "Iofrontend.h"


//Estos son los campos necesarios para identificar la aplicacion
//de dropbox que he dado de alta mediante oauth.
//No deben ser de dominio publico
const string cliendid="";
const string secret="";
const string googleClientId = "";
const string googleSecret = "";
const string onedriveClientId="";
const string onedriveSecret="";

const int MAXDBGAIN = 20;

/**
* Constructor
* HEREDA DE Ioutil
*
*/
Iofrontend::Iofrontend(){
    Traza::print("Constructor de IoFrontend", W_INFO);
    initUIObjs();
    setSelMenu(PANTALLAREPRODUCTOR);
    
    juke = new Jukebox();
    player = new AudioPlayer();
    scrapper = new Scrapper();
    Constant::setExecMethod(launch_create_process);
    juke->setObjectsMenu(getMenu(PANTALLAREPRODUCTOR));
    posAlbumSelected = 0;
    posSongSelected = 0;
    threadPlayer = NULL;
    threadDownloader = NULL;
    threadLyrics = NULL;
    tEvento evento;
    drawMenu(evento);
    Traza::print("Fin Constructor de IoFrontend", W_INFO);
}

/**
* Destructor
*/
Iofrontend::~Iofrontend(){
    Traza::print("Destructor de IoFrontend", W_INFO);
    delete juke;
    delete player;
    delete scrapper;
    Traza::print("Destructor de IoFrontend FIN", W_INFO);
}

/**
* Inicializa los objetos que se pintaran por pantalla para cada pantalla
* Debe llamarse a este metodo despues de haber inicializado SDL para que se puedan centrar los componentes correctamente
**/

void Iofrontend::initUIObjs(){
    tmenu_gestor_objects *obj = createMenu(PANTALLALOGIN);
    obj->add(uiborde, GUIPANELBORDER,0,0,0,0, "Seleccione una opci%C3%B3n", false);
    obj->add(uitextosBox, GUITEXTELEMENTSAREA, 0, -40 * zoomText, getWidth()-50, 70, "", true)->setVerContenedor(false);
    obj->add(btnGoogle, GUIBUTTON, -(BUTTONSERVERW/2 + 5)*2, 30,BUTTONSERVERW,BUTTONH, "Google", true)->setIcon(google_png);
    obj->add(btnDropbox, GUIBUTTON, 0, 30,BUTTONSERVERW,BUTTONH, "Dropbox", true)->setIcon(dropbox_png);
    obj->add(btnOnedrive, GUIBUTTON, (BUTTONSERVERW/2 + 5)*2, 30,BUTTONSERVERW,BUTTONH, "OneDrive", true)->setIcon(onedrive_png);
    obj->add(btnLoginCancel, GUIBUTTON, 0, 80,BUTTONSERVERW,BUTTONH, "Cancelar", true)->setIcon(cross);

    UITextElementsArea *infoTextRom = (UITextElementsArea *)obj->getObjByName(uitextosBox);
    TextElement detalleElement;
    detalleElement.setName(uilabelDetalle);
    detalleElement.setUseMaxLabelMargin(true);
    infoTextRom->addField(&detalleElement);
    infoTextRom->setTextColor(cBlanco);
    infoTextRom->setColor(cNegro);

    obj = createMenu(PANTALLABIENVENIDA);
    obj->add(uiborde, GUIPANEL, 2 + FAMFAMICONW, 2 + FAMFAMICONH,getWidth(),getHeight(), "Bienvenido!", false)->setEnabled(false);;
    obj->add(uitextosBox, GUITEXTELEMENTSAREA, 80,50,getWidth() - 80,150, "", false)->setVerContenedor(false);
    obj->add(btnSiConfirma, GUIBUTTON, 80, 200, BUTTONW,BUTTONH, "Entendido!", false)->setIcon(tick);
    obj->add(ImgFlecha, GUIBUTTON, 2 + FAMFAMICONW, 0, 50,50, "Ajustar volumen", false)->setIcon(FlechaEsquinaSupIzq)->setVerContenedor(false);

    UITextElementsArea *textLabel = (UITextElementsArea *)obj->getObjByName(uitextosBox);
    textLabel->addField(&detalleElement);
    textLabel->setTextColor(cBlanco);

    UIPanel *panel = (UIPanel *)obj->getObjByName(uiborde);
    panel->setColor(cNegro);
    panel->setAlpha(ALPHABACKGROUND);

    obj = createMenu(PANTALLAREPRODUCTOR);
    obj->add(panelMedia, GUIPANEL, 0,0,0,0, "", true)->setEnabled(false);
    obj->add(statusMessage,  GUILABEL,  0,0,0,0, "", false)->setEnabled(false);
    obj->add(albumList, GUITREELISTBOX, 0,0,0,0, "", true)->setEnabled(true);
    obj->add(playLists, GUILISTGROUPBOX, 0,0,0,0, "", true)->setEnabled(true);
    obj->add(LetrasBox, GUITEXTELEMENTSAREA, 0,0,0,0, "Letras", true)->setVerContenedor(true);
    obj->add(spectrum, GUISPECTRUM, 0,0,0,0, "", true)->setEnabled(true);
    obj->add(labelVol,  GUILABEL,  0,0,0,0, "100%", false)->setEnabled(false);
    obj->add(btnAddContent,  GUIBUTTON, 0,0,0,0, "Subir nuevo disco", true)->setIcon(add)->setVerContenedor(false);
    obj->add(btnOpenLocal,  GUIBUTTON, 0,0,0,0, "Abrir fichero local", true)->setIcon(folder)->setVerContenedor(false);
    obj->add(btnAddServer,  GUIBUTTON, 0,0,0,0, "Conectar a Google, Dropbox...", true)->setIcon(server_add)->setVerContenedor(false);
    obj->add(btnAddCD,  GUIBUTTON, 0,0,0,0, "Convertir y subir CD", true)->setIcon(cd_add)->setVerContenedor(false);
    obj->add(mediaTimerTotal,  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);
    obj->add(mediaTimer,  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);
    obj->add(progressBarMedia, GUIPROGRESSBAR, 0,0,0,0, "", true)->setShadow(false);
    obj->add(progressBarVolumen, GUIPROGRESSBAR, 0,0,0,0, "", true)->setShadow(false);
    obj->add(ImgVol, GUIBUTTON, 0,0,0,0, "Ajustar volumen", true)->setIcon(sound)->setVerContenedor(false);
    obj->add(btnBackward, GUIBUTTON, 0,0,0,0, "Saltar a canci%C3%B3n anterior", true)->setIcon(control_rewind)->setVerContenedor(false);
    obj->add(btnPlay,     GUIBUTTON, 0,0,0,0, "Reproducir", true)->setIcon(control_play)->setVerContenedor(false);
    obj->add(btnStop,     GUIBUTTON, 0,0,0,0, "Parar", true)->setIcon(control_stop)->setVerContenedor(false);
    obj->add(btnForward,  GUIBUTTON, 0,0,0,0, "Saltar a canci%C3%B3n siguiente", true)->setIcon(control_fastforward)->setVerContenedor(false);
    obj->add(btnRepeat, GUIBUTTON, 0,0,0,0, "Repetir disco", true)->setIcon(btn_repeat_off)->setVerContenedor(false);
    obj->add(btnRandom, GUIBUTTON, 0,0,0,0, "Aleatorio", true)->setIcon(btn_random_off)->setVerContenedor(false);
    obj->add(btnEqualizer, GUIBUTTON, 0,0,0,0, "Mostrar Ecualizador", true)->setIcon(control_equalizer)->setVerContenedor(false);
    obj->add(btnSwitchEq, GUIBUTTON, 0,0,0,0, "Ecualizador On/Off", true)->setIcon(btn_on)->setVerContenedor(false);
    obj->add(btnResetEq, GUIBUTTON, 0,0,0,0, "Resetear Ecualizador", true)->setIcon(btn_reset_eq)->setVerContenedor(false);
    obj->add(btnLetras,  GUIBUTTON, 0,0,0,0, "Letra de la canci%C3%B3n", true)->setIcon(fontIco)->setVerContenedor(false);
    obj->getObjByName(btnSwitchEq)->setVisible(false);
    obj->getObjByName(btnResetEq)->setVisible(false);

    for(int i=NBIQUADFILTERS - 1; i >= 0; i--){
        //Se anyade el slider
        obj->add(filtroAudio + Constant::TipoToStr(i), GUISLIDER, 0,0,0,0, frecsEQStr[i], true)->setShadow(false);
        UISlider *objfilterGraves = (UISlider *)obj->getObjByName(filtroAudio + Constant::TipoToStr(i));
        objfilterGraves->setVisible(false);
        objfilterGraves->setProgressMax(MAXDBGAIN);
        objfilterGraves->setProgressPos(MAXDBGAIN/2);
        objfilterGraves->setShowHint(false);
        objfilterGraves->setTextColor(cBlanco);
    }

    //obj->getObjByName(panelMedia)->setAlpha(150);
    ((UIPanel *)obj->getObjByName(panelMedia))->setColor(cGrisOscuro);
    obj->getObjByName(statusMessage)->setTextColor(cBlanco);
    obj->getObjByName(mediaTimerTotal)->setTextColor(cBlanco);
    obj->getObjByName(mediaTimer)->setTextColor(cBlanco);
    obj->getObjByName(labelVol)->setTextColor(cBlanco);

    UITextElementsArea *LetrasArea = (UITextElementsArea *)obj->getObjByName(LetrasBox);
    TextElement titleElement;
    t_posicion posLetras(10,10,0,0);
    titleElement.setPos(posLetras);
    titleElement.getStyle()->bold = true;
    titleElement.getStyle()->fontSize = 28;
    titleElement.setUseMaxLabelMargin(true);
    titleElement.setName(TituloLetraCancion);
    LetrasArea->addField(&titleElement);
    
    TextElement lyricsArea;
    posLetras.y += 50;
    lyricsArea.setPos(posLetras);
    lyricsArea.getStyle()->bold = false;
    lyricsArea.getStyle()->fontSize = 24;
    lyricsArea.setUseMaxLabelMargin(true);
    lyricsArea.setName(LetraCancion);
    LetrasArea->addField(&lyricsArea);
    
    LetrasArea->setTextColor(cBlanco);
    LetrasArea->setVisible(false);
    LetrasArea->setColor(cNegroClaro);
    LetrasArea->setIntervalDespl(Constant::getMENUSPACE()*2);
    
    //obj->getObjByName(NewWindowIco)->setIcon(new_window) ;


    ((UIProgressBar *)obj->getObjByName(progressBarMedia))->setTypeHint(HINT_TIME);
    UISpectrum *objSpectrum = (UISpectrum *)obj->getObjByName(spectrum);
    objSpectrum->setColor(cGrisOscuro);
    objSpectrum->setColorFondo(cGrisClaro);

    UIProgressBar *objProg = (UIProgressBar *)obj->getObjByName(progressBarVolumen);
    objProg->setProgressMax(SDL_MIX_MAXVOLUME);
    objProg->setProgressPos(SDL_MIX_MAXVOLUME);


    UITreeListBox *objAlbumList = ((UITreeListBox *)obj->getObjByName(albumList));
    UIListGroup *playList = ((UIListGroup *)obj->getObjByName(playLists));
    objAlbumList->setColor(cGrisOscuro);
    objAlbumList->setTextColor(cBlanco);
    objAlbumList->setEnableLastSelected(true);
    playList->setEnableLastSelected(true);

    UIPopupMenu * popup1 = addPopup(PANTALLAREPRODUCTOR, popupAlbum, albumList);
    if (popup1 != NULL){
        popup1->addElemLista(Constant::txtDisplay("Eliminar %C3%81lbum"), "delete", controller);
    }

    UIPopupMenu * popup2 = addPopup(PANTALLAREPRODUCTOR, popupUpload, btnAddContent);
    if (popup2 != NULL){
        popup2->addElemLista("Subir a Dropbox", "dropbox", dropbox_png, Constant::TipoToStr(DROPBOXSERVER));
        popup2->addElemLista("Subir a Google", google, google_png, Constant::TipoToStr(GOOGLEDRIVESERVER));
        popup2->addElemLista("Subir a Onedrive", onedrive, onedrive_png, Constant::TipoToStr(ONEDRIVESERVER));
    }

    UIPopupMenu * popup3 = addPopup(PANTALLAREPRODUCTOR, popupUploadCD, btnAddCD);


    vector <ListGroupCol *> miCabecera;
    miCabecera.push_back(new ListGroupCol("Canci%C3%B3n", "")); //Cancion
    miCabecera.push_back(new ListGroupCol("Artista", ""));
    miCabecera.push_back(new ListGroupCol("%C3%81lbum", "")); //Album
    miCabecera.push_back(new ListGroupCol("Duraci%C3%B3n", "")); //Duracion
    playList->setHeaderLista(miCabecera);
    playList->adjustToHeader(false);
    playList->addHeaderWith(100);
    playList->addHeaderWith(100);
    playList->addHeaderWith(100);
    playList->addHeaderWith(100);
    playList->setColor(cNegroClaro);
    playList->setTextColor(cBlanco);

    ((UISpectrum *)obj->getObjByName(spectrum))->calcZoom(ALBUMWIDTH);

    obj = createMenu(PANTALLACDDB);
    obj->add(listAlbumsCddb, GUILISTGROUPBOX, 0, 0, 0, 0, uilistadodir, false)->setVerContenedor(false)->setShadow(false);
    obj->add(aceptarCddb, GUIBUTTON, -(BUTTONW/2 + 5), 0, BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    obj->add(cancelarCddb, GUIBUTTON, (BUTTONW/2 + 5), 0, BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);
    obj->add(titleCddb, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "M%C3%A1s de una coincidencia encontrada. Seleccione un %C3%A1lbum", false)->setEnabled(false);

    UIListGroup *objCddb = ((UIListGroup *)obj->getObjByName(listAlbumsCddb));

    vector <ListGroupCol *> cabeceraAlbumscddb;
    cabeceraAlbumscddb.push_back(new ListGroupCol("%C3%81lbum", ""));
    cabeceraAlbumscddb.push_back(new ListGroupCol("A%C3%B1o", ""));
    cabeceraAlbumscddb.push_back(new ListGroupCol("G%C3%A9nero", ""));
    cabeceraAlbumscddb.push_back(new ListGroupCol("Id. Disco", ""));
    objCddb->setHeaderLista(cabeceraAlbumscddb);
    objCddb->adjustToHeader(false);
    objCddb->addHeaderWith(420);
    objCddb->addHeaderWith(70);
    objCddb->addHeaderWith(70);
    objCddb->addHeaderWith(70);
    objCddb->setColor(cGrisOscuro);
    objCddb->setTextColor(cBlanco);


    int desp = 110;
    string msgLabel = Constant::toAnsiString(string("Especifique los datos siguientes para poder hacer una llamada ") +
                     string("a la base de datos de cddb (freedb.org) mediante la que obtener los datos de los CD's que introduzca"));
    
    obj = createMenu(PANTALLACDDBDATA);
    obj->add(lblDataCDDB,  GUITEXTELEMENTSAREA,  0, -220 * zoomText + desp, INPUTW, Constant::getINPUTH()*5, "asdasd", true)->setEnabled(false);
    obj->add(inputUsuario, GUIINPUTWIDE, 0, -140 * zoomText + desp, INPUTW, Constant::getINPUTH(), "Usuario:", true);
    obj->add(inputHostname, GUIINPUTWIDE, 0, -100 * zoomText + desp, INPUTW, Constant::getINPUTH(), "Nombre PC:", true);
    obj->add(btnAceptarCDDB, GUIBUTTON, -(BUTTONW/2 + 5), 30 + desp,BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    obj->add(btnCancelarCDDB, GUIBUTTON, (BUTTONW/2 + 5), 30 + desp,BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);
    obj->add(uiborde, GUIPANELBORDER,0,0,0,0, "Rellene el formulario", false);
    obj->getObjByName(lblDataCDDB)->setTextColor(cBlanco);

    UITextElementsArea *LetrasCDDB = (UITextElementsArea *)obj->getObjByName(lblDataCDDB);
    TextElement cddbElement;
    t_posicion posLetrascddb(5,5,0,0);
    cddbElement.setPos(posLetrascddb);
    cddbElement.getStyle()->bold = false;
    cddbElement.getStyle()->fontSize = 14;
    cddbElement.setName(msgWarning);
    cddbElement.setUseMaxLabelMargin(true);
    cddbElement.setText(msgLabel);
    LetrasCDDB->addField(&cddbElement);
    
    LetrasCDDB->setTextColor(cBlanco);
    LetrasCDDB->setVisible(true);
    LetrasCDDB->setColor(cNegroClaro);
    LetrasCDDB->setIntervalDespl(Constant::getMENUSPACE()*2);

    //Establecemos los elementos que se redimensionan
    setDinamicSizeObjects();

    addEvent(btnGoogle, static_cast<typept2Func>(&Iofrontend::marcarBotonSeleccionado));
    addEvent(btnDropbox, static_cast<typept2Func>(&Iofrontend::marcarBotonSeleccionado));
    addEvent(btnOnedrive, static_cast<typept2Func>(&Iofrontend::marcarBotonSeleccionado));
    addEvent(btnLoginCancel, static_cast<typept2Func>(&Iofrontend::marcarBotonSeleccionado));
    addEvent(aceptarCddb, static_cast<typept2Func>(&Iofrontend::marcarBotonSeleccionado));
    addEvent(cancelarCddb, static_cast<typept2Func>(&Iofrontend::marcarBotonSeleccionado));

    //Botones para la pantalla de video
    addEvent(btnPlay,  static_cast<typept2Func>(&Iofrontend::accionesMediaPause));
    addEvent(btnStop,  static_cast<typept2Func>(&Iofrontend::accionesMediaStop));
    addEvent(btnForward,  static_cast<typept2Func>(&Iofrontend::accionesMediaAvanzar));
    addEvent(btnBackward,  static_cast<typept2Func>(&Iofrontend::accionesMediaRetroceder));
    addEvent(progressBarMedia, static_cast<typept2Func>(&Iofrontend::mediaClicked));
    addEvent(btnAddContent, static_cast<typept2Func>(&Iofrontend::showPopupUpload));
    addEvent(btnAddCD, static_cast<typept2Func>(&Iofrontend::showPopupUploadCD));
    addEvent(btnOpenLocal, static_cast<typept2Func>(&Iofrontend::openLocalDisc));
    addEvent(btnAddServer, static_cast<typept2Func>(&Iofrontend::btnActionAddServer));
    addEvent(albumList, static_cast<typept2Func>(&Iofrontend::selectTreeAlbum));
    addEvent(playLists, static_cast<typept2Func>(&Iofrontend::accionesPlaylist));
    addEvent(popupAlbum, static_cast<typept2Func>(&Iofrontend::accionAlbumPopup));
    addEvent(popupUpload, static_cast<typept2Func>(&Iofrontend::accionUploadPopup));
    addEvent(popupUploadCD, static_cast<typept2Func>(&Iofrontend::accionUploadCDPopup));
    addEvent(progressBarVolumen, static_cast<typept2Func>(&Iofrontend::accionVolumen));
    addEvent(ImgVol, static_cast<typept2Func>(&Iofrontend::accionVolumenMute));
    addEvent(btnRepeat, static_cast<typept2Func>(&Iofrontend::accionRepeat));
    addEvent(btnRandom, static_cast<typept2Func>(&Iofrontend::accionRandom));
    
    addEvent(btnEqualizer, static_cast<typept2Func>(&Iofrontend::accionesEqualizer));
    addEvent(filtroAudio0, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio0));
    addEvent(filtroAudio1, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio1));
    addEvent(filtroAudio2, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio2));
    addEvent(filtroAudio3, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio3));
    addEvent(filtroAudio4, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio4));
    addEvent(filtroAudio5, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio5));
    addEvent(filtroAudio6, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio6));
    addEvent(filtroAudio7, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio7));
    addEvent(filtroAudio8, static_cast<typept2Func>(&Iofrontend::accionesfiltroAudio8));
    addEvent(btnResetEq, static_cast<typept2Func>(&Iofrontend::accionesResetFiltros));
    addEvent(btnSwitchEq, static_cast<typept2Func>(&Iofrontend::accionesSwitchFiltros));
    addEvent(btnLetras, static_cast<typept2Func>(&Iofrontend::accionesLetras));
    addEvent(btnAceptarCDDB, static_cast<typept2Func>(&Iofrontend::accionesCddbAceptar));
    addEvent(btnCancelarCDDB, static_cast<typept2Func>(&Iofrontend::accionesCddbCancelar));
    addEvent(LetrasBox, static_cast<typept2Func>(&Iofrontend::accionesLetrasBox));
}

/**
*
*/
long Iofrontend::waitMedia(){
    long max_ = 0;
    return max_;
}

/**
*
*/
void Iofrontend::playMedia(tEvento *evento){
}

/**
*
*/
void Iofrontend::setDinamicSizeObjects(){
    try{
        BaseFrontend::setDinamicSizeObjects();

        getMenu(PANTALLACDDB)->getObjByName(listAlbumsCddb)->setTam(0, Constant::getINPUTH() + COMBOHEIGHT + 4,this->getWidth(), this->getHeight() - BUTTONH - Constant::getINPUTH() - COMBOHEIGHT - 10 - 4);
        getMenu(PANTALLACDDB)->getObjByName(aceptarCddb)->setTam( (this->getWidth() / 2) -(BUTTONW + 5), this->getHeight() - BUTTONH - 5, BUTTONW,BUTTONH);
        getMenu(PANTALLACDDB)->getObjByName(cancelarCddb)->setTam( (this->getWidth() / 2) + 5, this->getHeight() - BUTTONH - 5, BUTTONW,BUTTONH);
        getMenu(PANTALLACDDB)->getObjByName(titleCddb)->setTam( 0, 0, this->getWidth(), Constant::getINPUTH());

        //Redimension para la pantalla de videos multimedia
        int desp = (this->getWidth() / 2) - FAMFAMICONW*2 - BUTTONW/2;
        int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(panelMedia)->setTam(0, calculaPosPanelMedia(), this->getWidth(), this->getHeight() - calculaPosPanelMedia());
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnBackward)->setTam(desp += (BUTTONW/2), bottom, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnStop)->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnForward)->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnRepeat)->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnRandom)->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(statusMessage)->setTam(desp += FAMFAMICONW*2, bottom, this->getWidth() - desp, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(ImgVol)->setTam(TIMEW- FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(progressBarVolumen)->setTam(TIMEW + SEPTIMER, bottom + 2, TIMEW, PROGRESSHEIGHT);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(labelVol)->setTam(2*(TIMEW + SEPTIMER), bottom +1 , TIMEW, PROGRESSHEIGHT);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnAddContent)->setTam(2, 2, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnEqualizer)->setTam(FAMFAMICONW + 4, 2, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnLetras)->setTam(FAMFAMICONW * 2 + 4, 2, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnOpenLocal)->setTam(FAMFAMICONW * 3 + 4, 2, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnAddServer)->setTam(FAMFAMICONW * 4 + 4, 2, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnAddCD)->setTam(FAMFAMICONW * 5 + 4, 2, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(progressBarMedia)->setTam( TIMEW + SEPTIMER, bottom - PROGRESSSEPBOTTOM, this->getWidth() - TIMEW*2 - SEPTIMER*2, PROGRESSHEIGHT);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(mediaTimer)->setTam(SEPTIMER, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(mediaTimerTotal)->setTam(this->getWidth() - TIMEW, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);
        
        UITreeListBox *objAlbumList = getMenu(PANTALLAREPRODUCTOR)->getObjByName(albumList);
        objAlbumList->setTam(0, FAMFAMICONH + 2, ALBUMWIDTH, calculaPosPanelMedia() - FAMFAMICONH - 2 - ALBUMWIDTH);
        objAlbumList->calcularScrPos();
        objAlbumList->setImgDrawed(false);
        
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(spectrum)->setTam(0, FAMFAMICONH + 2 + calculaPosPanelMedia() - FAMFAMICONH - 2 - ALBUMWIDTH, ALBUMWIDTH, ALBUMWIDTH);

        int yFiltros = FAMFAMICONH + 2 + calculaPosPanelMedia() - FAMFAMICONH - 2 - ALBUMWIDTH;

        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnSwitchEq)->setTam(2, yFiltros + 2, FAMFAMICONW, FAMFAMICONH);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnResetEq)->setTam(4 + FAMFAMICONW, yFiltros + 2, FAMFAMICONW, FAMFAMICONH);

        int xFilter = 0;
        for(int i=0; i < NBIQUADFILTERS; i++){
            UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio + Constant::TipoToStr(i));
            xFilter = (ALBUMWIDTH - 10) / 2 + (i - 4) * 21;
            objfilterGraves->setTam(xFilter, yFiltros + 25, 10, ALBUMWIDTH - 50);
        }

        getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists)->setTam(ALBUMWIDTH, 0, this->getWidth() - ALBUMWIDTH, calculaPosPanelMedia());
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(LetrasBox)->setTam(ALBUMWIDTH, 0, this->getWidth() - ALBUMWIDTH, calculaPosPanelMedia());
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(NewWindowIco)->setTam(ALBUMWIDTH, 0, 45,45);
        
    } catch (Excepcion &e){
        Traza::print("setDinamicSizeObjects: " + string(e.getMessage()), W_ERROR);
    }
}

/**
*
*/
SongsReproduced::SongsReproduced(){
    clear();
}
/**
*
*/
SongsReproduced::~SongsReproduced(){
}
/**
*
*/
void SongsReproduced::clear(){
    for (int i=0; i < MAXSONG_REPEAT_AVOID; i++){
        arraySongReproduced[i] = false;
    }
    nSongsReproduced = 0;
}
/**
*
*/
void SongsReproduced::addSongReproduced(unsigned int pos, unsigned int max_){
    if (pos < max_){
        arraySongReproduced[pos] = true;
        nSongsReproduced++;
    }

    if (nSongsReproduced == MAXSONG_REPEAT_AVOID || nSongsReproduced == max_){
        clear();
    }
}
/**
*
*/
bool SongsReproduced::isReproduced(unsigned int pos){
    if (pos < MAXSONG_REPEAT_AVOID){
        return arraySongReproduced[pos];
    } else
        return false;
}

/**
*
*/
int Iofrontend::accionesMediaAvanzar(tEvento *evento){
    Traza::print("Iofrontend::accionesMediaAvanzar", W_INFO);
    UIListGroup *playList = ((UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists));
    if (player->getStatus() != STOPED && playList->getPosActualLista() < playList->getSize() - 1){
        playList->nextSelectedPos();
        startSongPlaylist(evento);
        return 0;
    } else {
        player->setStatus(FINISHEDALBUM);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(statusMessage)->setLabel("");
        return 1;
    }
}

/**
*
*/
int Iofrontend::accionesMediaRetroceder(tEvento *evento){
    Traza::print("Iofrontend::accionesMediaRetroceder", W_INFO);
    if (player->getStatus() != STOPED){
        UIListGroup *playList = ((UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists));
        playList->prevSelectedPos();
        startSongPlaylist(evento);
        return 0;
    } else return 1;
}

/**
*
*/
int Iofrontend::accionesMediaPause(tEvento *evento){
    Traza::print("Iofrontend::accionesMediaPause", W_INFO);
    UIListGroup *playList = ((UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists));
    if (player->getStatus() == PLAYING){
        player->pause();
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(statusMessage)->setLabel("Pausado");
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setIcon(control_play);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setImgDrawed(false);
    } else if (player->getStatus() == PAUSED){
        player->pause();
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setIcon(control_pause);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setImgDrawed(false);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(statusMessage)->setLabel(playList->getCol(playList->getLastSelectedPos(), 0)->getTexto());
    } else {
        accionesPlaylist(evento);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setIcon(control_pause);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setImgDrawed(false);
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio0(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio0);
    player->setFilter(0, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio1(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio1);
    player->setFilter(1, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio2(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio2);
    player->setFilter(2, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio3(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio3);
    player->setFilter(3, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio4(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio4);
    player->setFilter(4, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio5(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio5);
    player->setFilter(5, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio6(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio6);
    player->setFilter(6, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio7(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio7);
    player->setFilter(7, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio8(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio8);
    player->setFilter(8, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}
/**
*
*/
int Iofrontend::accionesResetFiltros(tEvento *evento){
    Traza::print("Iofrontend::accionesResetFiltros", W_INFO);
    for(int i=0; i < NBIQUADFILTERS; i++){
        UISlider *objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio + Constant::TipoToStr(i));
        objfilterGraves->setProgressPos(MAXDBGAIN/2);
    }
    accionesfiltroAudio0(evento);
    accionesfiltroAudio1(evento);
    accionesfiltroAudio2(evento);
    accionesfiltroAudio3(evento);
    accionesfiltroAudio4(evento);
    accionesfiltroAudio5(evento);
    accionesfiltroAudio6(evento);
    accionesfiltroAudio7(evento);
    accionesfiltroAudio8(evento);
    return 0;
}

/**
*
*/
int Iofrontend::accionesSwitchFiltros(tEvento *evento){
    Traza::print("Iofrontend::accionesSwitchFiltros", W_INFO);
    player->setEqualizerOn(!player->isEqualizerOn());
    if (!player->isEqualizerOn()){
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnSwitchEq)->setIcon(btn_off);
    } else {
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnSwitchEq)->setIcon(btn_on);
    }

    UISlider *objfilterGraves;
    for(int i=0; i < NBIQUADFILTERS; i++){
        objfilterGraves = (UISlider *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio + Constant::TipoToStr(i));
        objfilterGraves->setEnabled(player->isEqualizerOn());
    }

    return 0;
}
/**
*
*/
int Iofrontend::accionesLetras(tEvento *evento){
    Traza::print("Iofrontend::accionesLetras", W_INFO);
    UIListGroup *objPlayList = (UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists);
    UITextElementsArea *textElems = (UITextElementsArea *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(LetrasBox);

    objPlayList->setVisible(!objPlayList->isVisible());
    textElems->setVisible(!objPlayList->isVisible());
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(NewWindowIco)->setVisible(textElems->isVisible());

    if (textElems->isVisible()){
        getLyricsFromActualSong();
        getMenu(PANTALLAREPRODUCTOR)->setFocus(LetrasBox);
    } else {
        getMenu(PANTALLAREPRODUCTOR)->setFocus(playLists);
    }
    return 0;
}



/**
*
*/
void Iofrontend::getLyricsFromActualSong(){
    Traza::print("Iofrontend::getLyricsFromActualSong", W_INFO);
    UIListGroup *objPlayList = (UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists);
    UITextElementsArea *textElems = (UITextElementsArea *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(LetrasBox);
    if (textElems->isVisible()){
        string cancion = objPlayList->getCol(objPlayList->getLastSelectedPos(), 0)->getTexto();
        string Artist = objPlayList->getCol(objPlayList->getLastSelectedPos(), 1)->getValor();
        if (threadLyrics != NULL){
            delete threadLyrics;
            threadLyrics = NULL;
        }
        //Lanzamos el thread para obtener las letras
        scrapper->setObjectsMenu(getMenu(PANTALLAREPRODUCTOR));
        scrapper->setArtist(Artist);
        scrapper->setTrack(cancion);
        threadLyrics = new Thread<Scrapper>(scrapper, &Scrapper::getLyrics);
        threadLyrics->start();
    }
}

/**
*
*/
int Iofrontend::accionesEqualizer(tEvento *evento){
    Traza::print("Iofrontend::accionesEqualizer", W_INFO);
    player->setEqualizerVisible(!player->isEqualizerVisible());
    UISpectrum *objSpectrum = (UISpectrum *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(spectrum);

    Object *btnEq = getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnEqualizer);


    if (!player->isEqualizerVisible()){
        btnEq->setIcon(control_equalizer);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnSwitchEq)->setVisible(false);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnResetEq)->setVisible(false);

        for(int i=0; i < NBIQUADFILTERS; i++){
            getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio + Constant::TipoToStr(i))->setVisible(false);
        }
        objSpectrum->setEnabled(true);
        player->setViewSpectrum(true);

    } else {
        //Debemos dibujar el equalizador
        player->setViewSpectrum(false);
        objSpectrum->setEnabled(false);
        objSpectrum->setImgDrawed(false);
        btnEq->setIcon(control_equalizer_blue);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnSwitchEq)->setVisible(true);
        getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnResetEq)->setVisible(true);

        for(int i=0; i < NBIQUADFILTERS; i++){
            getMenu(PANTALLAREPRODUCTOR)->getObjByName(filtroAudio + Constant::TipoToStr(i))->setVisible(true);
        }
    }

    return 0;
}
/**
*
*/
int Iofrontend::accionesMediaStop(tEvento *evento){
    Traza::print("Iofrontend::accionesMediaStop", W_INFO);
    player->stop();
    Traza::print("Iofrontend::accionesMediaStop. Terminando thread...", W_DEBUG);
    if (threadPlayer != NULL)
        threadPlayer->join();

    Traza::print("Iofrontend::accionesMediaStop. Thread terminado. Actualizando pantalla", W_DEBUG);
    tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
    UIProgressBar *objProg = (UIProgressBar *)obj->getObjByName(progressBarMedia);
    obj->getObjByName(mediaTimerTotal)->setLabel(Constant::timeFormat(0));
    obj->getObjByName(mediaTimer)->setLabel(Constant::timeFormat(0));
    objProg->setProgressMax(0);
    objProg->setProgressPos(0);
    ((UISpectrum *) obj->getObjByName(spectrum))->setImgDrawed(false);
    obj->getObjByName(statusMessage)->setLabel("");
    obj->getObjByName(spectrum)->setImgDrawed(false);
    obj->getObjByName(btnPlay)->setIcon(control_play)->setImgDrawed(false);
    return 0;
}

/**
*
*/
int Iofrontend::accionesPlaylist(tEvento *evento){
    Traza::print("Iofrontend::accionesPlaylist", W_INFO);
    UIListGroup *playList = ((UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists));
    playList->refreshLastSelectedPos();
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setIcon(control_pause);
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setImgDrawed(false);
    getMenu(PANTALLAREPRODUCTOR)->setFocus(playLists);
    startSongPlaylist(evento);
    return 0;
}

/**
*
*/
int Iofrontend::openLocalDisc(tEvento *evento){
    Traza::print("Iofrontend::openLocalDisc", W_INFO);
    long delay = 0;
    unsigned long before = 0;

    try{
        Dirutil dir;
        //Abrimos el explorador de archivos y esperamos a que el usuario seleccione un fichero
        //o directorio
        string fichName = showExplorador(evento);
//        if (!dir.isDir(fichName)){
//            fichName = dir.getFolder(fichName);
//        }
        //Si se ha seleccionado algo, establecemos el texto en el objeto que hemos recibido por parametro
        Traza::print("Iofrontend::openLocalDisc. Reproduciendo: " + fichName, W_DEBUG);
        if (!fichName.empty()){
            this->addLocalAlbum(fichName);
        }
    } catch (Excepcion &e){
        Traza::print("Iofrontend::openLocalDisc: " + string(e.getMessage()), W_ERROR);
    }

    return 0;
}

int Iofrontend::btnActionAddServer(tEvento *evento){
    Traza::print("Iofrontend::btnActionAddServer", W_INFO);
    if (AddServer(evento) != MAXSERVERS){
        autenticateAndRefresh();
    }
}

/**
*
*/
int Iofrontend::AddServer(tEvento *evento){
    Traza::print("Iofrontend::AddServer", W_INFO);

    string strNameServer;
    int someErrorToken = -1;

    juke->getServerCloud(GOOGLEDRIVESERVER)->setClientid(googleClientId);
    juke->getServerCloud(GOOGLEDRIVESERVER)->setSecret(googleSecret);
    juke->getServerCloud(DROPBOXSERVER)->setClientid(cliendid);
    juke->getServerCloud(DROPBOXSERVER)->setSecret(secret);
    juke->getServerCloud(ONEDRIVESERVER)->setClientid(onedriveClientId);
    juke->getServerCloud(ONEDRIVESERVER)->setSecret(onedriveSecret);
    

    this->setSelMenu(PANTALLAREPRODUCTOR);
    tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);

    //Comprobamos si ha habido algun error en la obtencion del accesstoken
    someErrorToken = comprobarTokenServidores();

    //Si despues de autenticarse, no se ha podido obtener el access token, lo obtenemos manualmente
    if (someErrorToken != MAXSERVERS){
        string mensaje = "Para usar la aplicaci%C3%B3n debes dar permisos desde tu cuenta de Google, Dropbox o OneDrive. ";
        mensaje.append("A continuaci%C3%B3n se abrir%C3%A1 un explorador. Debes logarte y pulsar el bot%C3%B3n de \"PERMITIR\".");
        mensaje.append("Seguidamente deber%C3%A1s copiar el c%C3%B3digo obtenido y pegarlo en la ventana de Onmusik que aparecer%C3%A1 a continuaci%C3%B3n.");

        int serverSelected = casoPANTALLALOGIN("Autorizar aplicaci%C3%B3n", mensaje, false);
        if (serverSelected < MAXSERVERS){
            string tmpClient = juke->getServerCloud(serverSelected)->getClientid();
            string tmpSecret = juke->getServerCloud(serverSelected)->getSecret();
            strNameServer = arrNameServers[serverSelected];

            juke->getServerCloud(serverSelected)->launchAuthorize(tmpClient);
            string code = casoPANTALLAPREGUNTA("Autorizar aplicaci%C3%B3n", 
                    "Introduce el campo obtenido de la p%C3%A1gina de "
                    + strNameServer + " (CTRL+V)");
            
            if (!code.empty()){
                clearScr(cGrisOscuro);
                juke->getServerCloud(serverSelected)->storeAccessToken(tmpClient, tmpSecret, code, false);
            }
            //Commented to force the refresh of the list
            //someErrorToken = comprobarTokenServidores();
        } else {
            someErrorToken = MAXSERVERS;
        }
    }
    return someErrorToken;
}
/**
*
*/
int Iofrontend::showPopupUpload(tEvento *evento){
    getMenu(PANTALLAREPRODUCTOR)->setFocus(btnAddContent);
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnAddContent)->setPopup(true);
    procesarPopups(getMenu(PANTALLAREPRODUCTOR), evento);
    return 0;
}

/**
*
*/
int Iofrontend::mediaClicked(tEvento *evento){
    Traza::print("Iofrontend::mediaClicked", W_INFO);
    UIProgressBar * objProg = (UIProgressBar *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(progressBarMedia);
    Traza::print("Pos pulsada barra de tiempo", objProg->getProgressPos(), W_DEBUG);
    
    if (player->isSongDownloaded() && objProg->getProgressMax() > 0 && player->getStatus() == PLAYING){
        //Comprobamos si se ha terminado la descarga y recargamos en ese caso
        //reloadSong(posAlbumSelected, posSongSelected); 
        player->setPosicionCancion(objProg->getProgressPos() * 1000);
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionVolumen(tEvento *evento){
    Traza::print("Iofrontend::accionVolumen", W_INFO);
    UIProgressBar * objProg = (UIProgressBar *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(progressBarVolumen);
    UIButton * objMute = (UIButton *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(ImgVol);
    Traza::print("Pos pulsada volumen: ", objProg->getProgressPos(), W_DEBUG);

    int percent = (objProg->getProgressPos() / (float)objProg->getProgressMax()) * 100;

    player->setVol(objProg->getProgressPos());
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(labelVol)->setLabel(Constant::TipoToStr(percent) + "%");

    if (objMute->getIcon() == sound_mute){
        objMute->setIcon(sound);
    }

    return 0;
}

int Iofrontend::accionRepeat(tEvento *evento){
    Traza::print("Iofrontend::accionRepeat", W_INFO);
    UIButton * objButton = (UIButton *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnRepeat);
    if (objButton->getIcon() == btn_repeat_off){
        objButton->setIcon(btn_repeat);
    } else if (objButton->getIcon() == btn_repeat){
        objButton->setIcon(btn_repeat_off);
    }

    return 0;
}

int Iofrontend::accionRandom(tEvento *evento){
    Traza::print("Iofrontend::accionRandom", W_INFO);
    UIButton * objButton = (UIButton *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnRandom);
    if (objButton->getIcon() == btn_random_off){
        objButton->setIcon(btn_random);
    } else if (objButton->getIcon() == btn_random){
        objButton->setIcon(btn_random_off);
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionVolumenMute(tEvento *evento){
    Traza::print("Iofrontend::accionVolumenMute", W_INFO);
    UIProgressBar * objProg = (UIProgressBar *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(progressBarVolumen);
    UIButton * objMute = (UIButton *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(ImgVol);
    static int volAnt = 0;

    if (objMute->getIcon() == sound){
        volAnt = objProg->getProgressPos();
        objMute->setIcon(sound_mute);
        objProg->setProgressPos(0);
    } else if (objMute->getIcon() == sound_mute){
        objMute->setIcon(sound);
        objProg->setProgressPos(volAnt);
    }
    player->setVol(objProg->getProgressPos());
    int percent = (objProg->getProgressPos() / (float)objProg->getProgressMax()) * 100;
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(labelVol)->setLabel(Constant::TipoToStr(percent) + "%");

    objMute->setImgDrawed(false);
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(labelVol)->setImgDrawed(false);
    return 0;
}

/**
*
*/
void Iofrontend::setPanelMediaVisible(bool var){
    Traza::print("Iofrontend::setPanelMediaVisible", W_INFO);
    try{
        tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
        obj->getObjByName(panelMedia)->setVisible(var);
        obj->getObjByName(btnBackward)->setVisible(var);
        obj->getObjByName(btnPlay)->setVisible(var);
        obj->getObjByName(btnStop)->setVisible(var);
        obj->getObjByName(btnForward)->setVisible(var);
        obj->getObjByName(progressBarMedia)->setVisible(var);
        obj->getObjByName(mediaTimerTotal)->setVisible(var);
        obj->getObjByName(mediaTimer)->setVisible(var);
        if (!var) {
            t_region regionPantalla = {0 , calculaPosPanelMedia(), getWidth(), getHeight() - calculaPosPanelMedia()};
            clearScr(cGrisOscuro, regionPantalla);
        }

    } catch (Excepcion &e){
        Traza::print("setPanelMediaVisible: " + string(e.getMessage()), W_ERROR);
    }
}

/**
*
*/
int Iofrontend::startSongPlaylist(tEvento *evento){
    Traza::print("Iofrontend::startSongPlaylist", W_INFO);
    UIListGroup *playList = ((UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists));
    bool salir = false;
    do{
        string cancion = playList->getValue(playList->getLastSelectedPos());
        playList->setImgDrawed(false);
        string file = Constant::getAppDir() + tempFileSep + "temp.ogg";
        //Intentamos parar el thread del reproductor
        if (threadPlayer != NULL){
            if (threadPlayer->isRunning()){
                Traza::print("Iofrontend::startSongPlaylist. Terminando Thread de reproduccion...", W_DEBUG);;
                player->stop();
                waitFinishThread(threadPlayer, MAX_STOP_TIMEOUT);
                Traza::print("Iofrontend::startSongPlaylist. Reproduccion terminada.", W_DEBUG);
            }
            delete threadPlayer;
            threadPlayer = NULL;
        }

        Traza::print("Iofrontend::startSongPlaylist. Cancelando descarga", W_DEBUG);
        //Comprobamos que no haya ninguna descarga activa
        if (threadDownloader != NULL){
            if (threadDownloader->isRunning()){
                juke->abortDownload();
                waitFinishThread(threadDownloader, MAX_STOP_TIMEOUT);
            }
            delete threadDownloader;
            threadDownloader = NULL;
        }

        Traza::print("Iofrontend::startSongPlaylist. Seleccionando: " + cancion, W_DEBUG);
        juke->setFileToDownload(cancion);
        //Borramos el archivo antes de descargarlo
        Dirutil dir;
        dir.borrarArchivo(file);

        //Comprobamos si estamos intentando reproducir una cancion del disco
        //duro o de dropbox
        if (!dir.existe(cancion)){
            UITreeListBox *objAlbumList = ((UITreeListBox *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(albumList));
            string idservidor = objAlbumList->getDestino(objAlbumList->getLastSelectedPos());
            juke->setServerSelected(Constant::strToTipo<int>(idservidor));
            //Creamos el thread
            threadDownloader = new Thread<Jukebox>(juke, &Jukebox::downloadFile);
            //Lanzamos el thread
            if (threadDownloader->start()){
                Traza::print("Thread started with id: ",threadDownloader->getThreadID(), W_DEBUG);
                //esperamos a que se carguen al menos 100KB
                std::ifstream::pos_type tam = 0;

                tEvento tmpEvento;
                long before = SDL_GetTicks();
                //Esperamos a que se carguen al menos 50K de la cancion
                while ((tam = dir.filesize(file.c_str())) < 50000 && SDL_GetTicks() - before < 10000){
                    this->clearScr(cGrisOscuro);
                    tmpEvento = WaitForKey();
                    procesarControles(getMenu(PANTALLAREPRODUCTOR), &tmpEvento, NULL);
                    this->flipScr();
                    SDL_Delay(20);
                }
                salir = bucleReproductor();
            }
        } else {
            Traza::print("Lanzando reproduccion de ficheros local", W_DEBUG);
            salir = bucleReproductor();
            Traza::print("Finalizando reproduccion de ficheros local", W_DEBUG);
        }

        UIButton * objButtonRandom = (UIButton *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnRandom);
        UIButton * objButtonRepeat = (UIButton *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnRepeat);

        if (playList->getPosActualLista() >= playList->getSize() - 1 && objButtonRandom->getIcon() != btn_random){
            //Caso del fin de la lista de reproduccion. Comprobamos si esta activado el repeat,
            //y en ese caso volvemos al inicio de la lista
            if (objButtonRepeat->getIcon() == btn_repeat){
                playList->setPosActualLista(0);
                playList->setLastSelectedPos(0);
            } else {
                return 0;
            }
        } else if (player->getStatus() == FINISHEDSONG){
            //Caso en el que termina la cancion. Comprobamos si esta activado el orden aleatorio,
            //y en ese caso vamos a una cancion aleatoria
            if (objButtonRandom->getIcon() == btn_random && playList->getSize() > 0){
                static unsigned int latestRandomValue = 0;
                unsigned int next = playList->getSize()-1;

//                if (objButtonRepeat->getIcon() == btn_repeat_off
//                    && mySongsReproduced.getNSongsReproduced() == playList->getSize())
//                {
//                    //Ya hemos reproducido todas las canciones de forma aleatoria. Si no
//                    //Tenemos activado el boton Repeat, no seguimos reproduciendo
//                    return 0;
//                } else {
                    Traza::print("playList->getSize() - 1", playList->getSize() - 1, W_DEBUG);
                    //Se limita que no saldra la misma cancion hasta despues de haber reproducido
                    //un maximo de MAXSONG_REPEAT_AVOID veces con la llamada a mySongsReproduced.isReproduced
                    int contadorBucle = 0;
                    do{
                        next = Constant::getRandomRange(0, playList->getSize() - 1);
                        Traza::print("next", next, W_DEBUG);
                        contadorBucle++;
                    } while ( (next == latestRandomValue || mySongsReproduced.isReproduced(next))
                             && playList->getSize() > 1 && contadorBucle < MAXSONG_REPEAT_AVOID * 2
                             );

                    playList->setPosActualLista(next);
                    playList->setLastSelectedPos(next);
                    latestRandomValue = next;
                    mySongsReproduced.addSongReproduced(next, playList->getSize());
//                }
            } else {
                playList->nextSelectedPos();
            }
        }
    } while (!salir && player->getStatus() != STOPED && playList->getPosActualLista() < playList->getSize());
    
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setIcon(control_play);
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnPlay)->setImgDrawed(false);
    
    return 0;
}
/**
*
*/
bool Iofrontend::bucleReproductor(){
    Traza::print("Iofrontend::bucleReproductor", W_INFO);
    bool salir = false; //Control del bucle unicamente
    //Solo forzamos la salida en el caso de bloqueos. Por ahora solo se pueden producir
    //cuando esperamos a cargar el buffer del streamming
    bool salidaForzada = false; 
    long delay = 0;
    unsigned long before = 0;
    unsigned long timer1s = 0;
    unsigned long timerPanelMedia = 0;
    ignoreButtonRepeats = true;
    tEvento askEvento;
    long lenSongSec = 0;
    tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
    bool panelMediaVisible = true;
    player->setSongDownloaded(false);

    try {
        //Obtenemos de la lista de reproduccion, el tiempo, el nombre y la ruta de la cancion.
        UIListGroup *playList = ((UIListGroup *)obj->getObjByName(playLists));
        string cancion = playList->getValue(playList->getLastSelectedPos());
        string time = playList->getCol(playList->getLastSelectedPos(), 3)->getValor();
        string file = Constant::getAppDir() + tempFileSep + "temp.ogg";
        Traza::print("cancion: " + cancion + " time: " + time, W_DEBUG);

        //Obtenemos el codigo del album y la cancion que esta reproduciendose actualmente
        UITreeListBox *objAlbumList = ((UITreeListBox *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(albumList));
        posAlbumSelected = objAlbumList->getLastSelectedPos();
        posSongSelected = playList->getLastSelectedPos();

        //Asignamos el tiempo total de la cancion
        lenSongSec = time.empty() ? 0 : floor(Constant::strToTipo<double>(time));
        //Una vez sabemos el maximo de tiempo, damos valor a la barra de progreso con el maximo de segundos
        //y al label para mostrar el total de tiempo de la pelicula
        UIProgressBar *objProg = (UIProgressBar *)obj->getObjByName(progressBarMedia);
        objProg->setProgressMax(lenSongSec);
        obj->getObjByName(mediaTimerTotal)->setLabel(Constant::timeFormat(lenSongSec));

        clearEvento(&askEvento);
        timer1s = SDL_GetTicks();
        timerPanelMedia = timer1s;
        setPanelMediaVisible(true);
        tscreenobj screenEvents;
        screenEvents.drawComponents = true;
        screenEvents.execFunctions = true;
        //Inicializamos el reproductor
        player->setScreen(screen);
        player->initAudio();
        player->setStatus(PLAYING);
        player->setViewSpectrum(getMenu(PANTALLAREPRODUCTOR)->getObjByName(spectrum)->isEnabled());
        player->setObjectsMenu(getMenu(PANTALLAREPRODUCTOR));

        Dirutil dir;
        
        if (dir.existe(cancion)){
            //Local file reproduction case
            player->setFilename(cancion);
            player->setSongDownloaded(true);
        } else {
            //Streamming file reproduction case
            player->setFilename(file);
            player->setSongDownloaded(false);
        }

        if (threadPlayer != NULL){
            delete threadPlayer;
            threadPlayer = NULL;
        }

        obj->getObjByName(statusMessage)->setLabel(playList->getCol(playList->getLastSelectedPos(), 0)->getTexto());
        //Lanzamos el thread del reproductor
        threadPlayer = new Thread<AudioPlayer>(player, &AudioPlayer::loadFile);
        if (threadPlayer->start())
            Traza::print("Thread reproductor started with id: ", threadPlayer->getThreadID(), W_DEBUG);

        Traza::print("bucleReproductor player started", W_DEBUG);
        //Obtiene las letras de la cancion actual
        getLyricsFromActualSong();
        Traza::print("Estado Cancion 0",player->getStatus(), W_PARANOIC);

        do{
            Traza::print("Estado Cancion 1", player->getStatus(), W_PARANOIC);
            clearScr(cGrisOscuro);
            //Procesamos los controles de la aplicacion
            askEvento = WaitForKey();
            before = SDL_GetTicks();

            //Actualizamos el indicador de la barra de progreso y del tiempo actual
            if (before - timer1s > 500 && panelMediaVisible){
                obj->getObjByName(mediaTimer)->setLabel(Constant::timeFormat(player->getActualPlayTime()/1000));
                objProg->setProgressPos(player->getActualPlayTime()/1000);
                timer1s = before;
            }
            Traza::print("Estado Cancion 2", player->getStatus(), W_PARANOIC);
            //Procesamos los eventos para cada elemento que pintamos por pantalla
            procesarControles(obj, &askEvento, &screenEvents);
            Traza::print("Estado Cancion 3", player->getStatus(), W_PARANOIC);
            //Si pulsamos escape, paramos la ejecucion
            salir = player->getStatus() == STOPED || player->getStatus() == FINISHEDSONG;

            if (salir && (player->getStatus() == STOPED
                    || player->getStatus() == FINISHEDSONG))
                Traza::print("Saliendo por fin de cancion",player->getStatus(), W_DEBUG);

            Traza::print("Estado Cancion 4", player->getStatus(), W_PARANOIC);

            //Si estamos esperando para cargar el buffer y pulsamos escape, 
            //cancelamos el thread de descarga y salimos
            if (player->getStatus() == PAUSEDTOLOADBUFFER){
                if ((askEvento.isKey && askEvento.key == SDLK_ESCAPE) || askEvento.quit){
                    Traza::print("Estado Cancion 5 stop", player->getStatus(), W_DEBUG);
                    player->stop();
                    player->setSongDownloaded(true);
                    Traza::print("Esperando a que termine el thread", player->getStatus(), W_DEBUG);
                    waitFinishThread(threadPlayer, MAX_STOP_TIMEOUT);
                    Traza::print("Thread Terminado", player->getStatus(), W_DEBUG);
                    salir = true;
                    salidaForzada = true;
                }
            } else {
                if ((askEvento.isKey && askEvento.key == SDLK_SPACE) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_START)){
                    player->pause();
                    timerPanelMedia = SDL_GetTicks();
                } else if ((askEvento.isKey && askEvento.key == SDLK_RIGHT) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_RIGHT)){
                    if (player->isSongDownloaded() && objProg->getProgressPos() + 10 < objProg->getProgressMax()){
                        reloadSong(posAlbumSelected, posSongSelected);
                        player->forward(10000);
                        timerPanelMedia = SDL_GetTicks();
                    }
                } else if ((askEvento.isKey && askEvento.key == SDLK_LEFT) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_LEFT)){
                    if (player->isSongDownloaded()){
                        reloadSong(posAlbumSelected, posSongSelected);
                        player->rewind(10000);
                        timerPanelMedia = SDL_GetTicks();
                    }
                }
            }

            if (askEvento.resize){
                clearScr(cGrisOscuro);
                setDinamicSizeObjects();
            } else if (askEvento.isMouseMove){
                if (askEvento.mouse_y > calculaPosPanelMedia()){
                    timerPanelMedia = SDL_GetTicks();
                }
            } else if (askEvento.quit){
                Traza::print("Estado Cancion 5.1 stop", player->getStatus(), W_DEBUG);
                player->stop();
                Traza::print("Esperando a que termine el thread", player->getStatus(), W_DEBUG);
                waitFinishThread(threadPlayer, MAX_STOP_TIMEOUT);
                Traza::print("Thread Terminado", player->getStatus(), W_DEBUG);
                salir = true;
                exit(0);
            }

            Traza::print("Estado Cancion 7", player->getStatus(), W_PARANOIC);
            //Recargamos la cancion si se ha terminado la descarga de la misma
            //y no se habia obtenido informacion del tiempo total de la cancion
            if (threadDownloader != NULL){
                //Comprobamos si se ha terminado la descarga en cualquier caso
                if (!threadDownloader->isRunning()){
                    reloadSong(posAlbumSelected, posSongSelected);
                    player->setSongDownloaded(true);
                    threadDownloader = NULL;
                }
            }
            delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
            if(delay > 0) SDL_Delay(delay);
            Traza::print("Estado Cancion 6", player->getStatus(), W_PARANOIC);
            refreshSpectrum(player);
            Traza::print("Estado Cancion 8", player->getStatus(), W_PARANOIC);
            flipScr();
            
        } while (!salir);

        //Reseteamos la barra de progreso
        obj->getObjByName(mediaTimerTotal)->setLabel(Constant::timeFormat(0));
        obj->getObjByName(mediaTimer)->setLabel(Constant::timeFormat(0));
        objProg->setProgressMax(0);
        objProg->setProgressPos(0);
        player->setSongDownloaded(true);
        //player->stop();
        Traza::print("Fin del bucle de reproductor", W_DEBUG);
    } catch (Excepcion &e){
        Traza::print("Excepcion en bucle de reproductor: " + string(e.getMessage()), W_ERROR);
    }
    return salidaForzada;
}

/**
 * 
 * @param posAlbumSelected
 * @param posSongSelected
 */
void Iofrontend::reloadSong(int posAlbumSelected, int posSongSelected){
    Traza::print("Iofrontend::reloadSong", W_INFO);

    if (threadDownloader != NULL && player != NULL){
        Traza::print("Iofrontend::reloadSong", W_DEBUG);
        tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
        UITreeListBox *objAlbumList = ((UITreeListBox *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(albumList));
        //Cuando detectamos que se ha descargado el fichero totalmente, recargamos
        //el thread para que se pueda avanzar o retroceder. Esto es debido a limitaciones
        //en la libreria de SDL para Mix_LoadMUS_RW con un fichero a medio descargar
        //por streaming. Solo lo hacemos si se ha terminado el thread de descarga, se esta
        //reproduciendo una cancion y no se ha seleccionado otro album que haya cambiado la lista
        if (!threadDownloader->isRunning() && !player->isSongDownloaded() && player->getStatus() == PLAYING &&
            posAlbumSelected == objAlbumList->getLastSelectedPos()){
            UIListGroup *playList = ((UIListGroup *)obj->getObjByName(playLists));
            Traza::print("Descarga completada correctamente", W_DEBUG);
            //Indicamos que la cancion se ha terminado de descargar
            player->setSongDownloaded(true);
            //Realizamos la peticion de stop y esperamos a que termine el thread
            player->stop();
            threadPlayer->join();
            //Obtenemos la posicion en la que terminamos la reproduccion. Debe ser justo en un segundo
            //especifico, no nos vale en milesimas puesto que no tenemos tanta precision con SDL_MUSIC.
            //Esto se controla en el propio bucle del AudioPlayer
            long posActual = player->getActualPlayTime();
            Traza::print("Thread detenido correctamente", W_DEBUG);
            player->setPosicionInicial(posActual);
            delete threadPlayer;
            threadPlayer = new Thread<AudioPlayer>(player, &AudioPlayer::loadFile);
            if (threadPlayer->start())
                Traza::print("Thread reproductor started with id: ", threadPlayer->getThreadID(), W_DEBUG);

            Traza::print("reloadSong player started", W_DEBUG);
            threadDownloader = NULL;

            //Una vez que el fichero esta descargado, ya podemos obtener los tags id3 que contienen
            //mayor informacion sobre la cancion que se esta reproduciendo
            //Nos aseguramos que modificamos la posicion correcta con el campo posSongSelected
            playList->setLastSelectedPos(posSongSelected);
            Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshPlayListMetadata);
            thread->start();
        }
    }
}

/**
*
*/
int Iofrontend::calculaPosPanelMedia(){
    int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
    return (bottom - PROGRESSSEPBOTTOM - SEPPANELMEDIA);
}

/**
*
*/
void Iofrontend::autenticateAndRefresh(){
    Traza::print("Iofrontend::autenticateAndRefresh", W_INFO);

    int errorServers = autenticarServicios();
    if (errorServers == MAXSERVERS){
        showMessage("No se ha podido conectar a los servidores", 2000);
    } else {
        refrescarAlbums();
    }
}

void Iofrontend::refrescarAlbums(){
    tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
    juke->setObjectsMenu(obj);
    Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshAlbumAndPlaylist);
    tEvento evento;
    tEvento eventoNull;

    clearScr();
    procesarControles(obj, &eventoNull, NULL);
    drawTextCent(Constant::txtDisplay("Obteniendo %C3%81lbums. Espere...").c_str(),0,-70,true,true, cBlanco);
    flipScr();
    pintarIconoProcesando(true);
    thread->start();
    clearEvento(&evento);
    clearEvento(&eventoNull);

    while (thread->isRunning() && evento.key != SDLK_ESCAPE && !evento.quit){
        evento = WaitForKey();
        procesarControles(obj, &eventoNull, NULL);
        pintarIconoProcesando(false);
    }

    if (evento.key == SDLK_ESCAPE || evento.quit){
        juke->abortServers();
        thread->join();
        WaitForKey();
    }

    delete thread;

    ((UIListGroup *)obj->getObjByName(playLists))->setImgDrawed(false);
    ((UITreeListBox *)obj->getObjByName(albumList))->setImgDrawed(false);
}

/**
* Se llama cuando se hace doble click o se selecciona un nuevo album de la lista
*/
int Iofrontend::selectAlbum(tEvento *evento){
    Traza::print("Iofrontend::selectAlbum", W_INFO);
    UITreeListBox *objAlbumList = ((UITreeListBox *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(albumList));
    UIListGroup *playList = ((UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists));

    string albumSelected = objAlbumList->getValue(objAlbumList->getLastSelectedPos());
    Dirutil dir;

    if (dir.existe(albumSelected)){
        this->addLocalAlbum(albumSelected);
    } else {
        Traza::print("Comprobando autorizacion selectAlbum...", W_DEBUG);

        tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);

        int idservidor = Constant::strToTipo<int>(objAlbumList->getDestino(objAlbumList->getLastSelectedPos()));
        juke->setObjectsMenu(obj);
        juke->setServerSelected(idservidor);
        juke->setAlbumSelected(objAlbumList->getValue(objAlbumList->getLastSelectedPos()));

        Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshPlaylist);

        if (thread->start())
            Traza::print("Thread started with id: ",thread->getThreadID(), W_DEBUG);
        pintarIconoProcesando(true);


        tEvento evento;
        while (thread->isRunning() && evento.key != SDLK_ESCAPE && !evento.quit){
            evento = WaitForKey();
            procesarControles(obj, &evento, NULL);
            pintarIconoProcesando(false);
        }

        UITextElementsArea *textElems = (UITextElementsArea *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(LetrasBox);

        if (textElems->isVisible() && !playList->isVisible()){
            playList->setVisible(true);
            textElems->setVisible(false);
            getMenu(PANTALLAREPRODUCTOR)->getObjByName(NewWindowIco)->setVisible(false);
        }

        playList->setPosActualLista(0);
        playList->refreshLastSelectedPos();
        procesarControles(obj, &evento, NULL);
        flipScr();
        delete thread;

    }
    return 0;
}



/**
*
*/
//string Iofrontend::autenticarDropbox(){
//    Traza::print("Iofrontend::autenticarDropbox", W_INFO);
//    Dropbox *dropbox = new Dropbox();
//    if (this->accessToken.empty()){
//        Traza::print("Comprobando autorizacion autenticarDropbox...", W_DEBUG);
//        tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
//        Thread<Dropbox> *thread = new Thread<Dropbox>(dropbox, &Dropbox::authenticate);
//        tEvento evento;
//
//        clearScr();
//        procesarControles(obj, &evento, NULL);
//        drawTextCent("Conectando a dropbox. Espere...", 0, -70,true,true, cBlanco);
//        flipScr();
//
//        pintarIconoProcesando(true);
//
//        thread->start();
//        while (thread->isRunning()){
//            evento = WaitForKey();
//            procesarControles(obj, &evento, NULL);
//            pintarIconoProcesando(false);
//        }
//        procesarControles(obj, &evento, NULL);
//
//        string mensaje = "Para usar la aplicaciÃƒÂ³n debes dar permisos desde tu cuenta de dropbox. ";
//        mensaje.append("A continuaciÃƒÂ³n se abrirÃƒÂ¡ un explorador. Debes logarte en Dropbox y pulsar el botÃƒÂ³n de \"PERMITIR\".");
//        mensaje.append("Seguidamente deberÃƒÂ¡s copiar el cÃƒÂ³digo obtenido y pegarlo en la ventana de Onmusik que aparecerÃƒÂ¡ a continuaciÃƒÂ³n.");
//
//        if (thread->getExitCode() != ERRORCONNECT){
//            this->accessToken = dropbox->getAccessToken();
//            //Si despues de autenticarse, no se ha podido obtener el access token, lo obtenemos manualmente
//            if (this->accessToken.empty()){
//                bool permiso = casoPANTALLACONFIRMAR("Autorizar aplicaciÃƒÂ³n", mensaje);
//                if (permiso){
//                    dropbox->launchAuthorize(cliendid);
//                    string code = casoPANTALLAPREGUNTA("Autorizar aplicaciÃƒÂ³n", "Introduce el campo obtenido de la pÃƒÂ¡gina de dropbox (CTRL+V)");
//                    if (!code.empty()){
//                        clearScr(cGrisOscuro);
//                        procesarControles(obj, &evento, NULL);
//                        flipScr();
//                        this->accessToken = dropbox->storeAccessToken(cliendid, secret, code, false);
//                    }
//                }
//            }
//        } else {
//            showMessage("No se ha podido autenticar en dropbox. Revise su conexiÃƒÂ³n o especifique datos de proxy", 4000);
//        }
//        delete thread;
//        delete dropbox;
//    }
//    return this->accessToken;
//}

int Iofrontend::autenticarServicios(){
    Traza::print("Iofrontend::autenticarServicios", W_INFO);
    int someErrorToken = 0;

    juke->getServerCloud(GOOGLEDRIVESERVER)->setClientid(googleClientId);
    juke->getServerCloud(GOOGLEDRIVESERVER)->setSecret(googleSecret);
    juke->getServerCloud(DROPBOXSERVER)->setClientid(cliendid);
    juke->getServerCloud(DROPBOXSERVER)->setSecret(secret);
    juke->getServerCloud(ONEDRIVESERVER)->setClientid(onedriveClientId);
    juke->getServerCloud(ONEDRIVESERVER)->setSecret(onedriveSecret);

    Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::authenticateServers);

    //Realizamos la conexion de prueba a google y a dropbox
    if (thread->start())
        Traza::print("Thread started with id: ",thread->getThreadID(), W_DEBUG);
    
    tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
    tEvento evento;
    tEvento eventoNull;
    clearScr();
    procesarControles(obj, &eventoNull, NULL);
    drawTextCent("Conectando a servicios. Espere...", 0, -70,true,true, cBlanco);
    flipScr();
    
    pintarIconoProcesando(true);
    do{
        evento = WaitForKey();
        procesarControles(obj, &eventoNull, NULL);
        pintarIconoProcesando(false);
    }while (thread->isRunning() && evento.key != SDLK_ESCAPE && !evento.quit);
    procesarControles(obj, &eventoNull, NULL);
    
    if (evento.key == SDLK_ESCAPE || evento.quit){
        juke->abortServers();
        thread->join();
        evento = WaitForKey();
        someErrorToken = MAXSERVERS;
    } else {
        //En el caso de que sea la primera vez que se lanza la aplicacion, no existira
        //el fichero token.ini. Solo en ese caso, lanzamos el proceso de autenticacion
        string rutaIni = Constant::getAppDir() + Constant::getFileSep() + TOKENFILENAME;
        Dirutil dir;
        if (!dir.existe(rutaIni)){
            bool salir = false;
//            do {
                //En esta casuistica lanzamos el proceso hasta que tengamos todos los
                //servidores configurados
                someErrorToken = AddServer(&eventoNull);
//            } while (someErrorToken > 0);
        }
    }
    delete thread;
    return someErrorToken;
}

/**
*
*/
int Iofrontend::comprobarTokenServidores(){
    int someErrorToken = MAXSERVERS;
    tmenu_gestor_objects *obj = getMenu(PANTALLALOGIN);

    for (int i=0; i < MAXSERVERS; i++){
        if (errorTokenServidor(i)){
            someErrorToken = i;
        }
        string strAccessToken = juke->getServerCloud(i)->getAccessToken();
        if (i == DROPBOXSERVER){
            obj->getObjByName(btnDropbox)->setEnabled(strAccessToken.empty());
            obj->getObjByName(btnDropbox)->setImgDrawed(false);
        } else if (i == GOOGLEDRIVESERVER){
            obj->getObjByName(btnGoogle)->setEnabled(strAccessToken.empty());
            obj->getObjByName(btnGoogle)->setImgDrawed(false);
        } else if (i == ONEDRIVESERVER){
            obj->getObjByName(btnOnedrive)->setEnabled(strAccessToken.empty());
            obj->getObjByName(btnOnedrive)->setImgDrawed(false);
        } 
    }
    return someErrorToken;
}

/**
*
*/
bool Iofrontend::errorTokenServidor(int servidor){
    bool someErrorToken = false;
    if (juke->getServerCloud(servidor)->getOauthStatus() != ERRORCONNECT){
        string strAccessToken = juke->getServerCloud(servidor)->getAccessToken();
        if (strAccessToken.empty()){
            someErrorToken = true;
        }
    }
    return someErrorToken;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::accionUploadPopup(tEvento *evento){
    Traza::print("Iofrontend::accionUploadPopup", W_INFO);
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    string menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = getMenu(menu);
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    //Comprobamos que efectivamente, el elemento es un popup
    if (obj->getObjectType() == GUIPOPUPMENU){
        UIPopupMenu *objPopup = (UIPopupMenu *)obj;
        //Obtenemos el valor del elemento seleccionado en el popup
        string selected = objPopup->getListValues()->get(objPopup->getPosActualLista());
        int servidor = Constant::strToTipo<int>(objPopup->getListDestinos()->get(objPopup->getPosActualLista()));

        if (objPopup->getCallerPopup() != NULL){
            objsMenu->setFirstFocus();
            Traza::print("Subiendo para el servidor: " + selected, W_DEBUG);
            if (errorTokenServidor(servidor)){
                if (AddServer(evento) != MAXSERVERS){
                    refrescarAlbums();
                }
            } else {
                uploadToServer(evento, servidor);
            }
        }
    }
    return 0;
}

/**
*
*/
int Iofrontend::uploadToServer(tEvento *evento, int idServer){
    Traza::print("Iofrontend::uploadToServer", W_INFO);
    long delay = 0;
    unsigned long before = 0;

    try{
        Dirutil dir;
        //Abrimos el explorador de archivos y esperamos a que el usuario seleccione un fichero
        //o directorio
        string fichName = showExplorador(evento);
        if (!dir.isDir(fichName)){
            fichName = dir.getFolder(fichName);
        }
        //Si se ha seleccionado algo, establecemos el texto en el objeto que hemos recibido por parametro
        if (!fichName.empty()){
            Traza::print("Comprobando autorizacion...", W_DEBUG);
            juke->setObjectsMenu(getMenu(PANTALLAREPRODUCTOR));
            juke->setDirToUpload(fichName);
            juke->setServerSelected(idServer);

            unsigned int nFiles [2] = {0,0};
            vector<string> dirs;
            dir.countDir(fichName.c_str(), nFiles, &dirs, filtroFicheros);

            bool continuar = true;
            if (nFiles[0] > 5 || nFiles[1] > 30){
                string msg = "%C2%BFEst%C3%A1s seguro de subir " + Constant::TipoToStr(nFiles[0]);
                msg.append(" %C3%A1lbums y " + Constant::TipoToStr(nFiles[1]) + " canciones%3F");
                continuar = casoPANTALLACONFIRMAR("Advertencia", msg);
            }

            if (continuar){
                string msg = "%C2%BFDeseas recodificar los ficheros en formato ogg (Ogg Vorbis)%3F";
                bool recodificar = casoPANTALLACONFIRMAR("Advertencia", msg);
                
                Thread<Jukebox> *thread;
                if (recodificar){
                    juke->setFilterUploadExt(".ogg");
                    thread = new Thread<Jukebox>(juke, &Jukebox::convertAndUpload);
                } else {
                    juke->setFilterUploadExt(".mp3");
                    thread = new Thread<Jukebox>(juke, &Jukebox::upload);
                }
                
                if (thread->start())
                    Traza::print("Thread started with id: ",thread->getThreadID(), W_DEBUG);
            }

        }
    } catch (Excepcion &e){
        Traza::print("uploadDiscToDropbox: " + string(e.getMessage()), W_ERROR);
    }
}

/**
* Copia el texto seleccionado desde un popup al elemento que lo llama. Por ahora solo lo hace
* en campos input.
*/
int Iofrontend::accionAlbumPopup(tEvento *evento){
    Traza::print("Iofrontend::accionAlbumPopup", W_INFO);
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    string menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = getMenu(menu);
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    //Comprobamos que efectivamente, el elemento es un popup
    if (obj->getObjectType() == GUIPOPUPMENU){
        UIPopupMenu *objPopup = (UIPopupMenu *)obj;
        //Obtenemos el valor del elemento seleccionado en el popup
//        string selected = objPopup->getListValues()->get(objPopup->getPosActualLista());
        if (objPopup->getCallerPopup() != NULL){
            //Obtenemos el objeto llamador
            if (objPopup->getCallerPopup()->getObjectType() == GUITREELISTBOX){
                UITreeListBox *objList = (UITreeListBox *)objPopup->getCallerPopup();
                TreeNode node = objList->get(objList->getPosActualLista());
                
                string borrar = node.value;
                string text = node.text;
                int idservidor = Constant::strToTipo<int>(node.dest);
                Traza::print("Iofrontend::accionAlbumPopup. Borramos: " + borrar, W_DEBUG);

                bool confirm = false;

                confirm = casoPANTALLACONFIRMAR("Borrar %C3%A1lbum",
                                "%C2%BFEst%C3%A1 seguro de que desea eliminar: " + text + "%3F");

                if (confirm){
                    IOauth2 *server = juke->getServerCloud(idservidor);
                    bool res = server->deleteFiles(borrar, server->getAccessToken());
                    if (res){
                        showMessage("%C3%81lbum eliminado correctamente", 2000);
                        tEvento askEvento;
                        this->clearScr(cGrisOscuro);
                        clearEvento(&askEvento);
                        procesarControles(getMenu(menu), &askEvento, NULL);
                        flipScr();
                        //autenticateAndRefresh();
                        refrescarAlbums();
                    }
                    else
                        showMessage("Error al eliminar el %C3%81lbum", 2000);
                }
            }
        }
    }
    return 0;
}

/**
*
*/
void Iofrontend::refreshSpectrum(AudioPlayer *player){
    Traza::print("Iofrontend::refreshSpectrum", W_PARANOIC);
    UISpectrum *obj = ((UISpectrum *) getMenu(PANTALLAREPRODUCTOR)->getObjByName(spectrum));
    AudioPlayer::TStreamMusicPlaying *streamData = player->getStreamMusicPlaying();

    if (obj->isEnabled() && player->getNeed_refresh() == 1){
        //No permitimos que se actualice el buffer mientras vamos a pintar,
        //porque sino se producen artefactos por intentar dibujar algo que se
        //puede estar copiando en memoria en un thread a parte
        streamData->canUpdateBuffer = false;
        //Copiamos el puntero con la informacion de los datos de audio
        obj->buf = streamData->stream[streamData->which];

        obj->arrFreqVis = streamData->arrFreqVis;
        //De forma alternativa, podemos copiar el buffer entero en lugar de
        //usar un puntero a memoria
//        if (obj->buf == NULL)
//            obj->buf = new Sint16[2*BUFFERSPECTRUMVIS];
//        memcpy(obj->buf, streamData.stream[streamData.which], BUFFERSPECTRUMVIS*2);
        //Establecemos el tamanyo del buffer para que lo sepa el metodo de dibujado
        //y no produzca overflow
        obj->setBuffSize(streamData->bufSize);
        //Indicamos que debemos redibujar la imagen del visualizador del audio
        obj->setImgDrawed(false);
        //Especificamos que necesitamos una nueva muestra de audio cuando sea posible
        player->setNeed_refresh(0);
    }

    //Si ya hemos dibujado la imagen correctamente, indicamos al thread del audio
    //que podemos volver a rellenar el buffer del audio
    if (obj->getImgDrawed() == true){
        streamData->canUpdateBuffer = true;
    }
}

/**
*
*/
void Iofrontend::actualizaciones(){
    Traza::print("Iofrontend::actualizaciones", W_INFO);
    Updater *updater = new Updater();
    string ruta = Constant::getAppDir() + Constant::getFileSep() + "rsc";
    if (updater->needUpdate(ruta)){
        updater->setRuta(ruta);
        Thread<Updater> *threadUpdate = new Thread<Updater>(updater, &Updater::updates);
        pintarIconoProcesando(true);
        drawTextCent("Actualizando. Espere...",0,-70,true,true, cBlanco);
        flipScr();
        threadUpdate->start();

        tEvento eventoNulo;
        tEvento evento;
        clearEvento(&evento);

        while (threadUpdate->isRunning() && evento.key != SDLK_ESCAPE && !evento.quit ){
            procesarControles(getMenu(PANTALLAREPRODUCTOR), &eventoNulo, NULL);
            pintarIconoProcesando(false);
            evento = WaitForKey();
        }

        if (evento.key == SDLK_ESCAPE || evento.quit){
            updater->abort();
            threadUpdate->join(); //Esperamos a la finalizacion del thread
            WaitForKey();
            
        }
        
        cout << "salida del thread " << threadUpdate->getExitCode() << endl;
        delete threadUpdate;
        clearScr();
        procesarControles(getMenu(PANTALLAREPRODUCTOR), &eventoNulo, NULL);
        flipScr();
    }
    delete updater;
}

/**
*
*/
void Iofrontend::bienvenida(){
    Traza::print("bienvenida: Inicio", W_INFO);
    UITreeListBox *objAlbumList = ((UITreeListBox *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(albumList));

    bool someAccesToken = false;
    for (int i=0; i < MAXSERVERS; i++){
        if (!juke->getServerCloud(i)->getAccessToken().empty()){
            someAccesToken = true;
        }
    }

    if (someAccesToken && objAlbumList->getSize() == 0){
        ignoreButtonRepeats = true;
        bool salir = false;
        tEvento askEvento;
        clearEvento(&askEvento);
        bool salida = false;
        string menuInicial = getSelMenu();

        //Procesamos el menu antes de continuar para que obtengamos la captura
        //de pantalla que usaremos de fondo
        procesarControles(getMenu(menuInicial), &askEvento, NULL);
        SDL_Rect iconRectFondo = {0, 0, this->getWidth(), this->getHeight()};
        SDL_Surface *mySurface = NULL;
        takeScreenShot(&mySurface, iconRectFondo);

        //Seguidamente cambiamos la pantalla a la de la confirmacion
        setSelMenu(PANTALLABIENVENIDA);
        tmenu_gestor_objects *objMenu = getMenu(PANTALLABIENVENIDA);
        //objMenu->getObjByName(uiborde)->setLabel(titulo);

        string txtDetalle = "Que emocionante! Parece que es la primera vez que accedes a Onmusik!\n";
        txtDetalle.append("Empieza subiendo tus canciones haciendo click en el Bot%C3%B3n indicado con la flecha\n\n");
        txtDetalle.append("Para subir cada disco, puedes seleccionar la carpeta que contiene todas las canciones");
        txtDetalle.append("o alguna de las canciones contenidas en la carpeta\n");

        UITextElementsArea *textElems = (UITextElementsArea *)objMenu->getObjByName(uitextosBox);
        textElems->setImgDrawed(false);
        textElems->setFieldText(uilabelDetalle, Constant::txtDisplay(txtDetalle));

        long delay = 0;
        unsigned long before = 0;
        objMenu->setFocus(0);

        do{
            before = SDL_GetTicks();
            askEvento = WaitForKey();
            printScreenShot(&mySurface, iconRectFondo);
            procesarControles(objMenu, &askEvento, NULL);
            flipScr();
            salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
            (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

            if (objMenu->getObjByName(btnSiConfirma)->getTag().compare("selected") == 0){
                salir = true;
                salida = true;
                objMenu->getObjByName(btnSiConfirma)->setTag("");
                Traza::print("Detectado SI pulsado", W_DEBUG);
            }

            delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
            if(delay > 0) SDL_Delay(delay);
        } while (!salir);

        setSelMenu(menuInicial);
    }
}

/**
*
*/
void Iofrontend::addLocalAlbum(string ruta){
    tmenu_gestor_objects *pantRepr = getMenu(PANTALLAREPRODUCTOR);
    Traza::print("Iofrontend::addLocalAlbum. Reproduciendo: " + ruta, W_INFO);

    try{
        Dirutil dir;
        pantRepr->setFocus(playLists);

        if (!dir.isDir(ruta)){
            Traza::print("Iofrontend::addLocalAlbum. Detectado fichero", W_DEBUG);
            juke->setObjectsMenu(pantRepr);
            juke->setRutaInfoId3(ruta);
            juke->setCanPlay(false);
            Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshPlayListMetadataFromId3Dir);

            if (thread->start()){
                Traza::print("Thread addLocalAlbum started with id: ",thread->getThreadID(), W_DEBUG);
                tEvento evento;
                long before = SDL_GetTicks();
                while (!juke->isCanPlay() && SDL_GetTicks() - before < 7000){
                    procesarControles(pantRepr, &evento, NULL);
                }

                if (player->getStatus() != PLAYING){
                    Traza::print("Thread addLocalAlbum Lanzando Playlist", W_DEBUG);
                    pantRepr->setFocus(playLists);
                    accionesPlaylist(NULL);
                }
            }
        } else {
            Traza::print("Iofrontend::addLocalAlbum. Detectado directorio", W_DEBUG);
            juke->setObjectsMenu(pantRepr);
            juke->setRutaInfoId3(ruta);
            Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshPlayListMetadataFromId3Dir);

            if (thread->start())
                Traza::print("Thread addLocalAlbum started with id: ",thread->getThreadID(), W_DEBUG);
        }
    } catch (Excepcion &e){
        Traza::print("Excepcion al cargar album local: " + string(e.getMessage()), W_DEBUG);
    }
}

/**
*
*/
int Iofrontend::casoPANTALLALOGIN(string titulo, string txtDetalle, bool enableServersUp){
    ignoreButtonRepeats = true;
    Traza::print("casoPANTALLALOGIN: Inicio", W_INFO);
    bool salir = false;
    tEvento askEvento;
    clearEvento(&askEvento);
    int salida = MAXSERVERS;
    string menuInicial = getSelMenu();
    
    titulo = Constant::txtDisplay(titulo);
    txtDetalle = Constant::txtDisplay(txtDetalle);
    
    //Procesamos el menu antes de continuar para que obtengamos la captura
    //de pantalla que usaremos de fondo
    procesarControles(getMenu(menuInicial), &askEvento, NULL);
    SDL_Rect iconRectFondo = {0, 0, this->getWidth(), this->getHeight()};
    SDL_Surface *mySurface = NULL;
    drawRectAlpha(iconRectFondo.x, iconRectFondo.y, iconRectFondo.w, iconRectFondo.h , cNegro, ALPHABACKGROUND);
    takeScreenShot(&mySurface, iconRectFondo);

    //Seguidamente cambiamos la pantalla a la de la confirmacion
    setSelMenu(PANTALLALOGIN);
    tmenu_gestor_objects *objMenu = getMenu(PANTALLALOGIN);
    objMenu->getObjByName(uiborde)->setLabel(titulo);

    UITextElementsArea *textElems = (UITextElementsArea *)objMenu->getObjByName(uitextosBox);
    textElems->setImgDrawed(false);
    textElems->setFieldText(uilabelDetalle, txtDetalle);

    if (enableServersUp){
        objMenu->getObjByName(btnDropbox)->setEnabled(!objMenu->getObjByName(btnDropbox)->isEnabled());
        objMenu->getObjByName(btnDropbox)->setImgDrawed(false);
        objMenu->getObjByName(btnGoogle)->setEnabled(!objMenu->getObjByName(btnGoogle)->isEnabled());
        objMenu->getObjByName(btnGoogle)->setImgDrawed(false);
        objMenu->getObjByName(btnOnedrive)->setEnabled(!objMenu->getObjByName(btnOnedrive)->isEnabled());
        objMenu->getObjByName(btnOnedrive)->setImgDrawed(false);
    }

    long delay = 0;
    unsigned long before = 0;
    objMenu->setFocus(0);

    do{
        before = SDL_GetTicks();
        askEvento = WaitForKey();
//        clearScr(cBgScreen);
        printScreenShot(&mySurface, iconRectFondo);
        procesarControles(objMenu, &askEvento, NULL);

        flipScr();
        salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
        (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

        if (objMenu->getObjByName(btnGoogle)->getTag().compare("selected") == 0){
            salir = true;
            salida = GOOGLEDRIVESERVER;
            objMenu->getObjByName(btnGoogle)->setTag("");
            Traza::print("Detectado Google pulsado", W_DEBUG);
        } else if (objMenu->getObjByName(btnDropbox)->getTag().compare("selected") == 0){
            salir = true;
            salida = DROPBOXSERVER;
            objMenu->getObjByName(btnDropbox)->setTag("");
            Traza::print("Detectado Dropbox pulsado", W_DEBUG);
        } else if (objMenu->getObjByName(btnOnedrive)->getTag().compare("selected") == 0){
            salir = true;
            salida = ONEDRIVESERVER;
            objMenu->getObjByName(btnOnedrive)->setTag("");
            Traza::print("Detectado Onedrive pulsado", W_DEBUG);
        } else if (objMenu->getObjByName(btnLoginCancel)->getTag().compare("selected") == 0){
            salir = true;
            salida = MAXSERVERS;
            objMenu->getObjByName(btnLoginCancel)->setTag("");
            Traza::print("Detectado NO pulsado", W_DEBUG);
        }

        delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
        if(delay > 0) SDL_Delay(delay);
    } while (!salir);

    setSelMenu(menuInicial);
    return salida;
}

/**
*
*/
int Iofrontend::showPopupUploadCD(tEvento *evento){

    loadComboUnidades(popupUploadCD, PANTALLAREPRODUCTOR, DRIVE_CDROM);


    UIList *combo = (UIList *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(popupUploadCD);
    combo->addElemLista("Opciones CDDB", "cddbopt", bullet_wrench);

    getMenu(PANTALLAREPRODUCTOR)->setFocus(btnAddCD);
    getMenu(PANTALLAREPRODUCTOR)->getObjByName(btnAddCD)->setPopup(true);
    procesarPopups(getMenu(PANTALLAREPRODUCTOR), evento);
    return 0;
}

/**
*
*/
int Iofrontend::accionUploadCDPopup(tEvento *evento){
    Traza::print("Iofrontend::accionUploadCDPopup", W_INFO);
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    string menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = getMenu(menu);
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    //Comprobamos que efectivamente, el elemento es un popup
    if (obj->getObjectType() == GUIPOPUPMENU){
        UIPopupMenu *objPopup = (UIPopupMenu *)obj;
        //Obtenemos el valor del elemento seleccionado en el popup
        string selected = objPopup->getListValues()->get(objPopup->getPosActualLista());
        int servidor = Constant::strToTipo<int>(objPopup->getListDestinos()->get(objPopup->getPosActualLista()));

        if (selected.compare("cddbopt") == 0){
            showCDDBMenuData();
        } else if (objPopup->getCallerPopup() != NULL){
            objsMenu->setFirstFocus();
            Traza::print("Extrayendo cd de la unidad: " + selected, W_DEBUG);
            
            //Comprobamos si ha habido algun error en la obtencion del accesstoken
            comprobarTokenServidores();
            string mensaje = "Selecciona si quieres subir el CD a una cuenta de Google o Dropbox. ";
            int serverSelected = casoPANTALLALOGIN(Constant::toAnsiString("Seleccionar cuenta"), Constant::toAnsiString(mensaje), true);
            if (serverSelected < MAXSERVERS){

                vector<CdTrackInfo *> cdTrackList;
                FreedbQuery query;
                tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);

                juke->setObjectsMenu(obj);
                juke->setCdDrive(selected);
                juke->setCdTrackList(&cdTrackList);
                juke->setCdDrive(selected);
                juke->setExtractionPath(Constant::getAppDir());
                juke->setServerSelected(serverSelected);

                Thread<Jukebox> *threadCD = new Thread<Jukebox>(juke, &Jukebox::searchCddbAlbums);
                waitJukebox(threadCD, PANTALLAREPRODUCTOR);

                string categAlbum;
                string idAlbum;

                if (cdTrackList.size() == 0){
                    setSelMenu(PANTALLAREPRODUCTOR);
//                    return 0;
                } else if (cdTrackList.size() == 1){
                    categAlbum = cdTrackList.at(0)->genre;
                    idAlbum = cdTrackList.at(0)->discId;
                } else {

                    obj = getMenu(PANTALLACDDB);
                    UIListGroup *objCddb = ((UIListGroup *)obj->getObjByName(listAlbumsCddb));

                    for (int i=0; i < cdTrackList.size(); i++){
                        vector <ListGroupCol *> miFila;
                        miFila.push_back(new ListGroupCol(cdTrackList.at(i)->albumName, cdTrackList.at(i)->albumName));
                        miFila.push_back(new ListGroupCol(cdTrackList.at(i)->year, cdTrackList.at(i)->year));
                        miFila.push_back(new ListGroupCol(cdTrackList.at(i)->genre, cdTrackList.at(i)->genre));
                        miFila.push_back(new ListGroupCol(cdTrackList.at(i)->discId, cdTrackList.at(i)->discId));
                        objCddb->addElemLista(miFila);
                    }

                    if (waitAceptCancel(aceptarCddb, cancelarCddb, PANTALLACDDB)){
                        setSelMenu(PANTALLAREPRODUCTOR);
                        categAlbum = objCddb->getCol(objCddb->getPosActualLista(), 2)->getValor();
                        idAlbum = objCddb->getCol(objCddb->getPosActualLista(), 3)->getValor();
                    } else {
                        setSelMenu(PANTALLAREPRODUCTOR);
                        return 0;
                    }
                }

                setSelMenu(PANTALLAREPRODUCTOR);
                juke->setIdSelected(idAlbum);
                juke->setCategSelected(categAlbum);
                juke->setObjectsMenu(getMenu(PANTALLAREPRODUCTOR));
                Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::extraerCD);
                thread->start();
            }
        }
    }
    return 0;
}

/**
*
*/
void Iofrontend::waitJukebox( Thread<Jukebox> *var, string pantalla){
    tmenu_gestor_objects *obj = getMenu(pantalla);
    tEvento evento;
    tEvento eventoNull;
    clearScr();
    procesarControles(obj, &eventoNull, NULL);
    flipScr();
    pintarIconoProcesando(true);
    var->start();
    clearEvento(&evento);
    clearEvento(&eventoNull);

    while (var->isRunning() && evento.key != SDLK_ESCAPE && !evento.quit){
        evento = WaitForKey();
        procesarControles(obj, &eventoNull, NULL);
        pintarIconoProcesando(false);
    }
}

/**
*
*/
int Iofrontend::accionesCddbCancelar(tEvento *evento){
    setSelMenu(PANTALLAREPRODUCTOR);
    return 0;
}

/**
*
*/
int Iofrontend::accionesLetrasBox(tEvento *evento){
    UITextElementsArea *textElems = (UITextElementsArea *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(LetrasBox);
    Traza::print("Iofrontend::accionesLetrasBox", textElems->getSelectedPos(), W_DEBUG);
    if (textElems->getSelectedPos() >= 0){
        string tmpUrl = textElems->getTextVector()->at(textElems->getSelectedPos())->getUrl();
        Traza::print("Iofrontend::accionesLetrasBox: " + tmpUrl, W_DEBUG);
        if (!tmpUrl.empty()){
#ifdef WIN
            char infoBuf[MAX_PATH];
            GetWindowsDirectory( infoBuf, MAX_PATH );

            Launcher lanzador;
            FileLaunch emulInfo;
            emulInfo.rutaexe = infoBuf;
            emulInfo.fileexe = "explorer.exe";
            emulInfo.parmsexe = tmpUrl;
            bool resultado = lanzador.lanzarProgramaUNIXFork(&emulInfo);
#endif

#ifdef UNIX
            
//            string cmd = CMD_LAUNCH_BROWSER + " \"" + tmpUrl + "\"";
//            system(cmd.c_str());
            Launcher lanzador;
            FileLaunch emulInfo;
            emulInfo.rutaexe = "/usr/bin";
            emulInfo.fileexe = CMD_LAUNCH_BROWSER;
            emulInfo.parmsexe = tmpUrl;
            bool resultado = lanzador.lanzarProgramaUNIXFork(&emulInfo);
#endif            
        }
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionesCddbAceptar(tEvento *evento){
    Traza::print("Iofrontend::accionesCddbAceptar", W_INFO);

    string configIniFile = Constant::getAppDir() + Constant::getFileSep() + "config.ini";
    tmenu_gestor_objects *obj = getMenu(PANTALLACDDBDATA);

    string user =     ((UIInput *)obj->getObjByName(inputUsuario))->getText();
    string hostname = ((UIInput *)obj->getObjByName(inputHostname))->getText();

    if (user.empty() || hostname.empty()){
        showMessage("Rellene correctamente todos los campos", 2000);
    } else {

        ListaIni<Data> *config = new ListaIni<Data>();
        Traza::print("Cargando configuracion", W_DEBUG);
        config->loadFromFile(configIniFile);
        config->sort();

        Data *dat;
        int pos;

        dat = new Data();
        dat->setKeyValue("cddbuser",user);
        if ((pos = config->find("cddbuser")) < 0){
            config->add(*dat);
        } else {
            config->set(pos, dat);
        }

        dat = new Data();
        dat->setKeyValue("cddbhostname",hostname);
        if ((pos = config->find("cddbhostname")) < 0){
            config->add(*dat);
        } else {
            config->set(pos, dat);
        }

        dat = new Data();
        dat->setKeyValue("cddbclientname","ONMUSIK");
        if ((pos = config->find("cddbclientname")) < 0){
            config->add(*dat);
        } else {
            config->set(pos, dat);
        }

        dat = new Data();
        dat->setKeyValue("cddbclientversion","1");
        if ((pos = config->find("cddbclientversion")) < 0){
            config->add(*dat);
        } else {
            config->set(pos, dat);
        }

        config->writeToFile(configIniFile);
        delete config;
        setSelMenu(PANTALLAREPRODUCTOR);
    }

    return 0;
}

/**
*
*/
void Iofrontend::showCDDBMenuData(){
    setSelMenu(PANTALLACDDBDATA);
    getMenu(PANTALLACDDBDATA)->findNextFocus();
    tmenu_gestor_objects *obj = getMenu(PANTALLACDDBDATA);

    t_hostname structHostname;
    Constant::getHostname("localhost", &structHostname);

    FreedbQuery query;
    juke->loadConfigCDDB(&query);
    if (query.hostname.empty())
        query.hostname = structHostname.hostname;

    ((UIInput *)obj->getObjByName(inputUsuario))->setText(query.username);
    ((UIInput *)obj->getObjByName(inputHostname))->setText(query.hostname);
}


/**
*
*/
//int Iofrontend::accionesAlbumSelec(tEvento *evento){
//    Traza::print("Iofrontend::accionesAlbumSelec", W_INFO);
//    UITreeListBox *treeList = ((UITreeListBox *)ObjectsMenu[PANTALLAFOO]->getObjByName("treeAlbumList"));
//    TreeNode node = treeList->get(treeList->getPosActualLista());
//    Traza::print("Selected node: " + node.id + (node.show ? " S" : " N"), W_DEBUG);
//
//    if (node.estado != TREENODEOBTAINED){
//        node.estado = TREENODEOBTAINED;
//        node.ico = folder;
//        treeList->refreshNode(node);
//        Traza::print("Refreshed" + node.id + (node.show ? " S" : " N"), W_DEBUG);
//        for (int i=0; i < 5; i++){
//            string parentId = node.id + "." + Constant::TipoToStr(i);
//            treeList->add(parentId, "Nodo" + parentId, "Valor Nodo" + parentId, music, -1, true);
//        }
//        treeList->sort();
//    }
//    treeList->calcularScrPos();
//    return 0;
//}

int Iofrontend::accionesAlbumSelec(tEvento *evento){
    Traza::print("Iofrontend::accionesAlbumSelec", W_INFO);
    UITreeListBox *treeList = ((UITreeListBox *)getMenu("PANTALLAFOO")->getObjByName("treeAlbumList"));
    TreeNode node = treeList->get(treeList->getPosActualLista());
    Traza::print("Selected node: " + node.id + (node.show ? " S" : " N"), W_DEBUG);

    if (node.estado != TREENODEOBTAINED){
//        node.estado = TREENODEOBTAINED;
//        node.ico = folder;
//        treeList->refreshNode(node);
//        Traza::print("Refreshed" + node.id + (node.show ? " S" : " N"), W_DEBUG);
//        for (int i=0; i < 5; i++){
//            string parentId = node.id + "." + Constant::TipoToStr(i);
//            treeList->add(parentId, "Nodo" + parentId, "Valor Nodo" + parentId, music, -1, true);
//        }
//        treeList->sort();
    }
    treeList->calcularScrPos();
    return 0;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::selectTreeAlbum(tEvento *evento){
    Traza::print("Iofrontend::selectAlbum", W_INFO);
    UITreeListBox *objAlbumList = ((UITreeListBox *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(albumList));
    UIListGroup *playList = ((UIListGroup *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(playLists));

    TreeNode node = objAlbumList->get(objAlbumList->getLastSelectedPos());
    string albumSelected = node.value;
    
    Dirutil dir;

    if (dir.existe(albumSelected)){
        this->addLocalAlbum(albumSelected);
    } else if (node.estado != TREENODEOBTAINED){
        Traza::print("Comprobando autorizacion selectAlbum...", W_DEBUG);

        tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);

        int idservidor = Constant::strToTipo<int>(objAlbumList->getDestino(objAlbumList->getLastSelectedPos()));
        juke->setObjectsMenu(obj);
        juke->setServerSelected(idservidor);
        juke->setAlbumSelected(objAlbumList->getValue(objAlbumList->getLastSelectedPos()));

        Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshPlaylist);

        if (thread->start())
            Traza::print("Thread started with id: ",thread->getThreadID(), W_DEBUG);
        pintarIconoProcesando(true);

        tEvento evento;
        while (thread->isRunning() && evento.key != SDLK_ESCAPE && !evento.quit){
            evento = WaitForKey();
            procesarControles(obj, &evento, NULL);
            pintarIconoProcesando(false);
        }

        UITextElementsArea *textElems = (UITextElementsArea *)getMenu(PANTALLAREPRODUCTOR)->getObjByName(LetrasBox);

        if (textElems->isVisible() && !playList->isVisible()){
            playList->setVisible(true);
            textElems->setVisible(false);
        }
        
        
        procesarControles(obj, &evento, NULL);
        flipScr();
        delete thread;

    }
    return 0;
}

/**
 * 
 * @param thread
 * @param timeout
 * @return 
 */
bool Iofrontend::waitFinishThread(Thread<AudioPlayer> *thread, int timeout){
    bool ret = true;
    unsigned long before = SDL_GetTicks();
    while (thread->isRunning() && ret){
        std::this_thread::sleep_for(std::chrono::milliseconds(90));
        ret = SDL_GetTicks() - before < timeout;
    }
    return ret;
}

/**
 * 
 * @param thread
 * @param timeout
 * @return 
 */
bool Iofrontend::waitFinishThread(Thread<Jukebox> *thread, int timeout){
    bool ret = true;
    unsigned long before = SDL_GetTicks();
    while (thread->isRunning() && ret){
        std::this_thread::sleep_for(std::chrono::milliseconds(90));
        ret = SDL_GetTicks() - before < timeout;
    }
    return ret;
}