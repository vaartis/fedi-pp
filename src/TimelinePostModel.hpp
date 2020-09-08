#include <deque>

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

    QString newest_id;
    QString oldest_id;
    std::deque<TimelinePost> posts;
signals:
    void postFetchingDone();
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

    Q_INVOKABLE void fetchPosts(bool newer) {
        QUrlQuery query;
        if (newer && !newest_id.isEmpty())
            query.addQueryItem("since_id", newest_id);
        else if (!oldest_id.isEmpty())
            query.addQueryItem("max_id", oldest_id);

        client->get_request("/api/v1/timelines/home", [=, this](QJsonDocument response) {
            auto array = response.array();

            if (newer)
                // Insert in the beginning
                beginInsertRows(QModelIndex(), 0, array.size() - 1);
            else
                // Insert at the end
                beginInsertRows(QModelIndex(), posts.size(), posts.size() +  array.size() - 1);


            std::vector<TimelinePost> fetched_posts;
            fetched_posts.reserve(array.size());

            for (const auto &post : array) {
                auto post_object = post.toObject();

                fetched_posts.push_back(
                    TimelinePost {
                        .user_data = post_object["account"].toVariant(),
                        .content = post_object["content"].toString(),
                    }
                );
            }

            std::deque<TimelinePost>::iterator insert_at;
            if (newer) {
                // For new posts, insert them at the beginning
                insert_at = posts.begin();
            } else {
                // For old at, insert the end
                insert_at = posts.end();
            }
            posts.insert(insert_at, fetched_posts.begin(), fetched_posts.end());

            if (!fetched_posts.empty()) {
                // Update the ids if new posts came through

                // Only update the newest_id if newer posts are fetched or
                // one wasn't recorded previously
                if (newest_id.isEmpty() || newer)
                    newest_id = array.first()["id"].toString();

                // Only update oldest id when fetching older posts
                if (!newer)
                    oldest_id = array.last()["id"].toString();
            }

            endInsertRows();
            postFetchingDone();
        }, query);
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
