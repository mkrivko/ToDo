#include "pch.h"
#include "CTaskManager.h"
#include "CTask.h"

// класс для управления задачами

CTaskManager::CTaskManager()
{
    m_tasks = {
        //CTask(CString(_T("Задача")), 0),
        //CTask(CString(_T("Задача1")), 1),
        //CTask(CString(_T("Задача2")), 2),
        //CTask(CString(_T("Задача")), 3),
        //CTask(CString(_T("Задача")), 4),
        //CTask(CString(_T("Задача")), 5),
        //CTask(CString(_T("Задача")), 6),
        //CTask(CString(_T("Задача")), 7),
        //CTask(CString(_T("Задача")), 8),
        //CTask(CString(_T("Задача")), 9),
        //CTask(CString(_T("Задача")), 10)
    };
}

// получение количества задач
int CTaskManager::AmountOfTasks() {
    return m_tasks.size();
}

// проверка есть ли индекс в задачах
bool CTaskManager::CheckIfIndexInTasks(int index) {
    return index <= AmountOfTasks();
}

// добавление задачи
void CTaskManager::AddTask(const CTask& task)
{
    m_tasks.push_back(task);
}

// удаление задачи
void CTaskManager::RemoveTask(int index)
{
    if (CheckIfIndexInTasks(index))
    {
        m_tasks.erase(m_tasks.begin() + index);
    }
}

// поулчение задачи
CTask& CTaskManager::GetTask(int index)
{
    if (CheckIfIndexInTasks(index)) {
        return m_tasks.at(index);
    }
}

// получение всех задач вектором
std::vector<CTask>& CTaskManager::GetTasks() {
    return m_tasks;
}

// удаление всех задач
void CTaskManager::clear() {
    m_tasks.clear();
}

void CTaskManager::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // запись
        int taskCount = m_tasks.size();
        ar << taskCount;

        for (auto& task : m_tasks)
        {
            task.Serialize(ar);
        }
    }
    else
    {
        // считывание
        int taskCount;
        ar >> taskCount;
        m_tasks.clear(); // очистка текущих задач перед считыванием

        for (int i = 0; i < taskCount; ++i)
        {
            CTask task;
            task.Serialize(ar);
            m_tasks.push_back(task);
        }
    }
}