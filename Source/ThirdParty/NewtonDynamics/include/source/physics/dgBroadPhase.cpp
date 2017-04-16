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

#include "dgPhysicsStdafx.h"
#include "dgBody.h"
#include "dgWorld.h"
#include "dgContact.h"
#include "dgBroadPhase.h"
#include "dgDynamicBody.h"
#include "dgCollisionConvex.h"
#include "dgCollisionInstance.h"
#include "dgWorldDynamicUpdate.h"
#include "dgBilateralConstraint.h"
#include "dgBroadPhaseAggregate.h"
#include "dgCollisionLumpedMassParticles.h"
//#include "dgCollisionLumpedMassParticles.h"

#define DG_CONVEX_CAST_POOLSIZE			32
#define DG_BROADPHASE_AABB_SCALE		dgFloat32 (8.0f)
#define DG_BROADPHASE_AABB_INV_SCALE	(dgFloat32 (1.0f) / DG_BROADPHASE_AABB_SCALE)
#define DG_CONTACT_TRANSLATION_ERROR	dgFloat32 (1.0e-3f)
#define DG_CONTACT_ANGULAR_ERROR		(dgFloat32 (0.25f * 3.141592f / 180.0f))
#define DG_NARROW_PHASE_DIST			dgFloat32 (0.2f)
#define DG_CONTACT_DELAY_FRAMES			4


dgVector dgBroadPhase::m_velocTol(dgFloat32(1.0e-16f)); 
dgVector dgBroadPhase::m_angularContactError2(DG_CONTACT_ANGULAR_ERROR * DG_CONTACT_ANGULAR_ERROR);
dgVector dgBroadPhase::m_linearContactError2(DG_CONTACT_TRANSLATION_ERROR * DG_CONTACT_TRANSLATION_ERROR);
 
dgVector dgBroadPhaseNode::m_broadPhaseScale (DG_BROADPHASE_AABB_SCALE, DG_BROADPHASE_AABB_SCALE, DG_BROADPHASE_AABB_SCALE, dgFloat32 (0.0f));
dgVector dgBroadPhaseNode::m_broadInvPhaseScale (DG_BROADPHASE_AABB_INV_SCALE, DG_BROADPHASE_AABB_INV_SCALE, DG_BROADPHASE_AABB_INV_SCALE, dgFloat32 (0.0f));


dgInt32 dgBroadPhase::m_obbTestSimplex[4][4] =
{
	{ 0, 1, 2, 3 },
	{ 0, 2, 3, 1 },
	{ 2, 1, 3, 0 },
	{ 1, 0, 3, 2 },
};


class dgBroadPhase::dgSpliteInfo
{
	public:
	dgSpliteInfo (dgBroadPhaseNode** const boxArray, dgInt32 boxCount)
	{
		dgVector minP ( dgFloat32 (1.0e15f)); 
		dgVector maxP (-dgFloat32 (1.0e15f)); 

		if (boxCount == 2) {
			m_axis = 1;
			for (dgInt32 i = 0; i < boxCount; i ++) {
				dgBroadPhaseNode* const node = boxArray[i];
				dgAssert (node->IsLeafNode());
				minP = minP.GetMin (node->m_minBox); 
				maxP = maxP.GetMax (node->m_maxBox); 
			}
		} else {
			dgVector median (dgFloat32 (0.0f));
			dgVector varian (dgFloat32 (0.0f));
			for (dgInt32 i = 0; i < boxCount; i ++) {
				dgBroadPhaseNode* const node = boxArray[i];
				dgAssert (node->IsLeafNode());
				minP = minP.GetMin (node->m_minBox); 
				maxP = maxP.GetMax (node->m_maxBox); 
				dgVector p ((node->m_minBox + node->m_maxBox).CompProduct4(dgVector::m_half));
				median += p;
				varian += p.CompProduct4(p);
			}

			varian = varian.Scale4 (dgFloat32 (boxCount)) - median.CompProduct4(median);

			dgInt32 index = 0;
			dgFloat32 maxVarian = dgFloat32 (-1.0e10f);
			for (dgInt32 i = 0; i < 3; i ++) {
				if (varian[i] > maxVarian) {
					index = i;
					maxVarian = varian[i];
				}
			}

			dgVector center = median.Scale4 (dgFloat32 (1.0f) / dgFloat32 (boxCount));

			dgFloat32 test = center[index];

			dgInt32 i0 = 0;
			dgInt32 i1 = boxCount - 1;
			do {    
				for (; i0 <= i1; i0 ++) {
					dgBroadPhaseNode* const node = boxArray[i0];
					dgFloat32 val = (node->m_minBox[index] + node->m_maxBox[index]) * dgFloat32 (0.5f);
					if (val > test) {
						break;
					}
				}

				for (; i1 >= i0; i1 --) {
					dgBroadPhaseNode* const node = boxArray[i1];
					dgFloat32 val = (node->m_minBox[index] + node->m_maxBox[index]) * dgFloat32 (0.5f);
					if (val < test) {
						break;
					}
				}

				if (i0 < i1)	{
					dgSwap(boxArray[i0], boxArray[i1]);
					i0++; 
					i1--;
				}

			} while (i0 <= i1);

			if (i0 > 0){
				i0 --;
			}
			if ((i0 + 1) >= boxCount) {
				i0 = boxCount - 2;
			}
			m_axis = i0 + 1;
		}

		dgAssert (maxP.m_x - minP.m_x >= dgFloat32 (0.0f));
		dgAssert (maxP.m_y - minP.m_y >= dgFloat32 (0.0f));
		dgAssert (maxP.m_z - minP.m_z >= dgFloat32 (0.0f));
		m_p0 = minP;
		m_p1 = maxP;
	}

	dgInt32 m_axis;
	dgVector m_p0;
	dgVector m_p1;
};


dgBroadPhase::dgBroadPhase(dgWorld* const world)
	:m_world(world)
	,m_rootNode(NULL)
	,m_generatedBodies(world->GetAllocator())
	,m_updateList(world->GetAllocator())
	,m_aggregateList(world->GetAllocator())
	,m_lru(DG_CONTACT_DELAY_FRAMES)
	,m_contacJointLock()
	,m_criticalSectionLock()
	,m_pendingSoftBodyCollisions(world->GetAllocator(), 64)
	,m_pendingSoftBodyPairsCount(0)
	,m_dirtyNodesCount(0)
	,m_scanTwoWays(false)
	,m_recursiveChunks(false)
{
}

dgBroadPhase::~dgBroadPhase()
{
}


void dgBroadPhase::MoveNodes (dgBroadPhase* const dst)
{
	const dgBodyMasterList* const masterList = m_world;
	for (dgBodyMasterList::dgListNode* node = masterList->GetFirst(); node; node = node->GetNext()) {
		dgBody* const body = node->GetInfo().GetBody();
		if (body->GetBroadPhase() && !body->GetBroadPhaseAggregate()) {
			Remove(body);
			dst->Add(body);
		}
	}

	dgList<dgBroadPhaseAggregate*>::dgListNode* next;
	for (dgList<dgBroadPhaseAggregate*>::dgListNode* ptr = m_aggregateList.GetFirst(); ptr; ptr = next) {
		next = ptr->GetNext();
		dgBroadPhaseAggregate* const aggregate = ptr->GetInfo();
		m_aggregateList.Remove (aggregate->m_myAggregateNode);
		m_updateList.Remove(aggregate->m_updateNode);
		aggregate->m_updateNode = NULL;
		aggregate->m_myAggregateNode = NULL;
		UnlinkAggregate(aggregate);
		dst->LinkAggregate(aggregate);
	}
}

dgBroadPhaseTreeNode* dgBroadPhase::InsertNode(dgBroadPhaseNode* const root, dgBroadPhaseNode* const node)
{
	dgVector p0;
	dgVector p1;

	dgBroadPhaseNode* sibling = root;
	dgFloat32 surfaceArea = CalculateSurfaceArea(node, sibling, p0, p1);
	while (!sibling->IsLeafNode()) {

		if (surfaceArea > sibling->m_surfaceArea) {
			break;
		}

		sibling->m_minBox = p0;
		sibling->m_maxBox = p1;
		sibling->m_surfaceArea = surfaceArea;

		dgVector leftP0;
		dgVector leftP1;
		dgFloat32 leftSurfaceArea = CalculateSurfaceArea(node, sibling->GetLeft(), leftP0, leftP1);

		dgVector rightP0;
		dgVector rightP1;
		dgFloat32 rightSurfaceArea = CalculateSurfaceArea(node, sibling->GetRight(), rightP0, rightP1);

		if (leftSurfaceArea < rightSurfaceArea) {
			sibling = sibling->GetLeft();
			p0 = leftP0;
			p1 = leftP1;
			surfaceArea = leftSurfaceArea;
		} else {
			sibling = sibling->GetRight();
			p0 = rightP0;
			p1 = rightP1;
			surfaceArea = rightSurfaceArea;
		}
	}

	dgBroadPhaseTreeNode* const parent = new (m_world->GetAllocator()) dgBroadPhaseTreeNode(sibling, node);
	return parent;
}


