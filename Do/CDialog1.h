#pragma once
#include "afxdtctl.h"

class CDoDoc;

class CDialog1 : public CDialogEx
{
    DECLARE_DYNAMIC(CDialog1)

public:
    CDialog1(CWnd* pParent = nullptr, CDoDoc* pDoc = nullptr);
    virtual ~CDialog1();

    CDoDoc* m_pDoc;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG1 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

private:
    CString m_taskText; // текст задачи
    int m_priority; // приоритет задачи
    CTime m_reminderDate; // время для напоминания
    CEdit m_editControl; // эдит контрол приоритета
    CSpinButtonCtrl m_spinControl; // спин контрол приоритета

public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL OnInitDialog();
};
