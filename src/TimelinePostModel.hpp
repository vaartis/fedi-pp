#include <qqml.h>
#include <QAbstractListModel>
#include <QQmlEngine>
#include <QJsonArray>
#include <QJsonObject>

#include "Client.hpp"

struct TimelinePost {
    // User info
    QVariant user_data;

    QString content;
};

class TimelinePostModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(Client *client MEMBER client REQUIRED)
    Client *client;

    std::vector<TimelinePost> posts;
public:
    enum Roles : int {
        ContentRole = Qt::UserRole + 1,
        UserDataRole
    };

    QHash<int, QByteArray> roleNames() const override {
        return QHash<int, QByteArray> {
            { UserDataRole, "user" },
            { ContentRole, "content"},
        };
    }

    Q_INVOKABLE void fetchPosts() {
        client->get_request("/api/v1/timelines/home", [&](QJsonDocument response) {
            auto array = response.array();

            beginInsertRows(QModelIndex(), posts.size(), array.size() - 1);

            for (const auto &post : array) {
                auto post_object = post.toObject();

                TimelinePost x { .content = "A" };

                posts.push_back(
                    TimelinePost {
                        .user_data = post_object["account"].toVariant(),
                        .content = post_object["content"].toString(),
                    }
                );
            }

            endInsertRows();
        });
    }

    Q_INVOKABLE int rowCount(const QModelIndex &parent) const override  {
        return posts.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid())
            return QVariant();

        auto post_at = posts.at(index.row());
        switch (role) {
        case ContentRole:
            return post_at.content;
        case UserDataRole:
            return post_at.user_data;
        }

        return QVariant();
    }
};
