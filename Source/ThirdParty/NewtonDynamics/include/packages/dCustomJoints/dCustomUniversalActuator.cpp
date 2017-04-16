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


// dCustomUniversal.cpp: implementation of the dCustomUniversal class.
//
//////////////////////////////////////////////////////////////////////
#include "dCustomJointLibraryStdAfx.h"
#include "dCustomUniversalActuator.h"


dCustomUniversalActuator::dCustomUniversalActuator (const dMatrix& pinAndPivotFrame, dFloat angularRate0, dFloat minAngle0, dFloat maxAngle0, dFloat angularRate1, dFloat minAngle1, dFloat maxAngle1, NewtonBody* const child, NewtonBody* const parent)
	:dCustomUniversal(pinAndPivotFrame, child, parent)
	,m_angle0(0.0f)
	,m_angularRate0(angularRate0)
    ,m_maxForce0(D_CUSTOM_LARGE_VALUE)
	,m_angle1(0.0f)
	,m_angularRate1(angularRate1)
    ,m_maxForce1(D_CUSTOM_LARGE_VALUE)
{
	EnableLimit_0(false);
	EnableLimit_1(false);

	m_actuator_0 = true;
	m_actuator_1 = true;
	m_minAngle_0 = minAngle0;
	m_maxAngle_0 = maxAngle0;
	m_minAngle_1 = minAngle1;
	m_maxAngle_1 = maxAngle1;
}


dCustomUniversalActuator::dCustomUniversalActuator(const dMatrix& pinAndPivotFrame, NewtonBody* const child, NewtonBody* const parent)
	:dCustomUniversal(pinAndPivotFrame, child, parent)
	,m_angle0(0.0f)
	,m_angularRate0(0.0f)
	,m_maxForce0(D_CUSTOM_LARGE_VALUE)
	,m_angle1(0.0f)
	,m_angularRate1(0.0f)
	,m_maxForce1(D_CUSTOM_LARGE_VALUE)
{
	EnableLimit_0(false);
	EnableLimit_1(false);

	m_actuator_0 = true;
	m_actuator_1 = true;
	m_minAngle_0 = 0.0f;
	m_maxAngle_0 = 0.0f;
	m_minAngle_1 = 0.0f;
	m_maxAngle_1 = 0.0f;

}


dCustomUniversalActuator::dCustomUniversalActuator(NewtonBody* const child, NewtonBody* const parent, NewtonDeserializeCallback callback, void* const userData)
	:dCustomUniversal(child, parent, callback, userData)
{
	callback(userData, &m_angle0, sizeof(dFloat));
	callback(userData, &m_maxForce0, sizeof(dFloat));
	callback(userData, &m_angularRate0, sizeof(dFloat));

	callback(userData, &m_angle1, sizeof(dFloat));
	callback(userData, &m_maxForce1, sizeof(dFloat));
	callback(userData, &m_angularRate1, sizeof(dFloat));
}

dCustomUniversalActuator::~dCustomUniversalActuator()
{
}

void dCustomUniversalActuator::Serialize(NewtonSerializeCallback callback, void* const userData) const 
{ 
	dCustomUniversal::Serialize(callback, userData);

	callback(userData, &m_angle0, sizeof(dFloat));
	callback(userData, &m_maxForce0, sizeof(dFloat));
	callback(userData, &m_angularRate0, sizeof(dFloat));

	callback(userData, &m_angle1, sizeof(dFloat));
	callback(userData, &m_maxForce1, sizeof(dFloat));
	callback(userData, &m_angularRate1, sizeof(dFloat));
}

bool dCustomUniversalActuator::GetEnableFlag0 () const
{
	return m_actuator_0;
}

dFloat dCustomUniversalActuator::GetTargetAngle0() const
{
	return m_angle0;
}

dFloat dCustomUniversalActuator::GetAngularRate0() const
{
	return m_angularRate0;
}


bool dCustomUniversalActuator::GetEnableFlag1 () const
{
	return m_actuator_1;
}

