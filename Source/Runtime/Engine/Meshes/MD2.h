// ===========================================================================
// 
// HevEn (C) 2017 by Hevedy <https://github.com/Hevedy>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, 
// You can obtain one at https://mozilla.org/MPL/2.0/.
// 
// ---------------------------------------------------------------------------
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
//  File:			MD2.h
//  Description: 	MD2 models header
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================


struct md2;

static const float md2normaltable[256][3] =
{
    { -0.525731f,  0.000000f,  0.850651f },     { -0.442863f,  0.238856f,  0.864188f },     { -0.295242f,  0.000000f,  0.955423f },     { -0.309017f,  0.500000f,  0.809017f },
    { -0.162460f,  0.262866f,  0.951056f },     {  0.000000f,  0.000000f,  1.000000f },     {  0.000000f,  0.850651f,  0.525731f },     { -0.147621f,  0.716567f,  0.681718f },
    {  0.147621f,  0.716567f,  0.681718f },     {  0.000000f,  0.525731f,  0.850651f },     {  0.309017f,  0.500000f,  0.809017f },     {  0.525731f,  0.000000f,  0.850651f },
    {  0.295242f,  0.000000f,  0.955423f },     {  0.442863f,  0.238856f,  0.864188f },     {  0.162460f,  0.262866f,  0.951056f },     { -0.681718f,  0.147621f,  0.716567f },
    { -0.809017f,  0.309017f,  0.500000f },     { -0.587785f,  0.425325f,  0.688191f },     { -0.850651f,  0.525731f,  0.000000f },     { -0.864188f,  0.442863f,  0.238856f },
    { -0.716567f,  0.681718f,  0.147621f },     { -0.688191f,  0.587785f,  0.425325f },     { -0.500000f,  0.809017f,  0.309017f },     { -0.238856f,  0.864188f,  0.442863f },
    { -0.425325f,  0.688191f,  0.587785f },     { -0.716567f,  0.681718f, -0.147621f },     { -0.500000f,  0.809017f, -0.309017f },     { -0.525731f,  0.850651f,  0.000000f },
    {  0.000000f,  0.850651f, -0.525731f },     { -0.238856f,  0.864188f, -0.442863f },     {  0.000000f,  0.955423f, -0.295242f },     { -0.262866f,  0.951056f, -0.162460f },
    {  0.000000f,  1.000000f,  0.000000f },     {  0.000000f,  0.955423f,  0.295242f },     { -0.262866f,  0.951056f,  0.162460f },     {  0.238856f,  0.864188f,  0.442863f },
    {  0.262866f,  0.951056f,  0.162460f },     {  0.500000f,  0.809017f,  0.309017f },     {  0.238856f,  0.864188f, -0.442863f },     {  0.262866f,  0.951056f, -0.162460f },
    {  0.500000f,  0.809017f, -0.309017f },     {  0.850651f,  0.525731f,  0.000000f },     {  0.716567f,  0.681718f,  0.147621f },     {  0.716567f,  0.681718f, -0.147621f },
    {  0.525731f,  0.850651f,  0.000000f },     {  0.425325f,  0.688191f,  0.587785f },     {  0.864188f,  0.442863f,  0.238856f },     {  0.688191f,  0.587785f,  0.425325f },
    {  0.809017f,  0.309017f,  0.500000f },     {  0.681718f,  0.147621f,  0.716567f },     {  0.587785f,  0.425325f,  0.688191f },     {  0.955423f,  0.295242f,  0.000000f },
    {  1.000000f,  0.000000f,  0.000000f },     {  0.951056f,  0.162460f,  0.262866f },     {  0.850651f, -0.525731f,  0.000000f },     {  0.955423f, -0.295242f,  0.000000f },
    {  0.864188f, -0.442863f,  0.238856f },     {  0.951056f, -0.162460f,  0.262866f },     {  0.809017f, -0.309017f,  0.500000f },     {  0.681718f, -0.147621f,  0.716567f },
    {  0.850651f,  0.000000f,  0.525731f },     {  0.864188f,  0.442863f, -0.238856f },     {  0.809017f,  0.309017f, -0.500000f },     {  0.951056f,  0.162460f, -0.262866f },
    {  0.525731f,  0.000000f, -0.850651f },     {  0.681718f,  0.147621f, -0.716567f },     {  0.681718f, -0.147621f, -0.716567f },     {  0.850651f,  0.000000f, -0.525731f },
    {  0.809017f, -0.309017f, -0.500000f },     {  0.864188f, -0.442863f, -0.238856f },     {  0.951056f, -0.162460f, -0.262866f },     {  0.147621f,  0.716567f, -0.681718f },
    {  0.309017f,  0.500000f, -0.809017f },     {  0.425325f,  0.688191f, -0.587785f },     {  0.442863f,  0.238856f, -0.864188f },     {  0.587785f,  0.425325f, -0.688191f },
    {  0.688191f,  0.587785f, -0.425325f },     { -0.147621f,  0.716567f, -0.681718f },     { -0.309017f,  0.500000f, -0.809017f },     {  0.000000f,  0.525731f, -0.850651f },
    { -0.525731f,  0.000000f, -0.850651f },     { -0.442863f,  0.238856f, -0.864188f },     { -0.295242f,  0.000000f, -0.955423f },     { -0.162460f,  0.262866f, -0.951056f },
    {  0.000000f,  0.000000f, -1.000000f },     {  0.295242f,  0.000000f, -0.955423f },     {  0.162460f,  0.262866f, -0.951056f },     { -0.442863f, -0.238856f, -0.864188f },
    { -0.309017f, -0.500000f, -0.809017f },     { -0.162460f, -0.262866f, -0.951056f },     {  0.000000f, -0.850651f, -0.525731f },     { -0.147621f, -0.716567f, -0.681718f },
    {  0.147621f, -0.716567f, -0.681718f },     {  0.000000f, -0.525731f, -0.850651f },     {  0.309017f, -0.500000f, -0.809017f },     {  0.442863f, -0.238856f, -0.864188f },
    {  0.162460f, -0.262866f, -0.951056f },     {  0.238856f, -0.864188f, -0.442863f },     {  0.500000f, -0.809017f, -0.309017f },     {  0.425325f, -0.688191f, -0.587785f },
    {  0.716567f, -0.681718f, -0.147621f },     {  0.688191f, -0.587785f, -0.425325f },     {  0.587785f, -0.425325f, -0.688191f },     {  0.000000f, -0.955423f, -0.295242f },
    {  0.000000f, -1.000000f,  0.000000f },     {  0.262866f, -0.951056f, -0.162460f },     {  0.000000f, -0.850651f,  0.525731f },     {  0.000000f, -0.955423f,  0.295242f },
    {  0.238856f, -0.864188f,  0.442863f },     {  0.262866f, -0.951056f,  0.162460f },     {  0.500000f, -0.809017f,  0.309017f },     {  0.716567f, -0.681718f,  0.147621f },
    {  0.525731f, -0.850651f,  0.000000f },     { -0.238856f, -0.864188f, -0.442863f },     { -0.500000f, -0.809017f, -0.309017f },     { -0.262866f, -0.951056f, -0.162460f },
    { -0.850651f, -0.525731f,  0.000000f },     { -0.716567f, -0.681718f, -0.147621f },     { -0.716567f, -0.681718f,  0.147621f },     { -0.525731f, -0.850651f,  0.000000f },
    { -0.500000f, -0.809017f,  0.309017f },     { -0.238856f, -0.864188f,  0.442863f },     { -0.262866f, -0.951056f,  0.162460f },     { -0.864188f, -0.442863f,  0.238856f },
    { -0.809017f, -0.309017f,  0.500000f },     { -0.688191f, -0.587785f,  0.425325f },     { -0.681718f, -0.147621f,  0.716567f },     { -0.442863f, -0.238856f,  0.864188f },
    { -0.587785f, -0.425325f,  0.688191f },     { -0.309017f, -0.500000f,  0.809017f },     { -0.147621f, -0.716567f,  0.681718f },     { -0.425325f, -0.688191f,  0.587785f },
    { -0.162460f, -0.262866f,  0.951056f },     {  0.442863f, -0.238856f,  0.864188f },     {  0.162460f, -0.262866f,  0.951056f },     {  0.309017f, -0.500000f,  0.809017f },
    {  0.147621f, -0.716567f,  0.681718f },     {  0.000000f, -0.525731f,  0.850651f },     {  0.425325f, -0.688191f,  0.587785f },     {  0.587785f, -0.425325f,  0.688191f },
    {  0.688191f, -0.587785f,  0.425325f },     { -0.955423f,  0.295242f,  0.000000f },     { -0.951056f,  0.162460f,  0.262866f },     { -1.000000f,  0.000000f,  0.000000f },
    { -0.850651f,  0.000000f,  0.525731f },     { -0.955423f, -0.295242f,  0.000000f },     { -0.951056f, -0.162460f,  0.262866f },     { -0.864188f,  0.442863f, -0.238856f },
    { -0.951056f,  0.162460f, -0.262866f },     { -0.809017f,  0.309017f, -0.500000f },     { -0.864188f, -0.442863f, -0.238856f },     { -0.951056f, -0.162460f, -0.262866f },
    { -0.809017f, -0.309017f, -0.500000f },     { -0.681718f,  0.147621f, -0.716567f },     { -0.681718f, -0.147621f, -0.716567f },     { -0.850651f,  0.000000f, -0.525731f },
    { -0.688191f,  0.587785f, -0.425325f },     { -0.587785f,  0.425325f, -0.688191f },     { -0.425325f,  0.688191f, -0.587785f },     { -0.425325f, -0.688191f, -0.587785f },
    { -0.587785f, -0.425325f, -0.688191f },     { -0.688191f, -0.587785f, -0.425325f }
};

