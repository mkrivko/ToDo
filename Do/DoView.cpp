#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Do.h"
#endif

#include "DoDoc.h"
#include "DoView.h"

#include "CDialog1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDoView

IMPLEMENT_DYNCREATE(CDoView, CFormView)

BEGIN_MESSAGE_MAP(CDoView, CFormView)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CDoView::OnCustomDrawList) // ���������� ��������� ��������� ���� ��������
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CDoView::OnLvnItemChangedList) // ���������� ��������� ��������� ���� �������
	ON_BN_CLICKED(IDC_BUTTON3, &CDoView::OnBnClickedButton3) // ���������� ������ �������
	ON_BN_CLICKED(IDC_BUTTON1, &CDoView::OnBnClickedButton1) // ���������� ������ �������
	ON_BN_CLICKED(IDC_BUTTON2, &CDoView::OnBnClickedButton2) // ���������� ������ ��������
	ON_WM_TIMER() // ���������� ��������
END_MESSAGE_MAP()

// CDoView construction/destruction

CDoView::CDoView() noexcept
	: CFormView(IDD_DO_FORM)
{
	// TODO: add construction code here

}

CDoView::~CDoView()
{
}

// ���������� ���������� ��������� ���� ��������
void CDoView::OnLvnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	// ���� ����� �� �������� �������� ������
	if (m_bLockEvents) {
		*pResult = 0; // ���� ���� ���� �� ���������� ��� �������
		return;
	}
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// �������� ��� ��� ����� ������� � ������
	if ((pNMLV->uChanged & LVIF_STATE) &&
		(pNMLV->uNewState & LVIS_STATEIMAGEMASK) != (pNMLV->uOldState & LVIS_STATEIMAGEMASK))
	{
		int nItem = pNMLV->iItem; // ��������� ������� ��� ���� ��������
		BOOL isChecked = m_listCtrl.GetCheck(nItem); // ��������� �������� ��������

		int taskIndex = static_cast<int>(m_listCtrl.GetItemData(nItem)); // ��������� ������� ��� ��������� �����

		GetDocument()->task_manager.GetTask(taskIndex).ToggleStatus(); // ��������� �� ��������� ��������� ����� � ��������� �������� �������������
	}

	// ���� ���� �����-�� ���������� �������, �� ���������� ������ �������
	if ((pNMLV->uChanged & LVIF_STATE) &&
		(pNMLV->uNewState & LVIS_SELECTED || pNMLV->uOldState & LVIS_SELECTED))
	{
		// �������� ���� �� ���������� ��������
		bool hasSelectedItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED) != -1;


		// ��������� �� ������ ��������
		CWnd* pDeleteButton = GetDlgItem(IDC_BUTTON1);
		if (pDeleteButton)
		{
			// ��������� ��� ������ disabled 
			DWORD dwStyle = pDeleteButton->GetStyle();
			if (hasSelectedItem)
			{
				// ���� ���� ���������� ��������, ���������� ������
				if (dwStyle & WS_DISABLED) // ���� ������ ���������
				{
					pDeleteButton->ModifyStyle(WS_DISABLED, 0); // ������� WS_DISABLED
					pDeleteButton->RedrawWindow(); // �������������� ������
				}
			}
			else
			{
				// ���� ���������� ��������� ���, ������������ ������
				if (!(dwStyle & WS_DISABLED)) // ���� ������ �������
				{
					pDeleteButton->ModifyStyle(0, WS_DISABLED); // ��������� WS_DISABLED
					pDeleteButton->RedrawWindow(); // �������������� ������
				}
			}
		}
	}

	*pResult = 0;
}

