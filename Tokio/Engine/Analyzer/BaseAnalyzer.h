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

typedef int AnalyzerFlags;

class AnalyzerFlags_
{
public:
	enum Flags
	{
		None           = 0,		  // Just disassemble the region without doing anything else
		Symbol         = 1 << 0,  // Parse symbol for addresses, jumps and calls, if this flag is not set, fmtAddress will not be available
		CrossReference = 1 << 1,  // Analyze cross-reference
		Subroutine     = 1 << 2,  // Analyze subroutines, if this flag is set, subroutines will also be analyzed
		Comment        = 1 << 3,  // Detect comments, such as value, pointer and strings
	};
};

struct FullAnalyzeCallbackData
{
	bool isError = false;
	// indicates how many bytes have been analyzed
	POINTER done = 0;
	// the total bytes to analyze
	POINTER total = 0;
	
	// the analyze progress could be calculated be (done / total) * 100
};

typedef void(*FullAnalyzeResultCallback)(FullAnalyzeCallbackData data, const void* UserData);

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
		AnalyzerFlags flags,
		std::vector<byte_t>& outBuffer,
		AnalyzedData& outData
	) EXCEPT = 0;

	virtual void FullAnalyze(FullAnalyzeResultCallback callback, const void* UserData) noexcept = 0;
};

}

#endif // !TOKIO_ENGINE_BASEDISASSEMBLER_HPP