/* Copyright (c) <2009> <Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "dLSCstdafx.h"
#include "dDAG.h"
#include "dDAGTypeNode.h"
#include "dDAGClassNode.h"
#include "dDAGFunctionNode.h"
#include "dDAGDimensionNode.h"
#include "dDAGScopeBlockNode.h"
#include "dDAGExpressionNodeVariable.h"
#include "dDAGExpressionNodeAssigment.h"
#include "dDAGExpressionClassVariable.h"

dInitRtti(dDAGExpressionNodeVariable);

dDAGExpressionNodeVariable::dDAGExpressionNodeVariable(dList<dDAG*>& allNodes, const dString& name, const dString& modifiers, dDAGDimensionNode* const expressionDimIndex)
	:dDAGExpressionNode(allNodes)
	,m_type(NULL)
	,m_dimExpressions ()
{
	m_name = name;

	m_isFinal = modifiers.Find ("final") >= 0;
	m_isPublic = modifiers.Find ("public") >= 0;
	m_isStatic = modifiers.Find ("static") >= 0;

	dDAGDimensionNode* next;
	for (dDAGDimensionNode* node = expressionDimIndex; node; node = next) {
		next = node->m_next;
		node->m_next = NULL;
		dAssert (node->IsType(dDAGDimensionNode::GetRttiType()));
		m_dimExpressions.Append(node);
	}
}

dDAGExpressionNodeVariable::dDAGExpressionNodeVariable (dList<dDAG*>& allNodes, const dDAGExpressionNodeVariable& copySource)
	:dDAGExpressionNode(allNodes)
	,m_isFinal(copySource.m_isFinal)
	,m_isPublic(copySource.m_isPublic)
	,m_isStatic(copySource.m_isStatic)
{
	m_name = copySource.m_name;
	for (dList<dDAGDimensionNode*>::dListNode* node = copySource.m_dimExpressions.GetFirst(); node; node = node->GetNext()) {
		m_dimExpressions.Append((dDAGDimensionNode*) node->GetInfo()->Clone (allNodes));
	}
}

dDAGExpressionNodeVariable::~dDAGExpressionNodeVariable(void)
{
}

void dDAGExpressionNodeVariable::SetType(dDAGTypeNode* const type)
{
	m_type = type;
}

dDAG* dDAGExpressionNodeVariable::Clone (dList<dDAG*>& allNodes) const
{
	return new dDAGExpressionNodeVariable (allNodes, *this);
}

void dDAGExpressionNodeVariable::InitParam (const dDAGExpressionNodeVariable& source)
{
	m_isFinal = source.m_isFinal;
	m_isPublic = source.m_isPublic;
	m_isStatic = source.m_isStatic;
}

void dDAGExpressionNodeVariable::ConnectParent(dDAG* const parent)  
{
	m_parent = parent;

	dAssert (FindLocalVariable(m_name));
	if (m_type) {
		m_type->ConnectParent(this);
	}

	for (dList<dDAGDimensionNode*>::dListNode* node = m_dimExpressions.GetFirst(); node; node = node->GetNext()) {
		dDAGDimensionNode* const dim = node->GetInfo();
		dim->ConnectParent(this);
	}
}

dCIL::dReturnValue dDAGExpressionNodeVariable::Evalue(const dDAGFunctionNode* const function)
{
	if (function) {
		dTree<dCILInstr::dArg, dString>::dTreeNode* const node = FindLocalVariable (m_name);
		dAssert (node);

	} else {
		dAssert(0);
		dCIL::dReturnValue val;
		dDAGClassNode* const myClass = GetClass();
		for (dList<dDAGExpressionClassVariable*>::dListNode* node = myClass->m_variables.GetFirst(); node; node = node->GetNext()) {
			dDAGExpressionClassVariable* const variable = node->GetInfo();
			if (variable->m_variable->m_name == m_name) {
				if (variable->m_iniatilized) {
					return variable->m_initialValue;
				} else {
					dAssert (0);
				}
			}
		}
	}
	dAssert(0);
	return dCIL::dReturnValue();
}

void dDAGExpressionNodeVariable::CompileCIL(dCIL& cil)
{
//	dDAGFunctionNode* const function = GetFunction();
	if (m_dimExpressions.GetCount()) {
		dDAGDimensionNode* const dim = m_dimExpressions.GetFirst()->GetInfo();
		dim->CompileCIL(cil);

		dString result = dim->m_result.m_label;

		for (dList<dDAGDimensionNode*>::dListNode* node = m_dimExpressions.GetFirst()->GetNext(); node; node = node->GetNext()) {
			dAssert (0);
/*
			dDAGDimensionNode* const dim = node->GetInfo();
			dim->CompileCIL(cil);
			
			dThreeAdressStmt& stmtMul = cil.NewStatement()->GetInfo();
			stmtMul.m_instruction = dThreeAdressStmt::m_assigment;
			stmtMul.m_operator = dThreeAdressStmt::m_mul;
			stmtMul.m_arg0.m_label = cil.NewTemp();
			stmtMul.m_arg1.m_label = result;
			stmtMul.m_arg2.m_label = dim->m_arraySize;

			DTRACE_INTRUCTION (&stmtMul);

			dThreeAdressStmt& stmtAdd = cil.NewStatement()->GetInfo();
			stmtAdd.m_instruction = dThreeAdressStmt::m_assigment;
			stmtAdd.m_operator = dThreeAdressStmt::m_add;
			stmtAdd.m_arg0.m_label = cil.NewTemp();
			stmtAdd.m_arg1.m_label = stmtMul.m_arg0.m_label;
			stmtAdd.m_arg2 = dim->m_result;

			result = stmtAdd.m_arg0.m_label;

			DTRACE_INTRUCTION (&stmtAdd);
*/
		}

		dTree<dCILInstr::dArg, dString>::dTreeNode* const variable1 = dDAG::FindLocalVariable(m_name);
		dAssert (variable1);

		dCILInstr::dArg arg1 (LoadLocalVariable(cil, variable1->GetInfo()));
		dAssert (m_parent);

		int size = arg1.GetSizeInByte();
		dCILInstrIntergerLogical* const mulOffset = new dCILInstrIntergerLogical (cil, dCILThreeArgInstr::m_mul, cil.NewTemp(), dCILInstr::dArgType (dCILInstr::m_int), result, dCILInstr::dArgType (dCILInstr::m_int), dString(size), dCILInstr::m_constInt);
		mulOffset->Trace();

		dCILInstrIntergerLogical* const address = new dCILInstrIntergerLogical (cil, dCILThreeArgInstr::m_add, cil.NewTemp(), arg1.GetType(), arg1.m_label, arg1.GetType(), mulOffset->GetArg0().m_label, mulOffset->GetArg0().GetType());
		address->Trace();

		m_result = dCILInstr::dArg (cil.NewTemp(), arg1.GetType());
		m_result.m_isPointer = false;
		dCILInstrLoad* const load = new dCILInstrLoad (cil, m_result.m_label, m_result.GetType(), address->GetArg0().m_label, address->GetArg0().GetType()); 
		load->Trace();
		m_result = load->GetArg0();
		
	} else {
		dTree<dCILInstr::dArg, dString>::dTreeNode* const variable = dDAG::FindLocalVariable(m_name);
		dAssert (variable);
		m_result = variable->GetInfo();
	}
}


dDAGExpressionNodeVariable* dDAGExpressionNodeVariable::FindLeftVariable()
{
	return this;
}

