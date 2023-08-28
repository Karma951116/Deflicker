import QtQuick 2.14
import QtQuick.Controls 2.0
import QtCharts 2.3
import ImageCanvas 1.0

Rectangle {
    property alias header: titlebar
    implicitWidth: 1920
    implicitHeight: 1080
    color: "#FFFFFF"
    Titlebar {
        id: titlebar
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: 35
    }

    ImageCanvas {
        id: origin
        anchors.top: titlebar.bottom
        anchors.left: parent.left
        width: parent.width / 2 - 3
        height: parent.height - 240
    }

    ImageCanvas {
        id: processed
        anchors.top: titlebar.bottom
        anchors.right: parent.right
        width: parent.width / 2 - 3
        height: parent.height - 240
    }

    Rectangle {
        id: media_info
        width: parent.width / 4
        height: 200
        anchors.bottom: parent.bottom
        color: "gray"

        FolderSelector {
            id: folder_selector
            width: parent.width / 4
            anchors.top: parent.top
            anchors.left: parent.left
        }

        Text {
            id: cur_frame
            anchors.top: folder_selector.bottom
            anchors.left: parent.left
            text: srcBuffer.index;
        }

        Button {
            id: process_btn
            anchors.top: cur_frame.bottom
            anchors.left: parent.left
            text: "处理"
            onClicked: {
                globalDeflicker.process();
            }
        }

        Button {
            id: hist_btn
            anchors.top: process_btn.bottom
            anchors.left: parent.left
            text: "hist"
            onClicked: {
                histChart.visible = !histChart.visible;
            }
        }
        Button {
            id: lum_btn
            anchors.top: process_btn.bottom
            anchors.left: hist_btn.right
            text: "lum"
            onClicked: {
                lumChart.visible = !lumChart.visible;
            }
        }
    }

    ChartView {
        id: histChart
        width: 1920
        height: 1080
        anchors.centerIn: parent
        antialiasing: true
        visible: false

        ValueAxis {
            id: hist_axisX
            min: 0
            max: 65535
            tickInterval: 1
        }
        ValueAxis {
            id: hist_axisY
            min: 0
            max: 10000
        }
        SplineSeries {
            id: src_spline
            axisX: hist_axisX
            axisY: hist_axisY
            color: "red"
            name: "srcHist"
        }
        SplineSeries {
            id: dst_spline
            axisX: hist_axisX
            axisY: hist_axisY
            color: "blue"
            name: "dstHist"
        }
    }

    ChartView {
        id: lumChart
        width: 2160
        height: 1080
        anchors.centerIn: parent
        antialiasing: true
        visible: false

        ValueAxis {
            id: lum_axisX
            min: 0
            max: 65535
            tickInterval: 10
        }
        ValueAxis {
            id: lum_axisY
            min: 0
            max: 65535
            tickInterval: 1000
        }
        SplineSeries {
            id: src_lum
            axisX: lum_axisX
            axisY: lum_axisY
            color: "red"
            name: "srcAvgLum"
        }
        SplineSeries {
            id: dst_lum
            axisX: lum_axisX
            axisY: lum_axisY
            color: "blue"
            name: "dstAvgLum"
        }
    }

    function updateHist() {
        if (!histChart.visible) {
            return
        }
        src_spline.clear();
        dst_spline.clear();
        var srcHist = globalDeflicker.srcHist(srcBuffer.index);
        var dstHist = globalDeflicker.dstHist(srcBuffer.index);
        var max = 0;
        for (var valueIdx in srcHist) {
            var obj = srcHist[valueIdx];
            var value = obj["value"];
            var index = parseInt(obj["index"]);
            max = value > max ? value : max;
            src_spline.append(index, value);
        }
        for (valueIdx in dstHist) {
            obj = dstHist[valueIdx];
            value = obj["value"];
            index = parseInt(obj["index"]);
            max = value > max ? value : max;
            dst_spline.append(index, value);
        }
        hist_axisY.max = max;
    }

    function updateLum() {
        src_lum.clear();
        dst_lum.clear();
        var srcMean = globalDeflicker.srcLuminance();
        var dstMean = globalDeflicker.dstLuminance();
        lum_axisX.max = srcBuffer.nbFrames() - 1;
        lum_axisX.tickCount = srcBuffer.nbFrames() - 1;
        for (var valueIdx in srcMean) {
            src_lum.append(valueIdx, srcMean[valueIdx]);
        }
        for (valueIdx in dstMean) {
            dst_lum.append(valueIdx, dstMean[valueIdx]);
        }
    }

    Connections {
        target: imageLoader
        onSequenceUpdate: {
            origin.updateFrame();
        }
    }

    Connections {
        target: srcBuffer
        onIndexChanged: {
            origin.updateFrame();
        }
    }

    Connections {
        target: globalDeflicker
        onCalcDstMeanDone: {
            updateLum();
            //updateHist();
        }
    }

    Connections {
        target: root
        onShowHist: {
            //updateHist();
        }
    }

    Connections {
        target: dstBuffer
        onIndexChanged: {
            processed.updateFrame();
        }
    }

    Component.onCompleted: {
        origin.setBuffer(srcBuffer);
        processed.setBuffer(dstBuffer);
    }
}
