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