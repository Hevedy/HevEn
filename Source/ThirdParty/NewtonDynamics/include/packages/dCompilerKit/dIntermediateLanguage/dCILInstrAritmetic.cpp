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

#include "dCILstdafx.h"
#include "dCIL.h"
#include "dCILInstr.h"
#include "dDataFlowGraph.h"
#include "dCILInstrLoadStore.h"
#include "dCILInstrArithmetic.h"
#include "dConstantPropagationSolver.h"
#include "dConstantPropagationSolver.h"

/*
dCILInstrLocal::dCILInstrLocal (dCIL& program, const dString& name, const dArgType& type)
	:dCILSingleArgInstr (program, dArg (name, type))
{
}

void dCILInstrLocal::Serialize(char* const textOut) const
{
	sprintf (textOut, "\tlocal %s %s\n", m_arg0.GetTypeName().GetStr(), m_arg0.m_label.GetStr());
}

dCILInstrStore::dCILInstrStore (dCIL& program, const dString& name0, const dArgType& type0, const dString& name1, const dArgType& type1)
	:dCILTwoArgInstr (program, dArg (name0, type0), dArg (name1, type1))
{
}

void dCILInstrStore::Serialize(char* const textOut) const
{
	sprintf (textOut, "\t[%s %s] = %s %s\n", m_arg0.GetTypeName().GetStr(),  m_arg0.m_label.GetStr(), m_arg1.GetTypeName().GetStr(), m_arg1.m_label.GetStr());
}


dCILInstrLoad::dCILInstrLoad (dCIL& program, const dString& name, const dArgType& type)
	:dCILInstr (program)
	,m_source (name, type)
{
	m_destination.m_label = program.NewTemp();
	m_destination.SetType (m_source);
}

const dCILInstr::dArg& dCILInstrLoad::GetResult() const
{
	return m_destination;
}

void dCILInstrLoad::Serialize(char* const textOut) const
{
	sprintf (textOut, "\t%s %s = [%s %s]\n", m_destination.GetTypeName().GetStr(), m_destination.m_label.GetStr(), m_source.GetTypeName().GetStr(), m_source.m_label.GetStr());
}
*/

dCILInstrIntergerLogical::dCILInstrIntergerLogical (dCIL& program, dOperator operation, const dString& name0, const dArgType& type0, const dString& name1, const dArgType& type1, const dString& name2, const dArgType& type2)
	:dCILInstrThreeArgArithmetic(program, dArg(name0, type0), dArg(name1, type1), dArg(name2, type2))
	,m_operator(operation)
{

}

void dCILInstrIntergerLogical::Serialize(char* const textOut) const
{
	char* assignOperator = "";
	switch (m_operator)
	{
		case m_equal:
		{
			//dTrace ((" = "));
			assignOperator = " = " ;
			break;
		}

		case m_add:
		{
			//dTrace ((" + "));
			assignOperator = " + " ;
			break;
		}
		case m_sub:
		{
			//dTrace ((" - "));
			assignOperator = " - " ;
			break;
		}

		case m_mul:
		{
			//dTrace ((" * "));
			assignOperator = " * " ;
			break;
		}
		case m_div:
		{
			//dTrace ((" / "));
			assignOperator = " / " ;
			break;
		}

		case m_mod:
		{
			//dTrace ((" %c ", '%'));
			assignOperator = " %% " ;
			break;
		}

		case m_identical:
		{
			//dTrace ((" == "));
			assignOperator = " == " ;
			break;
		}

		case m_different:
		{
			//dTrace ((" != "));
			assignOperator = " != " ;
			break;
		}

		case m_less:
		{
			//dTrace ((" < "));
			assignOperator = " < " ;
			break;
		}

		case m_greather:
		{
			//dTrace ((" > "));
			assignOperator = " > " ;
			break;
		}

		case m_lessEqual:
		{
			//dTrace ((" <= "));
			assignOperator = " <= " ;
			break;
		}

		case m_greatherEqual:
		{
			//dTrace ((" >= "));
			assignOperator = " >= " ;
			break;
		}
		default:;
			dAssert (0);
	}

	sprintf(textOut, "\t%s %s = %s %s%s%s %s\n", m_arg0.GetTypeName().GetStr(), m_arg0.m_label.GetStr(), m_arg1.GetTypeName().GetStr(), m_arg1.m_label.GetStr(), assignOperator, m_arg2.GetTypeName().GetStr(), m_arg2.m_label.GetStr() );
}



