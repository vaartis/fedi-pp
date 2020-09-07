import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami

import ch.kotobank 1.0

Kirigami.ScrollablePage {
    title: `Home timeline`

    ListView {
        spacing: Kirigami.Units.smallSpacing
        
        model: TimelinePostModel {
            id: postModel
            
            client: Client
            Component.onCompleted: {
                postModel.fetchPosts()
            }
        }
        
        delegate: RowLayout {            
            Image {
                source: model.user.avatar
                
                Layout.preferredWidth: 64
                Layout.preferredHeight: 64
            }

            ColumnLayout {
                Layout.preferredWidth: window.width * 0.8
                
                Text {
                    text: model.user.display_name
                    textFormat: Text.StyledText
                    font.pointSize: 14
                    color: Kirigami.Theme.textColor

                    Layout.preferredWidth: parent.width * 0.9
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }
                
                Text {
                    text: model.content
                    textFormat: Text.StyledText
                    font.pointSize: 12
                    color: Kirigami.Theme.textColor

                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}
