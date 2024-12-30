#pragma once

#include <afx.h>
#include <vector>
#include "CTask.h"

class CTaskManager
{
public:
    CTaskManager();

    void AddTask(const CTask& task);
    void RemoveTask(int index);
    CTask& GetTask(int index);
    std::vector<CTask>& GetTasks();
    int AmountOfTasks();
    bool CheckIfIndexInTasks(int index);
    void clear();
    void Serialize(CArchive& ar);


private:
    std::vector<CTask> m_tasks; // вектор со всеми задачами
};