/****************************************************DUMP2VTKCONVERTER*****************************************************/
#include <fstream>
#include <iostream>
#include <cmath>
//#include "stdio.h"
#include "assert.h"
#include "stdio.h"
#include <limits>
#include <string>
#include <sstream>
#include <istream>
#include <Windows.h>

#include <stdio.h>  /* defines FILENAME_MAX */
    #ifdef WINDOWS
        #include <direct.h>
        #define GetCurrentDir _getcwd
    #else
        #include <unistd.h>
        #define GetCurrentDir getcwd
    #endif

using namespace std;
std::ifstream& GotoLine(std::ifstream& myfile, unsigned int num)
    {
        myfile.seekg(std::ios::beg);
        for (unsigned int i = 0; i < num - 1; ++i)
            {
                myfile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        return myfile;
    }

int CountFiles(string FirstName, string Extension, string SearchPath)
    {
        WIN32_FIND_DATA file;

        HANDLE search_handle=FindFirstFile(SearchPath.c_str(),&file);

        int DumpFileCount = 0;

        if(search_handle)
            {
                do
                    {
                        DumpFileCount++;
                    }while(FindNextFile(search_handle,&file));
                CloseHandle(search_handle);
            }
        return DumpFileCount;
    }

int ReadWriteFiles(string FirstName, string Extension, string SearchPath, int FilesCounted, int TimeStepLineNo, int NoOfAtomsLineNo, int FirstDataLineNo)
    {
        WIN32_FIND_DATA file;
        int WroteNoOfFiles = 0; double percent_completed = 0.0;
        int *ATOM_ID, *ATOM_TYPE01, *ATOM_TYPE02;
        double *X, *Y, *Z, *IX, *IY, *IZ, *VX, *VY, *VZ, *FX, *FY, *FZ, *OMEGAX, *OMEGAY, *OMEGAZ, *Radius;
        int TimeStep = 0, NoOfAtoms = 0, LineNo = 0;

        HANDLE search_handle=FindFirstFile(SearchPath.c_str(),&file);

        cout << endl << endl << endl << "%_COMPLETED    " << "FILE_FROM             ==>>             FILE_TO      " << endl << endl;
        if(search_handle)
            {
                do
                    {
                        /****************************************Reading dump file*******************************************/
                        ifstream dumpFile(file.cFileName);

                            if (!dumpFile)
                                {
                                    cout << "Couldn't open dumpFile" << endl;
                                    return 1;
                                }

                            GotoLine(dumpFile, TimeStepLineNo); dumpFile >> TimeStep;
                            GotoLine(dumpFile, NoOfAtomsLineNo); dumpFile >> NoOfAtoms;

                            ATOM_ID = new int[NoOfAtoms]; ATOM_TYPE01 = new int[NoOfAtoms]; ATOM_TYPE02 = new int[NoOfAtoms];
                            X = new double[NoOfAtoms]; Y = new double[NoOfAtoms]; Z = new double[NoOfAtoms];
                            IX = new double[NoOfAtoms]; IY = new double[NoOfAtoms]; IZ = new double[NoOfAtoms];
                            VX = new double[NoOfAtoms]; VY = new double[NoOfAtoms]; VZ = new double[NoOfAtoms];
                            FX = new double[NoOfAtoms]; FY = new double[NoOfAtoms]; FZ = new double[NoOfAtoms];
                            OMEGAX = new double[NoOfAtoms]; OMEGAY = new double[NoOfAtoms]; OMEGAZ = new double[NoOfAtoms];
                            Radius = new double[NoOfAtoms];

                            LineNo = FirstDataLineNo; int Current_ATOM_No = 0;
                            while (LineNo < (FirstDataLineNo + NoOfAtoms))
                                {
                                    GotoLine(dumpFile, LineNo);
                                    dumpFile >> ATOM_ID[Current_ATOM_No]; dumpFile >> ATOM_TYPE01[Current_ATOM_No]; dumpFile >> ATOM_TYPE02[Current_ATOM_No];
                                    dumpFile >> X[Current_ATOM_No]; dumpFile >> Y[Current_ATOM_No]; dumpFile >> Z[Current_ATOM_No];
                                    dumpFile >> IX[Current_ATOM_No]; dumpFile >> IY[Current_ATOM_No]; dumpFile >> IZ[Current_ATOM_No];
                                    dumpFile >> VX[Current_ATOM_No]; dumpFile >> VY[Current_ATOM_No]; dumpFile >> VZ[Current_ATOM_No];
                                    dumpFile >> FX[Current_ATOM_No]; dumpFile >> FY[Current_ATOM_No]; dumpFile >> FZ[Current_ATOM_No];
                                    dumpFile >> OMEGAX[Current_ATOM_No]; dumpFile >> OMEGAY[Current_ATOM_No]; dumpFile >> OMEGAZ[Current_ATOM_No];
                                    dumpFile >> Radius[Current_ATOM_No];
                                    LineNo++; Current_ATOM_No++;
                                }

                        dumpFile.clear();
                        dumpFile.close();
                        /***************************************************************************************************/

                        /****************************************Reading vtk file*******************************************/
                        ostringstream filetowrite;
                            filetowrite << FirstName << "_" << TimeStep << "_" << Extension << ".vtk";
                        string vtkfile = filetowrite.str();

                        ofstream vtkFile(filetowrite.str().c_str());
                            if (!vtkFile)
                                {
                                    cout << "Can't open vtkFile" << endl;
                                }

                            vtkFile << "# vtk DataFile Version 2.0" << endl;
                            vtkFile << "Generated by Rahul K Soni" << endl;
                            vtkFile << "ASCII" << endl;
                            vtkFile << "DATASET POLYDATA" << endl;
                            vtkFile << "POINTS " << NoOfAtoms << " float" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << X[i] << " " << Y[i] << " " << Z[i] << endl;
                                }

                            vtkFile << "VERTICES " << NoOfAtoms << " " << NoOfAtoms*2.0 << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << 1 << " " << i << endl;
                                }

                            vtkFile << "POINT_DATA " << NoOfAtoms << endl;
                            vtkFile << "VECTORS i float" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << IX[i] << " " << IY[i] << " " << IZ[i] << endl;
                                }

                            vtkFile << "VECTORS f float" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << FX[i] << " " << FY[i] << " " << FZ[i] << endl;
                                }

                            vtkFile << "VECTORS v float" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << VX[i] << " " << VY[i] << " " << VZ[i] << endl;
                                }

                            vtkFile << "SCALARS omegax float 1" << endl;
                            vtkFile << "LOOKUP_TABLE default" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << OMEGAX[i] << endl;
                                }

                            vtkFile << "SCALARS omegay float 1" << endl;
                            vtkFile << "LOOKUP_TABLE default" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << OMEGAY[i] << endl;
                                }

                            vtkFile << "SCALARS omegaz float 1" << endl;
                            vtkFile << "LOOKUP_TABLE default" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << OMEGAZ[i] << endl;
                                }

                            vtkFile << "SCALARS radius float 1" << endl;
                            vtkFile << "LOOKUP_TABLE default" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << Radius[i] << endl;
                                }

                            vtkFile << "SCALARS type float 1" << endl;
                            vtkFile << "LOOKUP_TABLE default" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << ATOM_TYPE01[i] << endl;
                                }

                            vtkFile << "SCALARS id float 1" << endl;
                            vtkFile << "LOOKUP_TABLE default" << endl;
                            for (int i = 0; i < NoOfAtoms; i++)
                                {
                                    vtkFile << ATOM_ID[i] << endl;
                                }
                        vtkFile.close();
                        /***************************************************************************************************/

                        WroteNoOfFiles++;

                        if(ATOM_ID) delete[] ATOM_ID; if(ATOM_TYPE01) delete[] ATOM_TYPE01; if(ATOM_TYPE02) delete[] ATOM_TYPE02;
                        if(X) delete[] X; if(Y) delete[] Y; if(Z) delete[] Z; if(IX) delete[] IX; if(IY) delete[] IY; if(IZ) delete[] IZ;
                        if(VX) delete[] VX; if(VY) delete[] VY; if(VZ) delete[] VZ; if(FX) delete[] FX; if(FY) delete[] FY; if(FZ) delete[] FZ;
                        if(OMEGAX) delete[] OMEGAX; if(OMEGAY) delete[] OMEGAY; if(OMEGAZ) delete[] OMEGAZ; if(Radius) delete[] Radius;

                        percent_completed = double(WroteNoOfFiles) / double(FilesCounted) * 100;

                        cout << percent_completed <<  "%    ";
                        std::wcout << file.cFileName << "   ==>>   ";
                        cout << vtkfile << endl;

                    }while(FindNextFile(search_handle,&file));
                CloseHandle(search_handle);
            }
        return WroteNoOfFiles;
    }

