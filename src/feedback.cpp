#include "include/feedback.h"

Feedback::Feedback(int rate, QString description, QDateTime createdAt, DBTypes::DBIndex id)
    : m_id{id}, m_rate{rate}, m_description{std::move(description)}, m_createdAt{std::move(createdAt)}
{
}

DBTypes::DBIndex Feedback::id() const { return m_id; }
int Feedback::rate() const { return m_rate; }
QString Feedback::description() const { return m_description; }
QDateTime Feedback::createdAt() const { return m_createdAt; }
