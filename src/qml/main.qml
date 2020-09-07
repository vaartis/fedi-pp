import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami

Kirigami.ApplicationWindow {
    id: window
    visible: true

    minimumWidth: 300
    minimumHeight: 500

    pageStack.initialPage: loginComponent

    Component { id: homeTimeline; Timeline { } }

    Component {
        id: loginComponent
        
        Kirigami.Page {
            title: "Login"
            id: loginPage

            state: "NotLoggedIn"
            states: [
                State {
                    name: "NotLoggedIn"
                    PropertyChanges { target: username; enabled: true }
                    PropertyChanges { target: password; enabled: true }
                    PropertyChanges { target: loginButton; enabled: true }
                },
                State {
                    name: "LoggingIn"
                    PropertyChanges { target: username; enabled: false }
                    PropertyChanges { target: password; enabled: false }
                    PropertyChanges { target: loginButton; enabled: false }
                }
            ]

            Connections {
                target: Client

                function onAppCreated() {
                    if (Client.loginDataKnown) {
                        loginPage.state = "LoggingIn"
                        
                        Client.login()
                    }
                }

                function onLoggedInChanged() {
                    window.pageStack.replace(homeTimeline)
                }

                function onLoginFailed() {
                    loginPage.state = "NotLoggedIn"
                    
                    showPassiveNotification("Couldn't log in", "short")                                       
                }
            }

            ColumnLayout {
                anchors.fill: parent
            
                ColumnLayout {                   
                    Layout.maximumWidth: 250
                    Layout.alignment: Qt.AlignHCenter
                    
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        
                        font.pointSize: 24
                        text: "Username"          
                    }
                    TextField {
                        id: username

                        Layout.fillWidth: true
                        horizontalAlignment: TextInput.AlignHCenter
                    }


                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        
                        font.pointSize: 24
                        text: "Password"
                    }
                    TextField {
                        id: password

                        Layout.fillWidth: true                        
                        horizontalAlignment: TextInput.AlignHCenter
                        echoMode: TextInput.Password
                    }

                    Button {
                        id: loginButton
                        text: "Login"

                        Layout.alignment: Qt.AlignHCenter

                        onClicked: function() {
                            loginPage.state = "LoggingIn"
                            
                            Client.login(username.text, password.text)
                        }
                    }
                }
            }
        }
    }
}