// ���������� ��������� ���� ��������
void CDoView::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW; // ����������� ����������� ��� ������� ��������
		return;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW; // ����������� ����������� ��� ������� �����������
		return;

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	{
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		// �������� ������ ������, ��������� � ���� ���������
		int taskIndex = static_cast<int>(m_listCtrl.GetItemData(nItem));
		std::vector<CTask>& tasks = GetDocument()->task_manager.GetTasks();

		// ���� ������ ����������
		if (taskIndex >= 0 && taskIndex < tasks.size()) {
			CTask& task = tasks[taskIndex];

			// ��������� ���������� � ������ ���������
			int priority = task.GetPriority();
			int contrast = min(255, priority * 25);

			// ���� ��������� 0 �� ����� ���, ������ �����
			if(priority == 0){
				pLVCD->clrText = RGB(0, 0, 0);
				pLVCD->clrTextBk = RGB(255, 255, 255);
			}
			// ���� ��������� 10 �� ������ ���, ����� �����
			else if(priority == 10){
				pLVCD->clrText = RGB(255, 255, 255);
				pLVCD->clrTextBk = RGB(0, 0, 0);
			}
			// � ���������� ������� ������������� ������� ������ �� ������ �� ����������
			else if (priority <= 5) {
				int blueValue = max(150, 255-contrast);
				int textColor = 0;
				pLVCD->clrText = RGB(textColor, textColor, textColor);
				pLVCD->clrTextBk = RGB(int(blueValue / 2), blueValue, blueValue);
			}
			else {
				int blueValue = min(150, 255 - contrast);
				int textColor = 255;
				pLVCD->clrText = RGB(textColor, textColor, textColor);
				pLVCD->clrTextBk = RGB(0, int(blueValue / 2), blueValue);
			}
		}
	}
	*pResult = CDRF_DODEFAULT;
	return;
	}

	*pResult = 0;
}


// DDX ��� ���� ��������
void CDoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

// ��������� ������ ����
BOOL CDoView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_OVERLAPPEDWINDOW;
	cs.style &= ~WS_THICKFRAME;
	return CFormView::PreCreateWindow(cs);
}

// ������� ��� ���������� ��������� � ���������� �����
void CDoView::UpdateTaskList()
{
	// ��������� ��������� ������� �� ����� ���������
	m_bLockEvents = true;

	// ��������� ������� �����
	std::vector<CTask>& tasks = GetDocument()->task_manager.GetTasks();

	// ������� ���� �������� ����� ����������
	m_listCtrl.DeleteAllItems();

	// ���������� ����� � �����������
	for (size_t i = 0; i < tasks.size(); ++i) {
		CTask& task = tasks[i];
		int nIndex = m_listCtrl.InsertItem(static_cast<int>(i), task.GetText());
		m_listCtrl.SetItemData(nIndex, static_cast<DWORD_PTR>(i)); // ������ �������� � ����������
		m_listCtrl.SetCheck(nIndex, task.GetStatus()); // ��������� ��������� ��������
	}

	// �������������� �������
	m_bLockEvents = false;
	// �������� � ��������� �����������
	CheckAndSetNextReminder();
}

// �������� ���������� ��� ��� ����������� � ��������� ����������
void CDoView::CheckAndSetNextReminder()
{
	// ��������� �����, ������������� ���������� ��� ������ ���������� �������
	auto& tasks = GetDocument()->task_manager.GetTasks();
	CTime currentTime = CTime::GetCurrentTime();
	CTime nearestReminderTime;
	bool foundNextReminder = false;

	// TRACE(_T("������� �����: %s\n"), currentTime.Format(_T("%Y-%m-%d %H:%M:%S")));

	// ������ � ����� �� �������
	for (auto& task : tasks)
	{
		//TRACE(_T("������: %s | �����������: %s | ������: %d\n"),
		//	task.GetText(),
		//	task.GetReminderDate().Format(_T("%Y-%m-%d %H:%M:%S")),
		//	task.GetStatus());

		// �������� ������ ������������� �����, ����� ����������� �������
		if (!task.GetStatus() && task.GetReminderDate() > currentTime)
		{
			// ���� ��� ����������� ��� ������� ����� �����, �� �������� �� � ����������
			if (!foundNextReminder || task.GetReminderDate() < nearestReminderTime)
			{
				nearestReminderTime = task.GetReminderDate();
				foundNextReminder = true;
			}
		}
	}

	// ���� ������� ��������� �����������, ��������� ������ � �������� �������� ������� � �������
	if (foundNextReminder)
	{
		//TRACE(_T("��������� �����������: %s\n"), nearestReminderTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		SetReminderTimer(nearestReminderTime - currentTime);
	}
}

// ��������� �������
void CDoView::SetReminderTimer(CTimeSpan timeDifference)
{
	// �������� ������ �� �� ����� ������ ������ ��� ��� 
	if (m_timerID != 0)
	{
		// �������� �������
		KillTimer(m_timerID);
		m_timerID = 0;
	}

	// �������������� ������� � ������������
	DWORD dwMilliseconds = static_cast<DWORD>(timeDifference.GetTotalSeconds() * 1000);

	if (dwMilliseconds > 0)
	{
		// ������ ����������
		m_timerID = SetTimer(1, dwMilliseconds, nullptr);
		//TRACE(_T("������ ���������� �� %lld �����������.\n"), dwMilliseconds);
	}
	else
	{
		//TRACE(_T("������: ������������� ��� ������� ������� �� �������.\n"));
		AfxMessageBox(_T("����� ��� ����������� �����������."));
	}
}




void CDoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	CWnd* pParentWnd = GetParentFrame();
	ResizeParentToFit();
	if (pParentWnd)
	{
		// ���������� ��������� �������
		pParentWnd->ModifyStyle(WS_THICKFRAME, 0, SWP_FRAMECHANGED);
		// ���������� ������ ��������� �������
		pParentWnd->ModifyStyle(WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);
	}


	// ���������� ������ ���������� � ����
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATA); // ������ ���������
	nid.hWnd = GetParentFrame()->GetSafeHwnd(); // ������� ����
	nid.uID = 1; // ���� ������
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER + 1;
	// �������� ������ �� ��������
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(nid.szTip, _T("Do"));
	// ���������� � ����
	Shell_NotifyIcon(NIM_ADD, &nid);
	// �������������� ����������
	ShowTrayNotification(_T("����� ����������!"), _T("����������� �� ��������� ������� ����� ������������ �����."));

	// ��������� ������� ��� ���� �������
	m_listCtrl.InsertColumn(0, _T("������"), LVCFMT_LEFT, 200);
	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER); // ������ ������� ��� ����� ���� ��������

	// ������ ���� �������� � ��������� �����
	UpdateTaskList();
}

// ����������� ������� ��������� � ���������� windows
void CDoView::ShowTrayNotification(LPCTSTR title, LPCTSTR message)
{
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATA); // ������ ���������
	nid.hWnd = GetParentFrame()->GetSafeHwnd(); // ������� ����
	nid.uID = 1; // ���� ������

	nid.uFlags = NIF_INFO; // ���� ������������ �����������
	_tcscpy_s(nid.szInfoTitle, title);  // ��������� ���������� �����������
	_tcscpy_s(nid.szInfo, message);    // ��������� ������ �����������
	nid.dwInfoFlags = NIIF_INFO;       // ������ ����������� (i)

	Shell_NotifyIcon(NIM_MODIFY, &nid); // ����������� �����������
}

// CDoView diagnostics

#ifdef _DEBUG
void CDoView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDoDoc* CDoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoDoc)));
	return (CDoDoc*)m_pDocument;
}
#endif //_DEBUG


// CDoView message handlers

// ��������� ������ ��� �������� �����
void CDoView::OnBnClickedButton3()
{
	CDoDoc* pDoc = GetDocument();
	if (pDoc == nullptr)
	{
		AfxMessageBox(_T("�� ������� �������� ��������!"));
		return;
	}

	// ������ � ���������� ��������� ����, ��������� � ����������� ��������� �� ��������
	CDialog1 dlg(this, pDoc);
	if (dlg.DoModal() == IDOK) // ���� ���� ��������� ������, �� ������ ��������� � ���� �������
	{
		UpdateTaskList(); 
	}
}


// ������� ������ � ��������� ��������
void CDoView::OnBnClickedButton1()
{
	// ��������� ������� ����������� ��������
	int selectedItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItem == -1)
	{
		AfxMessageBox(_T("�������� ������ ��� ��������"));
		return;
	}

	// ��������� ������� ������ �� ������
	int taskIndex = static_cast<int>(m_listCtrl.GetItemData(selectedItem));

	// �������� ������ �� ��������� �����
	CDoDoc* pDoc = GetDocument();
	if (pDoc != nullptr)
	{
		CTaskManager &task_manager = pDoc->task_manager;
		if (task_manager.CheckIfIndexInTasks(taskIndex))
		{
			task_manager.RemoveTask(taskIndex);
			UpdateTaskList();
		}
	}
}

// ���������� ������ ��� ������� ��������� �����
void CDoView::OnBnClickedButton2()
{
	CDoDoc* pDoc = GetDocument();
	pDoc->task_manager.clear();
	UpdateTaskList();
}


// ��� ��������� ������� �� ���������� ��������� ������
void CDoView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateTaskList();
}

// ���������� ��������� ������� �������
void CDoView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)  // ���������, ��� ��� ��� ������
	{
		// ����������� ������������ � ��� ������
		ShowTrayNotification(_T("�����������"), _T("� ��� ���� ������������� ������"));

		// ��������� �������
		KillTimer(1);
	}

	CFormView::OnTimer(nIDEvent);
}