void dgBroadPhase::UpdateAggregateEntropyKernel(void* const context, void* const node, dgInt32 threadID)
{
	dTimeTrackerEvent(__FUNCTION__);
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*)context;
	dgWorld* const world = descriptor->m_world;
	dgBroadPhase* const broadPhase = world->GetBroadPhase();
	broadPhase->UpdateAggregateEntropy(descriptor, (dgList<dgBroadPhaseAggregate*>::dgListNode*) node, threadID);
}

void dgBroadPhase::ForceAndToqueKernel(void* const context, void* const node, dgInt32 threadID)
{
	dTimeTrackerEvent(__FUNCTION__);
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*)context;
	dgWorld* const world = descriptor->m_world;
	dgBroadPhase* const broadPhase = world->GetBroadPhase();
	broadPhase->ApplyForceAndtorque(descriptor, (dgBodyMasterList::dgListNode*) node, threadID);
}

void dgBroadPhase::SleepingStateKernel(void* const context, void* const node, dgInt32 threadID)
{
	dTimeTrackerEvent(__FUNCTION__);
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*)context;
	dgWorld* const world = descriptor->m_world;
	dgBroadPhase* const broadPhase = world->GetBroadPhase();
	broadPhase->SleepingState(descriptor, (dgBodyMasterList::dgListNode*) node, threadID);
}

bool dgBroadPhase::DoNeedUpdate(dgBodyMasterList::dgListNode* const node) const
{
	dgBody* const body = node->GetInfo().GetBody();

	bool state = body->GetInvMass().m_w != dgFloat32 (0.0f);
	state = state || !body->m_equilibrium || (body->GetExtForceAndTorqueCallback() != NULL);
	return state;
}

void dgBroadPhase::UpdateAggregateEntropy (dgBroadphaseSyncDescriptor* const descriptor, dgList<dgBroadPhaseAggregate*>::dgListNode* node, dgInt32 threadID)
{
	const dgInt32 threadCount = descriptor->m_world->GetThreadCount();
	while (node) {
		node->GetInfo()->ImproveEntropy();
		for (dgInt32 i = 0; i < threadCount; i++) {
			node = node ? node->GetNext() : NULL;
		}
	}
}


void dgBroadPhase::ApplyForceAndtorque(dgBroadphaseSyncDescriptor* const descriptor, dgBodyMasterList::dgListNode* node, dgInt32 threadID)
{
	dgFloat32 timestep = descriptor->m_timestep;

	const dgInt32 threadCount = descriptor->m_world->GetThreadCount();
	while (node) {
		if (DoNeedUpdate(node)) {
			dgBody* const body = node->GetInfo().GetBody();

			if (body->IsRTTIType(dgBody::m_dynamicBodyRTTI)) {
				dgDynamicBody* const dynamicBody = (dgDynamicBody*)body;
				dynamicBody->ApplyExtenalForces(timestep, threadID);
			}
		}

		for (dgInt32 i = 0; i < threadCount; i++) {
			node = node ? node->GetPrev() : NULL;
		}
	}
}


void dgBroadPhase::SleepingState(dgBroadphaseSyncDescriptor* const descriptor, dgBodyMasterList::dgListNode* node, dgInt32 threadID)
{
	dgFloat32 timestep = descriptor->m_timestep;

	const dgInt32 threadCount = descriptor->m_world->GetThreadCount();
	while (node) {
		if (DoNeedUpdate(node)) {
			dgBody* const body = node->GetInfo().GetBody();

			if (body->IsRTTIType(dgBody::m_dynamicBodyRTTI)) {
				dgDynamicBody* const dynamicBody = (dgDynamicBody*)body;
				if (!dynamicBody->IsInEquilibrium()) {
					dynamicBody->m_sleeping = false;
					dynamicBody->m_equilibrium = false;
					dynamicBody->UpdateCollisionMatrix(timestep, threadID);
				}
				if (dynamicBody->GetInvMass().m_w == dgFloat32(0.0f) || body->m_collision->IsType(dgCollision::dgCollisionMesh_RTTI)) {
					dynamicBody->m_sleeping = true;
					dynamicBody->m_autoSleep = true;
					dynamicBody->m_equilibrium = true;
				}

				dynamicBody->m_savedExternalForce = dynamicBody->m_externalForce;
				dynamicBody->m_savedExternalTorque = dynamicBody->m_externalTorque;
			} else {
				dgAssert(body->IsRTTIType(dgBody::m_kinematicBodyRTTI));

				// kinematic bodies are always sleeping (skip collision with kinematic bodies)
				if (body->IsCollidable()) {
					body->m_sleeping = false;
					body->m_autoSleep = false;
				} else {
					body->m_sleeping = true;
					body->m_autoSleep = true;
				}
				body->m_equilibrium = true;

				// update collision matrix by calling the transform callback for all kinematic bodies
				body->UpdateMatrix(timestep, threadID);
			}
		}

		for (dgInt32 i = 0; i < threadCount; i++) {
			node = node ? node->GetPrev() : NULL;
		}
	}
}


void dgBroadPhase::ForEachBodyInAABB(const dgBroadPhaseNode** stackPool, dgInt32 stack, const dgVector& minBox, const dgVector& maxBox, OnBodiesInAABB callback, void* const userData) const
{
	while (stack) {
		stack--;
		const dgBroadPhaseNode* const rootNode = stackPool[stack];
		if (dgOverlapTest(rootNode->m_minBox, rootNode->m_maxBox, minBox, maxBox)) {

			dgBody* const body = rootNode->GetBody();
			if (body) {
				if (dgOverlapTest(body->m_minAABB, body->m_maxAABB, minBox, maxBox)) {
					if (!callback(body, userData)) {
						break;
					}
				}
			} else if (rootNode->IsAggregate()) {
				dgBroadPhaseAggregate* const aggregate = (dgBroadPhaseAggregate*)rootNode;
				if (aggregate->m_root) {
					stackPool[stack] = aggregate->m_root;
					stack++;
					dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
				}

			} else {
				dgAssert (!rootNode->IsLeafNode());
				dgBroadPhaseTreeNode* const node = (dgBroadPhaseTreeNode*)rootNode;
				stackPool[stack] = node->m_left;
				stack++;
				dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);

				stackPool[stack] = node->m_right;
				stack++;
				dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
			}
		}
	}
}



