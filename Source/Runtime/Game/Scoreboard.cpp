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
//  File:			Scoreboard.cpp
//  Description: 	Creation of scoreboard
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================


#include "Game.h"

namespace game
{
    VARP(showservinfo, 0, 1, 1);
    VARP(showclientnum, 0, 0, 1);
    VARP(showpj, 0, 0, 1);
    VARP(showping, 0, 1, 1);
    VARP(showspectators, 0, 1, 1);
    VARP(highlightscore, 0, 1, 1);
    VARP(showconnecting, 0, 0, 1);

    static teaminfo teaminfos[MAXTEAMS];

    void clearteaminfo()
    {
        loopi(MAXTEAMS) teaminfos[i].reset();
    }

    void setteaminfo(int team, int frags)
    {
        if(!validteam(team)) return;
        teaminfo &t = teaminfos[team-1];
        t.frags = frags;
    }

    static inline bool playersort(const gameent *a, const gameent *b)
    {
        if(a->state==CS_SPECTATOR)
        {
            if(b->state==CS_SPECTATOR) return strcmp(a->name, b->name) < 0;
            else return false;
        }
        else if(b->state==CS_SPECTATOR) return true;
        if(m_ctf)
        {
            if(a->flags > b->flags) return true;
            if(a->flags < b->flags) return false;
        }
        if(a->frags > b->frags) return true;
        if(a->frags < b->frags) return false;
        return strcmp(a->name, b->name) < 0;
    }

    void getbestplayers(vector<gameent *> &best)
    {
        loopv(players)
        {
            gameent *o = players[i];
            if(o->state!=CS_SPECTATOR) best.add(o);
        }
        best.sort(playersort);
        while(best.length() > 1 && best.last()->frags < best[0]->frags) best.drop();
    }

    void getbestteams(vector<int> &best)
    {
        if(cmode && cmode->hidefrags())
        {
            vector<teamscore> teamscores;
            cmode->getteamscores(teamscores);
            teamscores.sort(teamscore::compare);
            while(teamscores.length() > 1 && teamscores.last().score < teamscores[0].score) teamscores.drop();
            loopv(teamscores) best.add(teamscores[i].team);
        }
        else
        {
            int bestfrags = INT_MIN;
            loopi(MAXTEAMS)
            {
                teaminfo &t = teaminfos[i];
                bestfrags = max(bestfrags, t.frags);
            }
            loopi(MAXTEAMS)
            {
                teaminfo &t = teaminfos[i];
                if(t.frags >= bestfrags) best.add(1+i);
            }
        }
    }

    static vector<gameent *> teamplayers[1+MAXTEAMS], spectators;

    static void groupplayers()
    {
        loopi(1+MAXTEAMS) teamplayers[i].setsize(0);
        spectators.setsize(0);
        loopv(players)
        {
            gameent *o = players[i];
            if(!showconnecting && !o->name[0]) continue;
            if(o->state==CS_SPECTATOR) { spectators.add(o); continue; }
            int team = m_teammode && validteam(o->team) ? o->team : 0;
            teamplayers[team].add(o);
        }
        loopi(1+MAXTEAMS) teamplayers[i].sort(playersort);
        spectators.sort(playersort);
    }

    void removegroupedplayer(gameent *d)
    {
        loopi(1+MAXTEAMS) teamplayers[i].removeobj(d);
        spectators.removeobj(d);
    }

    void refreshscoreboard()
    {
        groupplayers();
    }

    COMMAND(refreshscoreboard, "");
    ICOMMAND(numscoreboard, "i", (int *team), intret(*team < 0 ? spectators.length() : (*team <= MAXTEAMS ? teamplayers[*team].length() : 0)));
    ICOMMAND(loopscoreboard, "rie", (ident *id, int *team, uint *body),
    {
        if(*team > MAXTEAMS) return;
        loopstart(id, stack);
        vector<gameent *> &p = *team < 0 ? spectators : teamplayers[*team];
        loopv(p)
        {
            loopiter(id, stack, p[i]->clientnum);
            execute(body);
        }
        loopend(id, stack);
    });

    ICOMMAND(scoreboardstatus, "i", (int *cn),
    {
        gameent *d = getclient(*cn);
        if(d)
        {
            int status = d->state!=CS_DEAD ? 0xFFFFFF : 0x606060;
            if(d->privilege)
            {
                status = d->privilege>=PRIV_ADMIN ? 0xFF8000 : 0x40FF80;
                if(d->state==CS_DEAD) status = (status>>1)&0x7F7F7F;
            }
            intret(status);
        }
    });

    ICOMMAND(scoreboardpj, "i", (int *cn),
    {
        gameent *d = getclient(*cn);
        if(d && d != player1)
        {
            if(d->state==CS_LAGGED) result("LAG");
            else intret(d->plag);
        }
    });

    ICOMMAND(scoreboardping, "i", (int *cn),
    {
        gameent *d = getclient(*cn);
        if(d)
        {
            if(!showpj && d->state==CS_LAGGED) result("LAG");
            else intret(d->ping);
        }
    });

    ICOMMAND(scoreboardshowfrags, "", (), intret(cmode && cmode->hidefrags() ? 0 : 1));
    ICOMMAND(scoreboardshowclientnum, "", (), intret(showclientnum || player1->privilege>=PRIV_MASTER ? 1 : 0));
    ICOMMAND(scoreboardmultiplayer, "", (), intret(multiplayer(false) || demoplayback ? 1 : 0));

    ICOMMAND(scoreboardhighlight, "i", (int *cn),
        intret(*cn == player1->clientnum && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1) ? 0x808080 : 0));

    ICOMMAND(scoreboardservinfo, "", (),
    {
        if(!showservinfo) return;
        const ENetAddress *address = connectedpeer();
        if(address && player1->clientnum >= 0)
        {
            if(servdesc[0]) result(servdesc);
            else
            {
                string hostname;
                if(enet_address_get_host_ip(address, hostname, sizeof(hostname)) >= 0)
                    result(tempformatstring("%s:%d", hostname, address->port));
            }
        }
    });

    ICOMMAND(scoreboardmode, "", (),
    {
        result(server::modeprettyname(gamemode));
    });

    ICOMMAND(scoreboardmap, "", (),
    {
        const char *mname = getclientmap();
        result(mname[0] ? mname : "[new map]");
    });

    ICOMMAND(scoreboardtime, "", (),
    {
        if(m_timed && getclientmap() && (maplimit >= 0 || intermission))
        {
            if(intermission) result("intermission");
            else
            {
                int secs = max(maplimit-lastmillis, 0)/1000;
                result(tempformatstring("%d:%02d", secs/60, secs%60));
            }
        }
    });

    ICOMMAND(getteamscore, "i", (int *team),
    {
        if(m_teammode && validteam(*team))
        {
            if(cmode && cmode->hidefrags()) intret(cmode->getteamscore(*team));
            else intret(teaminfos[*team-1].frags);
        }
    });

    void showscores(bool on) { UI::holdui("scoreboard", on); }
}

