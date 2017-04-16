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
#include "dDataFlowGraph.h"
#include "dCILInstrBranch.h"
#include "dBasicBlocksGraph.h"
#include "dCILInstrLoadStore.h"
#include "dConvertToSSASolver.h"
#include "dConstantPropagationSolver.h"


void dStatementBlockDictionary::BuildUsedVariableWorklist(dBasicBlocksGraph& list)
{
	RemoveAll();

	for (dBasicBlocksGraph::dListNode* node = list.GetFirst(); node; node = node->GetNext()) {
		dBasicBlock& block = node->GetInfo();

		for (dCIL::dListNode* node = block.m_end; node != block.m_begin; node = node->GetPrev()) {
			dCILInstr* const instruction = node->GetInfo();
			//instruction->Trace();

			dList<dCILInstr::dArg*> variablesList;
			instruction->GetUsedVariables(variablesList);
			for (dList<dCILInstr::dArg*>::dListNode* varNode = variablesList.GetFirst(); varNode; varNode = varNode->GetNext()) {
				const dCILInstr::dArg* const variable = varNode->GetInfo();
				dTreeNode* entry = Find(variable->m_label);
				if (!entry) {
					entry = Insert(variable->m_label);
				}
				dStatementBlockBucket& buckect = entry->GetInfo();
				buckect.Insert(&block, node);
			}
		}
	}
}



dBasicBlock::dBasicBlock (dCIL::dListNode* const begin)
	:m_mark (0)
	,m_begin (begin)
	,m_end(NULL)
	,m_idom(NULL)
{
}

dBasicBlock::dBasicBlock(const dBasicBlock& src)
	:m_mark(0)
	,m_begin(src.m_begin)
	,m_end(NULL)
	,m_idom(NULL)
{
	for (m_end = m_begin; m_end && !m_end->GetInfo()->IsBasicBlockEnd(); m_end = m_end->GetNext()) {
		m_end->GetInfo()->m_basicBlock = this;
	}
	m_end->GetInfo()->m_basicBlock = this;
}


void dBasicBlock::Trace() const
{
	bool terminate = false;
	for (dCIL::dListNode* node = m_begin; !terminate; node = node->GetNext()) {
		terminate = (node == m_end);
		node->GetInfo()->Trace();
	}

/*
	dCILInstrLabel* const label = m_begin->GetInfo()->GetAsLabel();
	dTrace ((" block-> %s\n", label->GetArg0().m_label.GetStr()));

	if (m_idom) {
		dTrace(("   immediateDominator-> %s\n", m_idom->m_begin->GetInfo()->GetAsLabel()->GetArg0().m_label.GetStr()));
	} else {
		dTrace(("   immediateDominator->\n"));
	}

	dTrace(("   dominatorTreeChildren-> "));
	for (dList<const dBasicBlock*>::dListNode* childNode = m_children.GetFirst(); childNode; childNode = childNode->GetNext()) {
		const dBasicBlock* const childBlock = childNode->GetInfo();
		dCILInstrLabel* const label = childBlock->m_begin->GetInfo()->GetAsLabel();
		dTrace(("%s ", label->GetArg0().m_label.GetStr()));
	}
	dTrace(("\n"));


	dTrace (("   dominators-> "));
	dTree<int, const dBasicBlock*>::Iterator iter (m_dominators);
	for (iter.Begin(); iter; iter ++) {
		const dBasicBlock& domBlock = *iter.GetKey();
		dCILInstrLabel* const label = domBlock.m_begin->GetInfo()->GetAsLabel();
		dTrace (("%s ", label->GetArg0().m_label.GetStr()));
	}
	dTrace (("\n"));
*/
/*
	dTrace(("   dominanceFrontier-> "));
	for (dList<const dBasicBlock*>::dListNode* childFrontierNode = m_dominanceFrontier.GetFirst(); childFrontierNode; childFrontierNode = childFrontierNode->GetNext()) {
		const dBasicBlock* const childBlock = childFrontierNode->GetInfo();
		dCILInstrLabel* const label = childBlock->m_begin->GetInfo()->GetAsLabel();
		dTrace(("%s ", label->GetArg0().m_label.GetStr()));
	}
	dTrace (("\n"));
*/
	dTrace (("\n"));
}


