#include <windows.h>
#include <iostream>
using namespace std;
int main()
{
    HANDLE COM_Port_1 = CreateFile("COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, NULL);
    HANDLE COM_Port_2 = CreateFile("COM2", GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, NULL);
    if (COM_Port_1 == INVALID_HANDLE_VALUE || COM_Port_2 == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            cout << "COM-port does not exist!\n";
        }
        cout << "Some other error.\n";
    }
    DCB Serial_Params = { 0 };
    Serial_Params.DCBlength = sizeof(Serial_Params);
    Serial_Params.BaudRate = CBR_9600;
    Serial_Params.ByteSize = 8;
    Serial_Params.StopBits = ONESTOPBIT;
    Serial_Params.Parity = NOPARITY;
    if (!SetCommState(COM_Port_2, &Serial_Params) || !SetCommState(COM_Port_1,&Serial_Params))
    {
         cout << "Error setting serial port state.\n";
         return 1;
    }
    char data = 'Y';
    DWORD Size = sizeof(data);
    DWORD Bytes_Written;
    cout<< Bytes_Written<<" -- \n";
    WriteFile(COM_Port_1, &data, Size, &Bytes_Written,NULL);
    cout << Size << " Bytes in string. " << Bytes_Written << " Bytes sended. " << endl;
    char Received_Char;
    ReadFile(COM_Port_2, &Received_Char, 1, &Size, NULL);
    if (Size > 0)
    {
        cout <<"Char sended: "<< Received_Char;
    }
    return 0;
}