dgInt32 dgBroadPhase::ConvexCast(const dgBroadPhaseNode** stackPool, dgFloat32* const distance, dgInt32 stack, const dgVector& velocA, const dgVector& velocB, dgFastRayTest& ray,
								 dgCollisionInstance* const shape, const dgMatrix& matrix, const dgVector& target, dgFloat32* const param, OnRayPrecastAction prefilter, void* const userData, dgConvexCastReturnInfo* const info, dgInt32 maxContacts, dgInt32 threadIndex) const
{
	dgVector boxP0;
	dgVector boxP1;
	dgTriplex points[DG_CONVEX_CAST_POOLSIZE];
	dgTriplex normals[DG_CONVEX_CAST_POOLSIZE];
	dgFloat32 penetration[DG_CONVEX_CAST_POOLSIZE];
	dgInt64 attributeA[DG_CONVEX_CAST_POOLSIZE];
	dgInt64 attributeB[DG_CONVEX_CAST_POOLSIZE];
	dgInt32 totalCount = 0;

	dgAssert(matrix.TestOrthogonal());
	shape->CalcAABB(matrix, boxP0, boxP1);

	maxContacts = dgMin (maxContacts, DG_CONVEX_CAST_POOLSIZE);
	dgAssert (!maxContacts || (maxContacts && info));
	dgFloat32 maxParam = *param;
	dgFloat32 timeToImpact = *param;
	while (stack) {
		stack--;

		dgFloat32 dist = distance[stack];

		if (dist > maxParam) {
			break;
		} else {
			const dgBroadPhaseNode* const me = stackPool[stack];

			dgBody* const body = me->GetBody();
			if (body) {
				if (!PREFILTER_RAYCAST(prefilter, body, body->m_collision, userData)) {
					dgInt32 count = m_world->CollideContinue(shape, matrix, velocA, velocB, body->m_collision, body->m_matrix, velocB, velocB, timeToImpact, points, normals, penetration, attributeA, attributeB, maxContacts, threadIndex);

					if (timeToImpact < maxParam) {
						if ((timeToImpact - maxParam) < dgFloat32(-1.0e-3f)) {
							totalCount = 0;
						}
						maxParam = timeToImpact;
						if (count >= (maxContacts - totalCount)) {
							count = maxContacts - totalCount;
						}

						for (dgInt32 i = 0; i < count; i++) {
							info[totalCount].m_point[0] = points[i].m_x;
							info[totalCount].m_point[1] = points[i].m_y;
							info[totalCount].m_point[2] = points[i].m_z;
							info[totalCount].m_point[3] = dgFloat32(0.0f);
							info[totalCount].m_normal[0] = normals[i].m_x;
							info[totalCount].m_normal[1] = normals[i].m_y;
							info[totalCount].m_normal[2] = normals[i].m_z;
							info[totalCount].m_normal[3] = dgFloat32(0.0f);
							info[totalCount].m_penetration = penetration[i];
							info[totalCount].m_contaID = attributeB[i];
							info[totalCount].m_hitBody = body;
							totalCount++;
						}
					}
					if (maxParam < 1.0e-8f) {
						break;
					}
				}
			} else if (me->IsAggregate()) {
				dgBroadPhaseAggregate* const aggregate = (dgBroadPhaseAggregate*)me;
				const dgBroadPhaseNode* const node = aggregate->m_root;
				if (node) {
					dgVector minBox(node->m_minBox - boxP1);
					dgVector maxBox(node->m_maxBox - boxP0);
					dgFloat32 dist1 = ray.BoxIntersect(minBox, maxBox);
					if (dist1 < maxParam) {
						dgInt32 j = stack;
						for (; j && (dist1 > distance[j - 1]); j--) {
							stackPool[j] = stackPool[j - 1];
							distance[j] = distance[j - 1];
						}
						stackPool[j] = node;
						distance[j] = dist1;
						stack++;
						dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
					}
				}

			} else {
				dgBroadPhaseTreeNode* const node = (dgBroadPhaseTreeNode*)me;
				const dgBroadPhaseNode* const left = node->m_left;
				dgAssert(left);
				dgVector minBox(left->m_minBox - boxP1);
				dgVector maxBox(left->m_maxBox - boxP0);
				dgFloat32 dist1 = ray.BoxIntersect(minBox, maxBox);
				if (dist1 < maxParam) {
					dgInt32 j = stack;
					for (; j && (dist1 > distance[j - 1]); j--) {
						stackPool[j] = stackPool[j - 1];
						distance[j] = distance[j - 1];
					}
					stackPool[j] = left;
					distance[j] = dist1;
					stack++;
					dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
				}

				const dgBroadPhaseNode* const right = node->m_right;
				dgAssert(right);
				minBox = right->m_minBox - boxP1;
				maxBox = right->m_maxBox - boxP0;
				dist1 = ray.BoxIntersect(minBox, maxBox);
				if (dist1 < maxParam) {
					dgInt32 j = stack;
					for (; j && (dist1 > distance[j - 1]); j--) {
						stackPool[j] = stackPool[j - 1];
						distance[j] = distance[j - 1];
					}
					stackPool[j] = right;
					distance[j] = dist1;
					stack++;
					dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
				}
			}
		}
	}
	*param = maxParam;
	return totalCount;
}

dgInt32 dgBroadPhase::Collide(const dgBroadPhaseNode** stackPool, dgInt32* const ovelapStack, dgInt32 stack, const dgVector& boxP0, const dgVector& boxP1, dgCollisionInstance* const shape, const dgMatrix& matrix, OnRayPrecastAction prefilter, void* const userData, dgConvexCastReturnInfo* const info, dgInt32 maxContacts, dgInt32 threadIndex) const
{
	dgTriplex points[DG_CONVEX_CAST_POOLSIZE];
	dgTriplex normals[DG_CONVEX_CAST_POOLSIZE];
	dgFloat32 penetration[DG_CONVEX_CAST_POOLSIZE];
	dgInt64 attributeA[DG_CONVEX_CAST_POOLSIZE];
	dgInt64 attributeB[DG_CONVEX_CAST_POOLSIZE];

	dgInt32 totalCount = 0;
	while (stack) {
		stack--;

		dgInt32 test = ovelapStack[stack];
		if (test) {
			const dgBroadPhaseNode* const me = stackPool[stack];

			dgBody* const body = me->GetBody();
			if (body) {
				if (!PREFILTER_RAYCAST(prefilter, body, body->m_collision, userData)) {
					dgInt32 count = m_world->Collide(shape, matrix, body->m_collision, body->m_matrix, points, normals, penetration, attributeA, attributeB, DG_CONVEX_CAST_POOLSIZE, threadIndex);

					if (count) {
						bool teminate = false;
						if (count >= (maxContacts - totalCount)) {
							count = maxContacts - totalCount;
							teminate = true;
						}

						for (dgInt32 i = 0; i < count; i++) {
							info[totalCount].m_point[0] = points[i].m_x;
							info[totalCount].m_point[1] = points[i].m_y;
							info[totalCount].m_point[2] = points[i].m_z;
							info[totalCount].m_point[3] = dgFloat32(0.0f);
							info[totalCount].m_normal[0] = normals[i].m_x;
							info[totalCount].m_normal[1] = normals[i].m_y;
							info[totalCount].m_normal[2] = normals[i].m_z;
							info[totalCount].m_normal[3] = dgFloat32(0.0f);
							info[totalCount].m_penetration = penetration[i];
							info[totalCount].m_contaID = attributeB[i];
							info[totalCount].m_hitBody = body;
							totalCount++;
						}

						if (teminate) {
							break;
						}
					}
				}
			} else if (me->IsAggregate()) {
				dgBroadPhaseAggregate* const aggregate = (dgBroadPhaseAggregate*)me;
				const dgBroadPhaseNode* const node = aggregate->m_root;
				if (node) {
					stackPool[stack] = node;
					ovelapStack[stack] = dgOverlapTest(node->m_minBox, node->m_maxBox, boxP0, boxP1);
					stack++;
					dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
				}

			} else {
				dgBroadPhaseTreeNode* const node = (dgBroadPhaseTreeNode*)me;
				const dgBroadPhaseNode* const left = node->m_left;
				stackPool[stack] = left;
				ovelapStack[stack] = dgOverlapTest(left->m_minBox, left->m_maxBox, boxP0, boxP1);
				stack ++;
				dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);

				const dgBroadPhaseNode* const right = node->m_right;
				stackPool[stack] = right;
				ovelapStack[stack] = dgOverlapTest(right->m_minBox, right->m_maxBox, boxP0, boxP1);
				stack++;
				dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
			}
		}
	}
	return totalCount;
}

void dgBroadPhase::RayCast(const dgBroadPhaseNode** stackPool, dgFloat32* const distance, dgInt32 stack, const dgVector& l0, const dgVector& l1, dgFastRayTest& ray, OnRayCastAction filter, OnRayPrecastAction prefilter, void* const userData) const
{
	dgLineBox line;
	line.m_l0 = l0;
	line.m_l1 = l1;

	dgFloat32 maxParam = dgFloat32 (1.2f);
	dgVector test(line.m_l0 <= line.m_l1);
	line.m_boxL0 = (line.m_l0 & test) | line.m_l1.AndNot(test);
	line.m_boxL1 = (line.m_l1 & test) | line.m_l0.AndNot(test);

	while (stack) {
		stack--;
		dgFloat32 dist = distance[stack];
		if (dist > maxParam) {
			break;
		} else {
			const dgBroadPhaseNode* const me = stackPool[stack];
			dgAssert(me);
			dgBody* const body = me->GetBody();
			if (body) {
				dgAssert(!me->GetLeft());
				dgAssert(!me->GetRight());
				dgFloat32 param = body->RayCast(line, filter, prefilter, userData, maxParam);
				if (param < maxParam) {
					maxParam = param;
					if (maxParam < dgFloat32(1.0e-8f)) {
						break;
					}
				}
			} else if (me->IsAggregate()) {
				dgBroadPhaseAggregate* const aggregate = (dgBroadPhaseAggregate*) me;
				if (aggregate->m_root) {
					const dgBroadPhaseNode* const child = aggregate->m_root;
					dgAssert(child);
					dgFloat32 dist1 = ray.BoxIntersect(child->m_minBox, child->m_maxBox);
					if (dist1 < maxParam) {
						dgInt32 j = stack;
						for (; j && (dist1 > distance[j - 1]); j--) {
							stackPool[j] = stackPool[j - 1];
							distance[j] = distance[j - 1];
						}
						stackPool[j] = child;
						distance[j] = dist1;
						stack++;
						dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
					}
				}
			} else {
				const dgBroadPhaseNode* const left = me->GetLeft();
				dgAssert(left);
				dgFloat32 dist1 = ray.BoxIntersect(left->m_minBox, left->m_maxBox);
				if (dist1 < maxParam) {
					dgInt32 j = stack;
					for (; j && (dist1 > distance[j - 1]); j--) {
						stackPool[j] = stackPool[j - 1];
						distance[j] = distance[j - 1];
					}
					stackPool[j] = left;
					distance[j] = dist1;
					stack++;
					dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
				}

				const dgBroadPhaseNode* const right = me->GetRight();
				dgAssert(right);
				dist1 = ray.BoxIntersect(right->m_minBox, right->m_maxBox);
				if (dist1 < maxParam) {
					dgInt32 j = stack;
					for (; j && (dist1 > distance[j - 1]); j--) {
						stackPool[j] = stackPool[j - 1];
						distance[j] = distance[j - 1];
					}
					stackPool[j] = right;
					distance[j] = dist1;
					stack++;
					dgAssert(stack < DG_BROADPHASE_MAX_STACK_DEPTH);
				}
			}
		}
	}
}