bool dBasicBlock::ComparedDominator(const dTree<int, const dBasicBlock*>& newdominators) const
{
	if (m_dominators.GetCount() != newdominators.GetCount()) {
		return true;
	}

	dTree<int, const dBasicBlock*>::Iterator iter0 (m_dominators);
	dTree<int, const dBasicBlock*>::Iterator iter1 (newdominators);
	for (iter0.Begin(), iter1.Begin(); iter0 && iter1; iter0++, iter1++) {
		if (iter0.GetKey() != iter1.GetKey()) {
			return true;
		}
	}
	return false;
}

void dBasicBlock::ReplaceDominator (const dTree<int, const dBasicBlock*>& newdominators)
{
	m_dominators.RemoveAll();
	dTree<int, const dBasicBlock*>::Iterator iter1(newdominators);
	for (iter1.Begin(); iter1; iter1++) {
		m_dominators.Insert(0, iter1.GetKey());
	}
}


dBasicBlocksGraph::dBasicBlocksGraph()
	:dList<dBasicBlock> ()
	,m_dominatorTree(NULL)
	,m_mark(0)
{
}


void dBasicBlocksGraph::Trace() const
{
	for (dList<dBasicBlock>::dListNode* blockNode = GetFirst(); blockNode; blockNode = blockNode->GetNext()) {
		dBasicBlock& block = blockNode->GetInfo();
		block.Trace();
	}
}

void dBasicBlocksGraph::Build (dCIL::dListNode* const functionNode)
{
	//RemoveAll();
	dAssert (!GetCount());

	m_begin = functionNode->GetNext();
	dAssert (m_begin->GetInfo()->GetAsLabel());
	for (m_end = functionNode->GetNext(); !m_end->GetInfo()->GetAsFunctionEnd(); m_end = m_end->GetNext());

	// find the root of all basic blocks leaders
	for (dCIL::dListNode* node = m_begin; node != m_end; node = node->GetNext()) {
		if (node->GetInfo()->IsBasicBlockBegin()) {
			dAssert (node->GetInfo()->GetAsLabel());
			Append(dBasicBlock(node));
		}
	}
	CalculateSuccessorsAndPredecessors ();
	BuildDominatorTree ();
//Trace();
}


void dBasicBlocksGraph::CalculateSuccessorsAndPredecessors ()
{
	m_mark += 1;
	dList<dBasicBlock*> stack;
	stack.Append(&GetFirst()->GetInfo());

	while (stack.GetCount()) {
		dBasicBlock* const block = stack.GetLast()->GetInfo();

		stack.Remove(stack.GetLast()->GetInfo());
		if (block->m_mark < m_mark) {

			block->m_mark = m_mark;
			//m_traversalBlocksOrder.Addtop(block);
//block->Trace();

			dCILInstr* const instruction = block->m_end->GetInfo();
			dAssert(instruction->IsBasicBlockEnd());
			if (instruction->GetAsIF()) {
				dCILInstrConditional* const ifInstr = instruction->GetAsIF();

				dAssert (ifInstr->GetTrueTarget());
				dAssert (ifInstr->GetFalseTarget());

				dCILInstrLabel* const target0 = ifInstr->GetTrueTarget()->GetInfo()->GetAsLabel();
				dCILInstrLabel* const target1 = ifInstr->GetFalseTarget()->GetInfo()->GetAsLabel();

				dBasicBlock* const block0 = target0->m_basicBlock;
				dAssert (block0);
				block->m_successors.Append (block0);
				block0->m_predecessors.Append(block);
				stack.Append (block0);

				dBasicBlock* const block1 = target1->m_basicBlock;
				dAssert(block1);
				block->m_successors.Append(block1);
				block1->m_predecessors.Append(block);
				stack.Append(block1);

			} else if (instruction->GetAsGoto()) {
				dCILInstrGoto* const gotoInst = instruction->GetAsGoto();

				dAssert(gotoInst->GetTarget());
				dCILInstrLabel* const target = gotoInst->GetTarget()->GetInfo()->GetAsLabel();
				dBasicBlock* const block0 = target->m_basicBlock;

				dAssert(block0);
				block->m_successors.Append(block0);
				block0->m_predecessors.Append(block);
				stack.Append(block0);
			}
		}
	}

	DeleteUnreachedBlocks();
}

