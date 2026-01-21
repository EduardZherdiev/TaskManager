#include "include/openglbarchart3d.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLFramebufferObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>
#include <QImage>
#include <QCoreApplication>
#include <algorithm>
#include <vector>

namespace {
struct Vertex {
    QVector3D pos;
    QVector2D uv;
};

class BarChartRenderer3D final : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions {
public:
    BarChartRenderer3D() : m_program(new QOpenGLShaderProgram) {}
    ~BarChartRenderer3D() override { 
        delete m_program;
        if (m_textures[0]) glDeleteTextures(3, m_textures);
    }

    void synchronize(QQuickFramebufferObject *item) override {
        auto *chart = static_cast<OpenGLBarChart3D *>(item);

        const float maxValue = static_cast<float>(std::max({1, chart->completed(), chart->inProgress(), chart->archived()}));
        m_heights[0] = chart->completed() / maxValue;
        m_heights[1] = chart->inProgress() / maxValue;
        m_heights[2] = chart->archived() / maxValue;

        m_yaw = chart->yaw();
        m_pitch = chart->pitch();
        m_scale = chart->scale();
        m_pendingUpdate = true;
    }

    void render() override {
        if (!m_initialized) {
            initializeOpenGLFunctions();
            loadTextures();
            initProgram();
            m_initialized = true;
        }

        glViewport(0, 0, m_size.width(), m_size.height());
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!m_program || !m_program->isLinked())
            return;

        // Build MVP
        QMatrix4x4 proj;
        proj.perspective(45.0f, m_size.width() / float(m_size.height() ? m_size.height() : 1), 0.1f, 20.0f);
        QMatrix4x4 view;
        view.translate(0.0f, 0.0f, -3.5f / m_scale);
        view.rotate(m_pitch, 1, 0, 0);
        view.rotate(m_yaw, 0, 1, 0);
        QMatrix4x4 model;
        model.scale(m_scale);
        QMatrix4x4 mvp = proj * view * model;

        m_program->bind();
        m_program->setUniformValue("mvp", mvp);

        // Layout bars
        static const float width = 0.35f;
        static const float depth = 0.35f;
        static const float spacing = 0.7f;
        const float xs[3] = {-spacing, 0.0f, spacing};
        const int texIds[3] = {0, 1, 2};  // green, orange, red

        for (int i = 0; i < 3; ++i) {
            drawBar(xs[i], width, depth, m_heights[i] * 2.0f, m_textures[texIds[i]]);
        }

        m_program->release();

