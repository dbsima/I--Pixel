//===========================================================================
//===========================================================================
//===========================================================================
//==   Direct_Access_Image_Sample.cpp  ==  Author: Costin-Anton BOIANGIU   ==
//===========================================================================
//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================
#include "stdafx.h"
#include "Direct_Access_Image.h"
#include <iostream>
#include <fstream> 
#include <string>
//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================
int _tmain(int argc, _TCHAR* argv[])
{
	//printf("%s\n", argv[1]);
    //Verify command-line usage correctness

    std::ofstream outfile ("C:\\Users\\Student\\Documents\\GitHub\\I--Pixel\\test\\BAM\\test.txt");

	std::wstring sStartDir=L"";
	sStartDir = argv[0];

	std::string str( sStartDir.begin(), sStartDir.end() );

  /*  if (argc != 2)
    {
        _tprintf(_T("Use: %s <Input_Image_File_Name (24BPP True-Color)>\n"), argv[0]);
        return -99;
    }
*/
	KImage *outputs[50];
	KImage *confidences[50];
	int j = 0;
	for (int i = 1; i < argc; i+= 2) {

		TCHAR strNewFileName[0x100];
         _tprintf(_T("%s %s\n"), argv[i], argv[i + 1]);
		KImage *pImage = new KImage(argv[i]);

		if (pImage != NULL && pImage->IsValid()) {
			outputs[j] = pImage;
			TCHAR strNewFileName[0x100];
			confidences[j] = new KImage(argv[i + 1]);
			j++;
		} else {
			std::cout << "Wrong input image\n";
			return -100;
		}
	}

    //Return with success
    return 0;
}
//===========================================================================
//===========================================================================