bool dCILInstrIntergerLogical::ApplySemanticReordering ()
{
	const dArg& arg1 = GetArg1();
	switch (arg1.GetType().m_intrinsicType)
	{
		case m_constInt:
		{
			dAssert (0);
			if (m_cil->m_commutativeOperator[m_operator]) {
//				dAssert (stmt.m_arg2.GetType().m_intrinsicType != dThreeAdressStmt::m_constInt);
//				dSwap (stmt.m_arg1, stmt.m_arg2);
			} else {
/*
				dCIL::dListNode* const node = m_cil->NewStatement();
				m_cil->InsertAfter (stmtNode, node);

				dThreeAdressStmt& tmpStmt = node->GetInfo();
				tmpStmt = stmt;
								
				stmt.m_instruction = dThreeAdressStmt::m_assigment;
				stmt.m_operator = dThreeAdressStmt::m_nothing;
				stmt.m_arg0.m_label += dCIL::m_variableUndercore; 
				stmt.m_arg2.m_label = "";
				tmpStmt.m_arg1 = stmt.m_arg0;
*/
			}
			return true;
		}
		default:
			break;
	}
	return false;
}


void dCILInstrIntergerLogical::AddGeneratedAndUsedSymbols (dDataFlowPoint& datFloatPoint) const
{
	dAssert(0);
/*
	datFloatPoint.m_generatedVariable = m_arg0.m_label;

	dAssert (m_arg1.GetType().m_intrinsicType != m_constInt);
	datFloatPoint.m_usedVariableSet.Insert(m_arg1.m_label);
	
	switch (m_arg2.GetType().m_intrinsicType)
	{
		case m_int:
		//case dThreeAdressStmt::m_classPointer:
		{
			datFloatPoint.m_usedVariableSet.Insert (m_arg2.m_label);
			break;
		}

		case m_constInt:
			break;

		default:	
			dAssert (0);
	}
*/	
}


bool dCILInstrIntergerLogical::ApplyDeadElimination (dDataFlowGraph& dataFlow)
{
	return DeadElimination (dataFlow);
}

void dCILInstrIntergerLogical::AddDefinedVariable (dInstructionVariableDictionary& dictionary) const 
{
	dAssert(0);
//	dInstructionVariableDictionary::dTreeNode* const node = dictionary.Insert (m_arg0.m_label);
//	node->GetInfo().Append (m_myNode);
}

void dCILInstrIntergerLogical::AddUsedVariable (dInstructionVariableDictionary& dictionary) const 
{
	dAssert(0);
/*
	dInstructionVariableDictionary::dTreeNode* const node = dictionary.Insert(m_arg1.m_label);
	node->GetInfo().Append(m_myNode);

	switch (m_arg2.GetType().m_intrinsicType) 
	{
		case m_int:
		{
			dInstructionVariableDictionary::dTreeNode* const node = dictionary.Insert(m_arg2.m_label);
			node->GetInfo().Append(m_myNode);
			break;
		}

		case m_constInt:
			break;

		default:
			dAssert(0);
	}
*/
}

