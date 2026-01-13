#pragma once
#include <QAbstractListModel>
#include "Feedback.h"
#include "Feedbackreader.h"

class UserModel;

class FeedbackModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
public:
    FeedbackModel();
    static void registerMe(const std::string& moduleName);
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString lastError() const;
    void setUserModel(UserModel* userModel);

    Q_INVOKABLE bool hasUserFeedback();
    Q_INVOKABLE QVariantMap getUserFeedback();
    Q_INVOKABLE bool saveFeedback(int rate, const QString& description);

signals:
    void lastErrorChanged();

private:
    std::vector<Feedback> m_Feedbacks;
    FeedbackReader m_reader;
    QString m_lastError;
    UserModel* m_userModel = nullptr;

    int getCurrentUserId() const;
    void setError(const QString& err);

    enum FeedbackRoles
    {
        IdRole = Qt::UserRole + 1,
        RateRole,
        DescriptionRole,
        CreatedAtRole
    };

    bool updateFeedbacks();
};
