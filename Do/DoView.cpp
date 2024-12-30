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
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CDoView::OnCustomDrawList) // обработчик рисования элементов лист контрола
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CDoView::OnLvnItemChangedList) // обработчик изменения элементов лист контрол
	ON_BN_CLICKED(IDC_BUTTON3, &CDoView::OnBnClickedButton3) // обработчик кнопки создать
	ON_BN_CLICKED(IDC_BUTTON1, &CDoView::OnBnClickedButton1) // обработчик кнопки удалить
	ON_BN_CLICKED(IDC_BUTTON2, &CDoView::OnBnClickedButton2) // обработчик кнопки очистить
	ON_WM_TIMER() // обработчик таймеров
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

// обработчик обновления элементов лист контрола
void CDoView::OnLvnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	// флаг чтобы не получить ненужные ивенты
	if (m_bLockEvents) {
		*pResult = 0; // если флаг есть то игнорируем это событие
		return;
	}
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// проверка что был нажат чекбокс у задачи
	if ((pNMLV->uChanged & LVIF_STATE) &&
		(pNMLV->uNewState & LVIS_STATEIMAGEMASK) != (pNMLV->uOldState & LVIS_STATEIMAGEMASK))
	{
		int nItem = pNMLV->iItem; // получение индекса для лист контрола
		BOOL isChecked = m_listCtrl.GetCheck(nItem); // получение значения чекбокса

		int taskIndex = static_cast<int>(m_listCtrl.GetItemData(nItem)); // получение индекса для менеджера задач

		GetDocument()->task_manager.GetTask(taskIndex).ToggleStatus(); // получение из документа менеджера задач и изменение значения переключателя
	}

	// если есть какой-то выделенный элемент, то подсветить кнопку удалить
	if ((pNMLV->uChanged & LVIF_STATE) &&
		(pNMLV->uNewState & LVIS_SELECTED || pNMLV->uOldState & LVIS_SELECTED))
	{
		// проверка есть ли выделенные эдементы
		bool hasSelectedItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED) != -1;


		// указатель на кнопку удаления
		CWnd* pDeleteButton = GetDlgItem(IDC_BUTTON1);
		if (pDeleteButton)
		{
			// установка или снятие disabled 
			DWORD dwStyle = pDeleteButton->GetStyle();
			if (hasSelectedItem)
			{
				// если есть выделенные элементы, активируем кнопку
				if (dwStyle & WS_DISABLED) // если кнопка отключена
				{
					pDeleteButton->ModifyStyle(WS_DISABLED, 0); // убираем WS_DISABLED
					pDeleteButton->RedrawWindow(); // перерисовываем кнопку
				}
			}
			else
			{
				// если выделенных элементов нет, деактивируем кнопку
				if (!(dwStyle & WS_DISABLED)) // если кнопка активна
				{
					pDeleteButton->ModifyStyle(0, WS_DISABLED); // добавляем WS_DISABLED
					pDeleteButton->RedrawWindow(); // перерисовываем кнопку
				}
			}
		}
	}

	*pResult = 0;
}

