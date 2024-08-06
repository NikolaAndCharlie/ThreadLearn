#include <thread>
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <numeric>
using namespace std;

template<typename Iterator, typename T>
struct accumulate_block
{
	void operator()(Iterator first, Iterator end, T& result) {
		result = accumulate(first, end, &result);
	}
};

vector<int> CreateRandomVector()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, 9999);

	vector<int> randVector;
	for (auto i = 0; i < 10000; i++) {
		randVector.push_back(distrib(gen));
	}

	return randVector;
}

static void SumNum(vector<int>::iterator begin, vector<int>::iterator end, unsigned long& result)
{
	result = std::accumulate(begin, end, result);

}

int main(int argc, char* argv[])
{
	vector<int> randomVector = CreateRandomVector();

	auto vectorBegin = begin(randomVector);
	auto vectorEnd = end(randomVector);
	unsigned int const length = distance(vectorBegin, vectorEnd);
	unsigned int nSum = 0;
	for (int i = 0; i < randomVector.size(); ++i) {
		nSum += randomVector.at(i);
	}
	//thread
	unsigned int const nHardWareThreadNum = thread::hardware_concurrency();
	unsigned int const nMinPreThread = 1000;
	unsigned int const nMinThreadNum = length / nMinPreThread;
	unsigned int const nThreadNum = min(nHardWareThreadNum != 0 ? nHardWareThreadNum : 2, nMinThreadNum);
	unsigned int const nBlockSize = length / nThreadNum;
	vector<unsigned long> resultVector(nThreadNum);

	vector<thread> threads(nThreadNum);  //因为有个主线程，所以线程数-1
	auto blockStart = vectorBegin;
	for (auto i = 0; i < nThreadNum; ++i) {
		auto blockEnd = blockStart;
		advance(blockEnd, nBlockSize);
		threads[i] = std::thread(SumNum, blockStart, blockEnd, ref(resultVector[i]));
		blockStart = blockEnd;
	}

	for_each(threads.begin(), threads.end(), mem_fn(&thread::join));
	int b = 0;
	int a = accumulate(resultVector.begin(), resultVector.end(), b);

	return 0;
}