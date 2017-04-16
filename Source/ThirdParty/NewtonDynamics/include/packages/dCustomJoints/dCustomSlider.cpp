/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/



// dCustomSlider.cpp: implementation of the dCustomSlider class.
//
//////////////////////////////////////////////////////////////////////
#include "dCustomJointLibraryStdAfx.h"
#include "dCustomSlider.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//dInitRtti(dCustomSlider);
IMPLEMENT_CUSTOM_JOINT(dCustomSlider);

dCustomSlider::dCustomSlider (const dMatrix& pinAndPivotFrame, NewtonBody* const child, NewtonBody* const parent)
	:dCustomJoint(6, child, parent)
	,m_speed(0.0f)
	,m_posit(0.0f)
	,m_minDist(-1.0f)
	,m_maxDist(1.0f)
	,m_spring(0.0f)
	,m_damper(0.0f)
	,m_springDamperRelaxation(0.6f)
	,m_limitsOn(false)
	,m_setAsSpringDamper(false)
	,m_lastRowWasUsed(false)
{
	// calculate the two local matrix of the pivot point
	CalculateLocalMatrix (pinAndPivotFrame, m_localMatrix0, m_localMatrix1);
}


dCustomSlider::dCustomSlider (NewtonBody* const child, NewtonBody* const parent, NewtonDeserializeCallback callback, void* const userData)
	:dCustomJoint(child, parent, callback, userData)
{
	callback (userData, &m_speed, sizeof (dFloat));
	callback (userData, &m_posit, sizeof (dFloat));
	callback (userData, &m_minDist, sizeof (dFloat));
	callback (userData, &m_maxDist, sizeof (dFloat));
	callback (userData, &m_spring, sizeof (dFloat));
	callback (userData, &m_damper, sizeof (dFloat));
	callback (userData, &m_springDamperRelaxation, sizeof (dFloat));
	callback (userData, &m_flags, sizeof (int));
}

dCustomSlider::~dCustomSlider()
{
}

void dCustomSlider::Serialize (NewtonSerializeCallback callback, void* const userData) const
{
	dCustomJoint::Serialize (callback, userData);

	callback (userData, &m_speed, sizeof (dFloat));
	callback (userData, &m_posit, sizeof (dFloat));
	callback (userData, &m_minDist, sizeof (dFloat));
	callback (userData, &m_maxDist, sizeof (dFloat));
	callback (userData, &m_spring, sizeof (dFloat));
	callback (userData, &m_damper, sizeof (dFloat));
	callback (userData, &m_springDamperRelaxation, sizeof (dFloat));
	callback (userData, &m_flags, sizeof (int));
}


void dCustomSlider::EnableLimits(bool state)
{
	m_limitsOn = state;
}

void dCustomSlider::SetLimits(dFloat minDist, dFloat maxDist)
{
	m_minDist = minDist;
	m_maxDist = maxDist;
}

void dCustomSlider::SetAsSpringDamper(bool state, dFloat springDamperRelaxation, dFloat spring, dFloat damper)
{
	m_spring = spring;
	m_damper = damper;
	m_setAsSpringDamper = state;
	m_springDamperRelaxation = dClamp(springDamperRelaxation, dFloat(0.0f), dFloat(0.99f));
}

dFloat dCustomSlider::GetJointPosit () const
{
	return m_posit;
}

dFloat dCustomSlider::GetJointSpeed () const
{
	return m_speed;
}

void dCustomSlider::GetInfo(NewtonJointRecord* const info) const
{
	strcpy(info->m_descriptionType, "slider");

	info->m_attachBody_0 = m_body0;
	info->m_attachBody_1 = m_body1;

	if (m_limitsOn) {
		dFloat dist;
		dMatrix matrix0;
		dMatrix matrix1;

		// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
		CalculateGlobalMatrix(matrix0, matrix1);
		dist = (matrix0.m_posit - matrix1.m_posit).DotProduct3(matrix0.m_front);

		info->m_minLinearDof[0] = m_minDist - dist;
		info->m_maxLinearDof[0] = m_maxDist - dist;
	} else {
		info->m_minLinearDof[0] = -D_CUSTOM_LARGE_VALUE;
		info->m_maxLinearDof[0] = D_CUSTOM_LARGE_VALUE;
	}


	info->m_minLinearDof[1] = 0.0f;
	info->m_maxLinearDof[1] = 0.0f;;

	info->m_minLinearDof[2] = 0.0f;
	info->m_maxLinearDof[2] = 0.0f;

	info->m_minAngularDof[0] = 0.0f;
	info->m_maxAngularDof[0] = 0.0f;

	info->m_minAngularDof[1] = 0.0f;
	info->m_maxAngularDof[1] = 0.0f;

	info->m_minAngularDof[2] = 0.0f;
	info->m_maxAngularDof[2] = 0.0f;

	memcpy(info->m_attachmenMatrix_0, &m_localMatrix0, sizeof (dMatrix));
	memcpy(info->m_attachmenMatrix_1, &m_localMatrix1, sizeof (dMatrix));
}


