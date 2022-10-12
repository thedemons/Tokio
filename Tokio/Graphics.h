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
#ifndef TOKIO_GRAPHICS_H
#define TOKIO_GRAPHICS_H

#include "Common/Exception.h"
#include "Common/TypeDef.h"

struct ImFont;
typedef void* ImTextureID;

namespace Graphics
{
inline ImFont* FontRegular = nullptr;
inline ImFont* FontBold = nullptr;

inline ImFont* FontMono = nullptr;
inline ImFont* FontMonoBold = nullptr;


void InitWindow() EXCEPT;
void InitDevice() EXCEPT;
void InitImgui() EXCEPT;

void LoadFont() noexcept;
void ReloadFont() noexcept;

void ShowWindow() noexcept;
bool BeginRender() noexcept;
void EndRender() noexcept;
void CleanUp() noexcept;

ImTextureID TextureFromBitmap(const void* pBitmap, unsigned int width, unsigned int height) noexcept NULLABLE;

WindowHandle GetWindow() noexcept NULLABLE;

}

#endif // !TOKIO_GRAPHICINTERFACE_H