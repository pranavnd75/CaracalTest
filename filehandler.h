//
// Created by prana on 2020-05-05.
//

#ifndef UNTITLED_FILEHANDLER_H
#define UNTITLED_FILEHANDLER_H
#include <iostream>
#include <fstream>
#include <string>


class FileHandler{
    std::string FileName;
    int LineCount;
    public:
        FileHandler(std::string filename):FileName(filename + ".temp"){
            ResetFile();
        }

        //void SetFilename(std::string filename);
        void AppendToFile(std::string newline);
        void ResetFile();
        void PrintContentOfFile();
        std::string* ReadFile();

};
#endif //UNTITLED_FILEHANDLER_H
