#include "stdafx.h"
#include "DisassemblerGraph.h"
#include "Engine/Engine.h"



void DisassemblerGraph::Node::Calculte(ImVec2 pos, bool override_visited)
{

	if (m_isVisited && !override_visited) return;

	m_isVisited = true;

	ImFont* font = MainApplication::FontMonoRegular;
	const float fontSize = font->FontSize;

	const float lineHeight = fontSize * 1.5f;
	const float mnemonic_width = 50.f;

	m_pos = pos;
	m_size = {0, m_instructions.size() * lineHeight};

	for (const Instruction& instruction : m_instructions)
	{
		ImVec2 textSize = instruction.fmtOperand.CalcSize(font, fontSize);
		if (textSize.x > m_size.x) m_size.x = textSize.x;
	}

	m_size.x += mnemonic_width;

	ImVec2 arrowPos{ m_pos.x + m_size.x / 2.f, m_pos.y + m_size.y + 30.f};

	if (m_nextNode != nullptr)
	{
		if (m_nextNode->m_isVisited)
		{
			if (m_nextNode->m_pos.y < arrowPos.y)
				m_nextNode->Calculte(arrowPos);
		}
		else
		{
			m_nextNode->Calculte(arrowPos);

			if (m_conditionalNode != nullptr)
				m_nextNode->m_pos.x -= m_nextNode->m_size.x - 20;
			else
				m_nextNode->m_pos.x -= m_nextNode->m_size.x / 2.f - 20;
		}
	}
	// make sure we don't jump into ourselve recurively
	if (m_conditionalNode != nullptr && m_conditionalNode != this)
	{
		// if the node is already visited, check if it fit the position properly
		// if not then we override with a new position
		if (m_conditionalNode->m_isVisited)
		{
			if (m_conditionalNode->m_pos.y < arrowPos.y)
				m_conditionalNode->Calculte(arrowPos);
		}
		else
		{
			m_conditionalNode->Calculte(arrowPos);
			m_conditionalNode->m_pos.x += 20;
		}
	}

}

void DisassemblerGraph::Node::Render(ImVec2 offset)
{
	if (m_isVisited) return;
	m_isVisited = true;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->AddRectFilled(m_pos + offset, m_pos + m_size + offset, 0xff2b2b2b);
	
	ImFont* font = MainApplication::FontMonoRegular;
	const float fontSize = font->FontSize;

	const float lineHeight = fontSize * 1.5f;
	const float mnemonic_width = 50.f;

	ImVec2 cursor = m_pos + offset;

	for (auto& instruction : m_instructions)
	{
		instruction.fmtMnemonic.Render(dl, cursor, font, fontSize);
		instruction.fmtOperand.Render(dl, cursor + ImVec2(mnemonic_width, 0), font, fontSize);
		cursor.y += lineHeight;
	}

	ImVec2 arrowPos1{ m_pos.x + m_size.x / 2.f, m_pos.y + m_size.y };

	// make sure we don't jump into ourselve recurively
	if (m_conditionalNode != nullptr && m_conditionalNode != this)
	{
		m_conditionalNode->Render(offset);
		ImVec2 arrowPos2{ m_conditionalNode->m_pos.x + m_conditionalNode->m_size.x / 2.f, m_conditionalNode->m_pos.y};
		dl->AddLine(arrowPos1 + offset, arrowPos2 + offset, 0xffababab);
	}

	if (m_nextNode != nullptr)
	{
		m_nextNode->Render(offset);
		ImVec2 arrowPos2{ m_nextNode->m_pos.x + m_nextNode->m_size.x / 2.f, m_nextNode->m_pos.y };
		dl->AddLine(arrowPos1 + offset, arrowPos2 + offset, 0xffababab);
	}
}

bool DisassemblerGraph::Init(POINTER address, const AnalyzedData& data)
{
	m_nodes.clear();
	
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

	m_nodes.reserve(pSubroutine->blocks.size());

	for (const SubroutineBlock& block : pSubroutine->blocks)
	{
		Node& node = m_nodes.emplace_back();
		node.m_instructions.reserve(block.instructionCount);

		// copy the mnemonic and operands
		for (size_t i = block.instructionIndex; i < block.instructionIndex + block.instructionCount; i++)
		{
			const AnalyzedInstruction& insData = data.GetInstruction(i);
			Instruction& insCopied = node.m_instructions.emplace_back();

			insCopied.address = insData.address;
			insCopied.fmtAddress = insData.fmtAddress;
			insCopied.fmtMnemonic = insData.fmtMnemonic;
			insCopied.fmtOperand = insData.fmtOperand;
		}


		// Note: we calculate the pointer Node* before it is even added to the vector
		if (block.nextBlockIndex != UPTR_UNDEFINED)
			node.m_nextNode = &m_nodes[0] + block.nextBlockIndex;

		if (block.nextCondBlockIndex != UPTR_UNDEFINED)
			node.m_conditionalNode = &m_nodes[0] + block.nextCondBlockIndex;
	}

	m_nodes[0].Calculte();

	return true;
}

// must call render() inside of an imgui window
void DisassemblerGraph::Render()
{
	for (auto& node : m_nodes)
	{
		node.m_isVisited = false;
	}

	ImVec2 windowPos = ImGui::GetWindowPos();

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

	windowPos += graph_offset;

	m_nodes[0].Render(windowPos);
}