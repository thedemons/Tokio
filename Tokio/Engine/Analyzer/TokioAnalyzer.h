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
