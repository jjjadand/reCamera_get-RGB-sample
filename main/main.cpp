#include <iostream>
#include <string>
#include <fstream>

#include "ma_transport_rtsp.h"
#include <sscma.h>
#include <video.h>

#include <opencv2/opencv.hpp>

//using namespace ma;

int main(int argc, char** argv) {

    ma::Device* device = ma::Device::getInstance();
    ma::Camera* camera = nullptr;

//     ma::Signal::install({SIGINT, USB Webcam / Raspberry Pi Camera V2.1 / IMX219-130 8MP cameras / High quality 12.3MP IMX477 cameras
// SIGSEGV, SIGABRT, SIGTRAP, SIGTERM, SIGHUP, SIGQUIT, SIGPIPE}, [device](int sig) {
//         std::cout << "Caught signal " << sig << std::endl;
//         for (auto& sensor : device->getSensors()) {
//             sensor->deInit();
//         }
//         exit(0);
//     });


    ma::Signal::install({SIGINT, SIGSEGV, SIGABRT, SIGTRAP, SIGTERM, SIGHUP, SIGQUIT, SIGPIPE}, [device](int sig) {
        std::cout << "Caught signal " << sig << std::endl;
        for (auto& sensor : device->getSensors()) {
            sensor->deInit();
        }
        exit(0);
    });

    ma::TransportRTSP rtsp;
    ma::TransportRTSP::RTSPConfig rtsp_config;
    rtsp_config.port    = 554;
    rtsp_config.format  = MA_PIXEL_FORMAT_H264;
    rtsp_config.session = "test";
    rtsp_config.user    = "admin";
    rtsp_config.pass    = "admin";
    rtsp.init(&rtsp_config);


    for (auto& sensor : device->getSensors()) {
        if (sensor->getType() == ma::Sensor::Type::kCamera) {
            camera = static_cast<ma::Camera*>(sensor);
            camera->init(0);
            ma::Camera::CtrlValue value;
            value.i32 = 0;
            camera->commandCtrl(ma::Camera::CtrlType::kChannel, ma::Camera::CtrlMode::kWrite, value);
            value.u16s[0] = 640;
            value.u16s[1] = 640;
            camera->commandCtrl(ma::Camera::CtrlType::kWindow, ma::Camera::CtrlMode::kWrite, value);
            value.i32 = 1;
            camera->commandCtrl(ma::Camera::CtrlType::kChannel, ma::Camera::CtrlMode::kWrite, value);
            value.u16s[0] = 640;
            value.u16s[1] = 640;
            camera->commandCtrl(ma::Camera::CtrlType::kWindow, ma::Camera::CtrlMode::kWrite, value);
            value.i32 = 2;
            camera->commandCtrl(ma::Camera::CtrlType::kChannel, ma::Camera::CtrlMode::kWrite, value);
            break;
        }
    }


    camera->startStream(ma::Camera::StreamMode::kRefreshOnReturn);
    static char buf[4 * 1024];
    while (true) {
        // for(auto & transport : device->getTransports()) {
        //     if(*transport && transport->available() > 0) {
        //         memset(buf, 0, sizeof(buf));
        //         int len = transport->receive(buf, sizeof(buf));
        //         transport->send(buf, len);
        //     }
        // }
        ma_img_t frame;
        static bool first = true;
        if (camera->retrieveFrame(frame, MA_PIXEL_FORMAT_H264) == MA_OK) 
        {
            MA_LOGI(MA_TAG, "frame size: %d", frame.size);
            if (first && !frame.key) {
                camera->returnFrame(frame);
            } else {
                 first = false;
                static int cnt = 0;
                if(cnt < 100)
                {
                    ma_img_t frame_copy = frame;
                    cv::Mat image(frame_copy.height, frame_copy.width, CV_8UC3, frame_copy.data);
                    std::string of_name = "out_" + std::to_string(cnt) + ".jpg";
                    cv::imwrite(of_name, image);
                    cnt++;
                }
                rtsp.send(reinterpret_cast<char*>(frame.data), frame.size);
                camera->returnFrame(frame);
            }
        }

        // (uint8_t*)image.data = frame.data
        // image.cols = frame.width;
        // image.rows = frame.height;
        // ma_img_t jpeg;
        // if (camera->retrieveFrame(jpeg, MA_PIXEL_FORMAT_JPEG) == MA_OK) {
        //     MA_LOGI(MA_TAG, "jpeg size: %d", jpeg.size);
        //     camera->returnFrame(jpeg);
        // }
        // ma_img_t raw;
        // if (camera->retrieveFrame(raw, MA_PIXEL_FORMAT_RGB888) == MA_OK) {
        //     MA_LOGI(MA_TAG, "raw size: %d", raw.size);
        //     camera->returnFrame(raw);
    }
    // }

    camera->stopStream();

    return 0;
}
