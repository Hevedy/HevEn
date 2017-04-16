/* Copyright (c) <2003-2016> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "dStdAfxNewton.h"
#include "dNewton.h"
#include "dNewtonMesh.h"
#include "dNewtonCollision.h"


dNewtonMesh::dNewtonMesh(dNewton* const world)
	:m_mesh (NewtonMeshCreate (world->GetNewton()))
{
}

dNewtonMesh::dNewtonMesh(const dNewtonMesh& clone)
	:m_mesh (NewtonMeshCreateFromMesh (clone.m_mesh))
{
}

dNewtonMesh::dNewtonMesh(const dNewtonCollision& collision)
	:m_mesh (NewtonMeshCreateFromCollision(collision.GetShape()))
{
}

dNewtonMesh::dNewtonMesh(dNewton* const world, int pointCount, const dFloat* const vertexCloud, int strideInBytes, dFloat tolerance)
	:m_mesh (NewtonMeshCreateConvexHull (world->GetNewton(), pointCount, vertexCloud, strideInBytes, tolerance))
{
}

//dNewtonMesh(NewtonMesh* const mesh)
//	:m_mesh(mesh)
//{
//}

dNewtonMesh::~dNewtonMesh()
{
	NewtonMeshDestroy (m_mesh);
}

NewtonMesh* dNewtonMesh::GetMesh() const
{
	return m_mesh;
}

void dNewtonMesh::BeginPolygon()
{
	dAssert (0);
//	NewtonMeshBeginFace(m_mesh);
}

void dNewtonMesh::AddFace (int vertexCount, const dFloat* const vertex, int strideInBytes, int materialIndex)
{
	dAssert(0);
//	NewtonMeshAddFace (m_mesh, vertexCount, vertex, strideInBytes, materialIndex);
}

void dNewtonMesh::EndPolygon()
{
	dAssert (0);
//	NewtonMeshEndFace(m_mesh);
}


void dNewtonMesh::CreateVoronoiConvexDecomposition (const dNewtonMesh& contexMesh)
{
	NewtonMeshDestroy (m_mesh);
	dAssert (0);
//	m_mesh = NewtonMeshCreateVoronoiConvexDecomposition (const NewtonWorld* const newtonWorld, int pointCount, const dFloat* const vertexCloud, int strideInBytes, int materialID, const dFloat* const textureMatrix);
}

void dNewtonMesh::CreateApproximateConvexDecomposition (const dNewtonMesh& mesh, dFloat maxConcavity, dFloat backFaceDistanceFactor, int maxCount, int maxVertexPerHull)
{
	NewtonMeshDestroy (m_mesh);
	m_mesh = NewtonMeshApproximateConvexDecomposition (mesh.m_mesh, maxConcavity, backFaceDistanceFactor, maxCount, maxVertexPerHull, NULL, NULL);
}



void dNewtonMesh::Polygonize ()
{
	NewtonMeshPolygonize (m_mesh);
}

void dNewtonMesh::Triangulate ()
{
	NewtonMeshTriangulate (m_mesh);
}

int dNewtonMesh::GetPointCount() const
{
	return NewtonMeshGetPointCount (m_mesh);
}

void dNewtonMesh::GetVertexStreams(dPoint* const posit, dPoint* const normal, dUV* const uv0, dUV* const uv1) const
{
	dAssert(0);
//	NewtonMeshGetVertexStreams (m_mesh, sizeof (dPoint), &posit[0].m_x, sizeof (dPoint), &normal[0].m_x, sizeof (dUV), &uv0[0].m_u, sizeof (dUV), &uv1[0].m_u);
}


int dNewtonMesh::GetTotalIndexCount() const
{
	return NewtonMeshGetTotalIndexCount (m_mesh); 
}

int dNewtonMesh::GetTotalFaceCount() const
{
	return NewtonMeshGetTotalFaceCount (m_mesh); 
}

void* dNewtonMesh::BeginMaterialHandle () const
{
	return NewtonMeshBeginHandle (m_mesh); 
}

void dNewtonMesh::EndMaterialHandle (void* const materialHandle) const
{
	NewtonMeshEndHandle (m_mesh, materialHandle); 
}

int dNewtonMesh::GetMaterialIndex (void* const materialHandle) const
{
	return NewtonMeshFirstMaterial (m_mesh, materialHandle);
}

int dNewtonMesh::GetNextMaterialIndex (void* const materialHandle, int materialIndex) const
{
	return NewtonMeshNextMaterial (m_mesh, materialHandle, materialIndex);
}

int dNewtonMesh::MaterialGetMaterial (void* const materialHandle, int materialIndex) const
{
	return NewtonMeshMaterialGetMaterial (m_mesh, materialHandle, materialIndex); 
}

int dNewtonMesh::MaterialGetIndexCount (void* const materialHandle, int materialIndex) const
{
	return NewtonMeshMaterialGetIndexCount (m_mesh, materialHandle, materialIndex); 
}

void dNewtonMesh::MaterialGetIndexStream (void* const materialHandle, int materialIndex, int* const indexes) const
{
	NewtonMeshMaterialGetIndexStream (m_mesh, materialHandle, materialIndex, indexes); 
}

void dNewtonMesh::ApplyBoxMapping (int topMatId, int sideMatId, int frontMatId)
{
	NewtonMeshApplyBoxMapping(m_mesh, topMatId, sideMatId, frontMatId);
}

void dNewtonMesh::ApplySphericalMapping (int matId)
{
	NewtonMeshApplySphericalMapping(m_mesh, matId);
}

void dNewtonMesh::ApplyCylindricalMapping (int cylinderMatId, int capMatId)
{
	NewtonMeshApplyCylindricalMapping(m_mesh, cylinderMatId, capMatId);
}

void dNewtonMesh::CalculateVertexNormals (dFloat angleInRadians)
{
	NewtonMeshCalculateVertexNormals (m_mesh, angleInRadians);
}


void dNewtonMesh::ApplyTransform (const dFloat* const matrix)
{
	NewtonMeshApplyTransform (m_mesh, matrix);
}
