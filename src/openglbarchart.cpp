#include "include/openglbarchart.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QDebug>
#include <algorithm>

namespace {
// Simple renderer that paints three vertical bars into an FBO
class BarChartRenderer final : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions {
public:
    struct RGB { float r, g, b; };

    BarChartRenderer()
        : m_program(new QOpenGLShaderProgram)
    {
    }

    ~BarChartRenderer() override {
        delete m_program;
    }

    void synchronize(QQuickFramebufferObject *item) override {
        auto *chart = static_cast<OpenGLBarChart *>(item);

        const float maxValue = static_cast<float>(std::max({1, chart->completed(), chart->inProgress(), chart->archived()}));
        m_values[0] = chart->completed() / maxValue;
        m_values[1] = chart->inProgress() / maxValue;
        m_values[2] = chart->archived() / maxValue;

        // Get colors from QML and convert to normalized RGB (0.0-1.0)
        QColor c0 = chart->completedColor();
        m_colors[0] = {c0.redF(), c0.greenF(), c0.blueF()};
        
        QColor c1 = chart->inProgressColor();
        m_colors[1] = {c1.redF(), c1.greenF(), c1.blueF()};
        
        QColor c2 = chart->archivedColor();
        m_colors[2] = {c2.redF(), c2.greenF(), c2.blueF()};

        // Trigger repaint when data changes
        m_pendingUpdate = true;
    }

    void render() override {
        if (!m_initialized) {
            initializeOpenGLFunctions();
            initProgram();
            m_initialized = true;
        }

        if (m_pendingUpdate) {
            update();
            m_pendingUpdate = false;
        }

        glViewport(0, 0, m_size.width(), m_size.height());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!m_program || !m_program->isLinked())
            return;

        m_program->bind();

        // Positions for three bars across the viewport
        const float barWidth = 0.35f;
        const float spacing = 0.5f;

        // Use colors from QML (Style.qml)
        drawBar(-spacing, barWidth, m_values[0], m_colors[0].r, m_colors[0].g, m_colors[0].b); // completed
        drawBar(0.0f, barWidth, m_values[1], m_colors[1].r, m_colors[1].g, m_colors[1].b);   // inProgress
        drawBar(spacing, barWidth, m_values[2], m_colors[2].r, m_colors[2].g, m_colors[2].b); // archived

        m_program->release();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        m_size = size;
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setTextureTarget(GL_TEXTURE_2D);
        format.setInternalTextureFormat(GL_RGBA8);
        return new QOpenGLFramebufferObject(size, format);
    }

private:
    void initProgram() {
        static const char *vertexSrc = R"(
            attribute vec3 position;
            attribute vec3 color;
            varying vec3 vColor;
            void main() {
                gl_Position = vec4(position, 1.0);
                vColor = color;
            }
        )";

        static const char *fragmentSrc = R"(
            varying vec3 vColor;
            void main() {
                gl_FragColor = vec4(vColor, 1.0);
            }
        )";

        if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSrc))
            qWarning() << "Vertex shader error:" << m_program->log();
        if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSrc))
            qWarning() << "Fragment shader error:" << m_program->log();
        if (!m_program->link())
            qWarning() << "Shader link error:" << m_program->log();

        m_posAttr = m_program->attributeLocation("position");
        m_colorAttr = m_program->attributeLocation("color");
    }

    void drawBar(float centerX, float width, float normalizedHeight, float r, float g, float b) {
        const float halfWidth = width * 0.5f;
        const float bottom = -1.0f;
        const float top = bottom + normalizedHeight * 2.0f; // scale into [-1,1]

        const float vertices[] = {
            centerX - halfWidth, bottom, 0.0f,
            centerX + halfWidth, bottom, 0.0f,
            centerX + halfWidth, top, 0.0f,
            centerX - halfWidth, bottom, 0.0f,
            centerX + halfWidth, top, 0.0f,
            centerX - halfWidth, top, 0.0f
        };

        const float colors[] = {
            r, g, b,
            r, g, b,
            r, g, b,
            r, g, b,
            r, g, b,
            r, g, b
        };

        GLuint vbos[2] = {0, 0};
        glGenBuffers(2, vbos);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        m_program->enableAttributeArray(m_posAttr);
        m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
        m_program->enableAttributeArray(m_colorAttr);
        m_program->setAttributeBuffer(m_colorAttr, GL_FLOAT, 0, 3, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(m_posAttr);
        glDisableVertexAttribArray(m_colorAttr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDeleteBuffers(2, vbos);
    }

    QOpenGLShaderProgram *m_program{nullptr};
    bool m_initialized{false};
    bool m_pendingUpdate{false};
    int m_posAttr{-1};
    int m_colorAttr{-1};
    QSize m_size;
    float m_values[3]{0.0f, 0.0f, 0.0f};
    RGB m_colors[3]{{0.29f, 0.80f, 0.31f}, {1.00f, 0.76f, 0.03f}, {0.96f, 0.26f, 0.21f}};
};
} // namespace

OpenGLBarChart::OpenGLBarChart(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    setTextureFollowsItemSize(true);
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *OpenGLBarChart::createRenderer() const
{
    return new BarChartRenderer();
}

void OpenGLBarChart::setCompleted(int value)
{
    if (m_completed == value)
        return;
    m_completed = value;
    emit valuesChanged();
    requestRepaint();
}

void OpenGLBarChart::setInProgress(int value)
{
    if (m_inProgress == value)
        return;
    m_inProgress = value;
    emit valuesChanged();
    requestRepaint();
}

void OpenGLBarChart::setArchived(int value)
{
    if (m_archived == value)
        return;
    m_archived = value;
    emit valuesChanged();
    requestRepaint();
}

void OpenGLBarChart::requestRepaint()
{
    update();
}

void OpenGLBarChart::setCompletedColor(const QColor &color)
{
    if (m_completedColor == color)
        return;
    m_completedColor = color;
    emit colorsChanged();
    requestRepaint();
}

void OpenGLBarChart::setInProgressColor(const QColor &color)
{
    if (m_inProgressColor == color)
        return;
    m_inProgressColor = color;
    emit colorsChanged();
    requestRepaint();
}

void OpenGLBarChart::setArchivedColor(const QColor &color)
{
    if (m_archivedColor == color)
        return;
    m_archivedColor = color;
    emit colorsChanged();
    requestRepaint();
}
