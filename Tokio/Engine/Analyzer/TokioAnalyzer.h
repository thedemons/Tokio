#pragma once
#ifndef TOKIO_ENGINE_ANALYZER_TOKIOANALYZER_H
#define TOKIO_ENGINE_ANALYZER_TOKIOANALYZER_H

#include "BaseAnalyzer.h"

namespace Engine
{

class TokioAnalyzer : public BaseAnalyzer
{
	using BaseAnalyzer::BaseAnalyzer;
private:
	void AnalyzeRegion(
		POINTER address,
		POINTER size,
		const std::vector<byte_t>& buffer,
		size_t bufferOffset,
		size_t& instructionIndex,
		AnalyzedData& data
	) EXCEPT;

	// analyze without symbol
	void AnalyzeRegionNoSymbol(
		POINTER address,
		POINTER size,
		const std::vector<byte_t>& buffer,
		size_t bufferOffset,
		size_t& instructionIndex,
		AnalyzedData& data
	) EXCEPT;

	// analyze without symbol
	void AnalyzeRegionBasic(
		POINTER address,
		POINTER size,
		const std::vector<byte_t>& buffer,
		size_t& instructionIndex,
		AnalyzedData& data
	) EXCEPT;

	void AnalyzeComment(AnalyzedData& data) noexcept;
	void AnalyzeCrossReferences(AnalyzedData& data, size_t numInstructions) noexcept;
	void AnalyzeSubroutines(AnalyzedData& data, size_t numInstructions, const std::vector<byte_t>& buffer);

public:
	// address:		      the virtual address in the target process
	// size:		      the size in bytes to analyze
	// bDisectSubroutine: true to disect subroutine, if false, the subroutines vector will have a size of zero, only the instructions are analyzed
	// outBuffer:		  the output buffer contains read memory
	// outData:			  the output analyzed data	
	_NODISCARD void Analyze(
		POINTER address,
		size_t size,
		AnalyzerFlags flags,
		std::vector<byte_t>& outBuffer,
		AnalyzedData& outData
	) EXCEPT override;

	void FullAnalyze(FullAnalyzeResultCallback callback, const void* UserData) noexcept override;
};


}

#endif // !TOKIO_ENGINE_ANALYZER_TOKIOANALYZER_H
