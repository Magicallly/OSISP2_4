#pragma once
#include <string>
#include <vector>
#include "TasksExecutor.h"
#include <Windows.h>

#define FILE_PATH "C:\\input.txt"
#define OUTPUT_FILE_PATH "C:\\output.txt"
#define DELIMITER '\n'
#define Vector std::vector<std::string>


class Sorter
{
public:
	explicit Sorter() {};
	Sorter(const std::string& fileName, int maxThreadsCount) {};
	~Sorter();
	void StartSorting();
	void Wait();

private:
	static bool GetDataFromFile(std::vector<std::string>& vecOfStr);
	static int GetThreadCapacity(std::vector<std::string>& vecOfStr);
	static void CreateTasks(std::vector<std::vector<std::string>>& vecOfParts, std::vector<std::string>& stringVector, short counter, TaskQueue* queue);
	static Vector MergeTwoVectors(Vector firstVector, Vector secondVector);
	static Vector MergeVectors(std::vector<std::vector<std::string>>& vecOfParts);
	static bool OutputVectorToFile(Vector vector);

	static DWORD WINAPI InputThreadFunction(LPVOID lpParam);
	static DWORD WINAPI OutputThreadFunction(LPVOID lpParam);

	std::vector<std::string> vectorOfStr;
	std::vector<std::vector<std::string>> vectorOfParts;

	TasksExecutor* executor = NULL;
	HANDLE hOutputThread = NULL;
	HANDLE hInputThread = NULL;
};