void dBasicBlocksGraph::DeleteUnreachedBlocks()
{
	dTree<dBasicBlock*, dCIL::dListNode*> blockMap;
	for (dBasicBlocksGraph::dListNode* blockNode = GetFirst(); blockNode; blockNode = blockNode->GetNext()) {
		dBasicBlock& block = blockNode->GetInfo();
		blockMap.Insert(&block, block.m_begin);
	}

	
	m_mark += 1;
	dList<dBasicBlock*> stack;
	stack.Append(&GetFirst()->GetInfo());
	while (stack.GetCount()) {
		dBasicBlock* const block = stack.GetLast()->GetInfo();

		stack.Remove(stack.GetLast()->GetInfo());
		if (block->m_mark < m_mark) {
			block->m_mark = m_mark;

			dCILInstr* const instruction = block->m_end->GetInfo();
			dAssert(instruction->IsBasicBlockEnd());
			if (instruction->GetAsIF()) {
				dCILInstrConditional* const ifInstr = instruction->GetAsIF();
				stack.Append(blockMap.Find(ifInstr->GetTrueTarget())->GetInfo());
				stack.Append(blockMap.Find(ifInstr->GetFalseTarget())->GetInfo());

			} else if (instruction->GetAsGoto()) {
				dCILInstrGoto* const gotoInst = instruction->GetAsGoto();
				stack.Append(blockMap.Find(gotoInst->GetTarget())->GetInfo());
			}
		}
	}

	dCIL* const cil = m_begin->GetInfo()->GetCil();
	dBasicBlocksGraph::dListNode* nextBlockNode;
	for (dBasicBlocksGraph::dListNode* blockNode = GetFirst(); blockNode; blockNode = nextBlockNode) {
		dBasicBlock& block = blockNode->GetInfo();
		nextBlockNode = blockNode->GetNext();
		if (block.m_mark != m_mark) {
			//block.Trace();
			bool terminate = false;
			dCIL::dListNode* nextNode;
			for (dCIL::dListNode* node = block.m_begin; !terminate; node = nextNode) {
				terminate = (node == block.m_end);
				nextNode = node->GetNext();
				cil->Remove(node);
			}
			Remove(blockNode);
		}
	}
}



void dBasicBlocksGraph::BuildDominatorTree ()
{
	// dominator of the start node is the start itself
	//Dom(n0) = { n0 }
	dBasicBlock& firstBlock = GetFirst()->GetInfo();
	firstBlock.m_dominators.Insert(0, &firstBlock);

	// for all other nodes, set all nodes as the dominators
	for (dListNode* node = GetFirst()->GetNext(); node; node = node->GetNext()) {
		dBasicBlock& block = node->GetInfo();
		for (dListNode* node1 = GetFirst(); node1; node1 = node1->GetNext()) {
			block.m_dominators.Insert(0, &node1->GetInfo());
		}
	}

	bool change = true;
	while (change) {
		change = false;
		for (dListNode* node = GetFirst()->GetNext(); node; node = node->GetNext()) {
			dBasicBlock& block = node->GetInfo();

//block.Trace();
			dTree<int, const dBasicBlock*> predIntersection;
			const dBasicBlock& predBlock = *block.m_predecessors.GetFirst()->GetInfo();

			dTree<int, const dBasicBlock*>::Iterator domIter (predBlock.m_dominators);
			for (domIter.Begin(); domIter; domIter ++) {
				const dBasicBlock* const block = domIter.GetKey();
				predIntersection.Insert (0, block);
			}

			for (dList<const dBasicBlock*>::dListNode* predNode = block.m_predecessors.GetFirst()->GetNext(); predNode; predNode = predNode->GetNext()) {
				const dBasicBlock& predBlock = *predNode->GetInfo();
				dTree<int, const dBasicBlock*>::Iterator predIter (predIntersection);
				for (predIter.Begin(); predIter; ) {
					const dBasicBlock* block = predIter.GetKey();
					predIter ++;
					if (!predBlock.m_dominators.Find(block)) {
						predIntersection.Remove(block);
					}
				}
			}

			dAssert (!predIntersection.Find(&block));
			predIntersection.Insert(&block);

			bool dominatorChanged = block.ComparedDominator (predIntersection);
			if (dominatorChanged) {
				block.ReplaceDominator (predIntersection);
				//block.Trace();
			}
			change |= dominatorChanged;
		}
	}

	// find the immediate dominator of each block
	for (dListNode* node = GetFirst()->GetNext(); node; node = node->GetNext()) {
		dBasicBlock& block = node->GetInfo();
//block.Trace();

		dAssert (block.m_dominators.GetCount() >= 2);
		dTree<int, const dBasicBlock*>::Iterator iter(block.m_dominators);

		for (iter.Begin(); iter; iter, iter++) {
			const dBasicBlock* const sblock = iter.GetKey();
			if (sblock != &block) {
				if (sblock->m_dominators.GetCount() == (block.m_dominators.GetCount() - 1)) {
					bool identical = true;
					dTree<int, const dBasicBlock*>::Iterator iter0(block.m_dominators);
					dTree<int, const dBasicBlock*>::Iterator iter1(sblock->m_dominators);
					for (iter0.Begin(), iter1.Begin(); identical && iter0 && iter1; iter0++, iter1++) {
						if (iter0.GetKey() == &block) {
							iter0 ++;
						}
						identical &= (iter0.GetKey() == iter1.GetKey());
					}
					if (identical) {
						if (sblock->m_dominators.GetCount() == (block.m_dominators.GetCount() - 1)) {
							block.m_idom = sblock;
							break;
						}
					}
				}
			}
		}
		dAssert (block.m_idom);
		//block.m_idom->Trace();
	}

//Trace();
	// build dominator tree
	m_dominatorTree = &GetFirst()->GetInfo();
	for (dListNode* node = GetFirst()->GetNext(); node; node = node->GetNext()) {
		const dBasicBlock& block = node->GetInfo();
		block.m_idom->m_children.Append(&block);
	}
	
//Trace();
}


