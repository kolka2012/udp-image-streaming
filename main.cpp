/*   C++ UDP socket server for live image upstreaming
*   Modified from http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoServer.cpp
*   Copyright (C) 2015
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <QCoreApplication>
#include <QTime>
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()

#define BUF_LEN 65540 // Larger than maximum UDP packet size
using namespace std;
#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (argc != 3) { // Test for correct number of parameters
           cerr << "Usage: " << argv[0] << "<[s]-server,[c]-client> [Server Address (for client)] <Server Port>" << endl;
           exit(1);
       }
    char typeofCLient=argv[1][0];
    QTime mytime;
    unsigned short servPort;
    switch (typeofCLient)
    {
    case 'C':
    case 'c':
    {
        string servAddress = argv[2]; // Second arg: server address
         servPort = Socket::resolveService(argv[3], "udp");
        try {
            UDPSocket sock;
            int jpegqual =  ENCODE_QUALITY; // Compression Parameter

            Mat frame, send;
            vector < uchar > encoded;
            VideoCapture cap(0); // Grab the camera
            namedWindow("send", CV_WINDOW_AUTOSIZE);
            if (!cap.isOpened()) {
                cerr << "OpenCV Failed to open camera";
                exit(1);
            }

            uint last_cycle = mytime.msecsSinceStartOfDay();
            while (1) {
                cap >> frame;
                if(frame.size().width==0)continue;//simple integrity check; skip erroneous data...
                resize(frame, send, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);
                vector < int > compression_params;
                compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
                compression_params.push_back(jpegqual);

                imencode(".jpg", send, encoded, compression_params);
                imshow("send", send);
                int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

                int ibuf[1];
                ibuf[0] = total_pack;
                sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

                for (int i = 0; i < total_pack; i++)
                    sock.sendTo( & encoded[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);

                waitKey(FRAME_INTERVAL);

                uint next_cycle = mytime.msecsSinceStartOfDay();
                double duration = (next_cycle - last_cycle) /(double) 1000;
                cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

                cout << next_cycle - last_cycle;
                last_cycle = next_cycle;
            }
            // Destructor closes the socket

        } catch (SocketException & e) {
            cerr << e.what() << endl;
            exit(1);
        }
        break;}
    case 'S':
    case 's':
    {
        servPort = atoi(argv[2]); // First arg:  local port
        namedWindow("recv", CV_WINDOW_AUTOSIZE);
        try {
            UDPSocket sock(servPort);

            char buffer[BUF_LEN]; // Buffer for echo string
            int recvMsgSize; // Size of received message
            string sourceAddress; // Address of datagram source
            unsigned short sourcePort; // Port of datagram source

            uint last_cycle = mytime.msecsSinceStartOfDay();

            while (1) {
                // Block until receive message from a client
                do {
                    recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                } while (recvMsgSize > sizeof(int));
                int total_pack = ((int * ) buffer)[0];

                cout << "expecting length of packs:" << total_pack << endl;
                char * longbuf = new char[PACK_SIZE * total_pack];
                for (int i = 0; i < total_pack; i++) {
                    recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                    if (recvMsgSize != PACK_SIZE) {
                        cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                        continue;
                    }
                    memcpy( & longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
                }

                cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;

                Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
                Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
                if (frame.size().width == 0) {
                    cerr << "decode failure!" << endl;
                    continue;
                }
                imshow("recv", frame);
                free(longbuf);

                waitKey(1);
                uint next_cycle = mytime.msecsSinceStartOfDay();
                double duration = (next_cycle - last_cycle) / (double) 1000;
                cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

                cout << next_cycle - last_cycle;
                last_cycle = next_cycle;
            }
        } catch (SocketException & e) {
            cerr << e.what() << endl;
            exit(1);
        }
        break;
    }
    default:
    {// Test for correct parameter
        cerr << "Usage: " << argv[1] << "<[s]-server,[c]-client> [Server Address (for client)] <Server Port>" << endl;
        cerr << "Error in first arg"<<endl;
        exit(1);
    }
        break;
    }




    return a.exec();
}