int main()
    {
        string FirstName; cout << "Enter file FirstName = "; cin >> FirstName;
        string Extension; cout << "Enter file Extension without (.) = "; cin >> Extension;
        bool use_default; char *yesorno = new char[3];  char yes[] = "yes";
        cout << "Do you want to use default values of \n         TimeStepLineNo, NoOfAtomsLineNo, FirstDataLineNo\nEnter 'yes' or 'no' : "; cin >> yesorno;
        if(strcmp(yesorno,yes)==0)   use_default = true;    else use_default = false;

        int TimeStepLineNo = 0, NoOfAtomsLineNo = 0, FirstDataLineNo = 0;

        if(use_default == true)
            {
                TimeStepLineNo = 2; NoOfAtomsLineNo = 4; FirstDataLineNo = 10;
            }else
            {
                cout << "Enter TimeStepLineNo = "; cin >> TimeStepLineNo;
                cout << "Enter NoOfAtomsLineNo = "; cin >> NoOfAtomsLineNo;
                cout << "Enter FirstDataLineNo = "; cin >> FirstDataLineNo;
            }

        char cCurrentPath[FILENAME_MAX];
        if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
            {
                return errno;
            }

        string SearchPath = cCurrentPath;
        SearchPath.push_back('\\'); SearchPath += FirstName; SearchPath.push_back('*'); SearchPath.push_back('.'); SearchPath += Extension;
        cout << endl << "SearchPath = " << SearchPath << endl;

        int FilesCounted = CountFiles(FirstName, Extension, SearchPath);
        cout << endl << FilesCounted << " files of type " << FirstName << "*." << Extension << " to be converted.d" << endl;

        int WroteTotalNoOfFiles = ReadWriteFiles(FirstName, Extension, SearchPath, FilesCounted, TimeStepLineNo, NoOfAtomsLineNo, FirstDataLineNo);
        cout << endl << "Converted " << WroteTotalNoOfFiles << " Files" << endl << endl;

        return 0;
    }
