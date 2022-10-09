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

#ifndef TOKIO_GUI_VIEWS_DISASSEMBLER_GRAPH_H
#define TOKIO_GUI_VIEWS_DISASSEMBLER_GRAPH_H

#include "GUI/MainView.h"
#include "Engine/EngineAnalyzerDef.h"
#include "GUI/ImGui/custom/TokenizedText.h"

#include <vector>
#include <string>

class DisassemblerGraph
{
private:
	struct Instruction
	{
		POINTER address = 0;

		ImGui::TokenizedText fmtAddress;
		ImGui::TokenizedText fmtMnemonic;
		ImGui::TokenizedText fmtOperand;

	};

	struct SugiyamaNode
	{
		bool isDummy = true;
		ImVec2 pos{ 0.f, 0.f };
	};

	struct SugiyamaEdge
	{
		size_t from;
		size_t to;
	};

	struct Block
	{
		ImVec2 size { 0.f, 0.f };

		Block* nextBlock        = nullptr;
		Block* conditionalBlock = nullptr;

		std::vector<Instruction> instructions;
	};

private:

	std::vector<Block> m_blocks;
	std::vector<SugiyamaEdge> m_edges;
	std::vector<SugiyamaNode> m_nodes;

	void CalculateGraph();
public:
	bool Init(POINTER address, const AnalyzedData& data);
	void Render();
};

#endif // !TOKIO_GUI_VIEWS_DISASSEMBLER_GRAPH_H