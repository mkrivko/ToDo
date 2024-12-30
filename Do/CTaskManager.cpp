#include "pch.h"
#include "CTaskManager.h"
#include "CTask.h"

// ����� ��� ���������� ��������

CTaskManager::CTaskManager()
{
    m_tasks = {
        //CTask(CString(_T("������")), 0),
        //CTask(CString(_T("������1")), 1),
        //CTask(CString(_T("������2")), 2),
        //CTask(CString(_T("������")), 3),
        //CTask(CString(_T("������")), 4),
        //CTask(CString(_T("������")), 5),
        //CTask(CString(_T("������")), 6),
        //CTask(CString(_T("������")), 7),
        //CTask(CString(_T("������")), 8),
        //CTask(CString(_T("������")), 9),
        //CTask(CString(_T("������")), 10)
    };
}

// ��������� ���������� �����
int CTaskManager::AmountOfTasks() {
    return m_tasks.size();
}

// �������� ���� �� ������ � �������
bool CTaskManager::CheckIfIndexInTasks(int index) {
    return index <= AmountOfTasks();
}

// ���������� ������
void CTaskManager::AddTask(const CTask& task)
{
    m_tasks.push_back(task);
}

// �������� ������
void CTaskManager::RemoveTask(int index)
{
    if (CheckIfIndexInTasks(index))
    {
        m_tasks.erase(m_tasks.begin() + index);
    }
}

// ��������� ������
CTask& CTaskManager::GetTask(int index)
{
    if (CheckIfIndexInTasks(index)) {
        return m_tasks.at(index);
    }
}

// ��������� ���� ����� ��������
std::vector<CTask>& CTaskManager::GetTasks() {
    return m_tasks;
}

// �������� ���� �����
void CTaskManager::clear() {
    m_tasks.clear();
}

void CTaskManager::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // ������
        int taskCount = m_tasks.size();
        ar << taskCount;

        for (auto& task : m_tasks)
        {
            task.Serialize(ar);
        }
    }
    else
    {
        // ����������
        int taskCount;
        ar >> taskCount;
        m_tasks.clear(); // ������� ������� ����� ����� �����������

        for (int i = 0; i < taskCount; ++i)
        {
            CTask task;
            task.Serialize(ar);
            m_tasks.push_back(task);
        }
    }
}