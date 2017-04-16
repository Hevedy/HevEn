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


// NewtonVehicleControllerManager.h: interface for the NewtonVehicleControllerManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef D_CUSTOM_VEHICLE_CONTROLLER_MANAGER_H_
#define D_CUSTOM_VEHICLE_CONTROLLER_MANAGER_H_

#include <dCustomJointLibraryStdAfx.h>
#include <dCustomAlloc.h>
#include <dCustomControllerManager.h>

#define VEHICLE_PLUGIN_NAME			"__vehicleManager__"


class dCustomVehicleController;

class dCustomVehicleController: public dCustomControllerBase
{
	public:
	class dAxelJoint;
	class dWheelJoint;
	class dEngineJoint;
	class dGearBoxJoint;
	class dEngineController;
	class dDifferentialJoint;
	class dSteeringController;
//	class dLateralDynamicsJoint;

	class dController: public dCustomAlloc
	{
		public:
		dController(dCustomVehicleController* const controller)
			:m_controller(controller)
			,m_param(0.0f)
			,m_paramMemory(0.0f)
			,m_timer(60)
		{
		}

		~dController()
		{
		}

		void SetParam(dFloat param)
		{
			m_paramMemory = m_param;
			m_param = param;
		}

		dFloat GetParam() const
		{
			return m_param;
		}

		bool ParamChanged() const
		{
			m_timer--;
			if (dAbs(m_paramMemory - m_param) > 1.e-3f) {
				m_timer = 30;
			}
			return m_timer > 0;
		}


		virtual void Update(dFloat timestep)
		{
		}

		dCustomVehicleController* m_controller;

		dFloat m_param;
		dFloat m_paramMemory;
		mutable dFloat m_timer;
	};

	class dBodyPart: public dCustomAlloc
	{
		public:
		dBodyPart()
			:m_parent(NULL)
			,m_body(NULL)
			,m_joint(NULL)
			,m_userData(NULL)
			,m_controller(NULL)
		{
		}
		virtual ~dBodyPart(){}

		dBodyPart* GetParent() const {return m_parent;}
		NewtonBody* GetBody() const	{return m_body;}
		dCustomJoint* GetJoint() const{return m_joint;}
		void* GetUserData() const {return m_userData;}
		dCustomVehicleController* GetController() const{return m_controller;}
		virtual void ProjectError() {}
		
		protected:
		dBodyPart* m_parent;
		NewtonBody* m_body;
		dCustomJoint* m_joint;
		
		void* m_userData;
		dCustomVehicleController* m_controller;
		friend class dCustomVehicleController;
	};

	class dBodyPartChassis: public dBodyPart
	{
		public:
		dBodyPartChassis ()
			:dBodyPart()
			,m_aerodynamicsDownForce0(0.0f)
			,m_aerodynamicsDownForce1(0.0f)
			,m_aerodynamicsDownSpeedCutOff(0.0f)
			,m_aerodynamicsDownForceCoefficient(0.0f)
		{
		}

		void Init(dCustomVehicleController* const controller, void* const userData)
		{
			m_joint = NULL;
			m_userData = userData;
			m_controller = controller;
			m_body = controller->GetBody();
		}

		CUSTOM_JOINTS_API void ApplyDownForce ();

		dFloat m_aerodynamicsDownForce0;
		dFloat m_aerodynamicsDownForce1;
		dFloat m_aerodynamicsDownSpeedCutOff;
		dFloat m_aerodynamicsDownForceCoefficient;
	};

	class dBodyPartEngine: public dBodyPart
	{
		public:
		dBodyPartEngine(dCustomVehicleController* const controller, dFloat mass, dFloat amatureRadius);
		~dBodyPartEngine();

		void ApplyTorque(dFloat torque);
		void ProjectError();
	};

	class dBodyPartDifferential: public dBodyPart
	{
		public:
		dBodyPartDifferential();
		void Init(dCustomVehicleController* const controller, dFloat mass, dFloat inertia);
		~dBodyPartDifferential();

		void SetTrackMode(bool mode, dFloat differentialSpeed);
		void ProjectError();
		dFloat m_differentialSpeed;
	};

