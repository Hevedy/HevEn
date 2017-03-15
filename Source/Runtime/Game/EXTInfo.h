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
//  File:			EXTInfo.h
//  Description: 	EXT Information
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================

#define EXT_ACK                         -1
#define EXT_VERSION                     105
#define EXT_NO_ERROR                    0
#define EXT_ERROR                       1
#define EXT_PLAYERSTATS_RESP_IDS        -10
#define EXT_PLAYERSTATS_RESP_STATS      -11
#define EXT_UPTIME                      0
#define EXT_PLAYERSTATS                 1
#define EXT_TEAMSCORE                   2

/*
    Client:
    -----
    A: 0 EXT_UPTIME
    B: 0 EXT_PLAYERSTATS cn #a client number or -1 for all players#
    C: 0 EXT_TEAMSCORE

    Server:
    --------
    A: 0 EXT_UPTIME EXT_ACK EXT_VERSION uptime #in seconds#
    B: 0 EXT_PLAYERSTATS cn #send by client# EXT_ACK EXT_VERSION 0 or 1 #error, if cn was > -1 and client does not exist# ...
         EXT_PLAYERSTATS_RESP_IDS pid(s) #1 packet#
         EXT_PLAYERSTATS_RESP_STATS pid playerdata #1 packet for each player#
    C: 0 EXT_TEAMSCORE EXT_ACK EXT_VERSION 0 or 1 #error, no teammode# remaining_time gamemode loop(teamdata [numbases bases] or -1)

    Errors:
    --------------
    B:C:default: 0 command EXT_ACK EXT_VERSION EXT_ERROR
*/

    VAR(extinfoip, 0, 0, 1);

    void extinfoplayer(ucharbuf &p, clientinfo *ci)
    {
        ucharbuf q = p;
        putint(q, EXT_PLAYERSTATS_RESP_STATS); // send player stats following
        putint(q, ci->clientnum); //add player id
        putint(q, ci->ping);
        sendstring(ci->name, q);
        sendstring(teamname(m_teammode ? ci->team : 0), q);
        putint(q, ci->state.frags);
        putint(q, ci->state.flags);
        putint(q, ci->state.deaths);
        putint(q, ci->state.teamkills);
        putint(q, ci->state.damage*100/max(ci->state.shotdamage,1));
        putint(q, ci->state.health);
        putint(q, 0);
        putint(q, ci->state.gunselect);
        putint(q, ci->privilege);
        putint(q, ci->state.state);
        uint ip = extinfoip ? getclientip(ci->clientnum) : 0;
        q.put((uchar*)&ip, 3);
        sendserverinforeply(q);
    }

    static inline void extinfoteamscore(ucharbuf &p, int team, int score)
    {
        sendstring(teamname(team), p);
        putint(p, score);
        if(!smode || !smode->extinfoteam(team, p))
            putint(p,-1); //no bases follow
    }

    void extinfoteams(ucharbuf &p)
    {
        putint(p, m_teammode ? 0 : 1);
        putint(p, gamemode);
        putint(p, max((gamelimit - gamemillis)/1000, 0));
        if(!m_teammode) return;

        vector<teamscore> scores;
        if(smode && smode->hidefrags()) smode->getteamscores(scores);
        loopv(clients)
        {
            clientinfo *ci = clients[i];
            if(ci->state.state!=CS_SPECTATOR && validteam(ci->team) && scores.htfind(ci->team) < 0)
            {
                if(smode && smode->hidefrags()) scores.add(teamscore(ci->team, 0));
                else { teaminfo &t = teaminfos[ci->team-1]; scores.add(teamscore(ci->team, t.frags)); }
            }
        }
        loopv(scores) extinfoteamscore(p, scores[i].team, scores[i].score);
    }

    void extserverinforeply(ucharbuf &req, ucharbuf &p)
    {
        int extcmd = getint(req); // extended commands

        //Build a new packet
        putint(p, EXT_ACK); //send ack
        putint(p, EXT_VERSION); //send version of extended info

        switch(extcmd)
        {
            case EXT_UPTIME:
            {
                putint(p, totalsecs); //in seconds
                break;
            }

            case EXT_PLAYERSTATS:
            {
                int cn = getint(req); //a special player, -1 for all

                clientinfo *ci = NULL;
                if(cn >= 0)
                {
                    loopv(clients) if(clients[i]->clientnum == cn) { ci = clients[i]; break; }
                    if(!ci)
                    {
                        putint(p, EXT_ERROR); //client requested by id was not found
                        sendserverinforeply(p);
                        return;
                    }
                }

                putint(p, EXT_NO_ERROR); //so far no error can happen anymore

                ucharbuf q = p; //remember buffer position
                putint(q, EXT_PLAYERSTATS_RESP_IDS); //send player ids following
                if(ci) putint(q, ci->clientnum);
                else loopv(clients) putint(q, clients[i]->clientnum);
                sendserverinforeply(q);

                if(ci) extinfoplayer(p, ci);
                else loopv(clients) extinfoplayer(p, clients[i]);
                return;
            }

            case EXT_TEAMSCORE:
            {
                extinfoteams(p);
                break;
            }

            default:
            {
                putint(p, EXT_ERROR);
                break;
            }
        }
        sendserverinforeply(p);
    }

