#include <iostream>
#include <string>
#include <fstream>
#include "stdio.h"
#include <cstdio>
#include <cmath>
#include "string.h"
using namespace std;

class Audio
{
private:
	struct riff_header
	{
		char chunkId[4];
		int32_t chunkSize;
		char format[4];
	};
	struct first_subchunck
	{
		char id[4];
		int32_t size;
		int16_t format,
			channelsNumber;
		int32_t sampleRate,
			byteRate;
		int16_t blockAlign,
			bitsPerSample;
	};
	struct second_subchunck
	{
		char id[4];
		int32_t size;
		int8_t* data;
	};
	string fileName;
	riff_header RiffHeader;
	first_subchunck firstSubchunck;
	second_subchunck secondSubchunck;

	FILE* audioFile;
	FILE* outputFile;

public:
	int rightSize;
	Audio(const char* fileName) {
		this->fileName;
		audioFile = fopen(fileName, "rb");
		if (!audioFile) {
			cout << "Error with audio file has occured";
		}
		else {

			fread(&RiffHeader, sizeof(RiffHeader), 1, audioFile);
			fread(&firstSubchunck, sizeof(firstSubchunck), 1, audioFile);
			fread(&secondSubchunck, sizeof(secondSubchunck), 1, audioFile);
			this->rightSize = secondSubchunck.size / firstSubchunck.blockAlign;
			secondSubchunck.data = new int8_t[rightSize];
			for (int i = 0; i < rightSize; i++)
			{
				fread(&secondSubchunck.data[i], firstSubchunck.blockAlign, 1, audioFile);
			}


		}
	}
	void show_info() {
		cout << "Riff header: " << endl;
		cout << "chunkId: " << RiffHeader.chunkId << endl;
		cout << "chunkSize: " << RiffHeader.chunkSize << endl;
		cout << "format: " << RiffHeader.format << endl;
		cout << "===========================" << endl;
		cout << "First subchunk: " << endl;
		cout << "id: " << firstSubchunck.id << endl;
		cout << "Size: " << firstSubchunck.size << endl;
		cout << "Format: " << firstSubchunck.format << endl;
		cout << "channelsNumber: " << firstSubchunck.channelsNumber << endl;
		cout << "sampleRate: " << firstSubchunck.sampleRate << endl;
		cout << "byteRate: " << firstSubchunck.byteRate << endl;
		cout << "blockAlign: " << firstSubchunck.blockAlign << endl;
		cout << "bitsPerSample: " << firstSubchunck.bitsPerSample << endl;
		cout << "===========================" << endl;
		cout << "Data: " << endl;
		cout << "SecondSubChunckSize: " << secondSubchunck.size << endl;
		cout << "SeconID: " << secondSubchunck.id << endl;
		cout << "Data actuallly: " << endl;
		for (int i = 0; i < rightSize; i++)
		{
			cout << secondSubchunck.data[i] << " ";
		}
		cout << endl;
		cout << "===========================" << endl;
	}

};
int main() {
	string name = "input.wav";
	//cout << "Enter name of .wav file: ";
	//getline(cin, name);
	//cout << "==============" << endl;
	const char  *fileName = name.data();
	Audio audio(fileName);
	/*string name1 = "Sound_out.wav";
	const char* fileName1 = name1.data();
	audio.writeTo(fileName);*/
	audio.show_info();

}