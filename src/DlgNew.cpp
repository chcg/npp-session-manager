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
    @file      DlgNew.cpp
    @copyright Copyright 2011-2015 Michael Foster <http://mfoster.com/npp/>

    The "New Session" dialog.
*/

#include "System.h"
#include "SessionMgr.h"
#include "DlgSessions.h"
#include "DlgNew.h"
#include "Util.h"
#include "res\resource.h"
#include <strsafe.h>

//------------------------------------------------------------------------------

namespace NppPlugin {

//------------------------------------------------------------------------------

namespace {

ChildDialogData *_dialogData;

void onInit(HWND hDlg);
bool onOk(HWND hDlg);
bool newAsEmpty(LPWSTR dstPathname);
bool newFromOpen(LPWSTR dstPathname);
bool newByCopy(LPWSTR dstPathname);

} // end namespace

//------------------------------------------------------------------------------

INT_PTR CALLBACK dlgNew_msgProc(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    INT_PTR status = FALSE;

    if (uMessage == WM_COMMAND) {
        switch (LOWORD(wParam)) {

            case IDOK:
                if (onOk(hDlg)) {
                    ::EndDialog(hDlg, 1);
                    status = TRUE;
                }
                break;

            case IDCANCEL:
                ::EndDialog(hDlg, 0);
                status = TRUE;
                break;

            case IDC_NEW_RAD_EMPTY:
            case IDC_NEW_RAD_OPEN:
            case IDC_NEW_RAD_COPY:
                if (HIWORD(wParam) == BN_CLICKED) {
                    WCHAR buf[SES_NAME_BUF_LEN];
                    buf[0] = 0;
                    dlg::getText(hDlg, IDC_NEW_ETX_NAME, buf, SES_NAME_BUF_LEN);
                    if (buf[0] == 0) {
                        dlg::setText(hDlg, IDC_NEW_ETX_NAME, app_getSessionName(_dialogData->selectedSessionIndex));
                    }
                    dlg::focus(hDlg, IDC_NEW_ETX_NAME, false);
                    status = TRUE;
                }
                break;
        }
    }
    else if (uMessage == WM_INITDIALOG) {
        _dialogData = (ChildDialogData*)lParam;
        onInit(hDlg);
    }

    return status;
}

//------------------------------------------------------------------------------

namespace {

void onInit(HWND hDlg)
{
    dlg::setCheck(hDlg, IDC_NEW_RAD_EMPTY, true);
    dlg::focus(hDlg, IDC_NEW_ETX_NAME);
    dlg::centerWnd(hDlg, NULL, 150, -40);
    ::ShowWindow(hDlg, SW_SHOW);
}

bool onOk(HWND hDlg)
{
    bool succ;
    WCHAR newName[SES_NAME_BUF_LEN];
    WCHAR dstPathname[MAX_PATH];

    // Set the destination file pathname.
    newName[0] = 0;
    dlg::getText(hDlg, IDC_NEW_ETX_NAME, newName, SES_NAME_BUF_LEN);
    if (newName[0] == 0) {
        msg::show(L"Missing file name.", M_WARN);
        return false;
    }
    ::StringCchCopyW(dstPathname, MAX_PATH, cfg::getStr(kSessionDirectory));
    ::StringCchCatW(dstPathname, MAX_PATH, newName);
    ::StringCchCatW(dstPathname, MAX_PATH, cfg::getStr(kSessionExtension));

    if (dlg::getCheck(hDlg, IDC_NEW_RAD_EMPTY)) {
        succ = newAsEmpty(dstPathname);
    }
    else if (dlg::getCheck(hDlg, IDC_NEW_RAD_OPEN)) {
        succ = newFromOpen(dstPathname);
    }
    else {
        succ = newByCopy(dstPathname);
    }
    if (succ) {
        ::StringCchCopyW(_dialogData->newSessionName, SES_NAME_BUF_LEN, newName);
    }
    return succ;
}

/** Creates a new, empty session. */
bool newAsEmpty(LPWSTR dstPathname)
{
    pth::createFileIfMissing(dstPathname, SES_DEFAULT_CONTENTS);
    return true;
}

/** Creates a new session containing the currently open files. */
bool newFromOpen(LPWSTR dstPathname)
{
    ::SendMessage(sys_getNppHandle(), NPPM_SAVECURRENTSESSION, 0, (LPARAM)dstPathname);
    return true;
}

/** Creates a new session by copying the selected session. */
bool newByCopy(LPWSTR dstPathname)
{
    bool status = false;
    WCHAR srcPathname[MAX_PATH];

    // Set the source file pathname.
    app_getSessionFile(_dialogData->selectedSessionIndex, srcPathname);
    // Copy the file.
    _dialogData->newSessionName[0] = 0;
    if (::CopyFileW(srcPathname, dstPathname, TRUE)) {
        status = true;
    }
    else {
        DWORD le = ::GetLastError();
        msg::error(le, L"%s: Error copying from \"%s\" to \"%s\".", _W(__FUNCTION__), srcPathname, dstPathname);
    }
    return status;
}

} // end namespace

} // end namespace NppPlugin
