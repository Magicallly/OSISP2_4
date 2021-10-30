#include "Sorter.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>


Sorter::~Sorter()
{
	delete executor;
}

void Sorter::StartSorting()
{
	hInputThread = CreateThread(NULL, 0, InputThreadFunction, this, 0, NULL);
	hOutputThread = CreateThread(NULL, 0, OutputThreadFunction, this, CREATE_SUSPENDED, NULL);
}

bool Sorter::GetDataFromFile(std::vector<std::string>& vecOfStr)
{
	std::ifstream reader(FILE_PATH);
	if (!reader.is_open())
	{
		reader.close();
		return false;
	}

	while (!reader.eof())
	{
		std::string tempString;
		getline(reader, tempString, DELIMITER);
		vecOfStr.push_back(tempString + DELIMITER);
	}
	reader.close();
	return true;
}

int Sorter::GetThreadCapacity(std::vector<std::string>& vecOfStr)
{
	int threadAmount, maxThreadAmount = std::thread::hardware_concurrency();
	std::cout << "Max number of threads: " << maxThreadAmount << std::endl;
	std::cout << "Enter amount of threads: ";
	std::cin >> threadAmount;
	std::cout << DELIMITER;
	if ((threadAmount > vecOfStr.size()) || (threadAmount > maxThreadAmount) || (threadAmount <= 0))
		return -1;
	return threadAmount;
}


void SortPiece(void* vect)
{
	std::vector<std::string>* obj = (std::vector<std::string>*)vect;
	std::sort(obj->begin(), obj->end());
}

void Sorter::CreateTasks(std::vector<std::vector<std::string>>& vecOfParts, std::vector<std::string>& stringVector, short counter, TaskQueue* queue)
{
	int onePartCount = floor((((double)stringVector.size()) / counter) + .5);
	int globalCounter = 0;
	for (int i = 0; i < counter; i++)
	{
		vecOfParts[i].clear();
		int tempCounter = 0;
		while (onePartCount != tempCounter)
		{
			if (globalCounter == stringVector.size())
				break;
			vecOfParts[i].push_back(stringVector[globalCounter]);
			globalCounter++;
			tempCounter++;
		}
		Task* newtask = new Task(&SortPiece, (void*)&vecOfParts[i]);
		queue->Enqueue(newtask);
	}
}

Vector Sorter::MergeTwoVectors(Vector firstVector, Vector secondVector)
{
	int firstVectorSize = firstVector.size();
	int secondVectorSize = secondVector.size();

	Vector resultVector;
	resultVector.reserve(static_cast<__int64>(firstVectorSize) + static_cast<__int64>(secondVectorSize));

	int i = 0, j = 0;
	while (i < firstVectorSize && j < secondVectorSize) {

		if (firstVector[i] <= secondVector[j])
			resultVector.push_back(firstVector[i++]);
		else
			resultVector.push_back(secondVector[j++]);
	}

	// secondVectorSize закончился
	while (i < firstVectorSize)
		resultVector.push_back(firstVector[i++]);

	// firstVectorSize закончился
	while (j < secondVectorSize)
		resultVector.push_back(secondVector[j++]);

	return resultVector;
}

Vector Sorter::MergeVectors(std::vector<std::vector<std::string>>& vecOfParts)
{
	Vector tmpVector;
	if (vecOfParts.size() > 0) {
		tmpVector = vecOfParts[0];
	}
	for (int i = 1; i < vecOfParts.size(); i++) {
		tmpVector = MergeTwoVectors(tmpVector, vecOfParts[i]);
	}
	return tmpVector;
}

bool Sorter::OutputVectorToFile(Vector vector)
{
	std::ofstream stream(OUTPUT_FILE_PATH);
	if (!stream.good()) {
		system("pause");
		return false;
	}
	for (int i = 0; i < vector.size(); i++) {
		const char* cstr = vector[i].c_str();
		stream.write(cstr, strlen(cstr));
	}
	stream.close();
	return true;
}

DWORD WINAPI Sorter::InputThreadFunction(LPVOID lpParam)
{
	auto sorter = static_cast<Sorter*>(lpParam);
	if (!sorter->GetDataFromFile(sorter->vectorOfStr))
	{
		std::cout << "File not found" << std::endl;
		return -1;
	}
	int threadsCount = sorter->GetThreadCapacity(sorter->vectorOfStr);
	if (threadsCount == -1)
	{
		std::cout << "Invalid thread amount" << std::endl;
		return -1;
	}
	sorter->vectorOfParts.resize(threadsCount);

	TaskQueue* taskQueue = new TaskQueue();
	sorter->CreateTasks(sorter->vectorOfParts, sorter->vectorOfStr, threadsCount, taskQueue);

	sorter->executor = new TasksExecutor(threadsCount);
	sorter->executor->StartProcessing(threadsCount, taskQueue);
	ResumeThread(sorter->hOutputThread);

	return 0;
}

DWORD WINAPI Sorter::OutputThreadFunction(LPVOID lpParam)
{
	auto sorter = static_cast<Sorter*>(lpParam);

	WaitForSingleObject(sorter->hInputThread, INFINITE);
	CloseHandle(sorter->hInputThread);

	sorter->executor->WaitAll();

	Vector vector = sorter->MergeVectors(sorter->vectorOfParts);

	bool result = sorter->OutputVectorToFile(vector);

	if (!result) {
		printf("Unable to output to file");
		return -1;
	}
	vector.clear();

	return 0;
}


void Sorter::Wait()
{
	WaitForSingleObject(hOutputThread, INFINITE);
	CloseHandle(hOutputThread);
}