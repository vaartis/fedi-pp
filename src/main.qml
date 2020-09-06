import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2

ApplicationWindow {
    id: window
    visible: true
    minimumWidth: 400
    minimumHeight: 600

    StackView {
        id: stack
        initialItem: loginComponent

        anchors {
            centerIn: parent
            fill: parent
        }

        Connections {
            target: Client

            function onAppCreated() {
                if (Client.login_data_known) {
                    Client.login()
                }
            }

            function onLoggedInChanged() {
                stack.replace(homeTimeline)
            }
        }
    }

    Component {
        id: homeTimeline

        Pane {
            Label { text: "logged in " }
        }
    }

    Component {
        id: loginComponent

        Pane {            
            ColumnLayout {
                anchors.centerIn: parent
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    
                    font.pointSize: 16
                    text: "Username"          
                }
                TextField {
                    id: username;
                    Layout.fillWidth: true
                    horizontalAlignment: TextInput.AlignHCenter            
                }


                Label {
                    Layout.alignment: Qt.AlignHCenter
                    
                    font.pointSize: 16
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
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter

                    property bool created_app: false

                    enabled: created_app

                    Connections {
                        target: Client
                        function onAppCreated() {
                            loginButton.created_app = true
                        }
                    }

                    onClicked: function() {
                        Client.login(username.text, password.text)
                    }
                }
            }
        }
    }

    /*
    Drawer {
        id: drawer
        width: 0.66 * window.width
        height: window.height
        dragMargin: 0.5 * window.width
    }
    */
}
