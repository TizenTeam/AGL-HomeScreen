/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import HomeScreen 1.0

Item {
    id: root
    width: 295
    height: 218

    property date now: new Date
    Timer {
        interval: 100; running: true; repeat: true;
        onTriggered: root.now = new Date
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 295 - 76
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 0

                Image {
                    id: dummy
                    width:0
                    height: 0
                    cache: false
                    property bool state: true
                    source:  "http://localhost:8080/?query=" + state
                }

                Image {
                    id: iotivity
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: ( dummy.state )  ? "images/iotivity.png" : "./images/Utility_Logo_Colour-01.png"

                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: { dummy.state = ! dummy.state }
                }

                Text {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: Qt.formatTime(now, 'h:mm ap').toUpperCase()
                    font.family: 'Roboto'
                    font.pixelSize: 40
                    color: 'white'
                    horizontalAlignment:  Text.AlignHCenter
                    verticalAlignment:  Text.AlignVCenter
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: 20
                    Image {
                        source: './images/Weather/WeatherIcons_Rain-01.png'
                    }
                    Text {
                        text: '64°F'
                        color: 'white'
                        font.family: 'Helvetica'
                        font.pixelSize: 32
                    }
                }
            }
        }
        ColumnLayout {
            id: icons
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 76
            spacing: -10
            Repeater {
                model: StatusBarModel {}
                delegate: Image {
                    Layout.preferredWidth: 77
                    Layout.preferredHeight: 73
                    source: model.modelData
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }
}
