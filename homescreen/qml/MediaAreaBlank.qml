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
import QtQuick.Controls 2.0
import AGL.Demo.Controls 1.0
import MasterVolume 1.0

Image {
    width: 1080
    height: 215
    source: './images/Utility_Logo_Background-01.png'
    property bool displayVolume: false;

    MouseArea {
        anchors.fill: parent
        function enableVolumeDisplay() {
            if (!displayVolume) {
                displayVolume = true
                master_volume.visible = true
                volume_timer.restart()
            }
        }
        onClicked: enableVolumeDisplay()
    }

    Image {
    id: logo_image
        anchors.centerIn: parent
        source: "./images/Utility_Logo_Colour-01.png"
    }

    Timer {
        id: volume_timer
        interval: 5000; running: false; repeat: false
        onTriggered: displayVolume = false
    }

    states: [
    State { when: displayVolume;
    PropertyChanges { target: master_volume; opacity: 1.0 }
    PropertyChanges { target: slider; enabled: true }
    PropertyChanges { target: logo_image; opacity: 0.0 }
    },
    State { when: !displayVolume;
    PropertyChanges { target: master_volume; opacity: 0.0 }
    PropertyChanges { target: slider; enabled: false }
    PropertyChanges { target: logo_image; opacity: 1.0 }
    }
    ]

    transitions: Transition {
    NumberAnimation { property: "opacity"; duration: 500}
    }

    MasterVolume {
        id: mv
        objectName: "mv"
        onVolumeChanged: slider.value = volume
    }

    Item {
        id: master_volume
        anchors.fill: parent
        anchors.centerIn: parent
        visible: false

        Label {
            font.pixelSize: 36
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            text: "Master Volume"
        }

        RowLayout {
            anchors.fill: parent
            anchors.centerIn: parent
            anchors.margins: 20
            spacing: 20
            Label {
                font.pixelSize: 36
                color: "white"
                text: "0 %"
            }
            Slider {
                id: slider
                Layout.fillWidth: true
                from: 0
                to: 65536
                stepSize: 256
                snapMode: Slider.SnapOnRelease
                onValueChanged: mv.volume = value
                Component.onCompleted: value = mv.volume
                onPressedChanged: {
                    if (pressed) {volume_timer.stop()}
                    else {volume_timer.restart()}
                }
                background: Rectangle {
                    id: slider_bg
                    height: 16
                    color: "#59FF7F"
                }
                handle: Rectangle {
                    anchors.verticalCenter: slider_bg.verticalCenter
                    width: 48
                    height: 48
                    radius: 24
                    x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
                    y: slider.topPadding + slider.availableHeight / 2 - height / 2
                    color: "white"
                }
            }
            Label {
                font.pixelSize: 36
                color: "white"
                text: "100 %"
            }
        }
    }
}
