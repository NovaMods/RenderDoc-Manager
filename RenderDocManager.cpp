/******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2014 Fredrik Lindh
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
******************************************************************************/

#include "RenderDocManager.h"
#include <string>

RenderDocManager::RenderDocManager(HWND p_Handle, LPCWSTR pRenderDocPath, const char* pCapturePath)
{
	m_Handle = p_Handle;
	m_CaptureStarted = false;

	m_RenderDocDLL = LoadLibrary(pRenderDocPath);
	if (!m_RenderDocDLL)
	{
		if (IsDebuggerPresent())
		{
			__debugbreak();
		}
		return;
	}
	
	pRENDERDOC_GetAPI getApi = (pRENDERDOC_GetAPI)GetProcAddress(m_RenderDocDLL, "RENDERDOC_GetAPI");
	if (!getApi)
	{
		if (IsDebuggerPresent())
		{
			__debugbreak();
		}
		return;
	}
	getApi(eRENDERDOC_API_Version_1_1_1, (void**)&m_renderDocFns);
	if (!m_renderDocFns)
	{
		if (IsDebuggerPresent())
		{
			__debugbreak();
		}
		return;
	}
	m_renderDocFns->SetLogFilePathTemplate(pCapturePath);

	m_renderDocFns->SetFocusToggleKeys(NULL, 0);
	m_renderDocFns->SetCaptureKeys(NULL, 0);

	// Uncomment to define a capture key.
	//RENDERDOC_InputButton captureKey =  eRENDERDOC_Key_F12;
	//m_renderDocFns->SetCaptureKeys(&captureKey, 1);


	m_renderDocFns->SetCaptureOptionU32(eRENDERDOC_Option_CaptureCallstacks, true);
	//m_renderDocFns->SetCaptureOptionU32(eRENDERDOC_Option_CaptureAllCmdLists, true);
	//m_renderDocFns->SetCaptureOptionU32(eRENDERDOC_Option_SaveAllInitials, true);

	// Init remote access.
	//m_SocketPort = 0;
	//m_RenderDocFns->LaunchReplayUI(&m_SocketPort, 0);

	RENDERDOC_OverlayBits overlayBits = eRENDERDOC_Overlay_Default;
	m_renderDocFns->MaskOverlayBits(0, overlayBits);
}

void RenderDocManager::StartFrameCapture()
{
	if (!m_renderDocFns)
	{
		return;
	}
	m_renderDocFns->StartFrameCapture(nullptr, m_Handle);
	m_CaptureStarted = true;
}

// In some cases a capture can fail. It happens when Map() was called before the StartFrameCapture() and then Unmap() is called.
// It also happen if you start recording a command list before the StartFrameCapture() (unless you have the option
// CaptureAllCmdLists enabled).
// In these cases, m_RenderDocEndFrameCapture will return false and start capturing again until a capture succeed, unless 
// m_RenderDocEndFrameCapture is called again.
void RenderDocManager::EndFrameCapture()
{
	if (!m_renderDocFns)
	{
		return;
	}

	if(!m_CaptureStarted)
		return;

	if(m_renderDocFns->EndFrameCapture(nullptr, m_Handle))
	{
		m_CaptureStarted = false;
		return;
	}
	
	OutputDebugString(L"Capture has failed !")
		;
	// The capture has failed, calling m_RenderDocEndFrameCapture several time to make sure it won't keep capturing forever.
	while (!m_renderDocFns->EndFrameCapture(nullptr, m_Handle))
	{
	}

	m_CaptureStarted = false;
	return;
}

RenderDocManager::~RenderDocManager()
{
	FreeLibrary(m_RenderDocDLL);
}

