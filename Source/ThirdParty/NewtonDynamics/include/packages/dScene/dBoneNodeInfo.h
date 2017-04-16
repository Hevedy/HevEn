/////////////////////////////////////////////////////////////////////////////
// Name:        dBoneNodeInfo.h
// Purpose:     
// Author:      Julio Jerez
// Modified by: 
// Created:     22/05/2010 08:02:08
// RCS-ID:      
// Copyright:   Copyright (c) <2010> <Newton Game Dynamics>
// License:     
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely
/////////////////////////////////////////////////////////////////////////////

#ifndef _D_BONE_NODE_H_
#define _D_BONE_NODE_H_


#include "dSceneNodeInfo.h"


class dBoneNodeInfo: public dSceneNodeInfo
{
	public:
	D_DEFINE_CLASS_NODE(dBoneNodeInfo,dSceneNodeInfo,DSCENE_API)

	dBoneNodeInfo();
	dBoneNodeInfo(dScene* const world) {}
	virtual ~dBoneNodeInfo(void);

//	virtual dMatrix GetTransform () const;
//	virtual void SetTransform (const dMatrix& matrix);
//	virtual dVector GetPosition () const;
//	virtual void SetPosition (const dVector& position);
//	virtual dVector GetEulers () const;
//	virtual void SetEulers (const dVector& angles);
//	virtual dVector GetScale () const;
//	virtual void SetScale (const dVector& scale);
//	virtual void BakeTransform (const dMatrix& transform);
//	virtual dFloat RayCast (const dVector& p0, const dVector& p1) const;

	protected:
	dFloat m_lengh;
//	void DrawOOBBGizmo (dScene* const world, dScene::dTreeNode* myNode) const;
	
	virtual void Serialize (TiXmlElement* const rootNode) const; 
	virtual bool Deserialize (const dScene* const scene, TiXmlElement* const rootNode);

};





#endif