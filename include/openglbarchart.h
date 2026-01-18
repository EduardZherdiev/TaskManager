#pragma once

#include <QQuickFramebufferObject>
#include <QColor>

// QQuickFramebufferObject-backed bar chart rendered with OpenGL
class OpenGLBarChart : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(int completed READ completed WRITE setCompleted NOTIFY valuesChanged)
    Q_PROPERTY(int inProgress READ inProgress WRITE setInProgress NOTIFY valuesChanged)
    Q_PROPERTY(int archived READ archived WRITE setArchived NOTIFY valuesChanged)
    Q_PROPERTY(QColor completedColor READ completedColor WRITE setCompletedColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor inProgressColor READ inProgressColor WRITE setInProgressColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor archivedColor READ archivedColor WRITE setArchivedColor NOTIFY colorsChanged)

public:
    explicit OpenGLBarChart(QQuickItem *parent = nullptr);

    Renderer *createRenderer() const override;

    int completed() const { return m_completed; }
    int inProgress() const { return m_inProgress; }
    int archived() const { return m_archived; }
    
    QColor completedColor() const { return m_completedColor; }
    QColor inProgressColor() const { return m_inProgressColor; }
    QColor archivedColor() const { return m_archivedColor; }

public slots:
    void setCompleted(int value);
    void setInProgress(int value);
    void setArchived(int value);
    void setCompletedColor(const QColor &color);
    void setInProgressColor(const QColor &color);
    void setArchivedColor(const QColor &color);

signals:
    void valuesChanged();
    void colorsChanged();

private:
    void requestRepaint();

    int m_completed{0};
    int m_inProgress{0};
    int m_archived{0};
    QColor m_completedColor{0x4CAF50};
    QColor m_inProgressColor{0xFFC107};
    QColor m_archivedColor{0xF44336};
};
