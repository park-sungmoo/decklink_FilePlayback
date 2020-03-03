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

#include <functional>
#include <thread>
#include "DeckLinkDeviceDiscovery.h"
#include "DeckLinkOutputDevice.h"
#include "PreviewWindow.h"
#include "ProfileCallback.h"
#include "SourceReader.h"
#include "Resource.h"
#include "DeckLinkAPI_h.h"
#include "atlcomcli.h"

// Custom Messages
#define WM_ADD_DEVICE_MESSAGE					(WM_APP + 1)
#define WM_REMOVE_DEVICE_MESSAGE				(WM_APP + 2)
#define WM_UPDATE_PROFILE_MESSAGE				(WM_APP + 3)
#define WM_UPDATE_STREAM_TIME_MESSAGE			(WM_APP + 4)
#define WM_OUTPUT_ENABLED_MESSAGE				(WM_APP + 5)
#define WM_OUTPUT_DISABLED_MESSAGE				(WM_APP + 6)
#define WM_SCHEDULED_PLAYBACK_STOPPED_MESSAGE	(WM_APP + 7)
#define WM_READ_SAMPLE_ERROR_MESSAGE			(WM_APP + 8)
#define WM_FRAME_DISPLAYED_LATE_MESSAGE			(WM_APP + 9)

// Forward declarations
class DeckLinkDeviceDiscovery;
class DeckLinkOutputDevice;
class PreviewWindow;
class ProfileCallback;


// CFilePlaybackDlg dialog
class CFilePlaybackDlg : public CDialog
{
	// Construction
public:
	explicit CFilePlaybackDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CFilePlaybackDlg() {};

	// Dialog Data
	enum { IDD = IDD_FILEPLAYBACK_DIALOG };

	// Playback state
	enum class PlaybackState { OutputDisabled, OutputEnabled, ScheduledPlayback };

	afx_msg void	OnBnClickedPlayPause();
	afx_msg void	OnBnClickedOpenFile();
	afx_msg void	OnBnClickedDeleteFile();
	afx_msg void	OnBnClickedNextFile();
	afx_msg void	OnLbnDblclkPlay();
	afx_msg void	OnBnClickedResetPlaylist();
	afx_msg void	OnBnClickedLoopCheck();
	afx_msg void	OnBnClickedAutoplayCheck();
	afx_msg void	OnNewDeviceSelected();
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg LRESULT	OnAddDevice(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnRemoveDevice(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnUpdateProfile(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnUpdateStreamTime(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnOutputEnabled(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnOutputDisabled(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnScheduledPlaybackStopped(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnReadSampleError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFrameDisplayedLate(WPARAM wParam, LPARAM lParam);

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
private:
	HICON						m_hIcon;
	//
	MSG							msg;
	HACCEL						m_hAccelTable;
	CButton						m_playPauseButton;
	CButton						m_playNextButton;
	CButton						m_openFileButton;
	CButton						m_deleteFileButton;
	CButton						m_resetPlaylistButton;
	CButton						m_loopButton;
	CButton						m_autoplayButton;
	CButton						m_nextButton;
	CButton						m_aboutButton;
	CComboBox					m_deviceListCombo;
	CListBox					m_playlist;
	CEdit						m_videoFormatEdit;
	CEdit						m_audioChannelEdit;
	CEdit						m_audioBitDepthEdit;
	CEdit						m_fileNameEdit;
	CEdit						m_filePositionEdit;
	CEdit						m_fileDurationEdit;
	CSliderCtrl					m_filePositionSlider;

	CStatic						m_previewBox;
	CComPtr<PreviewWindow>		m_previewWindow;

	int64_t						m_filePosition;
	int64_t						m_fileDuration;
	bool						m_endOfStream;

	CComPtr<DeckLinkOutputDevice> 		m_selectedDevice;
	CComPtr<DeckLinkDeviceDiscovery>	m_deckLinkDiscovery;
	CComPtr<IDeckLinkDisplayMode>		m_selectedDisplayMode;
	BMDVideoOutputFlags					m_selectedVideoOutputFlags;
	CComPtr<ProfileCallback>			m_profileCallback;

	CComPtr<SourceReader>		m_sourceReader;
	std::thread					m_scheduledPlaybackThread;
	std::atomic<PlaybackState>	m_playbackState;

	// Generated message map functions
	virtual BOOL	OnInitDialog();
	afx_msg void	OnPaint();
	afx_msg void	OnClose();
	afx_msg HCURSOR	OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	// File Playback Implementation
	void			UpdateInterface(void);
	void			EnableVideoOutput(void);
	void			DisableVideoOutput(void);
	void			StartScheduledPlayback(void);
	void			StopScheduledPlayback(void);
	void			EndOfStreamSelection(void);
	void			SeekPosition(void);
	BMDDisplayMode	LookupDisplayMode(void);

	void			AddDevice(CComPtr<IDeckLink> deckLink);
	void			RemoveDevice(CComPtr<IDeckLink> deckLink);

public:
	BOOL			m_loopCheck;
	BOOL			m_autoplayCheck;
	afx_msg void	OnEnChangeVideoFormatEdit();
	virtual BOOL	PreTranslateMessage(MSG* pMsg);
	void			LoopCheck();
	void			AutoplayCheck();
	afx_msg void OnBnClickedTestAbout();
};