void dgBroadPhase::CollisionChange (dgBody* const body, dgCollisionInstance* const collision)
{
	dgCollisionInstance* const bodyCollision = body->GetCollision();
	if (bodyCollision) {
		if (bodyCollision->IsType (dgCollision::dgCollisionNull_RTTI) && !collision->IsType (dgCollision::dgCollisionNull_RTTI)) {
			dgAssert (!body->GetBroadPhase());
			body->m_collision = m_world->m_pointCollision;
			Add (body);
			body->m_collision = bodyCollision;
		} else if (!bodyCollision->IsType (dgCollision::dgCollisionNull_RTTI) && collision->IsType (dgCollision::dgCollisionNull_RTTI)) {
			Remove(body);
		}
	}
}

void dgBroadPhase::UpdateBody(dgBody* const body, dgInt32 threadIndex)
{
	if (m_rootNode && !m_rootNode->IsLeafNode() && body->m_masterNode) {
		dgBroadPhaseBodyNode* const node = body->GetBroadPhase();
		dgBody* const body1 = node->GetBody();
		dgAssert(body1 == body);

		dgAssert(!node->GetLeft());
		dgAssert(!node->GetRight());
		dgAssert(!body1->GetCollision()->IsType(dgCollision::dgCollisionNull_RTTI));

		const dgBroadPhaseNode* const root = (m_rootNode->GetLeft() && m_rootNode->GetRight()) ? NULL : m_rootNode;

		dgThreadHiveScopeLock lock(m_world, &m_criticalSectionLock, true);
		if (body1->GetBroadPhaseAggregate()) {
			dgBroadPhaseAggregate* const aggregate = body1->GetBroadPhaseAggregate();
			aggregate->m_isInEquilibrium = body1->m_equilibrium;
			aggregate->SetAsDirty(m_lru + 1);
		}

		m_dirtyNodesCount += (node->m_nodeIsDirtyLru != (m_lru + 1)) ? 1 : 0;
		node->SetAsDirty(m_lru + 1);
		if (!dgBoxInclusionTest(body1->m_minAABB, body1->m_maxAABB, node->m_minBox, node->m_maxBox)) {
			dgAssert(!node->IsAggregate());
			node->SetAABB(body1->m_minAABB, body1->m_maxAABB);
			for (dgBroadPhaseNode* parent = node->m_parent; parent != root; parent = parent->m_parent) {
				if (!parent->IsAggregate()) {
					dgVector minBox;
					dgVector maxBox;
					dgFloat32 area = CalculateSurfaceArea(parent->GetLeft(), parent->GetRight(), minBox, maxBox);
					if (dgBoxInclusionTest(minBox, maxBox, parent->m_minBox, parent->m_maxBox)) {
						break;
					}
					parent->m_minBox = minBox;
					parent->m_maxBox = maxBox;
					parent->m_surfaceArea = area;
				} else {
					dgBroadPhaseAggregate* const aggregate = (dgBroadPhaseAggregate*)parent;
					aggregate->m_minBox = aggregate->m_root->m_minBox;
					aggregate->m_maxBox = aggregate->m_root->m_maxBox;
					aggregate->m_surfaceArea = aggregate->m_root->m_surfaceArea;
				}
			}
		}
	}
}


dgBroadPhaseNode* dgBroadPhase::BuildTopDown(dgBroadPhaseNode** const leafArray, dgInt32 firstBox, dgInt32 lastBox, dgFitnessList::dgListNode** const nextNode)
{
	dgAssert(firstBox >= 0);
	dgAssert(lastBox >= 0);

	if (lastBox == firstBox) {
		return leafArray[firstBox];
	} else {
		dgSpliteInfo info(&leafArray[firstBox], lastBox - firstBox + 1);

		dgBroadPhaseTreeNode* const parent = (*nextNode)->GetInfo();
		parent->m_parent = NULL;
		*nextNode = (*nextNode)->GetNext();

		parent->SetAABB(info.m_p0, info.m_p1);

		parent->m_left = BuildTopDown(leafArray, firstBox, firstBox + info.m_axis - 1, nextNode);
		parent->m_left->m_parent = parent;

		parent->m_right = BuildTopDown(leafArray, firstBox + info.m_axis, lastBox, nextNode);
		parent->m_right->m_parent = parent;
		return parent;
	}
}


dgBroadPhaseNode* dgBroadPhase::BuildTopDownBig(dgBroadPhaseNode** const leafArray, dgInt32 firstBox, dgInt32 lastBox, dgFitnessList::dgListNode** const nextNode)
{
	if (lastBox == firstBox) {
		return BuildTopDown(leafArray, firstBox, lastBox, nextNode);
	}

	dgInt32 midPoint = -1;
	const dgFloat32 scale = dgFloat32(10.0f);
	const dgFloat32 scale2 = dgFloat32(3.0f) * scale * scale;
	const dgInt32 count = lastBox - firstBox;
	for (dgInt32 i = 0; i < count; i++) {
		const dgBroadPhaseNode* const node0 = leafArray[firstBox + i];
		const dgBroadPhaseNode* const node1 = leafArray[firstBox + i + 1];
		if (node1->m_surfaceArea >(scale2 * node0->m_surfaceArea)) {
			midPoint = i;
			break;
		}
	}

	if (midPoint == -1) {
		return BuildTopDown(leafArray, firstBox, lastBox, nextNode);
	} else {
		dgBroadPhaseTreeNode* const parent = (*nextNode)->GetInfo();

		parent->m_parent = NULL;
		*nextNode = (*nextNode)->GetNext();

		dgVector minP(dgFloat32(1.0e15f));
		dgVector maxP(-dgFloat32(1.0e15f));
		for (dgInt32 i = 0; i <= count; i++) {
			const dgBroadPhaseNode* const node = leafArray[firstBox + i];
			dgAssert(node->IsLeafNode());
			minP = minP.GetMin(node->m_minBox);
			maxP = maxP.GetMax(node->m_maxBox);
		}

		parent->SetAABB(minP, maxP);
		parent->m_left = BuildTopDown(leafArray, firstBox, firstBox + midPoint, nextNode);
		parent->m_left->m_parent = parent;

		parent->m_right = BuildTopDownBig(leafArray, firstBox + midPoint + 1, lastBox, nextNode);
		parent->m_right->m_parent = parent;
		return parent;
	}
}


dgInt32 dgBroadPhase::CompareNodes(const dgBroadPhaseNode* const nodeA, const dgBroadPhaseNode* const nodeB, void* const)
{
	dgFloat32 areaA = nodeA->m_surfaceArea;
	dgFloat32 areaB = nodeB->m_surfaceArea;
	if (areaA < areaB) {
		return -1;
	}
	if (areaA > areaB) {
		return 1;
	}
	return 0;
}


void dgBroadPhase::ImproveFitness(dgFitnessList& fitness, dgFloat64& oldEntropy, dgBroadPhaseNode** const root)
{
	dTimeTrackerEvent(__FUNCTION__);
	if (*root) {
		dgBroadPhaseNode* const parent = (*root)->m_parent;
		(*root)->m_parent = NULL;
		dgFloat64 entropy = CalculateEntropy(fitness, root);

		if ((entropy > oldEntropy * dgFloat32(2.0f)) || (entropy < oldEntropy * dgFloat32(0.5f))) {
			if (fitness.GetFirst()) {
				m_world->m_solverJacobiansMemory.ResizeIfNecessary ((fitness.GetCount() * 2 + 16) * sizeof (dgBroadPhaseNode*));
				dgBroadPhaseNode** const leafArray = (dgBroadPhaseNode**)&m_world->m_solverJacobiansMemory[0];

				dgInt32 leafNodesCount = 0;
				for (dgFitnessList::dgListNode* nodePtr = fitness.GetFirst(); nodePtr; nodePtr = nodePtr->GetNext()) {
					dgBroadPhaseNode* const node = nodePtr->GetInfo();
					dgBroadPhaseNode* const leftNode = node->GetLeft();
					dgBody* const leftBody = leftNode->GetBody();
					if (leftBody) {
						node->SetAABB(leftBody->m_minAABB, leftBody->m_maxAABB);
						leafArray[leafNodesCount] = leftNode;
						leafNodesCount++;
					} else if (leftNode->IsAggregate()) {
						leafArray[leafNodesCount] = leftNode;
						leafNodesCount++;
					}
					dgBroadPhaseNode* const rightNode = node->GetRight();
					dgBody* const rightBody = rightNode->GetBody();
					if (rightBody) {
						rightNode->SetAABB(rightBody->m_minAABB, rightBody->m_maxAABB);
						leafArray[leafNodesCount] = rightNode;
						leafNodesCount++;
					} else if (rightNode->IsAggregate()) {
						leafArray[leafNodesCount] = rightNode;
						leafNodesCount++;
					}
				}

				dgFitnessList::dgListNode* nodePtr = fitness.GetFirst();

				dgSortIndirect(leafArray, leafNodesCount, CompareNodes);
				*root = BuildTopDownBig(leafArray, 0, leafNodesCount - 1, &nodePtr);
				dgAssert(!(*root)->m_parent);
				entropy = CalculateEntropy(fitness, root);
			}
		}
		(*root)->m_parent = parent;
		oldEntropy = entropy;
	}
}