	class dBodyPartTire: public dBodyPart
	{
		public:
		class Info
		{
			public:
			enum SuspensionType
			{
				m_offroad,
				m_confort,
				m_race,
				m_roller,
			};

			Info ()
			{
				memset (this, 0, sizeof (Info));
			}

			dVector m_location;
			dFloat m_mass;
			dFloat m_radio;
			dFloat m_width;
			dFloat m_pivotOffset;
			dFloat m_maxSteeringAngle;
			dFloat m_dampingRatio;
			dFloat m_springStrength;
			dFloat m_suspesionlenght;
			dFloat m_lateralStiffness;
			dFloat m_longitudialStiffness;
			dFloat m_aligningMomentTrail;
			int m_hasFender;
			SuspensionType m_suspentionType;
			void* m_userData;
		};

		class dFrictionModel
		{
			public:
			dFrictionModel(const dCustomVehicleController* const controller)
				:m_controller(controller)
			{
			}

			virtual dFloat GetFrictionCoefficient(const NewtonMaterial* const material, const NewtonBody* const tireBody, const NewtonBody* const otherBody) const
			{
				// the vehicle model is realistic, please do not use fake larger than one fiction coefficient, or you vehicle will simply role over
				return 1.0f;
			}

			virtual void CalculateTireFrictionCoefficents(const dBodyPartTire* const tire, const NewtonBody* const otherBody, const NewtonMaterial* const material,
				dFloat longitudinalSlip, dFloat lateralSlip, dFloat longitudinalStiffness, dFloat lateralStiffness,
				dFloat& longitudinalFrictionCoef, dFloat& lateralFrictionCoef, dFloat& aligningTorqueCoef) const;

			const dCustomVehicleController* m_controller;
		};

		CUSTOM_JOINTS_API dBodyPartTire();
		CUSTOM_JOINTS_API ~dBodyPartTire();

		CUSTOM_JOINTS_API dFloat GetRPM() const; 
		CUSTOM_JOINTS_API dFloat GetLateralSlip () const;
		CUSTOM_JOINTS_API dFloat GetLongitudinalSlip () const;

		CUSTOM_JOINTS_API Info GetInfo() const {return m_data;}
		CUSTOM_JOINTS_API void SetInfo(const Info& info) {};

		protected:
		void Init(dBodyPart* const parentPart, const dMatrix& locationInGlobalSpase, const Info& info);

		void ProjectError();
		void SetBrakeTorque(dFloat torque);
		void SetSteerAngle(dFloat angleParam, dFloat timestep);

		Info m_data;
		dFloat m_lateralSlip;
		dFloat m_longitudinalSlip;
		dFloat m_aligningTorque;
		int m_index;
		int m_collidingCount;
		NewtonWorldConvexCastReturnInfo m_contactInfo[4];
		friend class dWheelJoint;
		friend class dCustomVehicleController;
		friend class dCustomVehicleControllerManager;
	};

	class dEngineController: public dController
	{
		public:
		class dInfo
		{
			public:
			dInfo()
			{
				memset(this, 0, sizeof (dInfo));
			}

			dVector m_location;
			dFloat m_mass;
			dFloat m_radio;
			dFloat m_idleTorque;
			dFloat m_rpmAtIdleTorque;
			dFloat m_peakTorque;
			dFloat m_rpmAtPeakTorque;
			dFloat m_peakHorsePower;
			dFloat m_rpmAtPeakHorsePower;
			dFloat m_rpmAtRedLine;

			dFloat m_vehicleTopSpeed;
			dFloat m_reverseGearRatio;
			dFloat m_gearRatios[10];
			dFloat m_gearRatiosSign;
			dFloat m_clutchFrictionTorque;

			dFloat m_aerodynamicDownforceFactor; 
			dFloat m_aerodynamicDownforceFactorAtTopSpeed; 
			dFloat m_aerodynamicDownForceSurfaceCoeficident;

			int m_gearsCount;
			int m_differentialLock;
			void* m_userData;

			private:
			void ConvertToMetricSystem();

