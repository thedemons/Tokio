#pragma once
#ifndef TOKIO_ENGINE_BASEANALYZER_H
#define TOKIO_ENGINE_BASEANALYZER_H

#include "Engine/EngineDef.h"
#include "Engine/Memory/BaseMemory.h"
#include "Engine/Symbol/BaseSymbol.h"
#include "Engine/Disassembler/BaseDisassembler.h"

#include <memory>

struct AnalyzedData;

namespace Engine
{

class BaseAnalyzer
{
protected:
	std::shared_ptr<ProcessData> m_target;			    // target process
	std::shared_ptr<BaseSymbol> m_symbol;			    // symbol engine
	std::shared_ptr<BaseMemory> m_memory;			    // memeory engine
	std::shared_ptr<BaseDisassembler> m_disassembler;	// disassemble engine
public:
	BaseAnalyzer(
		const std::shared_ptr<ProcessData>& target,
		const std::shared_ptr<BaseSymbol>& symbol,
		const std::shared_ptr<BaseMemory>& memory,
		const std::shared_ptr<BaseDisassembler>& disassembler
	);

	// The user interface will call this function to refresh the new settings
	// any inherited disassembler of this class should handle settings changes
	virtual void UpdateSettings();

	
	// address:		      the virtual address in the target process
	// size:		      the size in bytes to analyze
	// bDisectSubroutine: true to disect subroutine, if false, the subroutines vector will have a size of zero, only the instructions are analyzed
	// outBuffer:		  the output buffer contains read memory
	// outData:			  the output analyzed data	
	_NODISCARD virtual void Analyze(
		POINTER address,
		size_t size,
		bool bDisectSubroutine,
		std::vector<byte_t>& outBuffer,
		AnalyzedData& outData
	) EXCEPT = 0;

};

}

#endif // !TOKIO_ENGINE_BASEDISASSEMBLER_HPP