#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define UART_DEVICE "/dev/ttyS3"  // Change this if needed
#define BAUDRATE B115200

int main() {
    int uart_fd;
    struct termios options;

    uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    if (uart_fd == -1) {
        perror("Failed to open UART");
        return 1;
    }

    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);

    options.c_cflag |= (CLOCAL | CREAD);     // Enable receiver, ignore modem control lines
    options.c_cflag &= ~PARENB;              // No parity
    options.c_cflag &= ~CSTOPB;              // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;                  // 8 data bits
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input mode
    options.c_iflag &= ~(IXON | IXOFF | IXANY);         // No software flow control
    options.c_oflag &= ~OPOST;                         // Raw output

    tcsetattr(uart_fd, TCSANOW, &options);

    // Send data
    const char* message = "UART Loopback Test\n";
    int bytes_written = write(uart_fd, message, strlen(message));
    if (bytes_written < 0) {
        perror("UART write failed");
        close(uart_fd);
        return 1;
    }

    // Wait for data to be received
    usleep(100000);  // 100ms delay

    // Receive data
    char buffer[100];
    int bytes_read = read(uart_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Received: %s\n", buffer);
    } else {
        printf("No data received.\n");
    }

    close(uart_fd);
    return 0;
}

