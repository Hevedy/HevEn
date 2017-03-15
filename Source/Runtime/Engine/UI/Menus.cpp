// ===========================================================================
// 
// Sauerbraten game engine source code, any release.
// Tesseract game engine source code, any release.
// 
// Copyright (C) 2001-2017 Wouter van Oortmerssen, Lee Salzman, Mike Dysart, Robert Pointon, and Quinton Reeves
// Copyright (C) 2001-2017 Wouter van Oortmerssen, Lee Salzman, Mike Dysart, Robert Pointon, Quinton Reeves, and Benjamin Segovia
// 
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// 
// ---------------------------------------------------------------------------
//  File:			Menus.cpp
//  Description: 	Main menus class
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================


#include "Engine.h"

void notifywelcome()
{
    UI::hideui("servers");
}

struct change
{
    int type;
    const char *desc;

    change() {}
    change(int type, const char *desc) : type(type), desc(desc) {}
};
static vector<change> needsapply;

VARP(applydialog, 0, 1, 1);
VAR(hidechanges, 0, 0, 1);

void addchange(const char *desc, int type)
{
    if(!applydialog) return;
    loopv(needsapply) if(!strcmp(needsapply[i].desc, desc)) return;
    needsapply.add(change(type, desc));
    if(!hidechanges) UI::showui("changes");
}

void clearchanges(int type)
{
    loopvrev(needsapply)
    {
        change &c = needsapply[i];
        if(c.type&type)
        {
            c.type &= ~type;
            if(!c.type) needsapply.remove(i);
        }
    }
    if(needsapply.empty()) UI::hideui("changes");
}

void applychanges()
{
    int changetypes = 0;
    loopv(needsapply) changetypes |= needsapply[i].type;
    if(changetypes&CHANGE_GFX) execident("resetgl");
    else if(changetypes&CHANGE_SHADERS) execident("resetshaders");
    if(changetypes&CHANGE_SOUND) execident("resetsound");
}

COMMAND(applychanges, "");
ICOMMAND(pendingchanges, "b", (int *idx), { if(needsapply.inrange(*idx)) result(needsapply[*idx].desc); else if(*idx < 0) intret(needsapply.length()); });

static int lastmainmenu = -1;

void menuprocess()
{
    if(lastmainmenu != mainmenu)
    {
        lastmainmenu = mainmenu;
        execident("mainmenutoggled");
    }    
    if(mainmenu && !isconnected(true) && !UI::hascursor()) UI::showui("main");
}

VAR(mainmenu, 1, 1, 0);

void clearmainmenu()
{
    hidechanges = 0;
    if(mainmenu && isconnected())
    {
        mainmenu = 0;
        UI::hideui(NULL);
    }
}