void dgBroadPhase::RotateLeft (dgBroadPhaseTreeNode* const node, dgBroadPhaseNode** const root)
{
	dgVector cost1P0;
	dgVector cost1P1;

	dgBroadPhaseTreeNode* const parent = (dgBroadPhaseTreeNode*)node->m_parent;
	dgAssert(parent && !parent->IsLeafNode());
	dgFloat32 cost1 = CalculateSurfaceArea(node->m_left, parent->m_left, cost1P0, cost1P1);

	dgVector cost2P0;
	dgVector cost2P1;
	dgFloat32 cost2 = CalculateSurfaceArea(node->m_right, parent->m_left, cost2P0, cost2P1);

	dgFloat32 cost0 = node->m_surfaceArea;
	if ((cost1 <= cost0) && (cost1 <= cost2)) {
		//dgBroadPhaseNode* const parent = node->m_parent;
		node->m_minBox = parent->m_minBox;
		node->m_maxBox = parent->m_maxBox;
		node->m_surfaceArea = parent->m_surfaceArea;

		dgBroadPhaseTreeNode* const grandParent = (dgBroadPhaseTreeNode*) parent->m_parent;
		if (grandParent) {
			if (grandParent->m_left == parent) {
				grandParent->m_left = node;
			} else {
				dgAssert(grandParent->m_right == parent);
				grandParent->m_right = node;
			}
		} else {
			(*root) = node;
		}

		node->m_parent = parent->m_parent;
		parent->m_parent = node;
		node->m_left->m_parent = parent;
		parent->m_right = node->m_left;
		node->m_left = parent;

		parent->m_minBox = cost1P0;
		parent->m_maxBox = cost1P1;
		parent->m_surfaceArea = cost1;

	} else if ((cost2 <= cost0) && (cost2 <= cost1)) {
		//dgBroadPhaseNode* const parent = node->m_parent;
		node->m_minBox = parent->m_minBox;
		node->m_maxBox = parent->m_maxBox;
		node->m_surfaceArea = parent->m_surfaceArea;

		dgBroadPhaseTreeNode* const grandParent = (dgBroadPhaseTreeNode*) parent->m_parent;
		if (grandParent) {
			if (grandParent->m_left == parent) {
				grandParent->m_left = node;
			} else {
				dgAssert(grandParent->m_right == parent);
				grandParent->m_right = node;
			}
		} else {
			(*root) = node;
		}

		node->m_parent = parent->m_parent;
		parent->m_parent = node;
		node->m_right->m_parent = parent;
		parent->m_right = node->m_right;
		node->m_right = parent;

		parent->m_minBox = cost2P0;
		parent->m_maxBox = cost2P1;
		parent->m_surfaceArea = cost2;
	}
}

void dgBroadPhase::RotateRight (dgBroadPhaseTreeNode* const node, dgBroadPhaseNode** const root)
{
	dgVector cost1P0;
	dgVector cost1P1;

	dgBroadPhaseTreeNode* const parent = (dgBroadPhaseTreeNode*) node->m_parent;
	dgAssert (parent && !parent->IsLeafNode());

	dgFloat32 cost1 = CalculateSurfaceArea(node->m_right, parent->m_right, cost1P0, cost1P1);

	dgVector cost2P0;
	dgVector cost2P1;
	dgFloat32 cost2 = CalculateSurfaceArea(node->m_left, parent->m_right, cost2P0, cost2P1);

	dgFloat32 cost0 = node->m_surfaceArea;
	if ((cost1 <= cost0) && (cost1 <= cost2)) {
		//dgBroadPhaseNode* const parent = node->m_parent;
		node->m_minBox = parent->m_minBox;
		node->m_maxBox = parent->m_maxBox;
		node->m_surfaceArea = parent->m_surfaceArea;

		dgBroadPhaseTreeNode* const grandParent = (dgBroadPhaseTreeNode*) parent->m_parent;
		if (grandParent) {
			dgAssert (!grandParent->IsLeafNode());
			if (grandParent->m_left == parent) {
				grandParent->m_left = node;
			} else {
				dgAssert(grandParent->m_right == parent);
				grandParent->m_right = node;
			}
		} else {
			(*root) = node;
		}

		node->m_parent = parent->m_parent;
		parent->m_parent = node;
		node->m_right->m_parent = parent;
		parent->m_left = node->m_right;
		node->m_right = parent;
		parent->m_minBox = cost1P0;
		parent->m_maxBox = cost1P1;
		parent->m_surfaceArea = cost1;

	} else if ((cost2 <= cost0) && (cost2 <= cost1)) {
		//dgBroadPhaseNode* const parent = node->m_parent;
		node->m_minBox = parent->m_minBox;
		node->m_maxBox = parent->m_maxBox;
		node->m_surfaceArea = parent->m_surfaceArea;

		dgBroadPhaseTreeNode* const grandParent = (dgBroadPhaseTreeNode*) parent->m_parent;
		if (parent->m_parent) {
			if (grandParent->m_left == parent) {
				grandParent->m_left = node;
			} else {
				dgAssert(grandParent->m_right == parent);
				grandParent->m_right = node;
			}
		} else {
			(*root) = node;
		}

		node->m_parent = parent->m_parent;
		parent->m_parent = node;
		node->m_left->m_parent = parent;
		parent->m_left = node->m_left;
		node->m_left = parent;

		parent->m_minBox = cost2P0;
		parent->m_maxBox = cost2P1;
		parent->m_surfaceArea = cost2;
	}
}


bool dgBroadPhase::ValidateContactCache(dgContact* const contact, dgFloat32 timestep) const
{
	dgAssert(contact && (contact->GetId() == dgConstraint::m_contactConstraint));

	dgBody* const body0 = contact->GetBody0();
	dgBody* const body1 = contact->GetBody1();

	dgVector deltaTime(timestep);
	dgVector positStep((body0->m_veloc - body1->m_veloc).CompProduct4(deltaTime));
	positStep = ((positStep.DotProduct4(positStep)) > m_velocTol) & positStep;
	contact->m_positAcc += positStep;

	dgVector positError2(contact->m_positAcc.DotProduct4(contact->m_positAcc));
	if ((positError2 < m_linearContactError2).GetSignMask()) {
		dgVector rotationStep((body0->m_omega - body1->m_omega).CompProduct4(deltaTime));
		rotationStep = ((rotationStep.DotProduct4(rotationStep)) > m_velocTol) & rotationStep;
		contact->m_rotationAcc = contact->m_rotationAcc * dgQuaternion(dgFloat32(1.0f), rotationStep.m_x, rotationStep.m_y, rotationStep.m_z);

		dgVector angle(contact->m_rotationAcc.m_q1, contact->m_rotationAcc.m_q2, contact->m_rotationAcc.m_q3, dgFloat32(0.0f));
		dgVector rotatError2(angle.DotProduct4(angle));
		if ((rotatError2 < m_angularContactError2).GetSignMask()) {
			return true;
		}
	}
	return false;
}


void dgBroadPhase::CalculatePairContacts (dgPair* const pair, dgInt32 threadID)
{
    dgContactPoint contacts[DG_MAX_CONTATCS];

	pair->m_cacheIsValid = false;
	pair->m_contactBuffer = contacts;
	m_world->CalculateContacts(pair, threadID, false, false);

	if (pair->m_contactCount) {
		if (pair->m_contact->m_body0->m_invMass.m_w != dgFloat32 (0.0f)) {
			pair->m_contact->m_body0->m_equilibrium = false;
		}
		if (pair->m_contact->m_body1->m_invMass.m_w != dgFloat32 (0.0f)) {
			pair->m_contact->m_body1->m_equilibrium = false;
		}
		dgAssert(pair->m_contactCount <= (DG_CONSTRAINT_MAX_ROWS / 3));
		m_world->ProcessContacts(pair, threadID);
		KinematicBodyActivation(pair->m_contact);
	} else {
		if (pair->m_cacheIsValid) {
			//m_world->ProcessCachedContacts (pair->m_contact, timestep, threadID);
			KinematicBodyActivation(pair->m_contact);
		} else {
			pair->m_contact->m_maxDOF = 0;
		}
	}
}