// обработчик отрисовки лист контрола
void CDoView::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW; // запрашиваем уведомления для каждого элемента
		return;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW; // запрашиваем уведомления для каждого подэлемента
		return;

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	{
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		// получаем индекс задачи, связанной с этим элементом
		int taskIndex = static_cast<int>(m_listCtrl.GetItemData(nItem));
		std::vector<CTask>& tasks = GetDocument()->task_manager.GetTasks();

		// если индекс правильный
		if (taskIndex >= 0 && taskIndex < tasks.size()) {
			CTask& task = tasks[taskIndex];

			// получение приоритета и расчёт контраста
			int priority = task.GetPriority();
			int contrast = min(255, priority * 25);

			// если приоритет 0 то белый фон, черный текст
			if(priority == 0){
				pLVCD->clrText = RGB(0, 0, 0);
				pLVCD->clrTextBk = RGB(255, 255, 255);
			}
			// если приоритет 10 то черный фон, белый текст
			else if(priority == 10){
				pLVCD->clrText = RGB(255, 255, 255);
				pLVCD->clrTextBk = RGB(0, 0, 0);
			}
			// в оставшихся случаях высчитываются оттенки синего на основе их приоритета
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


// DDX для лист контрола
void CDoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

// установка стилей окна
BOOL CDoView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_OVERLAPPEDWINDOW;
	cs.style &= ~WS_THICKFRAME;
	return CFormView::PreCreateWindow(cs);
}

// функция для связывания отрисовки с менеджером задач
void CDoView::UpdateTaskList()
{
	// блокируем обработку событий во время отрисовки
	m_bLockEvents = true;

	// получение вектора задач
	std::vector<CTask>& tasks = GetDocument()->task_manager.GetTasks();

	// очистка лист контрола перед отрисовкой
	m_listCtrl.DeleteAllItems();

	// добавление задач в листконтрол
	for (size_t i = 0; i < tasks.size(); ++i) {
		CTask& task = tasks[i];
		int nIndex = m_listCtrl.InsertItem(static_cast<int>(i), task.GetText());
		m_listCtrl.SetItemData(nIndex, static_cast<DWORD_PTR>(i)); // связка индексов с элементами
		m_listCtrl.SetCheck(nIndex, task.GetStatus()); // установка состояния чекбокса
	}

	// разрблокировка событий
	m_bLockEvents = false;
	// проверка и установка напоминаний
	CheckAndSetNextReminder();
}

// проверка актуальных дат для напоминаний и установка напомиания
void CDoView::CheckAndSetNextReminder()
{
	// получение задач, инициализация переменных для поиска ближайшего времени
	auto& tasks = GetDocument()->task_manager.GetTasks();
	CTime currentTime = CTime::GetCurrentTime();
	CTime nearestReminderTime;
	bool foundNextReminder = false;

	// TRACE(_T("Текущее время: %s\n"), currentTime.Format(_T("%Y-%m-%d %H:%M:%S")));

	// проход в цикле по задачам
	for (auto& task : tasks)
	{
		//TRACE(_T("Задача: %s | Напоминание: %s | Статус: %d\n"),
		//	task.GetText(),
		//	task.GetReminderDate().Format(_T("%Y-%m-%d %H:%M:%S")),
		//	task.GetStatus());

		// проверка только невыполненных задач, поиск актуального времени
		if (!task.GetStatus() && task.GetReminderDate() > currentTime)
		{
			// если нет напоминания или нашлось время ближе, то записать их в переменную
			if (!foundNextReminder || task.GetReminderDate() < nearestReminderTime)
			{
				nearestReminderTime = task.GetReminderDate();
				foundNextReminder = true;
			}
		}
	}

	// если найдено ближайшее напоминание, запускаем таймер с разницей текущего времени и нужного
	if (foundNextReminder)
	{
		//TRACE(_T("Ближайшее напоминание: %s\n"), nearestReminderTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		SetReminderTimer(nearestReminderTime - currentTime);
	}
}

// установка таймера
void CDoView::SetReminderTimer(CTimeSpan timeDifference)
{
	// проверка создан ли до этого другой таймер или нет 
	if (m_timerID != 0)
	{
		// удаление таймера
		KillTimer(m_timerID);
		m_timerID = 0;
	}

	// преорбазование времени в миллисекунды
	DWORD dwMilliseconds = static_cast<DWORD>(timeDifference.GetTotalSeconds() * 1000);

	if (dwMilliseconds > 0)
	{
		// таймер установлен
		m_timerID = SetTimer(1, dwMilliseconds, nullptr);
		//TRACE(_T("Таймер установлен на %lld миллисекунд.\n"), dwMilliseconds);
	}
	else
	{
		//TRACE(_T("Ошибка: отрицательная или нулевая разница во времени.\n"));
		AfxMessageBox(_T("Время для напоминания некорректно."));
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
		// запрещение изменение размера
		pParentWnd->ModifyStyle(WS_THICKFRAME, 0, SWP_FRAMECHANGED);
		// отключение кнопок изменения размера
		pParentWnd->ModifyStyle(WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);
	}


	// добавление значка приложения в трей
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATA); // размер структуры
	nid.hWnd = GetParentFrame()->GetSafeHwnd(); // главное окно
	nid.uID = 1; // айди значка
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER + 1;
	// загрузка иконки из ресурвов
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(nid.szTip, _T("Do"));
	// добавление в трей
	Shell_NotifyIcon(NIM_ADD, &nid);
	// приветственное оповещение
	ShowTrayNotification(_T("Добро пожаловать!"), _T("Напоминания по созданным задачам будут показываться здесь."));

	// настройка колонок для лист контрол
	m_listCtrl.InsertColumn(0, _T("Задача"), LVCFMT_LEFT, 200);
	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER); // ширина колонки под хедер лист контрола

	// связка лист контрола и менеджера задач
	UpdateTaskList();
}

// отображение нужного сообщения в оповещения windows
void CDoView::ShowTrayNotification(LPCTSTR title, LPCTSTR message)
{
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(NOTIFYICONDATA); // размер структуры
	nid.hWnd = GetParentFrame()->GetSafeHwnd(); // главное окно
	nid.uID = 1; // айди значка

	nid.uFlags = NIF_INFO; // флаг всплывающего уведомления
	_tcscpy_s(nid.szInfoTitle, title);  // установка заголовока уведомления
	_tcscpy_s(nid.szInfo, message);    // установка текста уведомления
	nid.dwInfoFlags = NIIF_INFO;       // значок уведомления (i)

	Shell_NotifyIcon(NIM_MODIFY, &nid); // отображение уведомления
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

// обработка кнопки для создания задач
void CDoView::OnBnClickedButton3()
{
	CDoDoc* pDoc = GetDocument();
	if (pDoc == nullptr)
	{
		AfxMessageBox(_T("Не удалось получить документ!"));
		return;
	}

	// создаём и подгружаем модальное окно, передавая в конструктор указатель на документ
	CDialog1 dlg(this, pDoc);
	if (dlg.DoModal() == IDOK) // если была добавлена задача, то вносим изменения в лист контрол
	{
		UpdateTaskList(); 
	}
}


// нажатие кнопки с удалением элемента
void CDoView::OnBnClickedButton1()
{
	// получение индекса выделенного элемента
	int selectedItem = m_listCtrl.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItem == -1)
	{
		AfxMessageBox(_T("Выберите задачу для удаления"));
		return;
	}

	// получение индекса задачи из списка
	int taskIndex = static_cast<int>(m_listCtrl.GetItemData(selectedItem));

	// удаление задачи из менеджера задач
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

// обработчик кнопки для очистки менеджера задач
void CDoView::OnBnClickedButton2()
{
	CDoDoc* pDoc = GetDocument();
	pDoc->task_manager.clear();
	UpdateTaskList();
}


// при получении запроса на обновление обновляем задачи
void CDoView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateTaskList();
}

// обработчик истечения времени таймера
void CDoView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)  // Проверяем, что это наш таймер
	{
		// напоминание пользователю о его задаче
		ShowTrayNotification(_T("Напоминание"), _T("У вас есть невыполненные задачи"));

		// остановка таймера
		KillTimer(1);
	}

	CFormView::OnTimer(nIDEvent);
}