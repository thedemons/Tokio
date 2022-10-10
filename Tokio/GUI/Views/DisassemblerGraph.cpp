/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "stdafx.h"
#include "DisassemblerGraph.h"
#include "Engine/Engine.h"

#include "sfg/sfg.h"

#include "Graphics.h"

void DisassemblerGraph::CalculateGraph()
{
	ImFont* font = Graphics::FontMono;
	const float fontSize = font->FontSize;

	const float lineHeight = fontSize * 1.5f;
	const float mnemonic_width = 50.f;

	sfg::sfg_init();

	int nodeNum = 1;
	int edgeNum = 1;

	for (Block& block : m_blocks)
	{
		block.size = { 0, float(block.instructions.size()) * lineHeight };

		for (const Instruction& instruction : block.instructions)
		{
			ImVec2 operandSize = instruction.fmtOperand.CalcSize(font, fontSize);
			ImVec2 addressSize = instruction.fmtAddress.CalcSize(font, fontSize);
			float sumSize = operandSize.x + addressSize.x + 10.f;
			if (sumSize > block.size.x) block.size.x = sumSize;
		}

		block.size.x += mnemonic_width;

		sfg::sfg_addnode(nodeNum++, int(block.size.x), int(block.size.y));

	}

	for (Block& block : m_blocks)
	{
		size_t nodeIndex = &block - &m_blocks[0];
		if (block.nextBlock != nullptr)
		{
			size_t nextIndex = block.nextBlock - &m_blocks[0];
			sfg::sfg_addedge(edgeNum++, static_cast<int>(nodeIndex + 1), static_cast<int>(nextIndex + 1), 0, 0);
		}

		if (block.conditionalBlock != nullptr)
		{
			size_t conditionalIndex = block.conditionalBlock - &m_blocks[0];
			sfg::sfg_addedge(edgeNum++, static_cast<int>(nodeIndex + 1), static_cast<int>(conditionalIndex + 1), 0, 0);
		}
	}

	sfg::sfg_xspacing(10);
	sfg::sfg_yspacing(10);
	sfg::sfg_layout();

	m_nodes.clear();
	m_edges.clear();
	//m_edges.resize(static_cast<size_t>(sfg::maingraph->edgemax));
	m_nodes.resize(static_cast<size_t>(sfg::maingraph->nodemax));

	sfg::gml_nlist* nl = NULL;
	sfg::gml_node* n = NULL;
	nl = sfg::maingraph->nodelist;

	while (nl)
	{
		n = nl->node;

		SugiyamaNode& node = m_nodes[static_cast<size_t>(n->nr - 1)];
		node.isDummy = (n->tx == 0) && (n->ty == 0);
		node.pos = { float(n->finx), float(n->finy) };

		nl = nl->next;
	}

	sfg::gml_edge* e = NULL;
	sfg::gml_elist* el = sfg::maingraph->edgelist;
	while (el)
	{
		e = el->edge;

		SugiyamaEdge& edge = m_edges.emplace_back();
		edge.from = static_cast<size_t>(e->from_node->nr - 1);
		edge.to = static_cast<size_t>(e->to_node->nr - 1);

		el = el->next;
	}

	sfg::sfg_deinit();
}


bool DisassemblerGraph::Init(POINTER address, const AnalyzedData& data)
{
	m_blocks.clear();
	
	// do this to avoid nested hell
	const SubroutineInfo* pSubroutine = nullptr;

	for (const SubroutineInfo& subroutine : data.subroutines)
	{
		if (subroutine.address <= address && address <= subroutine.address + subroutine.size)
		{
			pSubroutine = &subroutine;
			break;
		}
	}

	if (pSubroutine == nullptr) return false;

	m_blocks.reserve(pSubroutine->blocks.size());

	for (const SubroutineBlock& block : pSubroutine->blocks)
	{
		Block& thisBlock = m_blocks.emplace_back();
		thisBlock.instructions.reserve(block.instructionCount);

		// copy the mnemonic and operands
		for (size_t i = block.instructionIndex; i < block.instructionIndex + block.instructionCount; i++)
		{
			const AnalyzedInstruction& insData = data.GetInstruction(i);
			Instruction& insCopied = thisBlock.instructions.emplace_back();

			insCopied.address = insData.address;
			insCopied.fmtAddress = insData.fmtAddress;
			insCopied.fmtMnemonic = insData.fmtMnemonic;
			insCopied.fmtOperand = insData.fmtOperand;
		}


	}

	size_t x = 0;
	for (const SubroutineBlock& block : pSubroutine->blocks)
	{
		Block& thisBlock = m_blocks[x++];

		// Note: we calculate the pointer Node* before it is even added to the vector
		if (block.nextBlockIndex != UPTR_UNDEFINED)
			thisBlock.nextBlock = &m_blocks[0] + block.nextBlockIndex;

		if (block.nextCondBlockIndex != UPTR_UNDEFINED)
			thisBlock.conditionalBlock = &m_blocks[0] + block.nextCondBlockIndex;
	}

	CalculateGraph();

	return true;
}