void dBasicBlocksGraph::ConvertToSSA ()
{
	dConvertToSSASolver ssa (this);
	ssa.Solve();
}




//void dCIL::OptimizeSSA(dListNode* const functionStart)
void dBasicBlocksGraph::OptimizeSSA ()
{
//	dDataFlowGraph dataFlow(this, functionStart);
	Trace();

	bool pass = true;
	while (pass) {
		pass = false;
//ApplyDeadCodeEliminationSSA ();
		//pass |= ApplyConstantPropagationSSA();
//		pass |= ApplyConstantPropagationSSA();

		//Trace();
//		pass |= ApplyDeadCodeEliminationSSA ();
//		Trace();
		
		//Trace();
		//pass |= ApplyConstantConditionalSSA();
		//Trace();
		//pass |= ApplyCopyPropagationSSA ();
		//Trace();
		//pass |= dataFlow.ApplyConditionalConstantPropagationSSA();
	}

Trace();
}


void dBasicBlocksGraph::GetStatementsWorklist(dTree <int, dCIL::dListNode*>& list) const
{
	for (dCIL::dListNode* node = m_begin; node != m_end; node = node->GetNext()) {
		list.Insert(0, node);
	}
}


bool dBasicBlocksGraph::ApplyConstantConditionalSSA()
{
	bool anyChanges = false;

	dTree<int, dCIL::dListNode*> phyMap;
	for (dCIL::dListNode* node = m_begin; node != m_end; node = node->GetNext()) {
		dCILInstr* const instruction = node->GetInfo();
		if (instruction->GetAsPhi()) {
			phyMap.Insert(0, node);
		}
	}

	dCIL* const cil = m_begin->GetInfo()->GetCil();
	for (dCIL::dListNode* node = m_begin; node != m_end; node = node->GetNext()) {
		dCILInstr* const instruction = node->GetInfo();
		if (instruction->GetAsIF()) {
			dCILInstrConditional* const conditinal = instruction->GetAsIF();

			const dCILInstr::dArg& arg0 = conditinal->GetArg0();
			if ((arg0.GetType().m_intrinsicType == dCILInstr::m_constInt) || (arg0.GetType().m_intrinsicType == dCILInstr::m_constFloat)) {
				dAssert(conditinal->GetTrueTarget());
				dAssert(conditinal->GetFalseTarget());
				int condition = arg0.m_label.ToInteger();
				if (conditinal->m_mode == dCILInstrConditional::m_ifnot) {
					condition = !condition;
				}

				dCILInstrLabel* label;
				if (condition) {
					label = conditinal->GetTrueTarget()->GetInfo()->GetAsLabel();
				}
				else {
					label = conditinal->GetFalseTarget()->GetInfo()->GetAsLabel();
				}

				dCILInstrGoto* const jump = new dCILInstrGoto(*cil, label->GetArg0().m_label);
				jump->SetTarget(label);
				conditinal->ReplaceInstruction(jump);
				anyChanges = true;
			}
		}
	}

	return anyChanges;
}



