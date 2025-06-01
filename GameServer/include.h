#pragma once


#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601 // Windows 7 или выше
#endif

//#define _WIN32_WINNT 0x0A00
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <map>


#include "IPlayer.h"
#include "Player.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <sstream>


//#include "C:\Users\User\Desktop\BSUIR\GameServer\Cards.h"
#include "Bank.h"