// must call render() inside of an imgui window
void DisassemblerGraph::Render()
{
	ImVec2 graphPos = ImGui::GetWindowPos();

	static ImVec2 graph_offset{ 0,0 };
	static ImVec2 graph_offset_old = graph_offset;
	static bool isDragging = false;
	
	if (!isDragging && ImGui::IsMouseDragging(0))
	{
		graph_offset_old = graph_offset;
		isDragging = true;
	}
	else if (isDragging && !ImGui::IsMouseDragging(0))
	{
		isDragging = false;
	}
	else if (isDragging)
	{
		graph_offset = graph_offset_old + ImGui::GetMouseDragDelta(0);
	}

	graphPos += graph_offset;



	ImFont* font = Graphics::FontMono;
	const float fontSize = font->FontSize;

	const float lineHeight = fontSize * 1.5f;
	const float mnemonic_width = 50.f;
	ImDrawList* dl = ImGui::GetWindowDrawList();

	for (size_t nodeIndex = 0; nodeIndex < m_nodes.size(); nodeIndex++)
	{
		SugiyamaNode& node = m_nodes[nodeIndex];
		if (node.isDummy) continue;

		Block& block = m_blocks[nodeIndex];
		ImVec2 blockPos = node.pos + graphPos;
		dl->AddRectFilled(blockPos, blockPos + block.size, 0xff2b2b2b);

		if (ImGui::IsMouseHoveringRect(blockPos, blockPos + block.size, false))
		{
			ImGui::BeginTooltip();
			if (block.nextBlock != nullptr)
			{
				ImGui::Text("Next");
				ImGui::SameLine();
				block.nextBlock->instructions[0].fmtAddress.Render();
			}
			if (block.conditionalBlock != nullptr)
			{
				ImGui::Text("Cond");
				ImGui::SameLine();
				block.conditionalBlock->instructions[0].fmtAddress.Render();
			}
			ImGui::EndTooltip();
		}

		ImVec2 cursor = blockPos;

		for (auto& instruction : block.instructions)
		{
			ImVec2 addrSize = instruction.fmtAddress.CalcSize(font, fontSize);
			instruction.fmtAddress.Render(dl, cursor, font, fontSize);
			instruction.fmtMnemonic.Render(dl, cursor + ImVec2(addrSize.x + 10.f, 0.f), font, fontSize);
			instruction.fmtOperand.Render(dl, cursor + ImVec2(addrSize.x + 10.f + mnemonic_width, 0), font, fontSize);
			cursor.y += lineHeight;
		}
	}

	for (SugiyamaEdge& edge : m_edges)
	{
		SugiyamaNode& fromNode = m_nodes[edge.from];
		SugiyamaNode& toNode = m_nodes[edge.to];
		//Block& fromBlock = m_blocks[]
		ImVec2 fromPos = fromNode.pos + graphPos;
		ImVec2 toPos = toNode.pos + graphPos;

		if (!fromNode.isDummy)
		{
			Block& fromBlock = m_blocks[edge.from];
			fromPos += ImVec2(fromBlock.size.x / 2.f, fromBlock.size.y);
		}

		if (!toNode.isDummy)
		{
			Block& toBlock = m_blocks[edge.to];
			toPos += ImVec2(toBlock.size.x / 2.f, 0.f);
		}

		dl->AddLine(fromPos, toPos, 0xffffffff);
	}

}