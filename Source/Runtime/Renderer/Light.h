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
//  File:			Light.h
//  Description: 	Light structure
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================


struct PackNode
{
    PackNode *child1, *child2;
    ushort x, y, w, h;
    int available;

    PackNode(ushort x, ushort y, ushort w, ushort h) : child1(0), child2(0), x(x), y(y), w(w), h(h), available(min(w, h)) {}

    void discardchildren()
    {
        DELETEP(child1);
        DELETEP(child2);
    }

    void forceempty()
    {
        discardchildren();
        available = 0;
    }

    void reset()
    {
        discardchildren();
        available = min(w, h);
    }

    bool resize(int nw, int nh)
    {
        if(w == nw && h == nw) return false;
        discardchildren();
        w = nw;
        h = nh;
        available = min(w, h);
        return true;
    }

    ~PackNode()
    {
        discardchildren();
    }

    bool insert(ushort &tx, ushort &ty, ushort tw, ushort th);
    void reserve(ushort tx, ushort ty, ushort tw, ushort th);
};

extern bvec ambient, skylight, sunlight;
extern float ambientscale, skylightscale, sunlightscale;
extern float sunlightyaw, sunlightpitch;
extern vec sunlightdir;
extern int fullbright, fullbrightlevel;

extern void clearlights();
extern void initlights();
extern void clearlightcache(int id = -1);
extern void brightencube(cube &c);
extern void setsurfaces(cube &c, const surfaceinfo *surfs, const vertinfo *verts, int numverts);
extern void setsurface(cube &c, int orient, const surfaceinfo &surf, const vertinfo *verts, int numverts);
extern void previewblends(const ivec &bo, const ivec &bs);

extern void calcnormals(bool lerptjoints = false);
extern void clearnormals();
extern void resetsmoothgroups();
extern int smoothangle(int id, int angle);
extern void findnormal(const vec &key, int smooth, const vec &surface, vec &v);

#define CHECK_CALCLIGHT_PROGRESS_LOCKED(exit, show_calclight_progress, before, after) \
    if(check_calclight_progress) \
    { \
        if(!calclight_canceled) \
        { \
            before; \
            show_calclight_progress(); \
            check_calclight_canceled(); \
            after; \
        } \
        if(calclight_canceled) { exit; } \
    }
#define CHECK_CALCLIGHT_PROGRESS(exit, show_calclight_progress) CHECK_CALCLIGHT_PROGRESS_LOCKED(exit, show_calclight_progress, , )

extern bool calclight_canceled;
extern volatile bool check_calclight_progress;

extern void check_calclight_canceled();

extern const vector<int> &checklightcache(int x, int y);