void dCustomSlider::SubmitConstraints (dFloat timestep, int threadIndex)
{
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
	CalculateGlobalMatrix (matrix0, matrix1);

	// Restrict the movement on the pivot point along all two orthonormal axis direction perpendicular to the motion
	dVector p0(matrix0.m_posit);
	dVector p1(matrix1.m_posit + matrix1.m_front.Scale((p0 - matrix1.m_posit).DotProduct3(matrix1.m_front)));
	NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix1.m_up[0]);
	NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix1.m_right[0]);

 	// three rows to restrict rotation around around the parent coordinate system
	NewtonUserJointAddAngularRow(m_joint, CalculateAngle (matrix0.m_up, matrix1.m_up, matrix1.m_front), &matrix1.m_front[0]);
	NewtonUserJointAddAngularRow(m_joint, CalculateAngle (matrix0.m_front, matrix1.m_front, matrix1.m_up), &matrix1.m_up[0]);
	NewtonUserJointAddAngularRow(m_joint, CalculateAngle (matrix0.m_front, matrix1.m_front, matrix1.m_right), &matrix1.m_right[0]);

	// calculate position and speed	
	dVector veloc0(0.0f); 
	dVector veloc1(0.0f);  
	dAssert (m_body0);
	NewtonBodyGetPointVelocity(m_body0, &matrix0.m_posit[0], &veloc0[0]);
	if (m_body1) {
		NewtonBodyGetPointVelocity(m_body1, &matrix1.m_posit[0], &veloc1[0]);
	}
	m_posit = (matrix0.m_posit - matrix1.m_posit).DotProduct3(matrix1.m_front);
	m_speed = (veloc0 - veloc1).DotProduct3(matrix1.m_front);

	m_lastRowWasUsed = false;
	SubmitConstraintsFreeDof (timestep, matrix0, matrix1);
 }

void dCustomSlider::SubmitConstraintsFreeDof(dFloat timestep, const dMatrix& matrix0, const dMatrix& matrix1)
{
	// if limit are enable ...
	if (m_limitsOn && m_setAsSpringDamper) {
		m_lastRowWasUsed = true;
		if (m_posit < m_minDist) {
			const dVector& p0 = matrix0.m_posit;
			dVector p1 (p0 + matrix0.m_front.Scale (m_minDist - m_posit));
			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_front[0]);
			dFloat accel = NewtonUserJointGetRowAcceleration(m_joint) + NewtonCalculateSpringDamperAcceleration(timestep, m_spring, m_posit, m_damper, m_speed);
			NewtonUserJointSetRowAcceleration (m_joint, accel);
			NewtonUserJointSetRowMinimumFriction (m_joint, 0.0f);

		} else if (m_posit > m_maxDist) {
			const dVector& p0 = matrix0.m_posit;
			dVector p1 (p0 + matrix0.m_front.Scale (m_maxDist - m_posit));
			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_front[0]);
			dFloat accel = NewtonUserJointGetRowAcceleration(m_joint) + NewtonCalculateSpringDamperAcceleration(timestep, m_spring, m_posit, m_damper, m_speed);
			NewtonUserJointSetRowAcceleration (m_joint, accel);
			NewtonUserJointSetRowMaximumFriction (m_joint, 0.0f);
		} else {
			const dVector& p0 = matrix0.m_posit;
			const dVector& p1 = matrix1.m_posit;
			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_front[0]);
			NewtonUserJointSetRowSpringDamperAcceleration(m_joint, m_springDamperRelaxation, m_spring, m_damper);
		}

	} else if (m_limitsOn) {
		if (m_posit < m_minDist) {
			// get a point along the up vector and set a constraint  
			const dVector& p0 = matrix0.m_posit;
			dVector p1 (p0 + matrix0.m_front.Scale (m_minDist - m_posit));
			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_front[0]);
			// allow the object to return but not to kick going forward
			NewtonUserJointSetRowMinimumFriction (m_joint, 0.0f);
			m_lastRowWasUsed = true;
		} else if (m_posit > m_maxDist) {
			// get a point along the up vector and set a constraint  

			const dVector& p0 = matrix0.m_posit;
			dVector p1 (p0 + matrix0.m_front.Scale (m_maxDist - m_posit));
			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_front[0]);
			// allow the object to return but not to kick going forward
			NewtonUserJointSetRowMaximumFriction (m_joint, 0.0f);
			m_lastRowWasUsed = true;
		} else {
/*
			// uncomment this for a slider with friction

			// take any point on body0 (origin)
			const dVector& p0 = matrix0.m_posit;

			dVector veloc0(0.0f); 
			dVector veloc1(0.0f); 
			dVector omega1(0.0f); 

			NewtonBodyGetVelocity(m_body0, &veloc0[0]);
			NewtonBodyGetVelocity(m_body1, &veloc1[0]);
			NewtonBodyGetOmega(m_body1, &omega1[0]);

			// this assumes the origin of the bodies the matrix pivot are the same
			veloc1 += omega1 * (matrix1.m_posit - p0);

			dFloat relAccel; 
			relAccel = ((veloc1 - veloc0) % matrix0.m_front) / timestep;

			#define MaxFriction 10.0f
			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p0[0], &matrix0.m_front[0]);
			NewtonUserJointSetRowAcceleration (m_joint, relAccel);
			NewtonUserJointSetRowMinimumFriction (m_joint, -MaxFriction);
			NewtonUserJointSetRowMaximumFriction(m_joint, MaxFriction);
			m_lastRowWasUsed = false;
*/
		}
	} else if (m_setAsSpringDamper) {
		m_lastRowWasUsed = true;
		const dVector& p0 = matrix0.m_posit;
		const dVector& p1 = matrix1.m_posit;
		NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_front[0]);
		NewtonUserJointSetRowSpringDamperAcceleration(m_joint, m_springDamperRelaxation, m_spring, m_damper);
	} 
}