struct md2 : vertmodel, vertloader<md2>
{
    struct md2_header
    {
        int magic;
        int version;
        int skinwidth, skinheight;
        int framesize;
        int numskins, numvertices, numtexcoords;
        int numtriangles, numglcommands, numframes;
        int offsetskins, offsettexcoords, offsettriangles;
        int offsetframes, offsetglcommands, offsetend;
    };

    struct md2_vertex
    {
        uchar vertex[3], normalindex;
    };

    struct md2_frame
    {
        float      scale[3];
        float      translate[3];
        char       name[16];
    };

    md2(const char *name) : vertmodel(name) {}

    static const char *formatname() { return "md2"; }
    static bool multiparted() { return false; }
    static bool multimeshed() { return false; }
    int type() const { return MDL_MD2; }

    int linktype(animmodel *m, part *p) const { return LINK_COOP; }

    struct md2meshgroup : vertmeshgroup
    {
        void genverts(int *glcommands, vector<tcvert> &tcverts, vector<ushort> &vindexes, vector<tri> &tris)
        {
            hashtable<ivec, int> tchash;
            vector<ushort> idxs;
            for(int *command = glcommands; (*command)!=0;)
            {
                int numvertex = *command++;
                bool isfan = numvertex<0;
                if(isfan) numvertex = -numvertex;
                idxs.setsize(0);
                loopi(numvertex)
                {
                    union { int i; float f; } u, v;
                    u.i = *command++;
                    v.i = *command++;
                    int vindex = *command++;
                    ivec tckey(u.i, v.i, vindex);
                    int *idx = tchash.access(tckey);
                    if(!idx)
                    {
                        idx = &tchash[tckey];
                        *idx = tcverts.length();
                        tcvert &tc = tcverts.add();
                        tc.tc = vec2(u.f, v.f);
                        vindexes.add((ushort)vindex);
                    }
                    idxs.add(*idx);
                }
                loopi(numvertex-2)
                {
                    tri &t = tris.add();
                    if(isfan)
                    {
                        t.vert[0] = idxs[0];
                        t.vert[1] = idxs[i+1];
                        t.vert[2] = idxs[i+2];
                    }
                    else loopk(3) t.vert[k] = idxs[i&1 && k ? i+(1-(k-1))+1 : i+k];
                }
            }
        }

