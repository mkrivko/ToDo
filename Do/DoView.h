#pragma once


class CDoView : public CFormView
{
protected: // create from serialization only
	CDoView() noexcept;
	DECLARE_DYNCREATE(CDoView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_DO_FORM };
#endif

// Attributes
public:
	CDoDoc* GetDocument() const; // указатель на документ
	CListCtrl m_listCtrl; // лист контрол

private:
	bool m_bLockEvents = false; // переменная для блокировки нежелательных ивентов во время обновления интерфейса
	int m_timerID; // айди таймера

// Operations
public:
	void OnLvnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	void ShowTrayNotification(LPCTSTR title, LPCTSTR message);
	void UpdateTaskList();
	void OnTimer(UINT_PTR nIDEvent);
	void SetReminderTimer(CTimeSpan timeDifference);
	void CheckAndSetNextReminder();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);

// Implementation
public:
	virtual ~CDoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // debug version in DoView.cpp
inline CDoDoc* CDoView::GetDocument() const
   { return reinterpret_cast<CDoDoc*>(m_pDocument); }
#endif