			dFloat m_crownGearRatio;
			dFloat m_idleFriction;
			dFloat m_peakPowerTorque;
			dFloat m_viscousDrag0;
			dFloat m_viscousDrag1;
			dFloat m_viscousDrag2;
			friend class dEngineController;
		};

		public:
		CUSTOM_JOINTS_API dEngineController(dCustomVehicleController* const controller, const dInfo& info, dBodyPartDifferential* const differential, dBodyPartTire* const crownGearCalculator);
		CUSTOM_JOINTS_API ~dEngineController();

		CUSTOM_JOINTS_API void ApplyTorque(dFloat torque);

		CUSTOM_JOINTS_API dInfo GetInfo() const;
		CUSTOM_JOINTS_API void SetInfo(const dInfo& info);

		CUSTOM_JOINTS_API dFloat GetRPM() const;
		CUSTOM_JOINTS_API dFloat GetIdleRPM() const;
		CUSTOM_JOINTS_API dFloat GetRedLineRPM() const;
		CUSTOM_JOINTS_API dFloat GetSpeed() const;
		CUSTOM_JOINTS_API dFloat GetTopSpeed() const;

		CUSTOM_JOINTS_API int GetGear() const;
		CUSTOM_JOINTS_API void SetGear(int gear);

		CUSTOM_JOINTS_API void SetIgnition(bool key);
		CUSTOM_JOINTS_API bool GetIgnition() const;

		CUSTOM_JOINTS_API int GetFirstGear() const;
		CUSTOM_JOINTS_API int GetLastGear() const;
		CUSTOM_JOINTS_API int GetNeutralGear() const;
		CUSTOM_JOINTS_API int GetReverseGear() const;

		CUSTOM_JOINTS_API bool GetTransmissionMode() const;
		CUSTOM_JOINTS_API void SetTransmissionMode(bool mode);

		CUSTOM_JOINTS_API bool GetDifferentialLock () const;
		CUSTOM_JOINTS_API void SetDifferentialLock (bool mode);
		CUSTOM_JOINTS_API void SetClutchParam (dFloat cluthParam);

		CUSTOM_JOINTS_API void PlotEngineCurve () const;

		protected:
		dFloat GetTopGear() const;
		dFloat GetRadiansPerSecond() const;
		void InitEngineTorqueCurve();
		void CalculateCrownGear();
		dFloat GetGearRatio () const;
		virtual void Update(dFloat timestep);
		void UpdateAutomaticGearBox(dFloat timestep, dFloat omega);

		dInfo m_info;
		dInfo m_infoCopy;
		dCustomVehicleController* m_controller;
		dBodyPartTire* m_crownGearCalculator;
		dBodyPartDifferential* m_differential;
		dGearBoxJoint* m_gearBoxJoint;
		dFloat m_clutchParam;
		int m_gearTimer;
		int m_currentGear;
		bool m_ignitionKey;
		bool m_automaticTransmissionMode;
		friend class dCustomVehicleController;
	};

	class dSteeringController: public dController
	{
		public:
		CUSTOM_JOINTS_API dSteeringController (dCustomVehicleController* const controller, dBodyPartDifferential* const differential);
		CUSTOM_JOINTS_API void AddTire (dBodyPartTire* const tire);
		
		protected:
		virtual void Update(dFloat timestep);
		dList<dBodyPartTire*> m_tires;
		dBodyPartDifferential* m_differential;
		bool m_isSleeping;
		friend class dCustomVehicleController;
	};

	class dBrakeController: public dController
	{
		public:
		CUSTOM_JOINTS_API dBrakeController (dCustomVehicleController* const controller, dFloat maxBrakeTorque);
		CUSTOM_JOINTS_API void AddTire (dBodyPartTire* const tire);

		protected:
		virtual void Update(dFloat timestep);

		dList<dBodyPartTire*> m_tires;
		dFloat m_maxTorque;
		friend class dCustomVehicleController;
	};


