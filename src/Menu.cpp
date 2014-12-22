/*
    This file is part of SessionMgr, A Plugin for Notepad++. SessionMgr is free
    software: you can redistribute it and/or modify it under the terms of the
    GNU General Public License as published by the Free Software Foundation,
    either version 3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License along with this program. If not, see <http://www.gnu.org/licenses/>.
*//**
    @file      Menu.cpp
    @copyright Copyright 2011,2013,2014 Michael Foster <http://mfoster.com/npp/>

    The NPP "Plugins" menu entries for Session Manager.
*/

#include "System.h"
#include "SessionMgr.h"
#include "Config.h"
#include "DlgSessions.h"
#include "DlgSettings.h"
#include "Menu.h"
#include "Util.h"
#include "res\resource.h"
#include <strsafe.h>

//------------------------------------------------------------------------------

namespace NppPlugin {

//------------------------------------------------------------------------------

namespace {

#define PLUGIN_MENU_NAME L"&" PLUGIN_FULL_NAME
#define PLUGIN_ABOUT PLUGIN_FULL_NAME SPACE_STR PLUGIN_VERSION L"\nA plugin for Notepad++\nhttp://mfoster.com/npp/"
#define HELP_FILE_SUFFIX L"doc\\" PLUGIN_DLL_NAME L".html"

/// Menu callback functions
extern "C" {
    void cbSessions();
    void cbSettings();
    void cbSaveCurrent();
    void cbLoadPrevious();
    void cbHelp();
    void cbAbout();
    void cbFav1();
    void cbFav2();
    void cbFav3();
    void cbFav4();
    void cbFav5();
    void cbFav6();
    void cbFav7();
    void cbFav8();
    void cbFav9();
    void cbFav10();
    void cbFav11();
    void cbFav12();
    void cbFav13();
    void cbFav14();
    void cbFav15();
    void cbFav16();
    void cbFav17();
    void cbFav18();
    void cbFav19();
    void cbFav20();
};

/// Menu config
INT _menuItemsCount = MNU_BASE_MAX_ITEMS;
WCHAR _menuMainLabel[MNU_MAX_NAME_LEN + 1];
FuncItem _menuItems[] = {
    { L"&Sessions...",   cbSessions,     0, false, NULL },
    { L"Se&ttings...",   cbSettings,     0, false, NULL },
    { L"Sa&ve current",  cbSaveCurrent,  0, false, NULL },
    { L"Load &previous", cbLoadPrevious, 0, false, NULL },
    { EMPTY_STR,         NULL,           0, false, NULL },
    { L"&Help",          cbHelp,         0, false, NULL },
    { L"&About...",      cbAbout,        0, false, NULL },
    { EMPTY_STR,         NULL,           0, false, NULL },
    { EMPTY_STR,         cbFav1,         0, false, NULL },
    { EMPTY_STR,         cbFav2,         0, false, NULL },
    { EMPTY_STR,         cbFav3,         0, false, NULL },
    { EMPTY_STR,         cbFav4,         0, false, NULL },
    { EMPTY_STR,         cbFav5,         0, false, NULL },
    { EMPTY_STR,         cbFav6,         0, false, NULL },
    { EMPTY_STR,         cbFav7,         0, false, NULL },
    { EMPTY_STR,         cbFav8,         0, false, NULL },
    { EMPTY_STR,         cbFav9,         0, false, NULL },
    { EMPTY_STR,         cbFav10,        0, false, NULL },
    { EMPTY_STR,         cbFav11,        0, false, NULL },
    { EMPTY_STR,         cbFav12,        0, false, NULL },
    { EMPTY_STR,         cbFav13,        0, false, NULL },
    { EMPTY_STR,         cbFav14,        0, false, NULL },
    { EMPTY_STR,         cbFav15,        0, false, NULL },
    { EMPTY_STR,         cbFav16,        0, false, NULL },
    { EMPTY_STR,         cbFav17,        0, false, NULL },
    { EMPTY_STR,         cbFav18,        0, false, NULL },
    { EMPTY_STR,         cbFav19,        0, false, NULL },
    { EMPTY_STR,         cbFav20,        0, false, NULL }
};

} // end namespace

//------------------------------------------------------------------------------

namespace api {

void mnu_onLoad()
{
}

void mnu_onUnload()
{
}

void mnu_init()
{
    INT mnuIdx, cfgIdx = 1;

    // main
    ::StringCchCopyW(_menuMainLabel, MNU_MAX_NAME_LEN, PLUGIN_MENU_NAME);
    gCfg.getMenuLabel(-1, _menuMainLabel);
    // sub
    for (mnuIdx = 0; mnuIdx <= 3; ++mnuIdx) {
        gCfg.getMenuLabel(cfgIdx, _menuItems[mnuIdx]._itemName);
        ++cfgIdx;
    }
    for (mnuIdx = 5; mnuIdx <= 6; ++mnuIdx) {
        gCfg.getMenuLabel(cfgIdx, _menuItems[mnuIdx]._itemName);
        ++cfgIdx;
    }
    // fav
    cfgIdx = 1;
    for (mnuIdx = MNU_FIRST_FAV_IDX; mnuIdx <= MNU_LAST_FAV_IDX; ++mnuIdx) {
        if (!gCfg.getFavMenuLabel(cfgIdx, _menuItems[mnuIdx]._itemName)) {
            break;
        }
        ++cfgIdx;
        ++_menuItemsCount;
    }
    if (cfgIdx > 1) {
        ++_menuItemsCount; // for the 2nd separator if any fav was added
    }
}

FuncItem* mnu_getItems(INT *pNum)
{
    *pNum = _menuItemsCount;
    return _menuItems;
}

} // end namespace NppPlugin::api

//------------------------------------------------------------------------------

/** @return a pointer to the main or the 0-based mnuIdx'th sub label. */
LPCWSTR mnu_getMenuLabel(INT mnuIdx)
{
    LPCWSTR lbl = NULL;

    if (mnuIdx == -1) {
        lbl = _menuMainLabel;
    }
    else if (mnuIdx >= 0 && mnuIdx < MNU_BASE_MAX_ITEMS) {
        lbl = _menuItems[mnuIdx]._itemName;
    }
    return lbl;
}

/** @return true if sesName is in the favorites part of the _menuItems array. */
bool mnu_isFavorite(LPCWSTR sesName)
{
    INT mnuIdx;
    WCHAR fav[MNU_MAX_NAME_LEN];

    for (mnuIdx = MNU_FIRST_FAV_IDX; mnuIdx <= MNU_LAST_FAV_IDX; ++mnuIdx) {
        if (_menuItems[mnuIdx]._itemName[0] == 0) {
            break;
        }
        pth::removeAmp(_menuItems[mnuIdx]._itemName, fav);
        if (::lstrcmpW(sesName, fav) == 0) {
            return true;
        }
    }
    return false;
}

/** Assigns an empty string to all items in the favorites part of the _menuItems array. */
void mnu_clearFavorites()
{
    for (INT mnuIdx = MNU_FIRST_FAV_IDX; mnuIdx <= MNU_LAST_FAV_IDX; ++mnuIdx) {
        _menuItems[mnuIdx]._itemName[0] = 0;
    }
}

/** Copies favName to the prpIdx'th (1-based) item in the favorites part of the
    _menuItems array. */
void mnu_addFavorite(INT prpIdx, LPCWSTR favName)
{
    INT mnuIdx = MNU_FIRST_FAV_IDX + prpIdx - 1;
    ::StringCchCopyW(_menuItems[mnuIdx]._itemName, MNU_MAX_NAME_LEN, favName);
}

//------------------------------------------------------------------------------

namespace {

extern "C" void cbSessions()
{
    app_readSessionDirectory();
    ::DialogBox(sys_getDllHandle(), MAKEINTRESOURCE(IDD_SES_DLG), sys_getNppHandle(), dlgSes_msgProc);
}

extern "C" void cbSettings()
{
    ::DialogBox(sys_getDllHandle(), MAKEINTRESOURCE(IDD_CFG_DLG), sys_getNppHandle(), dlgCfg_msgProc);
}

extern "C" void cbSaveCurrent()
{
    app_saveSession(SI_CURRENT);
}

extern "C" void cbLoadPrevious()
{
    app_loadSession(SI_PREVIOUS);
}

extern "C" void cbHelp()
{
    WCHAR helpFile[MAX_PATH];

    ::GetModuleFileNameW((HMODULE)sys_getDllHandle(), helpFile, MAX_PATH);
    pth::removeName(helpFile, MAX_PATH);
    ::StringCchCatW(helpFile, MAX_PATH, HELP_FILE_SUFFIX);

    HINSTANCE h = ::ShellExecuteW(NULL, L"open", helpFile, NULL, NULL, SW_SHOW);
    if ((int)h <= 32) {
        DWORD le = ::GetLastError();
        msg::error(le, L"%s: Error shelling to default browser.", _W(__FUNCTION__));
    }
}

extern "C" void cbAbout()
{
    const size_t s = 7 * MAX_PATH;
    WCHAR m[s + 1], b[MAX_PATH];

    ::StringCchCopyW(m, s, PLUGIN_ABOUT);
    ::StringCchCatW(m, s, L"\n\nConfiguration directory:\n");
    ::StringCchCatW(m, s, sys_getCfgDir());
    ::StringCchCatW(m, s, L"\n\nSpecial thanks to...\n- Don Ho, for Notepad++\n- Dave Brotherstone, for PluginManager\n- Julien Audo, for ResEdit\n- Lee Thomason, for TinyXML2\n- Nemanja Trifunovic, for UTF8-CPP\n- Jens Lorenz and Thell Fowler, for example code\n- Users at the plugin forum, for testing and feedback\n- You, for using Session Manager");
    msg::show(m);
}

void loadFavorite(INT mnuIdx)
{
    INT sesIdx;
    WCHAR sesName[MNU_MAX_NAME_LEN];

    pth::removeAmp(_menuItems[mnuIdx]._itemName, sesName);
    sesIdx = app_getSessionIndex(sesName);
    app_loadSession(sesIdx);
}

extern "C" void cbFav1() { loadFavorite(1 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav2() { loadFavorite(2 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav3() { loadFavorite(3 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav4() { loadFavorite(4 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav5() { loadFavorite(5 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav6() { loadFavorite(6 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav7() { loadFavorite(7 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav8() { loadFavorite(8 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav9() { loadFavorite(9 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav10() { loadFavorite(10 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav11() { loadFavorite(11 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav12() { loadFavorite(12 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav13() { loadFavorite(13 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav14() { loadFavorite(14 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav15() { loadFavorite(15 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav16() { loadFavorite(16 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav17() { loadFavorite(17 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav18() { loadFavorite(18 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav19() { loadFavorite(19 + MNU_BASE_MAX_ITEMS); }
extern "C" void cbFav20() { loadFavorite(20 + MNU_BASE_MAX_ITEMS); }

} // end namespace

} // end namespace NppPlugin
