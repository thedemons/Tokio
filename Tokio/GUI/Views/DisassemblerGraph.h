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
	class Node
	{
	public:
		bool m_isVisited        = false;

		size_t m_layer = 0;
		size_t m_row = 0;

		ImVec2 m_pos  { 0.f, 0.f };
		ImVec2 m_size { 0.f, 0.f };

		Node* m_nextNode        = nullptr;
		Node* m_conditionalNode = nullptr;

		std::vector<Instruction> m_instructions;

	public:
		void Calculte(ImVec2 pos = { 0.f, 0.f }, bool override_visited = false);
		void Render(ImVec2 offset);
	};

	struct Layer
	{
		ImVec2 size;
		//std::vector<Node*> nodes;
	};

private:

	std::vector<Node> m_nodes;
	std::vector<Layer> m_layers;

public:
	bool Init(POINTER address, const AnalyzedData& data);
	void Render();
};

#endif // !TOKIO_GUI_VIEWS_DISASSEMBLER_GRAPH_H