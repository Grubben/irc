
#include "Server.hpp"

#include <iostream>
#include <sstream>

void sendNumericResponse(int clientSocket, int numericCode, const std::string& message = "") {

    std::stringstream response;
    response << ":" << SERVER_HOSTNAME << " " << numericCode;
    if (!message.empty()) {
        response << " " << SERVER_NAME << " " << message;
    }
    response << "\r\n";

    // Convert the response string to a C-string
    std::string responseStr = response.str();
    const char* responseCStr = responseStr.c_str();

    // Send the response to the client
    ssize_t bytesSent = send(clientSocket, responseCStr, responseStr.size(), 0);

    if (bytesSent < 0) {
        std::cerr << "Error sending numeric response to client." << std::endl;
    }
}