void dgBroadPhase::AddPair (dgContact* const contact, dgFloat32 timestep, dgInt32 threadIndex)
{
	dgWorld* const world = (dgWorld*) m_world;
	dgBody* const body0 = contact->m_body0;
	dgBody* const body1 = contact->m_body1;

	dgAssert (body0 != m_world->m_sentinelBody);
	dgAssert (body1 != m_world->m_sentinelBody);
	dgAssert (contact->GetId() == dgConstraint::m_contactConstraint);
	dgAssert (body0->GetWorld());
	dgAssert (body1->GetWorld());
	dgAssert (body0->GetWorld() == world);
	dgAssert (body1->GetWorld() == world);
	if (!(body0->m_collideWithLinkedBodies & body1->m_collideWithLinkedBodies)) {
		if (world->AreBodyConnectedByJoints (body0, body1)) {
			return;
		}
	}

	const dgContactMaterial* const material = contact->m_material;
	if (material->m_flags & dgContactMaterial::m_collisionEnable) {
		dgInt32 processContacts = 1;
		if (material->m_aabbOverlap) {
			processContacts = material->m_aabbOverlap (*material, *body0, *body1, threadIndex);
		}
		if (processContacts) {
            dgPair pair;
			dgAssert (!body0->m_collision->IsType (dgCollision::dgCollisionNull_RTTI));
			dgAssert (!body1->m_collision->IsType (dgCollision::dgCollisionNull_RTTI));

			pair.m_contact = contact;
			pair.m_timestep = timestep;
            CalculatePairContacts (&pair, threadIndex);
		}
	}
}


void dgBroadPhase::AddPair (dgBody* const body0, dgBody* const body1, const dgFloat32 timestep, dgInt32 threadID)
{
	dTimeTrackerEvent(__FUNCTION__);

	dgAssert ((body0->GetInvMass().m_w != dgFloat32 (0.0f)) || (body1->GetInvMass().m_w != dgFloat32 (0.0f)) || (body0->IsRTTIType(dgBody::m_kinematicBodyRTTI)) || (body1->IsRTTIType(dgBody::m_kinematicBodyRTTI)));
	dgThreadHiveScopeLock lock(m_world, &m_contacJointLock, true);
	dgContact* contact = m_world->FindContactJoint(body0, body1);
	if (!contact) {
		const dgBilateralConstraint* const bilateral = m_world->FindBilateralJoint (body0, body1);
		const bool isCollidable = bilateral ? bilateral->IsCollidable() : true;

		if (isCollidable) {
			dgUnsigned32 group0_ID = dgUnsigned32 (body0->m_bodyGroupId);
			dgUnsigned32 group1_ID = dgUnsigned32 (body1->m_bodyGroupId);
			if (group1_ID < group0_ID) {
				dgSwap (group0_ID, group1_ID);
			}

			dgUnsigned32 key = (group1_ID << 16) + group0_ID;
			const dgBodyMaterialList* const materialList = m_world;  
			const dgContactMaterial* const material = &materialList->Find (key)->GetInfo();

			if (material->m_flags & dgContactMaterial::m_collisionEnable) {
				bool kinematicBodyEquilibrium = (((body0->IsRTTIType(dgBody::m_kinematicBodyRTTI) ? true : false) & body0->IsCollidable()) | ((body1->IsRTTIType(dgBody::m_kinematicBodyRTTI) ? true : false) & body1->IsCollidable())) ? false : true;
				if (!(body0->m_equilibrium & body1->m_equilibrium & kinematicBodyEquilibrium)) {
					const dgInt32 isSofBody0 = body0->m_collision->IsType(dgCollision::dgCollisionLumpedMass_RTTI);
					const dgInt32 isSofBody1 = body1->m_collision->IsType(dgCollision::dgCollisionLumpedMass_RTTI);
					if (isSofBody0 || isSofBody1) {
						m_pendingSoftBodyCollisions[m_pendingSoftBodyPairsCount].m_body0 = body0;
						m_pendingSoftBodyCollisions[m_pendingSoftBodyPairsCount].m_body1 = body1;
						m_pendingSoftBodyPairsCount++;
					} else {
						contact = new (m_world->m_allocator) dgContact(m_world, material);
						contact->AppendToActiveList();
						m_world->AttachConstraint(contact, body0, body1);

						dgAssert(contact);
						contact->m_contactActive = 0;
						contact->m_positAcc = dgVector(dgFloat32(10.0f));
						contact->m_timeOfImpact = dgFloat32(1.0e10f);
					}
				}
			}
		}
	}

	if (contact) {
		contact->m_broadphaseLru = m_lru;
	}
}


void dgBroadPhase::FindGeneratedBodiesCollidingPairs(dgBroadphaseSyncDescriptor* const descriptor, dgInt32 threadID)
{
dgAssert (0);
/*
	dgList<dgBody*>::dgListNode* node = NULL;
	{
		dgThreadHiveScopeLock lock(m_world, &m_criticalSectionLock, false);
		node = descriptor->m_newBodiesNodes;
		if (node) {
			descriptor->m_newBodiesNodes = node->GetNext();
		}
	}

	dgVector timestep2(descriptor->m_timestep * descriptor->m_timestep * dgFloat32(4.0f));
	while (node) {
		dgBody* const body = node->GetInfo();
		dgBroadPhaseNode* const breadPhaseNode = body->GetBroadPhase();
		if (breadPhaseNode) {
			if (!body->m_collision->IsType(dgCollision::dgCollisionNull_RTTI)) {
				for (dgBroadPhaseNode* ptr = breadPhaseNode; ptr->m_parent; ptr = ptr->m_parent) {
					dgBroadPhaseNode* const sibling = ptr->m_parent->m_right;
					if (sibling != ptr) {
						dgAssert(0);
						//SubmitPairs (bodyNode, sibling, timestep2, threadID);
					}
					else {
						dgAssert(0);
						//dgNode* const sibling = ptr->m_parent->m_left;
						//dgAssert (sibling);
						//dgAssert (sibling != ptr);
						//dgAssert (0);
						//SubmitPairs (bodyNode, sibling, timestep2, threadID);
					}
				}
			}
		}

		dgThreadHiveScopeLock lock(m_world, &m_criticalSectionLock, false);
		node = descriptor->m_newBodiesNodes;
		if (node) {
			descriptor->m_newBodiesNodes = node->GetNext();
		}
	}
*/
}


void dgBroadPhase::SubmitPairs(dgBroadPhaseNode* const leafNode, dgBroadPhaseNode* const node, dgFloat32 timestep, dgInt32 threadCount, dgInt32 threadID)
{
	dgBroadPhaseNode* pool[DG_BROADPHASE_MAX_STACK_DEPTH];
	pool[0] = node;
	dgInt32 stack = 1;

	dgAssert (leafNode->IsLeafNode());
	dgBody* const body0 = leafNode->GetBody();
	const dgVector boxP0 (body0 ? body0->m_minAABB : leafNode->m_minBox);
	const dgVector boxP1 (body0 ? body0->m_maxAABB : leafNode->m_maxBox);

	const bool test0 = body0 ? (body0->GetInvMass().m_w != dgFloat32(0.0f)) : true;
	while (stack) {
		stack--;
		dgBroadPhaseNode* const rootNode = pool[stack];
		if (dgOverlapTest(rootNode->m_minBox, rootNode->m_maxBox, boxP0, boxP1)) {
			if (rootNode->IsLeafNode()) {
				dgAssert(!rootNode->GetRight());
				dgAssert(!rootNode->GetLeft());
				dgBody* const body1 = rootNode->GetBody();
				if (body0) {
					if (body1) {
						if (test0 || (body1->GetInvMass().m_w != dgFloat32(0.0f))) {
							AddPair(body0, body1, timestep, threadID);
						}
					} else {
						dgAssert (rootNode->IsAggregate());
						dgBroadPhaseAggregate* const aggregate = (dgBroadPhaseAggregate*) rootNode;
						aggregate->SummitPairs(body0, timestep, threadID);
					}
				} else {
					dgAssert (leafNode->IsAggregate());
					dgBroadPhaseAggregate* const aggregate = (dgBroadPhaseAggregate*) leafNode;
					if (body1) {
						aggregate->SummitPairs(body1, timestep, threadID);
					} else {
						dgAssert (rootNode->IsAggregate());
						aggregate->SummitPairs((dgBroadPhaseAggregate*) rootNode, timestep, threadID);
					}
				}
			} else {
				dgBroadPhaseTreeNode* const tmpNode = (dgBroadPhaseTreeNode*) rootNode;
				dgAssert (tmpNode->m_left);
				dgAssert (tmpNode->m_right);

				pool[stack] = tmpNode->m_left;
				stack++;
				dgAssert(stack < dgInt32(sizeof (pool) / sizeof (pool[0])));

				pool[stack] = tmpNode->m_right;
				stack++;
				dgAssert(stack < dgInt32(sizeof (pool) / sizeof (pool[0])));
			}
		}
	}
}


