#include <iostream>
#include <fstream>
#include <string>
#include "filehandler.h"

void FileHandler::AppendToFile(std::string newline)
{
    std::ofstream File;
    File.open(FileName,std::ios::app);
    File << newline<<'\n';
    LineCount++;

}

void FileHandler::ResetFile()
{
    std::ofstream File;
    File.open(FileName);
    File << "";
    LineCount=0;

}

void FileHandler::PrintContentOfFile(){
    std::string line;
    std::ifstream File(FileName);
    if (File.is_open()){
        while(std::getline(File,line))
        {
            std::cout << line << '\n';
        }
    }
    File.close();

}

std::string* FileHandler::ReadFile()
{   std::string line;
    std::ifstream File(FileName);
    int NumberOfLines = 0;

    if(File.is_open()){
        while(std::getline(File,line))
        {
            NumberOfLines++;
        }
    }

    const int Size = NumberOfLines;
    std::ifstream File2(FileName);
    std::string * Lines = new std::string[Size];
    //Lines = (std::string *)malloc(sizeof(std::string)* Size);
    int index = 0;
    if(File2.is_open()){
        while(std::getline(File2,line))
        {
            Lines[index++] = line;
            //std::cout << Lines[index] << '\n';
        }
    }
    File2.close();
    return Lines;
}

