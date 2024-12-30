#include "pch.h"
#include "CTask.h"
#include <stdexcept>

//  ласс задачи CTask

CTask::CTask(const CString& text, int priority, bool status, const CTime& reminderDate)
    : m_text(text),
    m_status(status),
    m_priority(priority >= 0 && priority <= 10 ? priority : 0)
{
    CTime currentDate = CTime::GetCurrentTime();

    m_reminderDate = CTime(currentDate.GetYear(), currentDate.GetMonth(), currentDate.GetDay(),
        reminderDate.GetHour(), reminderDate.GetMinute(), reminderDate.GetSecond());
}

// геттеры дл€ получени€ атрибутов

CString CTask::GetText() const
{
    return m_text;
}

bool CTask::GetStatus() const
{
    return m_status;
}

int CTask::GetPriority() const
{
    return m_priority;
}

CTime CTask::GetReminderDate() const
{
    return m_reminderDate;
}

// сеттеры дл€ выставлени€ нужных значений

void CTask::SetText(const CString& text)
{
    m_text = text;
}

void CTask::SetStatus(bool status)
{
    m_status = status;
}

void CTask::SetPriority(int priority)
{
    if (priority >= 0 && priority <= 10)
    {
        m_priority = priority;
    }
}

void CTask::SetReminderDate(const CTime& reminderDate)
{
    m_reminderDate = reminderDate;
}

// переключение статуса чекбокса
void CTask::ToggleStatus()
{
    m_status = !m_status;
}

// сериализаци€
void CTask::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // сохранение текста, статуса, приоритета, времени задачи
        ar << m_text << m_status << m_priority << m_reminderDate.GetHour() << m_reminderDate.GetMinute();
    }
    else
    {
        // считывание текста, статуса, проиртитета, времени
        int hour, minute;
        ar >> m_text >> m_status >> m_priority >> hour >> minute;

        m_reminderDate = CTime(m_reminderDate.GetYear(), m_reminderDate.GetMonth(), m_reminderDate.GetDay(),
            hour, minute, 0);
    }
}
