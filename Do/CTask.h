#pragma once

#include <afx.h>
#include <vector>
#include <iostream>
#include <optional>

class CTask
{
public:
    CTask(const CString& text = CString(_T("")), int priority = 0, bool status = false, const CTime& reminderDate = CTime());

    CString GetText() const;
    bool GetStatus() const;
    int GetPriority() const;
    CTime GetReminderDate() const;

    void SetText(const CString& text);
    void SetStatus(bool status);
    void SetPriority(int priority);
    void SetReminderDate(const CTime& reminderDate);

    void ToggleStatus();

    virtual void Serialize(CArchive& ar);

private:
    CString m_text; // текст задачи
    bool m_status; // статус задачи
    int m_priority; // приоритет задачи
    CTime m_reminderDate; // время напоминания
};