dFloat dCustomUniversalActuator::GetTargetAngle1() const
{
	return m_angle1;
}

dFloat dCustomUniversalActuator::GetAngularRate1() const
{
	return m_angularRate1;
}


void dCustomUniversalActuator::SetEnableFlag0 (bool flag)
{
	m_actuator_0 = flag;
}

void dCustomUniversalActuator::SetTargetAngle0(dFloat angle)
{
	m_angle0 = dClamp (angle, m_minAngle_0, m_maxAngle_0);
}


dFloat dCustomUniversalActuator::GetMaxTorquePower0() const
{
    return m_maxForce0;
}

void dCustomUniversalActuator::SetMaxTorquePower0(dFloat force)
{
    m_maxForce0 = dAbs (force);
}


void dCustomUniversalActuator::SetAngularRate0(dFloat rate)
{
	m_angularRate0 = rate;
}

void dCustomUniversalActuator::SetEnableFlag1 (bool flag)
{
	m_actuator_1 = flag;
}

void dCustomUniversalActuator::SetTargetAngle1(dFloat angle)
{
	m_angle1 = dClamp (angle, m_minAngle_1, m_maxAngle_1);
}


void dCustomUniversalActuator::SetAngularRate1(dFloat rate)
{
	m_angularRate1 = rate;
}

dFloat dCustomUniversalActuator::GetMaxTorquePower1() const
{
    return m_maxForce1;
}

void dCustomUniversalActuator::SetMaxTorquePower1(dFloat force)
{
    m_maxForce1 = dAbs (force);
}

void dCustomUniversalActuator::GetInfo (NewtonJointRecord* const info) const
{
	dAssert (0);
}

void dCustomUniversalActuator::SubmitConstraints (dFloat timestep, int threadIndex)
{
	dCustomUniversal::SubmitConstraints (timestep, threadIndex);

	if (m_actuator_0 | m_actuator_1){
		dMatrix matrix0;
		dMatrix matrix1;

		CalculateGlobalMatrix (matrix0, matrix1);
		if (m_actuator_0) {
			dFloat jointAngle = GetJointAngle_0();
			dFloat relAngle = jointAngle - m_angle0;

			dFloat currentSpeed = GetJointOmega_0();
			dFloat step = dFloat(2.0f) * m_angularRate0 * timestep;
			dFloat desiredSpeed = (dAbs(relAngle) > dAbs(step)) ? dSign(relAngle) * m_angularRate0 : dFloat(0.1f) * relAngle / timestep;
			dFloat accel = (desiredSpeed - currentSpeed) / timestep;

			NewtonUserJointAddAngularRow(m_joint, relAngle, &matrix0.m_front[0]);
			NewtonUserJointSetRowAcceleration(m_joint, accel);
            NewtonUserJointSetRowMinimumFriction (m_joint, -m_maxForce0);
            NewtonUserJointSetRowMaximumFriction (m_joint,  m_maxForce0);
			NewtonUserJointSetRowStiffness (m_joint, 1.0f);
		}

		if (m_actuator_1) {
			dFloat jointAngle = GetJointAngle_1();
			dFloat relAngle = jointAngle - m_angle1;

			dFloat currentSpeed = GetJointOmega_1();
			dFloat step = dFloat(2.0f) * m_angularRate1 * timestep;
			dFloat desiredSpeed = (dAbs(relAngle) > dAbs(step)) ? dSign(relAngle) * m_angularRate1 : dFloat(0.1f) * relAngle / timestep;
			dFloat accel = (desiredSpeed - currentSpeed) / timestep;

			NewtonUserJointAddAngularRow(m_joint, relAngle, &matrix1.m_up[0]);
			NewtonUserJointSetRowAcceleration(m_joint, accel);

            NewtonUserJointSetRowMinimumFriction (m_joint, -m_maxForce1);
            NewtonUserJointSetRowMaximumFriction (m_joint,  m_maxForce1);
			NewtonUserJointSetRowStiffness (m_joint, 1.0f);
		}
	}
}


