/*
	Title:  Morse Input to STM32 Microcontroller
	Author: Yun Kim, Brett, Connor, Dominic
	Date:  4/15/26
	Description:  Sends keyboard inputs to a port where the STM32
    Microcontroller is connected to, in which it'll translates and display
    morse code on the STM32 side.
*/

#include <string>
#include <iostream>
#include <windows.h>
#include <cctype>
using namespace std;

// function prototypes for the recursive functions
string translate(string s);

int main()
{
    // Variables
    string text;    

    // COM PORT
    // Opens Serial Port
    HANDLE hSerial;
    hSerial = CreateFileA("COM3", GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        cout << "Serial Port does not exist, change COMX\n";
    }

    // Port Settings
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);

   if (!GetCommState(hSerial, &dcbSerialParams)) 
    {
        //error getting state
        cout << "GetCommState not working.\n";
    }
    dcbSerialParams.BaudRate=CBR_115200;
    dcbSerialParams.ByteSize=8;
    dcbSerialParams.StopBits=ONESTOPBIT;
    dcbSerialParams.Parity=NOPARITY;
    if(!SetCommState(hSerial, &dcbSerialParams))
    {
        //error setting serial port state
        cout << "SetCommState not working. \n";
    }

    // Timeout for when inputs aren't read
    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=50;
    timeouts.ReadTotalTimeoutMultiplier=10;
    timeouts.WriteTotalTimeoutConstant=50;
    timeouts.WriteTotalTimeoutMultiplier=10;
    if(!SetCommTimeouts(hSerial, &timeouts))
    {
    //error occureed. Inform user
    cout << "SetCommTimeouts not working. \n";
    }

    DWORD dwBytesWritten = 0;

    // Main Process
    while(1)
    {
        
        cout << "Input Morse Code:\n";
        getline (cin, text);
        string sendText = translate(text) + "\r\n";
        if(!WriteFile(hSerial, sendText.c_str(), sendText.length(), &dwBytesWritten, NULL))
        {
        //error occurred. Report to user.
        cout << "Failed to write to port.\n";
        }

        
    }
}
string translate(string initial){
    string final = "";
    string morseArray[] = {".- "/* a */, "-... "/* b */, "-.-. "/* c */, "-.. " /* d */, ". " /* e */,
                          "..-. "/* f */, "--. "/* g */, ".... " /* h */, ".. "/* i */, ".--- " /* j */,
                          "-.- " /* k */, ".-.. " /* l */, "-- " /* m */, "-. " /* n */, "--- " /* o */,
                          ".--. " /* p */, "--.- " /* q */, ".-. " /* r */, "... " /* s */, "- " /* t */,
                          "..- " /* u */, "...- " /* v */, ".-- " /* w */, "-..- " /* x */, "-.-- " /* y */,
                          "--.. " /* z */};
    
    bool isLetter = 0;                  // bool to check if there is a character in the string
    int index;                          // index for the string
    int length = initial.length();
    
    // iterates through the string to check for characters, digits, and symbols
    for(int i = 0; i < length; i++){
        
        // if there is a space in the originial string, it won't be ignored
        if(initial[i] == ' '){
            i++;
            final = final.append(" ");
        }

        initial[i] = tolower(initial[i]);           // makes all the characters lowercase
        
        //  checks to see if the current character is a lowercase letter, if there is, it will be skipped
        if(!islower(initial[i])){ 
            continue;
        }
        // if the current character is a lower case letter, it will convert to morse
        else{
            index = initial[i] - 97;                    // this will be the index for the morseArray to translate each letter
            final = final.append(morseArray[index]);    // appends the translated letter to the final string
            isLetter = 1;
        }       
    }
    // if there is at least one letter in the inputed string, we will send it to the LCD
    if(isLetter)
        return final;
   
    // if there are not letters in the inputed string, nothing will change on the LCD
    cout << "\nPlease enter only letters.\n";
    return "";
    
    
}