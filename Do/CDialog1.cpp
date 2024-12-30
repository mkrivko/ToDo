#include "pch.h"
#include "framework.h"
#include "Do.h"
#include "CDialog1.h"
#include "afxdialogex.h"
#include "CTask.h"
#include "DoDoc.h"
#include "DoView.h"

IMPLEMENT_DYNAMIC(CDialog1, CDialogEx)

CDialog1::CDialog1(CWnd* pParent, CDoDoc* pDoc)
    : CDialogEx(IDD_DIALOG1, pParent), m_pDoc(pDoc), m_priority(0)
{
}

CDialog1::~CDialog1()
{
}

// привязка элементов к DDX, установка граничных значений
void CDialog1::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_taskText);
    DDX_Text(pDX, IDC_EDIT2, m_priority);

    DDV_MinMaxInt(pDX, m_priority, 0, 10);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_reminderDate);
}

// обработка кнопки окей и спин контрол
BEGIN_MESSAGE_MAP(CDialog1, CDialogEx)
    ON_BN_CLICKED(IDOK, &CDialog1::OnBnClickedOk)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CDialog1::OnDeltaposSpin)
END_MESSAGE_MAP()

// обработчик кнопки окей
void CDialog1::OnBnClickedOk()
{
    if (UpdateData(TRUE))
    {

        m_pDoc->task_manager.AddTask(CTask(m_taskText, m_priority, false, m_reminderDate));
    }

    CDialogEx::OnOK();
}

// обработчик с спин контрола
void CDialog1::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    int currentPos = m_spinControl.GetPos();
    m_priority = currentPos; // получаем значение со спин контрола и устанавливаем в приоритет

    *pResult = 0; 
}

BOOL CDialog1::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // опредлеение спин и эдит контролов
    m_spinControl.SubclassDlgItem(IDC_SPIN1, this);
    m_editControl.SubclassDlgItem(IDC_EDIT2, this);

    // привязка спин контрола к эдит контролу, установка значений
    m_spinControl.SetBuddy(&m_editControl);
    m_spinControl.SetRange(0, 10);
    m_spinControl.SetPos(0);

    return TRUE;
}