bool dBasicBlocksGraph::ApplyDeadCodeEliminationSSA()
{
	bool anyChanges = false;

	dWorkList workList;
	dStatementBlockDictionary usedVariablesList;
	usedVariablesList.BuildUsedVariableWorklist(*this);

	dTree<dCIL::dListNode*, dString> map;
	for (dCIL::dListNode* node = m_begin; node != m_end; node = node->GetNext()) {
		dCILInstr* const instruction = node->GetInfo();
		const dCILInstr::dArg* const variable = instruction->GetGeneratedVariable();
		if (variable) {
			map.Insert(node, variable->m_label);
		}
	}

	GetStatementsWorklist(workList);
	while (workList.GetCount()) {
		dCIL::dListNode* const node = workList.GetRoot()->GetKey();
		workList.Remove(workList.GetRoot());
		dCILInstr* const instruction = node->GetInfo();
//instruction->Trace();
		if (!instruction->GetAsCall()) {
			const dCILInstr::dArg* const variable = instruction->GetGeneratedVariable();
			if (variable) {
				dStatementBlockDictionary::dTreeNode* const usesNodeBuckect = usedVariablesList.Find(variable->m_label);
				dAssert(!usesNodeBuckect || usesNodeBuckect->GetInfo().GetCount());
				if (!usesNodeBuckect) {
					anyChanges = true;
					dList<dCILInstr::dArg*> variablesList;
					instruction->GetUsedVariables(variablesList);
					for (dList<dCILInstr::dArg*>::dListNode* varNode = variablesList.GetFirst(); varNode; varNode = varNode->GetNext()) {
						const dCILInstr::dArg* const variable = varNode->GetInfo();
						dAssert(usedVariablesList.Find(variable->m_label));
						dStatementBlockDictionary::dTreeNode* const entry = usedVariablesList.Find(variable->m_label);
						if (entry) {
							dStatementBlockBucket& buckect = entry->GetInfo();
							buckect.Remove(node);
							dAssert(map.Find(variable->m_label) || instruction->GetAsPhi());
							if (map.Find(variable->m_label)) {
								workList.Insert(map.Find(variable->m_label)->GetInfo());
								if (!buckect.GetCount()) {
									usedVariablesList.Remove(usesNodeBuckect);
								}
							}
						}
					}
					instruction->Nullify();
				}
			}
		}
	}
	return anyChanges;
}


bool dBasicBlocksGraph::ApplyCopyPropagationSSA()
{
	bool anyChanges = false;

	dWorkList workList;
	dStatementBlockDictionary usedVariablesList;
	usedVariablesList.BuildUsedVariableWorklist (*this);

	GetStatementsWorklist(workList);
	while (workList.GetCount()) {
		dCIL::dListNode* const node = workList.GetRoot()->GetKey();
		workList.Remove(workList.GetRoot());
		dCILInstr* const instruction = node->GetInfo();
		anyChanges |= instruction->ApplyCopyPropagationSSA(workList, usedVariablesList);
	}
	return anyChanges;
}

/*
bool dBasicBlocksGraph::ApplyConstantPropagationSSA()
{
	bool anyChanges = false;

	dWorkList workList;
	dStatementBlockDictionary usedVariablesList;
	usedVariablesList.BuildUsedVariableWorklist(*this);

	GetStatementsWorklist(workList);
	while (workList.GetCount()) {
		dCIL::dListNode* const node = workList.GetRoot()->GetKey();
		workList.Remove(workList.GetRoot());
		dCILInstr* const instruction = node->GetInfo();
		anyChanges |= instruction->ApplyConstantPropagationSSA(workList, usedVariablesList);
	}

	if (anyChanges) {
		for (dCIL::dListNode* node = m_begin; node != m_end; node = node->GetNext()) {
			dCILInstr* const instruction = node->GetInfo();
			instruction->ApplyConstantFoldingSSA();
		}
	}
	return anyChanges;
}
*/


bool dBasicBlocksGraph::ApplyConstantPropagationSSA()
{
	dConstantPropagationSolver constantPropagation (this);
	return constantPropagation.Solve();
}