bool dCILInstrIntergerLogical::ApplyCopyPropagation(dCILInstrMove* const moveInst)
{
	bool ret = false;
	if (moveInst->m_arg0.m_label == m_arg1.m_label) {
		ret = true;
		m_arg1.m_label = moveInst->m_arg1.m_label;
	}

	switch (m_arg2.GetType().m_intrinsicType) 
	{
		case m_int:
		{
		  	if (moveInst->m_arg0.m_label == m_arg2.m_label) {
				ret = true;
				m_arg2.m_label = moveInst->m_arg1.m_label;
			}
			break;
		}

		case m_constInt:
			break;

		default:
			dAssert(0);
	}

	dAssert (ret);
	return ret; 
}

void dCILInstrIntergerLogical::AddKilledStatements(const dInstructionVariableDictionary& dictionary, dDataFlowPoint& datFloatPoint) const
{ 
	dCILInstr::AddKilledStatementLow(m_arg0, dictionary, datFloatPoint);
}


void dCILInstrIntergerLogical::EmitOpcode(dVirtualMachine::dOpCode* const codeOutPtr) const
{
	dVirtualMachine::dOpCode& code = codeOutPtr[m_byteCodeOffset];

	bool isRegister = true;
	int immediateSign = 1;
	switch (m_arg2.GetType().m_intrinsicType)
	{
		case m_constInt:
			isRegister = false;
			break;

		case m_int:
			break;

		default:
			dAssert(0);
	}

	switch (m_operator)
	{
		case m_identical:
		{
			//dTrace ((" == "));
			code.m_type3.m_opcode = isRegister ? unsigned (dVirtualMachine::m_cmpeq) : unsigned (dVirtualMachine::m_cmpeqi);
			break;
		}

		case m_add:
		{
			//dTrace ((" == "));
			code.m_type3.m_opcode = isRegister ? unsigned(dVirtualMachine::m_add) : unsigned(dVirtualMachine::m_addi);
			break;
		}

		case m_sub:
		{
			dAssert (RegisterToIndex(m_arg2.m_label) >= 0);
			immediateSign = isRegister ? 1 : -1;
			code.m_type3.m_opcode = isRegister ? unsigned (dVirtualMachine::m_sub) : unsigned(dVirtualMachine::m_addi);
			break;
		}

	/*
		case m_equal:
		{
			break;
		}

	case m_add:
	{
				  //dTrace ((" + "));
				  assignOperator = " + ";
				  break;
	}

	case m_mul:
	{
				  //dTrace ((" * "));
				  assignOperator = " * ";
				  break;
	}
	case m_div:
	{
				  //dTrace ((" / "));
				  assignOperator = " / ";
				  break;
	}

	case m_mod:
	{
				  //dTrace ((" %c ", '%'));
				  assignOperator = " %% ";
				  break;
	}


	case m_different:
	{
						//dTrace ((" != "));
						assignOperator = " != ";
						break;
	}

	case m_less:
	{
				   //dTrace ((" < "));
				   assignOperator = " < ";
				   break;
	}

	case m_greather:
	{
					   //dTrace ((" > "));
					   assignOperator = " > ";
					   break;
	}

	case m_lessEqual:
	{
						//dTrace ((" <= "));
						assignOperator = " <= ";
						break;
	}

	case m_greatherEqual:
	{
							//dTrace ((" >= "));
							assignOperator = " >= ";
							break;
	}
*/
		default:;
			dAssert(0);
	}

	code.m_type3.m_reg0 = RegisterToIndex(m_arg0.m_label);
	code.m_type3.m_reg1 = RegisterToIndex(m_arg1.m_label);
	if (isRegister) {
		code.m_type4.m_reg2 = RegisterToIndex(m_arg2.m_label);
		code.m_type4.m_imm4 = 0;
	} else {
		code.m_type3.m_imm3 = m_arg2.m_label.ToInteger() * immediateSign;
	}
}


void dCILInstrIntergerLogical::GetUsedVariables (dList<dArg*>& variablesList)
{
	variablesList.Append(&m_arg1);
	if (m_arg1.m_isPointer) {
		variablesList.Append(&m_arg1);
	} else {
		switch (m_arg2.GetType().m_intrinsicType) 
		{
			case m_constInt:
			case m_constFloat:			
				break;

			default:
				variablesList.Append(&m_arg2);
		}
	}
}

