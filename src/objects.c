#include "App.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "Database.h"
#include "CANService.h"

#include "canTinyTimber.h"
#include "sciTinyTimber.h"

App app = initApp();
SoundPlayer soundPlayer = initSoundPlayer();
MusicPlayer musicPlayer = initMusicPlayer();
Database database = initDatabase();
CANService canService = initCANService();

Can can0 = initCan(CAN_PORT0, &app, receiver);
Serial sci0 = initSerial(SCI_PORT0, &app, reader);