void dgBroadPhase::ImproveNodeFitness(dgBroadPhaseTreeNode* const node, dgBroadPhaseNode** const root)
{
	dgAssert(node->GetLeft());
	dgAssert(node->GetRight());

	dgBroadPhaseNode* const parent = node->m_parent;
	if (parent && parent->m_parent) {
		dgAssert (!parent->IsLeafNode());
		if (parent->GetLeft() == node) {
			RotateRight(node, root);
		} else {
			RotateLeft(node, root);
		}
	}
	dgAssert(!m_rootNode->m_parent);
}

dgFloat64 dgBroadPhase::CalculateEntropy (dgFitnessList& fitness, dgBroadPhaseNode** const root)
{
	dgFloat64 cost0 = fitness.TotalCost();
	dgFloat64 cost1 = cost0;
	do {
		cost0 = cost1;
		for (dgFitnessList::dgListNode* node = fitness.GetFirst(); node; node = node->GetNext()) {
			ImproveNodeFitness(node->GetInfo(), root);
		}
		cost1 = fitness.TotalCost();
	} while (cost1 < (dgFloat32(0.99f)) * cost0);
	return cost1;
}


void dgBroadPhase::KinematicBodyActivation (dgContact* const contatJoint) const
{
	dgBody* const body0 = contatJoint->GetBody0();
	dgBody* const body1 = contatJoint->GetBody1();
	if (body0->IsCollidable() | body1->IsCollidable()) {
		if (body0->IsRTTIType(dgBody::m_kinematicBodyRTTI)) {
			if (body1->IsRTTIType(dgBody::m_dynamicBodyRTTI) && (body1->GetInvMass().m_w > dgFloat32 (0.0f))) {
				if (body1->m_equilibrium) {
					dgVector relVeloc (body0->m_veloc - body1->m_veloc);
					dgVector relOmega (body0->m_omega - body1->m_omega);
					dgVector mask2 ((relVeloc.DotProduct4(relVeloc) < dgDynamicBody::m_equilibriumError2) & (relOmega.DotProduct4(relOmega) < dgDynamicBody::m_equilibriumError2));

					dgThreadHiveScopeLock lock (m_world, &body1->m_criticalSectionLock, false);
					body1->m_sleeping = false;
					body1->m_equilibrium = mask2.GetSignMask() ? true : false;
				}
			}
		} else if (body1->IsRTTIType(dgBody::m_kinematicBodyRTTI)) {
			if (body0->IsRTTIType(dgBody::m_dynamicBodyRTTI) && (body0->GetInvMass().m_w > dgFloat32 (0.0f))) {
				if (body0->m_equilibrium) {
					dgVector relVeloc (body0->m_veloc - body1->m_veloc);
					dgVector relOmega (body0->m_omega - body1->m_omega);
					dgVector mask2 ((relVeloc.DotProduct4(relVeloc) < dgDynamicBody::m_equilibriumError2) & (relOmega.DotProduct4(relOmega) < dgDynamicBody::m_equilibriumError2));

					dgThreadHiveScopeLock lock (m_world, &body0->m_criticalSectionLock, false);
					body0->m_sleeping = false;
					body0->m_equilibrium = mask2.GetSignMask() ? true : false;
				}
			}
		}
	}
}


void dgBroadPhase::CollidingPairsKernel(void* const context, void* const node, dgInt32 threadID)
{
	dTimeTrackerEvent(__FUNCTION__);
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*)context;
	dgWorld* const world = descriptor->m_world;
	dgBroadPhase* const broadPhase = world->GetBroadPhase();
	if (broadPhase->m_scanTwoWays) {
		broadPhase->FindCollidingPairsForwardAndBackward(descriptor, (dgList<dgBroadPhaseNode*>::dgListNode*) node, threadID);
	} else {
		broadPhase->FindCollidingPairsForward(descriptor, (dgList<dgBroadPhaseNode*>::dgListNode*) node, threadID);
	}
}


void dgBroadPhase::AddGeneratedBodiesContactsKernel (void* const context, void* const worldContext, dgInt32 threadID)
{
	dTimeTrackerEvent(__FUNCTION__);
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*) context;
	dgWorld* const world = (dgWorld*) worldContext;
	dgBroadPhase* const broadPhase = world->GetBroadPhase();
	broadPhase->FindGeneratedBodiesCollidingPairs (descriptor, threadID);
}


void dgBroadPhase::ScanForContactJoints(dgBroadphaseSyncDescriptor& syncPoints)
{
	dTimeTrackerEvent(__FUNCTION__);
	dgInt32 threadsCount = m_world->GetThreadCount();
	dgList<dgBroadPhaseNode*>::dgListNode* node = m_updateList.GetFirst();
	for (dgInt32 i = 0; i < threadsCount; i++) {
		m_world->QueueJob(CollidingPairsKernel, &syncPoints, node);
		node = node ? node->GetNext() : NULL;
	}
	m_world->SynchronizationBarrier();

	const dgUnsigned32 lru = m_lru - DG_CONTACT_DELAY_FRAMES;
	dgActiveContacts* const contactList = m_world;
	for (dgActiveContacts::dgListNode* contactNode = contactList->GetFirst(); contactNode;) {
		dgContact* const contact = contactNode->GetInfo();
		contactNode = contactNode->GetNext();
		const dgBody* const body0 = contact->GetBody0();
		const dgBody* const body1 = contact->GetBody1();
		const dgInt32 equilbriun0 = body0->m_equilibrium;
		const dgInt32 equilbriun1 = body1->m_equilibrium;
		if (equilbriun0 & equilbriun1) {
			contact->m_broadphaseLru = lru;
		}
		if (contact->m_broadphaseLru < lru) {
			m_world->DestroyConstraint(contact);
		}
	}


#if 0
static dgInt32 xxx;
dgInt32 xxx0 = 0;
dgInt32 xxx1 = 0;
	for (dgList<dgBroadPhaseNode*>::dgListNode* nodePtr = m_updateList.GetFirst(); nodePtr; nodePtr = nodePtr->GetNext()) {
xxx0 ++;
		dgBroadPhaseNode* const node = nodePtr->GetInfo();
		dgAssert (node->IsLeafNode());
		dgBroadPhaseBodyNode* const bodyNode = (dgBroadPhaseBodyNode*)node;
xxx1 += bodyNode->m_nodeIsDirtyLru == (m_lru + 1) ? 1 : 0;
	}
dgTrace (("%d %d %d\n", xxx, xxx0, xxx1));
xxx ++;
#endif

}

void dgBroadPhase::UpdateSoftBodyContactKernel(void* const context, void* const worldContext, dgInt32 threadID)
{
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*)context;
	dgWorld* const world = descriptor->m_world;
	dgBroadPhase* const broadPhase = world->GetBroadPhase();
	broadPhase->UpdateSoftBodyContacts(descriptor, descriptor->m_timestep, threadID);
}

void dgBroadPhase::UpdateRigidBodyContactKernel(void* const context, void* const node, dgInt32 threadID)
{
	dgBroadphaseSyncDescriptor* const descriptor = (dgBroadphaseSyncDescriptor*)context;
	dgWorld* const world = descriptor->m_world;
	dgBroadPhase* const broadPhase = world->GetBroadPhase();
	broadPhase->UpdateRigidBodyContacts(descriptor, (dgActiveContacts::dgListNode*) node, descriptor->m_timestep, threadID);
}

void dgBroadPhase::UpdateSoftBodyContacts(dgBroadphaseSyncDescriptor* const descriptor, dgFloat32 timeStep, dgInt32 threadID)
{
	const dgInt32 count = m_pendingSoftBodyPairsCount;
	for (dgInt32 i = dgAtomicExchangeAndAdd(&descriptor->m_pairsAtomicCounter, 1); i < count; i = dgAtomicExchangeAndAdd(&descriptor->m_pairsAtomicCounter, 1)) {
		dgPendingCollisionSofBodies& pair = m_pendingSoftBodyCollisions[i];
		if (pair.m_body0->m_collision->IsType(dgCollision::dgCollisionLumpedMass_RTTI)) {
			dgCollisionLumpedMassParticles* const lumpedMassShape = (dgCollisionLumpedMassParticles*)pair.m_body0->m_collision->GetChildShape();
			dgAssert(pair.m_body0->IsRTTIType(dgBody::m_dynamicBodyRTTI));
			dgAssert (pair.m_body0 == lumpedMassShape->GetOwner ());
			lumpedMassShape->RegisterCollision(pair.m_body1);
		} else if (pair.m_body1->m_collision->IsType(dgCollision::dgCollisionLumpedMass_RTTI)) {
			dgCollisionLumpedMassParticles* const lumpedMassShape = (dgCollisionLumpedMassParticles*)pair.m_body1->m_collision->GetChildShape();
			dgAssert(pair.m_body1->IsRTTIType(dgBody::m_dynamicBodyRTTI));
			dgAssert (pair.m_body1 == lumpedMassShape->GetOwner ());
			lumpedMassShape->RegisterCollision(pair.m_body0);
		}
	}
}

