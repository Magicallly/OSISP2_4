#include <iostream>
#include "Sorter.h"


int main()
{
	Sorter* sorter = new Sorter();
	sorter->StartSorting();
	sorter->Wait();

	std::cout << "Success";
}
