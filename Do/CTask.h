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
    CString m_text; // ����� ������
    bool m_status; // ������ ������
    int m_priority; // ��������� ������
    CTime m_reminderDate; // ����� �����������
};
