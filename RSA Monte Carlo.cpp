// RSA Monte Carlo.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <ctime>
#include <thread>
#include <future>
#include <vector>
#include <random>
#include <string>

using namespace std;

void MonteCarlo(promise<unsigned int> && prm, unsigned int numberOfPoints, int threadNum, bool isInQuietMode);

float const cicleRadious = 1.0;
float const squareSide = 2.0 * cicleRadious;
float const radiousSquared = cicleRadious * cicleRadious;

int main(int argc, char* argv[])
{
	clock_t start = clock();
	unsigned int numberOfPoints = 0;
	int numberOfThreads = 0;
	bool isInQuietMode = false;
	bool isPointSet = false;
	bool isThreadSet = false;
	for (size_t i = 1; i < argc; i++)
	{
		if (argv[i] == string("-s"))
		{
			numberOfPoints = atoi(argv[i + 1]);
			isPointSet = true;
		}
		if (argv[i] == string("-t") || argv[i] == string("-task"))
		{
			numberOfThreads = atoi(argv[i + 1]);
			isThreadSet = true;
		}
		if (argv[i] == string("-q"))
		{
			isInQuietMode = true;
		}
	}
	if (!isPointSet && !isThreadSet)
	{
		cout << "Incorrect command line parameters" << endl;
		return 1;
	}

	int mod = numberOfPoints % numberOfThreads;
	unsigned int pointsPerThread = numberOfPoints / numberOfThreads;
	vector<thread> workThreads;
	vector<future<unsigned int>> results;
	for (int i = 0; i < numberOfThreads; i++)
	{
		std::promise<unsigned int> prm;
		results.push_back(prm.get_future());
		unsigned int points = pointsPerThread + mod;
		workThreads.push_back(thread(MonteCarlo, move(prm), points, i, isInQuietMode));
		mod = 0;
	}

	for (int i = 0; i < workThreads.size(); i++)
		workThreads.at(i).join();


	unsigned int numPoints = 0;
	for (int i = 0; i < results.size(); i++)
		numPoints += results.at(i).get();

	double myPI = 4.0 * (static_cast <double> (numPoints) / static_cast <double> (numberOfPoints));
	cout << "Pi: " << myPI << endl;
	cout << "Total execution time for current run in s: " << float(clock() - start) / 1000 << endl;
	return 0;
}

void MonteCarlo(promise<unsigned int> && prm, unsigned int numberOfPoints, int threadNum, bool isInQuietMode)
{
	clock_t start = clock();
	if (!isInQuietMode)
	{
		string startMessage = "Thread " + to_string(threadNum) + " execution started.\n";
		cout << startMessage;
	}
	random_device rand;
	mt19937 rnge(rand());
	uniform_real_distribution<> dist(0, squareSide);

	unsigned int pointsInCircle = 0;
	for (size_t i = 0; i < numberOfPoints; i++)
	{
		float coordX = dist(rnge);
		float coordY = dist(rnge);;
		float distX = coordX - cicleRadious;
		float distY = coordY - cicleRadious;
		float distanceSquared = distX * distX + distY * distY;

		if (distanceSquared < radiousSquared)
		{
			pointsInCircle++;
		}
	}
	prm.set_value(pointsInCircle);

	if (!isInQuietMode)
	{
		string endMessage = "Thread " + to_string(threadNum) + " execution finished in s: "
			+ to_string(float(clock() - start) / 1000) + '\n';
		cout << endMessage;
	}
}
