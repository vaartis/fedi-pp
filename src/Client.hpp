#pragma once

#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJSValue>
#include <QJsonDocument>

#include <KWallet>

class Client : public QObject {
    Q_OBJECT

    QQmlApplicationEngine &engine;
    QNetworkAccessManager network_manager;

    QUrl instance_url;

    std::unique_ptr<KWallet::Wallet> wallet;

    inline static const QString client_data_id = "client-data";
    inline static const QString login_data_id = "login-data";

    QString client_id;
    QString client_secret;

    QString token;
signals:
    void appCreated();
    void loggedInChanged();
    void loginFailed();
public:
    void create_app() {
        if (!wallet->hasEntry(client_data_id)) {
            auto url = QUrl(instance_url.url() + "/api/v1/apps");
            auto request = QNetworkRequest(url);
            request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

            QUrlQuery post_data = {
                {"client_name", "fedi-pp"},
                {"redirect_uris", "urn:ietf:wg:oauth:2.0:oob"},
                {"scopes", "read write follow push"}
            };

            auto reply = network_manager.post(request, post_data.toString().toUtf8());
            connect(reply, &QNetworkReply::finished, this, [&, reply]() {
                if (reply->error() != QNetworkReply::NoError) {
                    engine.throwError(reply->errorString());

                    return;
                }

                auto json = QJsonDocument::fromJson(reply->readAll());

                client_id = json["client_id"].toString();
                client_secret = json["client_secret"].toString();

                wallet->writeMap(
                    client_data_id,
                    {
                        { "client-id", client_id },
                        { "client-secret", client_secret }
                    }
                );

                appCreated();

                reply->deleteLater();
            });
        } else {
            QMap<QString, QString> client_data;
            wallet->readMap(client_data_id, client_data);

            client_id = client_data["client-id"];
            client_secret = client_data["client-secret"];

            appCreated();
        }
    }

    Q_PROPERTY(bool loggedIn READ logged_in NOTIFY loggedInChanged)
    bool logged_in() {
        return !token.isEmpty();
    }

    Q_PROPERTY(bool loginDataKnown READ login_data_known)
    bool login_data_known() {
        return wallet->hasEntry(login_data_id);
    }

    Q_INVOKABLE void login() {
        QMap<QString, QString> login_data;
        wallet->readMap(login_data_id, login_data);

        login(login_data["username"], login_data["password"]);
    }

    Q_INVOKABLE void login(QString username, QString password) {
        auto url = QUrl(instance_url.url() + "/oauth/token");
        auto request = QNetworkRequest(url);
        request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

        QUrlQuery post_data = {
            {"client_id", client_id},
            {"client_secret", client_secret},
            {"redirect_uris", "urn:ietf:wg:oauth:2.0:oob"},
            {"grant_type", "password"},
            {"username", username},
            {"password", password}
        };

        auto reply = network_manager.post(request, post_data.toString().toUtf8());
        // Variables refered directly in the capture list HAVE to captured by value, or they break
        connect(reply, &QNetworkReply::finished, this, [&, reply, username, password]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "Login error:" << reply->errorString();
                
                loginFailed();

                reply->deleteLater();
                return;
            }

            wallet->writeMap(
                login_data_id,
                { { "username", username }, { "password", password } }
            );

            auto json = QJsonDocument::fromJson(reply->readAll());
            token = json["access_token"].toString();

            loggedInChanged();

            reply->deleteLater();
        });
    }

    void get_request(const QString &added_url, std::function<void(QJsonDocument)> &&signal_callback, const QUrlQuery &query = QUrlQuery()) {
        auto url = QUrl(instance_url.url() + added_url);
        url.setQuery(query);
        
        auto request = QNetworkRequest(url);
        request.setRawHeader(
            "Authorization",
            QString("Bearer %1").arg(token).toUtf8()
        );        

        auto reply = network_manager.get(request);
        connect(reply, &QNetworkReply::finished, this, [&, reply, signal_callback] {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "Request error:" << reply->errorString();

                reply->deleteLater();
                return;
            }

            auto json = QJsonDocument::fromJson(reply->readAll());
            
            signal_callback(json);

            reply->deleteLater();
        });
    }

    Client(QQmlApplicationEngine &engine, QUrl instance_url)
        : network_manager(QNetworkAccessManager(this)), instance_url(instance_url), engine(engine)  {

        using namespace KWallet;

        wallet = std::unique_ptr<Wallet>(Wallet::openWallet(Wallet::PasswordFolder(), 0));
        if (!wallet)
            throw std::runtime_error("Couldn't open wallet");

        if (!wallet->hasFolder("fedi-pp")) {
            wallet->createFolder("fedi-pp");
        }
        wallet->setFolder("fedi-pp");
    }
};
