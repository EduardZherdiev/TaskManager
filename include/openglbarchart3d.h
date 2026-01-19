#pragma once

#include <QQuickFramebufferObject>
#include <QColor>

class OpenGLBarChart3D : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(int completed READ completed WRITE setCompleted NOTIFY valuesChanged)
    Q_PROPERTY(int inProgress READ inProgress WRITE setInProgress NOTIFY valuesChanged)
    Q_PROPERTY(int archived READ archived WRITE setArchived NOTIFY valuesChanged)
    Q_PROPERTY(QColor completedColor READ completedColor WRITE setCompletedColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor inProgressColor READ inProgressColor WRITE setInProgressColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor archivedColor READ archivedColor WRITE setArchivedColor NOTIFY colorsChanged)
    Q_PROPERTY(float yaw READ yaw WRITE setYaw NOTIFY rotationChanged)
    Q_PROPERTY(float pitch READ pitch WRITE setPitch NOTIFY rotationChanged)

public:
    explicit OpenGLBarChart3D(QQuickItem *parent = nullptr);
    Renderer *createRenderer() const override;

    int completed() const { return m_completed; }
    int inProgress() const { return m_inProgress; }
    int archived() const { return m_archived; }

    QColor completedColor() const { return m_completedColor; }
    QColor inProgressColor() const { return m_inProgressColor; }
    QColor archivedColor() const { return m_archivedColor; }

    float yaw() const { return m_yaw; }
    float pitch() const { return m_pitch; }

public slots:
    void setCompleted(int value);
    void setInProgress(int value);
    void setArchived(int value);
    void setCompletedColor(const QColor &color);
    void setInProgressColor(const QColor &color);
    void setArchivedColor(const QColor &color);
    void setYaw(float value);
    void setPitch(float value);

signals:
    void valuesChanged();
    void colorsChanged();
    void rotationChanged();

private:
    void requestRepaint();

    int m_completed{0};
    int m_inProgress{0};
    int m_archived{0};
    QColor m_completedColor{0x4CAF50};
    QColor m_inProgressColor{0xFFC107};
    QColor m_archivedColor{0xF44336};
    float m_yaw{30.0f};
    float m_pitch{-20.0f};
};
