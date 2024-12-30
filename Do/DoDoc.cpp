#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Do.h"
#endif

#include "DoDoc.h"
#include "CTaskManager.h"
#include "CTask.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDoDoc

IMPLEMENT_DYNCREATE(CDoDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoDoc, CDocument)
END_MESSAGE_MAP()


// CDoDoc construction/destruction

CDoDoc::CDoDoc() noexcept
{
	// TODO: add one-time construction code here

}

CDoDoc::~CDoDoc()
{
}

BOOL CDoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	

	return TRUE;
}





void CDoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		task_manager.Serialize(ar);
	}
	else
	{
		task_manager.Serialize(ar);
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CDoDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CDoDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CDoDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CDoDoc diagnostics

#ifdef _DEBUG
void CDoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDoDoc commands


BOOL CDoDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString fileExtension = PathFindExtension(lpszPathName);
	// проверка расширения файла
	if (fileExtension.CompareNoCase(_T(".do")) != 0)
	{
		AfxMessageBox(_T("Неправильное расширение файла. Поддерживается только .do"));
		return FALSE;
	}
	// TODO:  Add your specialized creation code here
	UpdateAllViews(nullptr);

	return TRUE;
}
