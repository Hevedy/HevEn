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
#include "dDAGExpressionNode.h"
#include "dDAGFunctionStatementCase.h"
#include "dDAGFunctionStatementSWITCH.h"


dInitRtti(dDAGFunctionStatementSWITCH);

dDAGFunctionStatementSWITCH::dDAGFunctionStatementSWITCH(dList<dDAG*>& allNodes, dDAGExpressionNode* const expression, dDAGFunctionStatementCase* const caseList)
	:dDAGFunctionStatementFlow(allNodes, NULL, NULL)
	,m_expression(expression)
	,m_caseList()
{
//	m_expression->AddRef();
	for (dDAGFunctionStatementCase* node = (dDAGFunctionStatementCase*)caseList; node; node = (dDAGFunctionStatementCase*)node->m_next) {
		m_caseList.Append(node);
		dAssert (0);
//		node->AddRef();
	}
}


dDAGFunctionStatementSWITCH::~dDAGFunctionStatementSWITCH()
{
//	m_expression->Release();
	for (dList<dDAGFunctionStatementCase*>::dListNode* node = m_caseList.GetFirst(); node; node = node->GetNext()) {
		dAssert (0);
//		dDAGFunctionStatementCase* const caseBlock = node->GetInfo();
//		caseBlock->Release();
	}
}


void dDAGFunctionStatementSWITCH::ConnectParent(dDAG* const parent)
{
dAssert (0);
	m_parent = parent;
	m_expression->ConnectParent(this);
	for (dList<dDAGFunctionStatementCase*>::dListNode* node = m_caseList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionStatementCase* const caseBlock = node->GetInfo();
		caseBlock->ConnectParent(this);
	}
}

dDAGFunctionStatement* const dDAGFunctionStatementSWITCH::GetPostFixStatement() const
{
	dAssert (0);
	return NULL;
}

void dDAGFunctionStatementSWITCH::CompileCIL(dCIL& cil)  
{
dAssert (0);
/*
	string exitLabel (cil.NewLabel());
	m_currentBreakLabel = exitLabel;

	dCILInstr& startLabel = cil.NewStatement()->GetInfo();
	startLabel.m_instruction = dCILInstr::m_goto;
	startLabel.m_arg0.m_label = cil.NewLabel();
	DTRACE_INTRUCTION (&startLabel);

	for (dList<dDAGFunctionStatementCase*>::dListNode* node = m_caseList.GetFirst(); node; node = node->GetNext()) {
		string caseLabel (cil.NewLabel());

		dDAGFunctionStatementCase* const caseBlock = node->GetInfo();
		caseBlock->m_entryLabel = caseLabel;

		dCILInstr& caseLabelStmt = cil.NewStatement()->GetInfo();
		caseLabelStmt.m_instruction = dCILInstr::m_label;
		caseLabelStmt.m_arg0.m_label = caseLabel;
		DTRACE_INTRUCTION (&caseLabelStmt);
		
		caseBlock->CompileCIL(cil);
	}

	dCILInstr& startSwitchStmt = cil.NewStatement()->GetInfo();
	startSwitchStmt.m_instruction = dCILInstr::m_label;
	startSwitchStmt.m_arg0.m_label = startLabel.m_arg0.m_label;
	DTRACE_INTRUCTION (&startSwitchStmt);

	m_expression->CompileCIL(cil);

	int defaultCount = 0;
	dDAGFunctionStatementCase* defualtNode = NULL;
	for (dList<dDAGFunctionStatementCase*>::dListNode* node = m_caseList.GetFirst(); node; node = node->GetNext()) {
		dDAGFunctionStatementCase* const caseBlock = node->GetInfo();
		if (caseBlock->m_nameId != "default") {
			dCILInstr& stmt = cil.NewStatement()->GetInfo();
			stmt.m_instruction = dCILInstr::m_if;
			stmt.m_operator = dCILInstr::m_identical;
			stmt.m_arg0.m_label = m_expression->m_result;
			stmt.m_arg1.m_label = caseBlock->m_nameId;
			stmt.m_arg2.m_label = caseBlock->m_entryLabel;
			DTRACE_INTRUCTION (&stmt);
		} else {
			defaultCount ++;
			defualtNode = caseBlock;
		}
	}

	_ASSERTE (defaultCount <= 1);
	if (defualtNode) {
		dCILInstr& stmt = cil.NewStatement()->GetInfo();
		stmt.m_instruction = dCILInstr::m_goto;
		stmt.m_arg0.m_label = defualtNode->m_entryLabel;
		DTRACE_INTRUCTION (&stmt);
	}

	dCILInstr& exitStmt = cil.NewStatement()->GetInfo();
	exitStmt.m_instruction = dCILInstr::m_label;
	exitStmt.m_arg0.m_label = exitLabel;
	DTRACE_INTRUCTION (&exitStmt);
*/
}