void dgBroadPhase::UpdateRigidBodyContacts(dgBroadphaseSyncDescriptor* const descriptor, dgActiveContacts::dgListNode* const nodePtr, dgFloat32 timeStep, dgInt32 threadID)
{
	dTimeTrackerEvent(__FUNCTION__);

	dgActiveContacts::dgListNode* node = nodePtr;
	const dgFloat32 timestep = descriptor->m_timestep;
	const dgInt32 threadCount = descriptor->m_world->GetThreadCount();
	while (node) {
		dgContact* const contact = node->GetInfo();

		const dgBody* const body0 = contact->GetBody0();
		const dgBody* const body1 = contact->GetBody1();
		if (!(body0->m_equilibrium & body1->m_equilibrium)) {
			if (ValidateContactCache(contact, timestep)) {
				contact->m_timeOfImpact = dgFloat32(1.0e10f);
			} else {
				contact->m_contactActive = 0;
				contact->m_positAcc = dgVector::m_zero;
				contact->m_rotationAcc = dgQuaternion();

				dgFloat32 distance = contact->m_separationDistance;
				if (distance >= DG_NARROW_PHASE_DIST) {
					const dgVector veloc0 (body0->GetVelocity());
					const dgVector veloc1 (body1->GetVelocity());
					const dgVector omega0 (body0->GetOmega());
					const dgVector omega1 (body1->GetOmega());
					const dgCollisionInstance* const collision0 = body0->GetCollision();
					const dgCollisionInstance* const collision1 = body1->GetCollision();
					const dgFloat32 maxDiameter0 = dgFloat32 (3.5f) * collision0->GetBoxMaxRadius(); 
					const dgFloat32 maxDiameter1 = dgFloat32 (3.5f) * collision1->GetBoxMaxRadius(); 

					const dgVector velocLinear (veloc1 - veloc0);
					const dgFloat32 velocAngular0 = dgSqrt((omega0.DotProduct4(omega0)).GetScalar()) * maxDiameter0;
					const dgFloat32 velocAngular1 = dgSqrt((omega1.DotProduct4(omega1)).GetScalar()) * maxDiameter1;
					const dgFloat32 speed = dgSqrt ((velocLinear.DotProduct4(velocLinear)).GetScalar()) + velocAngular1 + velocAngular0 + dgFloat32 (0.5f);
					distance -= speed * timestep;
					contact->m_separationDistance = distance;
				}
				if (distance < DG_NARROW_PHASE_DIST) {
					AddPair(contact, timestep, threadID);
					if (contact->m_maxDOF) {
						contact->m_timeOfImpact = dgFloat32(1.0e10f);
					}
				}
			}
		}

		for (dgInt32 i = 0; i < threadCount; i++) {
			node = node ? node->GetNext() : NULL;
		}
	}
}


void dgBroadPhase::UpdateContacts(dgFloat32 timestep)
{
	dTimeTrackerEvent(__FUNCTION__);
	const dgInt32 lastDirtyCount = m_dirtyNodesCount;
    m_lru = m_lru + 1;
	m_pendingSoftBodyPairsCount = 0;
	m_dirtyNodesCount = 0;

	m_recursiveChunks = true;
	dgInt32 threadsCount = m_world->GetThreadCount();

	const dgBodyMasterList* const masterList = m_world;
	dgBroadphaseSyncDescriptor syncPoints(timestep, m_world);

	dgBodyMasterList::dgListNode* node = masterList->GetLast();
	for (dgInt32 i = 0; i < threadsCount; i++) {
		m_world->QueueJob(ForceAndToqueKernel, &syncPoints, node);
		node = node ? node->GetPrev() : NULL;
	}
	m_world->SynchronizationBarrier();

	// update pre-listeners after the force and true are applied
	if (m_world->m_preListener.GetCount()) {
		dTimeTrackerEvent("preListeners");
		for (dgWorld::dgListenerList::dgListNode* node1 = m_world->m_preListener.GetFirst(); node1; node1 = node1->GetNext()) {
			dgWorld::dgListener& listener = node1->GetInfo();
			listener.m_onListenerUpdate(m_world, listener.m_userData, timestep);
		}
	}

	node = masterList->GetLast();
	for (dgInt32 i = 0; i < threadsCount; i++) {
		m_world->QueueJob(SleepingStateKernel, &syncPoints, node);
		node = node ? node->GetPrev() : NULL;
	}
	m_world->SynchronizationBarrier();


#if 0
	static dgInt32 xxx;
	xxx ++;
	for (dgBodyMasterList::dgListNode* node = masterList->GetLast(); node; node = node->GetPrev()) {
		dgDynamicBody* const body = (dgDynamicBody*)node->GetInfo().GetBody();
		if ((body->GetType() == dgBody::m_dynamicBody) && (body->GetInvMass().m_w > dgFloat32 (0.0f))) {
			#if 0
				static FILE* file = fopen("replay.bin", "wb");
				if (file) {
					dgInt32 sleepState = body->GetSleepState();
					fwrite(&body->m_accel, sizeof (dgVector), 1, file);
					fwrite(&body->m_alpha, sizeof (dgVector), 1, file);
					fwrite(&body->m_veloc, sizeof (dgVector), 1, file);
					fwrite(&body->m_omega, sizeof (dgVector), 1, file);
					fwrite(&body->m_externalForce, sizeof (dgVector), 1, file);
					fwrite(&body->m_externalTorque, sizeof (dgVector), 1, file);
					fwrite(&sleepState, sizeof (dgInt32), 1, file);
					//dgTrace (("%d f(%f %f %f) v(%f %f %f) f(%f %f %f)\n", xxx, body->m_accel.m_x, body->m_accel.m_y, body->m_accel.m_z, body->m_veloc.m_x, body->m_veloc.m_y, body->m_veloc.m_z, body->m_externalForce.m_x, body->m_externalForce.m_y, body->m_externalForce.m_z));
					fflush(file);
				}
			#else 
				static FILE* file = fopen("replay.bin", "rb");
				if (file) {
					fread(&body->m_accel, sizeof (dgVector), 1, file);
					fread(&body->m_alpha, sizeof (dgVector), 1, file);
					fread(&body->m_veloc, sizeof (dgVector), 1, file);
					fread(&body->m_omega, sizeof (dgVector), 1, file);
					fread(&body->m_externalForce, sizeof (dgVector), 1, file);
					fread(&body->m_externalTorque, sizeof (dgVector), 1, file);
					dgInt32 sleepState;
					fread(&sleepState, sizeof (dgInt32), 1, file);
					body->SetSleepState (sleepState ? true : false);
					//dgTrace (("%d f(%f %f %f) v(%f %f %f) f(%f %f %f)\n", xxx, body->m_accel.m_x, body->m_accel.m_y, body->m_accel.m_z, body->m_veloc.m_x, body->m_veloc.m_y, body->m_veloc.m_z, body->m_externalForce.m_x, body->m_externalForce.m_y, body->m_externalForce.m_z));
				}
			#endif
		}
	}
#endif


	dgList<dgBroadPhaseAggregate*>::dgListNode* aggregateNode = m_aggregateList.GetFirst();
	for (dgInt32 i = 0; i < threadsCount; i++) {
		m_world->QueueJob (UpdateAggregateEntropyKernel, &syncPoints, aggregateNode);
		aggregateNode = aggregateNode ? aggregateNode->GetNext() : NULL;
	}
	m_world->SynchronizationBarrier();
	UpdateFitness();

	m_scanTwoWays = (lastDirtyCount * 100) < (40 * m_updateList.GetCount());
	ScanForContactJoints (syncPoints);

	dgActiveContacts* const contactList = m_world;
	dgActiveContacts::dgListNode* contactListNode = contactList->GetFirst();
	for (dgInt32 i = 0; i < threadsCount; i++) {
		m_world->QueueJob(UpdateRigidBodyContactKernel, &syncPoints, contactListNode);
		contactListNode = contactListNode ? contactListNode->GetNext() : NULL;
	}
	m_world->SynchronizationBarrier();

	if (m_pendingSoftBodyPairsCount) {
		for (dgInt32 i = 0; i < threadsCount; i++) {
			m_world->QueueJob(UpdateSoftBodyContactKernel, &syncPoints, contactListNode);
		}
		m_world->SynchronizationBarrier();
	}


	m_recursiveChunks = false;
	if (m_generatedBodies.GetCount()) {
        dgAssert (0);
    /*
		syncPoints.m_newBodiesNodes = m_generatedBodies.GetFirst();
		for (dgInt32 i = 0; i < threadsCount; i++) {
			m_world->QueueJob(AddGeneratedBodiesContactsKernel, &syncPoints, m_world);
		}
		m_world->SynchronizationBarrier();

		for (dgInt32 i = 0; i < threadsCount; i++) {
			m_world->QueueJob(UpdateContactsKernel, &syncPoints, m_world);
		}
		m_world->SynchronizationBarrier();

		m_generatedBodies.RemoveAll();
*/
	}
}
