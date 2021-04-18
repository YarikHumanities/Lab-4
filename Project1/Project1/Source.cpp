#include <iostream>
#include <string>
#include <fstream>
#include "stdio.h"
#include "string.h"
using namespace std;

class Audio
{
private:
#pragma pack(push, 1)
    struct riff_header
    {
        int32_t chunkId;
        int32_t chunkSize;
        int32_t format;
    };
#pragma pack(pop)
#pragma pack(push, 2)
    struct first_subchunck
    {
        int32_t id;
        int32_t size;
        int16_t format,
            channelsNumber;
        int32_t sampleRate,
            byteRate;
        int16_t blockAlign,
            bitsPerSample;
    };
#pragma pack(pop)
#pragma pack(push, 3)
    struct second_subchunck
    {
        int32_t id;
        int32_t size;
        int16_t* data;
    };
#pragma pack(pop)
    string fileName;
    riff_header RiffHeader;
    first_subchunck firstSubchunck;
    second_subchunck secondSubchunck;

    FILE* audioFile;
    FILE* outputFile;

public:
    int16_t* newData;;
    int32_t newSize;
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
            secondSubchunck.data = new int16_t[rightSize];
            for (int i = 0; i < rightSize; i++)
            {
                fread(&secondSubchunck.data[i], firstSubchunck.blockAlign, 1, audioFile);
            }


        }
        //fclose(audioFile);
    }
    void resize(int k) {
        int16_t* resizedData = new int16_t[rightSize * k];
        for (int i = 0; i < rightSize; i++)
        {
            for (int j = 0; j < k; j++)
            {
                resizedData[k * i + j] = secondSubchunck.data[i];
            }
        }
        secondSubchunck.size *= k;
        RiffHeader.chunkSize = 36 + secondSubchunck.size;
        delete[] secondSubchunck.data;
        secondSubchunck.data = resizedData;
    }
    void interpolation(double k) {
        this->newSize = this->secondSubchunck.size * k;
        this->newData = new int16_t[this->newSize];
        int temp;
        int32_t x0, x1, y0, y1, y, x;
        for (int i = 0; i < this->secondSubchunck.size / this->firstSubchunck.blockAlign; i++)
        {
            temp = i * k;
            this->newData[temp] = this->secondSubchunck.data[i];
        }
        for (int i = 0; i < this->secondSubchunck.size / this->firstSubchunck.blockAlign - 1; i++)
        {
            x0 = i * k;
            x1 = (i + 1) * k;
            y0 = this->newData[x0];
            y1 = this->newData[x1];
            temp = x1 - x0 - 1;
            for (int j = 1; j < temp + 1; j++)
            {
                y = ((y1 - y0) * j) / (x1 - x0) + y0;
                x = j + x0;
                this->newData[x] = y;
            }
        }
        this->RiffHeader.chunkSize += this->newSize - this->secondSubchunck.size;
        this->secondSubchunck.size = this->newSize;
    }
    void saveTo(const char* fileName) {
        ofstream writeTo;
        writeTo.open(fileName, ios::binary);
        writeTo.close();
        FILE* output = fopen(fileName, "w");
        fwrite(&RiffHeader, sizeof(RiffHeader), 1, output);
        fwrite(&firstSubchunck, sizeof(firstSubchunck), 1, output);
        fwrite(&secondSubchunck, sizeof(secondSubchunck), 1, output);
        for (size_t i = 0; i < secondSubchunck.size / firstSubchunck.blockAlign; i++)
        {
            fwrite(&secondSubchunck.data[i], firstSubchunck.blockAlign, 1, output);
        }
        fclose(output);
    }
    void saveTo_in_case_of_interpol(const char* fileName) {
        ofstream writeTo;
        writeTo.open(fileName, ios::binary);
        writeTo.close();
        FILE* output = fopen(fileName, "w");
        fwrite(&RiffHeader, sizeof(RiffHeader), 1, output);
        fwrite(&
            firstSubchunck, sizeof(firstSubchunck), 1, output);
        fwrite(&secondSubchunck.id, sizeof(secondSubchunck.id), 1, output);
        fwrite(&secondSubchunck.size, sizeof(secondSubchunck.size), 1, output);

        for (int i = 0; i < this->secondSubchunck.size / this->firstSubchunck.blockAlign; i++) {
            fwrite(&this->newData[i], sizeof(this->newData[i]), 1, output);
        }
        fclose(output);

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
    const char* fileName = name.data();
    string name2 = "output.wav";
    const char* fileName2 = name2.data();
    Audio audio(fileName);
    cout << "Choose the way of resize:\n 1-simple resize\n2-interpolation resize\n";
    int choose;
    cin >> choose;
    if (choose == 1) {
        cout << "Enter change value: ";
        int k;
        cin >> k;
        string name2 = "output.wav";
        audio.resize(k);
        audio.saveTo(fileName2);
    }
    else if (choose == 2) {
        cout << "Enter change value: ";
        double k;
        cin >> k;
        audio.interpolation(k);
        audio.saveTo_in_case_of_interpol(fileName2);
    }
}