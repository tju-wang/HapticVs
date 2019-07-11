#ifndef SERIALLIB_H
#define SERIALLIB_H

#define S_PORTS 256
#define S_BUFFER 15

// These two variables control how many and which ports are open, if any
typedef struct AVAILABLE_PORTS {
    int openQuantity;
    int statusList[S_PORTS];
} AVAILABLE_PORTS;

// This struct is a library standard to send/receive the serial data, this way any runtime error messages can reach the main code
typedef struct DATA_BUFFER {
    int code;
    char message[15];
    char data[S_BUFFER];
} DATA_BUFFER;

char* formatSerialPortName (int num);
char* getSerialPortAvailability (char *portName);
void updateAvailablePortsInfo (AVAILABLE_PORTS *ports);
void writeSerialPort (char *portName, int baudRate, DATA_BUFFER *buffer);
void listenSerialPort (char *portName, int baudRate, DATA_BUFFER *buffer);
BOOL InitSerialPort(char *portName,int baudRate,HANDLE *hComOpen);
BOOL writeData(HANDLE hComm,unsigned char *data,int sizeData);
BOOL ReadSerialPort(HANDLE hComm,unsigned char *buf);

#endif