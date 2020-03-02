/* -LICENSE-START-
** Copyright (c) 2019 Blackmagic Design
**
** Permission is hereby granted, free of charge, to any person or organization
** obtaining a copy of the software and accompanying documentation covered by
** this license (the "Software") to use, reproduce, display, distribute,
** execute, and transmit the Software, and to prepare derivative works of the
** Software, and to permit third-parties to whom the Software is furnished to
** do so, all subject to the following:
**
** The copyright notices in the Software and this entire statement, including
** the above license grant, this restriction and the following disclaimer,
** must be included in all copies of the Software, in whole or in part, and
** all derivative works of the Software, unless such copies or derivative
** works are solely in the form of machine-executable object code generated by
** a source language processor.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
** -LICENSE-END-
*/

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include "FilePlaybackDlg.h"
#include "PlaybackVideoFrame.h"
#include "PlaybackAudioPacket.h"
#include "SourceReader.h"
#include "DeckLinkAPI_h.h"

class CFilePlaybackDlg;

class DeckLinkOutputDevice : public IDeckLinkVideoOutputCallback, public IDeckLinkAudioOutputCallback, public IDeckLinkNotificationCallback
{
	using UpdateStreamTimeCallback				= std::function<void(void)>;
	using ScheduledPlaybackStoppedCallback		= std::function<void(bool)>;
	using OutputStateChangeCallback				= std::function<void(bool)>;
	using FrameDisplayedLateCallback			= std::function<void(void)>;
	using DeckLinkDisplayModeQueryFunc			= std::function<void(CComPtr<IDeckLinkDisplayMode>&)>;

public:
	DeckLinkOutputDevice(CComPtr<IDeckLink> deckLink);
	virtual ~DeckLinkOutputDevice() {}

	bool								Initialize(void);
	bool								Uninitialize(void);

	// IUnknown
	virtual HRESULT	__stdcall			QueryInterface(REFIID iid, LPVOID *ppv) override;
	virtual ULONG	__stdcall			AddRef() override;
	virtual ULONG	__stdcall			Release() override;

	// IDeckLinkVideoOutputCallback
	virtual HRESULT	__stdcall			ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult result) override;
	virtual HRESULT	__stdcall			ScheduledPlaybackHasStopped() override;

	// IDeckLinkAudioOutputCallback
	virtual HRESULT	__stdcall			RenderAudioSamples(BOOL preroll) override;

	// IDeckLinkNotificationCallback
	virtual HRESULT __stdcall			Notify(BMDNotifications topic, ULONGLONG param1, ULONGLONG param2) override;

	HRESULT								GetDeviceName(CString& deviceName);
	CComPtr<IDeckLink>					GetDeckLinkInstance(void) const { return m_deckLink; }
	CComPtr<IDeckLinkProfileManager>	GetDeviceProfileManager(void) const { return m_deckLinkProfileManager; }
	HRESULT								GetDisplayModeName(CString& displayModeName);
	BMDTimeValue						GetCurrentStreamTime(BMDTimeScale timescale);

	bool								EnableOutput(BMDDisplayMode displayMode, BMDPixelFormat pixelFormat, BMDAudioSampleType audioSampleType, uint32_t audioChannelCount, IDeckLinkScreenPreviewCallback* screenPreviewCallback);
	void								DisableOutput();

	void								SchedulePlaybackThread(CComPtr<SourceReader>& sourceReader);
	void								StopScheduledPlayback(void);
	bool								DisplayPreviewFrame(CComPtr<PlaybackVideoFrame>& videoFrame, bool endOfStream);
	void								QueryDisplayModes(std::function<void(CComPtr<IDeckLinkDisplayMode>&)> func);

	void								OnUpdateStreamTime(const UpdateStreamTimeCallback& callback) { m_updateStreamTimeCallback = callback; }
	void								OnScheduledPlaybackStopped(const ScheduledPlaybackStoppedCallback& callback) { m_scheduledPlaybackStoppedCallback = callback; }
	void								OnOutputStateChanged(const OutputStateChangeCallback& callback) { m_outputStateChangeCallback = callback; }
	void								OnFrameDisplayedLate(const FrameDisplayedLateCallback& callback) { m_frameDisplayedLateCallback = callback; }

private:
	std::atomic<ULONG>					m_refCount;
	
	CComPtr<IDeckLink>					m_deckLink;
	CComQIPtr<IDeckLinkOutput>			m_deckLinkOutput;
	CComQIPtr<IDeckLinkProfileManager>	m_deckLinkProfileManager;
	CComQIPtr<IDeckLinkStatus>			m_deckLinkStatus;
	CComQIPtr<IDeckLinkNotification>	m_deckLinkNotification;
	CComPtr<IDeckLinkVideoConversion>	m_deckLinkFrameConverter;

	BMDTimeValue						m_frameDuration;
	BMDTimeScale						m_frameTimescale;
	BMDTimeValue						m_scheduledStartTime;
	BMDTimeValue						m_scheduledStopTime;
	BMDTimeValue						m_currentStreamTime;
	bool								m_outputWithConversion;
	bool								m_prerollingAudio;
	bool								m_endOfStream;

	uint32_t							m_bufferedAudioSampleCount;
	uint32_t							m_bufferedVideoFrameCount;
	uint32_t							m_bufferedAudioWaterLevel;
	uint32_t							m_bufferedVideoWaterLevel;

	UpdateStreamTimeCallback			m_updateStreamTimeCallback;
	ScheduledPlaybackStoppedCallback	m_scheduledPlaybackStoppedCallback;
	OutputStateChangeCallback			m_outputStateChangeCallback;
	FrameDisplayedLateCallback			m_frameDisplayedLateCallback;

	bool								m_stopPlayback;
	bool								m_scheduledPlaybackStopped;

	std::mutex							m_playbackMutex;
	std::condition_variable				m_scheduleAudioPacketCondition;
	std::condition_variable				m_scheduleVideoFrameCondition;

	bool								ScheduleNextVideoFrame(CComPtr<PlaybackVideoFrame>& videoFrame, bool endOfStream);
	bool								ScheduleNextAudioPacket(CComPtr<PlaybackAudioPacket>& audioPacket, bool endOfStream);

	void								ScheduleVideoFramesThread(CComPtr<SourceReader>& sourceReader);
	void								ScheduleAudioSamplesThread(CComPtr<SourceReader>& sourceReader);

	bool								GetOutputVideoFrame(CComPtr<PlaybackVideoFrame>& videoFrame, IDeckLinkVideoFrame** outputVideoFrame);
};