	CUSTOM_JOINTS_API void Finalize();
	CUSTOM_JOINTS_API dBodyPartTire* AddTire (const dBodyPartTire::Info& tireInfo);
	CUSTOM_JOINTS_API dBodyPartDifferential* AddDifferential(dBodyPartTire* const leftTire, dBodyPartTire* const rightTire);
	CUSTOM_JOINTS_API dBodyPartDifferential* AddDifferential(dBodyPartDifferential* const leftDifferential, dBodyPartDifferential* const rightDifferential);

	CUSTOM_JOINTS_API void LinkTiresKinematically(dBodyPartTire* const tire0, dBodyPartTire* const tire1);

	CUSTOM_JOINTS_API dBodyPartEngine* AddEngine (dFloat mass, dFloat armatureRadius);

	CUSTOM_JOINTS_API void SetCenterOfGravity(const dVector& comRelativeToGeomtriCenter);
	const CUSTOM_JOINTS_API dBodyPart* GetChassis() const;
//	const CUSTOM_JOINTS_API dMatrix& GetLocalFrame() const;

	CUSTOM_JOINTS_API dMatrix GetTransform() const;
	CUSTOM_JOINTS_API void SetTransform(const dMatrix& matrix);

	CUSTOM_JOINTS_API dList<dBodyPartTire>::dListNode* GetFirstTire() const;
	CUSTOM_JOINTS_API dList<dBodyPartTire>::dListNode* GetNextTire (dList<dBodyPartTire>::dListNode* const tireNode) const;

	CUSTOM_JOINTS_API dList<dBodyPart*>::dListNode* GetFirstBodyPart() const;
	CUSTOM_JOINTS_API dList<dBodyPart*>::dListNode* GetNextBodyPart(dList<dBodyPart*>::dListNode* const partNode) const;

	CUSTOM_JOINTS_API dVector GetTireNormalForce(const dBodyPartTire* const tire) const;
	CUSTOM_JOINTS_API dVector GetTireLateralForce(const dBodyPartTire* const tire) const;
	CUSTOM_JOINTS_API dVector GetTireLongitudinalForce(const dBodyPartTire* const tire) const;

	CUSTOM_JOINTS_API dBrakeController* GetBrakes() const;
	CUSTOM_JOINTS_API dEngineController* GetEngine() const;
	CUSTOM_JOINTS_API dBrakeController* GetHandBrakes() const;
	CUSTOM_JOINTS_API dSteeringController* GetSteering() const;
	
	CUSTOM_JOINTS_API void SetEngine(dEngineController* const engine);
	CUSTOM_JOINTS_API void SetBrakes(dBrakeController* const brakes);
	CUSTOM_JOINTS_API void SetHandBrakes(dBrakeController* const brakes);
	CUSTOM_JOINTS_API void SetSteering(dSteeringController* const steering);
	CUSTOM_JOINTS_API void SetContactFilter(dBodyPartTire::dFrictionModel* const filter);

	CUSTOM_JOINTS_API dFloat GetAerodynamicsDowforceCoeficient() const;
	CUSTOM_JOINTS_API void SetAerodynamicsDownforceCoefficient(dFloat downWeightRatioAtSpeedFactor, dFloat speedFactor, dFloat maxWeightAtTopSpeed);

	CUSTOM_JOINTS_API dFloat GetWeightDistribution() const;
	CUSTOM_JOINTS_API void SetWeightDistribution(dFloat weightDistribution);

	CUSTOM_JOINTS_API void DrawSchematic (dFloat scale) const;	

	protected:
	CUSTOM_JOINTS_API virtual void PreUpdate(dFloat timestep, int threadIndex);
	CUSTOM_JOINTS_API virtual void PostUpdate(dFloat timestep, int threadIndex);

	bool ControlStateChanged() const;
	void Init (NewtonBody* const body, const dMatrix& vehicleFrame, NewtonApplyForceAndTorque forceAndTorque, void* const userData, dFloat gravityMag);
	void Init (NewtonCollision* const chassisShape, const dMatrix& vehicleFrame, dFloat mass, NewtonApplyForceAndTorque forceAndTorque, void* const userData, dFloat gravityMag);
	
