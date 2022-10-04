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
	_NODISCARD common::errcode AnalyzeRegion(
		const MemoryReadRegion& region,
		const std::vector<BYTE>& buffer,
		const size_t bufferOffset,
		size_t& instructionIndex,
		AnalyzedData& data
	);

public:
	_NODISCARD common::errcode Analyze(
		POINTER address,
		size_t size,
		bool bDisectSubroutine,
		std::vector<BYTE>& outBuffer,
		AnalyzedData& outData
	) override;
};


}

#endif // !TOKIO_ENGINE_ANALYZER_TOKIOANALYZER_H