void dCILInstrIntergerLogical::ReplaceArgument (const dArg& arg, const dArg& newArg)
{
	if (arg.m_label == m_arg1.m_label) {
		m_arg1 = newArg;
	}
	if (arg.m_label == m_arg2.m_label) {
		m_arg2 = newArg;
	}
}

dString dCILInstrIntergerLogical::Evalue (const dString& arg1, const dString& arg2) const
{
	int c = 0;
	int a = arg1.ToInteger();
	int b = arg2.ToInteger();

	switch (m_operator) 
	{
		case m_add:
		{
			c = a + b;
			break;
		}

		case m_identical:
		{
			c = (a == b) ? 1 : 0;
			break;
		}

		case m_less:
		{
		   c = (a < b) ? 1 : 0;
		   break;
		}

		default:;
		dAssert(0);
	}
	return dString (c);
}

bool dCILInstrIntergerLogical::ApplyConstantFoldingSSA ()
{
	bool ret = false;

	bool arg1 = !m_arg1.m_isPointer && ((m_arg1.GetType().m_intrinsicType == m_constInt) || (m_arg1.GetType().m_intrinsicType == m_constFloat));
	bool arg2 = !m_arg2.m_isPointer && ((m_arg2.GetType().m_intrinsicType == m_constInt) || (m_arg2.GetType().m_intrinsicType == m_constFloat));
	
	if (arg1 && arg2) {
/*
		int c = 0;
		switch (m_operator) 
		{
			case m_less:
			{
				int a = m_arg1.m_label.ToInteger();
				int b = m_arg2.m_label.ToInteger();
				c = a < b ? 1 : 0;
				break;
			}

			default:;
				dAssert(0);
		}
*/

		dCILInstrMove* const move = new dCILInstrMove (*m_cil, m_arg0.m_label, m_arg0.GetType(), Evalue(m_arg1.m_label, m_arg2.m_label), dArgType (dCILInstr::m_constInt));
		ReplaceInstruction (move);
		ret = true;
	}

	return ret;
}


void dCILInstrIntergerLogical::ApplyConstantPropagationSSA (dConstantPropagationSolver& solver)
{
	dArg arg1(m_arg1);
	dConstantPropagationSolver::dVariable::dValueTypes type1 = dConstantPropagationSolver::dVariable::m_constant;
	if (!((m_arg1.GetType().m_intrinsicType == m_constInt) || (m_arg1.GetType().m_intrinsicType == m_constFloat))) {
		dConstantPropagationSolver::dVariable& variable = solver.m_variablesList.Find(m_arg1.m_label)->GetInfo();
		type1 = variable.m_type;
		arg1.m_label = variable.m_constValue;
	}

	dArg arg2(m_arg2);
	dConstantPropagationSolver::dVariable::dValueTypes type2 = dConstantPropagationSolver::dVariable::m_constant;
	if (!((m_arg2.GetType().m_intrinsicType == m_constInt) || (m_arg2.GetType().m_intrinsicType == m_constFloat))) {
		dConstantPropagationSolver::dVariable& variable = solver.m_variablesList.Find(m_arg2.m_label)->GetInfo();
		type2 = variable.m_type;
		arg2.m_label = variable.m_constValue;
	}

	if ((type1 == dConstantPropagationSolver::dVariable::m_constant) && (type2 == dConstantPropagationSolver::dVariable::m_constant)) {
		solver.UpdateLatice (m_arg0, Evalue(arg1.m_label, arg2.m_label), dConstantPropagationSolver::dVariable::m_constant);
	} else {
		solver.UpdateLatice (m_arg0, m_arg0.m_label, dConstantPropagationSolver::dVariable::m_variableValue);
	}
}