        bool load(const char *filename, float smooth)
        {
            stream *file = openfile(filename, "rb");
            if(!file) return false;

            md2_header header;
            file->read(&header, sizeof(md2_header));
            lilswap(&header.magic, sizeof(md2_header)/sizeof(int));

            if(header.magic!=844121161 || header.version!=8 ||
               header.numframes <= 0 || header.numframes > 1000 ||
               header.numglcommands <= 0 || header.numglcommands > (1<<20) ||
               header.numvertices <= 0 || header.numvertices > (1<<20))
            {
                delete file;
                return false;
            }

            name = newstring(filename);

            numframes = header.numframes;

            vertmesh &m = *new vertmesh;
            m.group = this;
            meshes.add(&m);

            int *glcommands = new int[header.numglcommands];
            file->seek(header.offsetglcommands, SEEK_SET);
            int numglcommands = file->read(glcommands, header.numglcommands*sizeof(int))/sizeof(int);
            lilswap(glcommands, numglcommands);
            if(numglcommands < header.numglcommands) memset(&glcommands[numglcommands], 0, (header.numglcommands-numglcommands)*sizeof(int));

            vector<tcvert> tcgen;
            vector<ushort> vgen;
            vector<tri> trigen;
            genverts(glcommands, tcgen, vgen,trigen);
            delete[] glcommands;

            m.numverts = tcgen.length();

            m.tcverts = new tcvert[m.numverts];
            memcpy(m.tcverts, tcgen.getbuf(), m.numverts*sizeof(tcvert));
            m.numtris = trigen.length();
            m.tris = new tri[m.numtris];
            memcpy(m.tris, trigen.getbuf(), m.numtris*sizeof(tri));

            m.verts = new vert[m.numverts*numframes];

            md2_vertex *tmpverts = new md2_vertex[header.numvertices];
            int frame_offset = header.offsetframes;
            vert *curvert = m.verts;
            loopi(header.numframes)
            {
                md2_frame frame;
                file->seek(frame_offset, SEEK_SET);
                file->read(&frame, sizeof(md2_frame));
                lilswap(frame.scale, 6);

                file->read(tmpverts, header.numvertices*sizeof(md2_vertex));
                loopj(m.numverts)
                {
                    const md2_vertex &v = tmpverts[vgen[j]];
                    curvert->pos = vec(v.vertex[0]*frame.scale[0]+frame.translate[0],
                                       -(v.vertex[1]*frame.scale[1]+frame.translate[1]),
                                       v.vertex[2]*frame.scale[2]+frame.translate[2]);
                    const float *norm = md2normaltable[v.normalindex];
                    curvert->norm = vec(norm[0], -norm[1], norm[2]);
                    ++curvert;
                }
                frame_offset += header.framesize;
            }
            delete[] tmpverts;

            m.calctangents();

            delete file;

            return true;
        }
    };

    vertmeshgroup *newmeshes() { return new md2meshgroup; }

    bool load()
    {
        part &mdl = addpart();
        const char *pname = parentdir(name);
        defformatstring(name1, "Game/Data/Meshes/%s/tris.md2", name);
        mdl.meshes = sharemeshes(path(name1));
        if(!mdl.meshes)
        {
            defformatstring(name2, "Game/Data/Meshes/%s/tris.md2", pname);    // try md2 in parent folder (vert sharing)
            mdl.meshes = sharemeshes(path(name2));
            if(!mdl.meshes) return false;
        }
        Texture *tex, *masks;
        loadskin(name, pname, tex, masks);
        mdl.initskins(tex, masks);
        if(tex==notexture) conoutf("could not load model skin for %s", name1);
        loading = this;
        identflags &= ~IDF_PERSIST;
        defformatstring(name3, "Game/Data/Meshes/%s/DefMD2.hed", name);
        if(!execfile(name3, false))
        {
            formatstring(name3, "Game/Data/Meshes/%s/DefMD2.hed", pname);
            execfile(name3, false);
        }
        identflags |= IDF_PERSIST;
        loading = 0;
        translate.y = -translate.y;
        loaded();
        return true;
    }
};

vertcommands<md2> md2commands;