        if (m_pendingUpdate) {
            update();
            m_pendingUpdate = false;
        }
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
    void loadTextures() {
        const char* texNames[3] = {
            "green-bar.jpg",   // completed
            "orange-bar.jpg",  // inProgress
            "red-bar.jpg"      // archived
        };

        glGenTextures(3, m_textures);
        
        for (int i = 0; i < 3; ++i) {
            QString texPath = QCoreApplication::applicationDirPath() + "/assets/img/" + texNames[i];
            QImage img(texPath);
            if (img.isNull()) {
                qWarning() << "Failed to load texture:" << texPath;
                continue;
            }

            img = img.convertToFormat(QImage::Format_RGBA8888);
            
            glBindTexture(GL_TEXTURE_2D, m_textures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
    }

    void initProgram() {
        static const char *vs = R"(
            attribute vec3 position;
            attribute vec2 uv;
            varying vec2 vUv;
            uniform mat4 mvp;
            void main() {
                gl_Position = mvp * vec4(position, 1.0);
                vUv = uv;
            }
        )";

        static const char *fs = R"(
            varying vec2 vUv;
            uniform sampler2D texture;
            void main() {
                gl_FragColor = texture2D(texture, vUv);
            }
        )";

        if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vs))
            qWarning() << "Vertex shader error:" << m_program->log();
        if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fs))
            qWarning() << "Fragment shader error:" << m_program->log();
        if (!m_program->link())
            qWarning() << "Shader link error:" << m_program->log();

        m_posAttr = m_program->attributeLocation("position");
        m_uvAttr = m_program->attributeLocation("uv");
        m_texUniform = m_program->uniformLocation("texture");
    }

    void appendFace(std::vector<Vertex> &verts,
                    float x0, float y0, float z0, float u0, float v0,
                    float x1, float y1, float z1, float u1, float v1,
                    float x2, float y2, float z2, float u2, float v2,
                    float x3, float y3, float z3, float u3, float v3) {
        verts.push_back({{x0,y0,z0}, {u0,v0}});
        verts.push_back({{x1,y1,z1}, {u1,v1}});
        verts.push_back({{x2,y2,z2}, {u2,v2}});
        verts.push_back({{x0,y0,z0}, {u0,v0}});
        verts.push_back({{x2,y2,z2}, {u2,v2}});
        verts.push_back({{x3,y3,z3}, {u3,v3}});
    }

    void drawBar(float centerX, float w, float d, float h, GLuint texture) {
        const float hx = w * 0.5f;
        const float hz = d * 0.5f;
        const float y0 = -1.0f;
        const float y1 = y0 + h;

        std::vector<Vertex> verts;
        verts.reserve(36);

        // Front
        appendFace(verts, centerX - hx, y0, hz, 0, 0, centerX + hx, y0, hz, 1, 0, centerX + hx, y1, hz, 1, 1, centerX - hx, y1, hz, 0, 1);
        // Back
        appendFace(verts, centerX + hx, y0, -hz, 0, 0, centerX - hx, y0, -hz, 1, 0, centerX - hx, y1, -hz, 1, 1, centerX + hx, y1, -hz, 0, 1);
        // Left
        appendFace(verts, centerX - hx, y0, -hz, 0, 0, centerX - hx, y0, hz, 1, 0, centerX - hx, y1, hz, 1, 1, centerX - hx, y1, -hz, 0, 1);
        // Right
        appendFace(verts, centerX + hx, y0, hz, 0, 0, centerX + hx, y0, -hz, 1, 0, centerX + hx, y1, -hz, 1, 1, centerX + hx, y1, hz, 0, 1);
        // Top
        appendFace(verts, centerX - hx, y1, hz, 0, 0, centerX + hx, y1, hz, 1, 0, centerX + hx, y1, -hz, 1, 1, centerX - hx, y1, -hz, 0, 1);
        // Bottom
        appendFace(verts, centerX - hx, y0, -hz, 0, 0, centerX + hx, y0, -hz, 1, 0, centerX + hx, y0, hz, 1, 1, centerX - hx, y0, hz, 0, 1);

        m_program->enableAttributeArray(m_posAttr);
        m_program->enableAttributeArray(m_uvAttr);

        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &verts[0].pos);
        glVertexAttribPointer(m_uvAttr, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &verts[0].uv);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        m_program->setUniformValue(m_texUniform, 0);

        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));

        m_program->disableAttributeArray(m_posAttr);
        m_program->disableAttributeArray(m_uvAttr);
    }

    QOpenGLShaderProgram *m_program{nullptr};
    bool m_initialized{false};
    bool m_pendingUpdate{false};
    GLint m_posAttr{-1};
    GLint m_uvAttr{-1};
    GLint m_texUniform{-1};
    GLuint m_textures[3]{0, 0, 0};
    QSize m_size;
    float m_heights[3]{0.0f, 0.0f, 0.0f};
    float m_yaw{30.0f};
    float m_pitch{-20.0f};
    float m_scale{1.0f};
};
} // namespace

OpenGLBarChart3D::OpenGLBarChart3D(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    setTextureFollowsItemSize(true);
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *OpenGLBarChart3D::createRenderer() const
{
    return new BarChartRenderer3D();
}

void OpenGLBarChart3D::setCompleted(int value)
{
    if (m_completed == value)
        return;
    m_completed = value;
    emit valuesChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setInProgress(int value)
{
    if (m_inProgress == value)
        return;
    m_inProgress = value;
    emit valuesChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setArchived(int value)
{
    if (m_archived == value)
        return;
    m_archived = value;
    emit valuesChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setCompletedColor(const QColor &color)
{
    if (m_completedColor == color)
        return;
    m_completedColor = color;
    emit colorsChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setInProgressColor(const QColor &color)
{
    if (m_inProgressColor == color)
        return;
    m_inProgressColor = color;
    emit colorsChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setArchivedColor(const QColor &color)
{
    if (m_archivedColor == color)
        return;
    m_archivedColor = color;
    emit colorsChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setYaw(float value)
{
    if (qFuzzyCompare(m_yaw, value))
        return;
    m_yaw = value;
    emit rotationChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setPitch(float value)
{
    if (qFuzzyCompare(m_pitch, value))
        return;
    m_pitch = value;
    emit rotationChanged();
    requestRepaint();
}

void OpenGLBarChart3D::setScale(float value)
{
    if (qFuzzyCompare(m_scale, value))
        return;
    m_scale = value;
    emit scaleChanged();
    requestRepaint();
}

void OpenGLBarChart3D::requestRepaint()
{
    update();
}