	void CalculateSideSlipDynamics(dFloat timestep);
	void ApplySuspensionForces (dFloat timestep) const;
	dVector GetLastLateralForce(dBodyPartTire* const tire) const;
	void Cleanup();
	
//	dMatrix m_localFrame;
	dBodyPartChassis m_chassis;
	dList<dBodyPartTire> m_tireList;
	dList<dBodyPartDifferential> m_differentialList;
	dList<dBodyPart*> m_bodyPartsList;
	dBodyPartEngine* m_engine;
	
	void* m_collisionAggregate;
	
	dBrakeController* m_brakesControl;
	dEngineController* m_engineControl;
	dBrakeController* m_handBrakesControl;
	dSteeringController* m_steeringControl; 
//	dLateralDynamicsJoint* m_lateralDynamicControl;
	dBodyPartTire::dFrictionModel* m_contactFilter;
	NewtonApplyForceAndTorque m_forceAndTorqueCallback;

	dFloat m_speed;
	dFloat m_totalMass;
	dFloat m_sideSlip;
	dFloat m_prevSideSlip;
	dFloat m_gravityMag;
	dFloat m_weightDistribution;

	bool m_finalized;

	friend class dCustomVehicleControllerManager;
};


class dCustomVehicleControllerManager: public dCustomControllerManager<dCustomVehicleController> 
{
	class dTireFilter;
	public:
	CUSTOM_JOINTS_API dCustomVehicleControllerManager(NewtonWorld* const world, int materialCount, int* const otherMaterials);
	CUSTOM_JOINTS_API virtual ~dCustomVehicleControllerManager();

	CUSTOM_JOINTS_API virtual dCustomVehicleController* CreateVehicle (NewtonBody* const body, const dMatrix& vehicleFrame, NewtonApplyForceAndTorque forceAndTorque, void* const userData, dFloat gravityMag);
	CUSTOM_JOINTS_API virtual dCustomVehicleController* CreateVehicle (NewtonCollision* const chassisShape, const dMatrix& vehicleFrame, dFloat mass, NewtonApplyForceAndTorque forceAndTorque, void* const userData, dFloat gravityMag);
	CUSTOM_JOINTS_API virtual void DestroyController (dCustomVehicleController* const controller);

	CUSTOM_JOINTS_API virtual int OnTireAABBOverlap(const NewtonMaterial* const material, const dCustomVehicleController::dBodyPartTire* const tire, const NewtonBody* const otherBody) const;

	CUSTOM_JOINTS_API int GetTireMaterial() const;
	CUSTOM_JOINTS_API void DrawSchematic (const dCustomVehicleController* const controller, dFloat scale) const;

	protected:
	CUSTOM_JOINTS_API void OnTireContactsProcess (const NewtonJoint* const contactJoint, dCustomVehicleController::dBodyPartTire* const tire, const NewtonBody* const otherBody, dFloat timestep);
	CUSTOM_JOINTS_API virtual void DrawSchematicCallback (const dCustomVehicleController* const controller, const char* const partName, dFloat value, int pointCount, const dVector* const lines) const;
	CUSTOM_JOINTS_API int OnContactGeneration (const dCustomVehicleController::dBodyPartTire* const tire, const NewtonBody* const otherBody, const NewtonCollision* const othercollision, NewtonUserContactPoint* const contactBuffer, int maxCount, int threadIndex) const;

	int Collide (dCustomVehicleController::dBodyPartTire* const tire, int threadIndex) const;
	static void OnTireContactsProcess(const NewtonJoint* const contactJoint, dFloat timestep, int threadIndex);
	static int OnTireAABBOverlap(const NewtonMaterial* const material, const NewtonBody* const body0, const NewtonBody* const body1, int threadIndex);
	static int OnContactGeneration (const NewtonMaterial* const material, const NewtonBody* const body0, const NewtonCollision* const collision0, const NewtonBody* const body1, const NewtonCollision* const collision1, NewtonUserContactPoint* const contactBuffer, int maxCount, int threadIndex);

	const void* m_tireShapeTemplateData;
	NewtonCollision* m_tireShapeTemplate;
	int m_tireMaterial;

	friend class dCustomVehicleController;
};


#endif 

