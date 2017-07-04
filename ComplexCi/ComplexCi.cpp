// ComplexCi.cpp : �������̨Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"

#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iterator>
#include <queue>
#include <algorithm>
#include <chrono>
#include <bitset>

using namespace std;
using namespace chrono;




class util
{
private:
	util(){}

public:

	template<typename Out>
	static void split(const string &s, char delim, Out result) {
		stringstream ss;
		ss.str(s);
		string item;
		while (getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

	static vector<string> split(const string &s, char delim) {
		vector<string> elems;
		split(s, delim, back_inserter(elems));
		return elems;
	}

};


class graphUtil
{
private:

	vector<bool> alreadyAccessBool;
	vector<int> bfsQueue;
	int startIt;
	int endIt;

public:
	graphUtil(int totalSize) : startIt(0),endIt(1)
	{
		bfsQueue.resize(totalSize, -1);
		alreadyAccessBool.resize(totalSize, 0);	
	}

	vector<int>& getBfsQueue()
	{
		return bfsQueue;
	}

	int getEndIt()
	{
		return endIt;
	}


	void getNeighbourFrontierAndScope(const vector<vector<int> > &adjListGraph, int scope, int currentNode)
	{
		startIt = 0;
		endIt = 1;

		//startIt and endIt will never execeed bfsQueue.size();
		//alreadyAccess is between 0 and endIt
		//fontier is between startIt and endIt

		bfsQueue[0] = currentNode;
		alreadyAccessBool[currentNode] = 1;

		for (int i = 0; i < scope; i++)
		{
			int lastEndIt = endIt;
			while (startIt != lastEndIt)
			{
				const vector<int>& neighbourNodeList = adjListGraph[bfsQueue[startIt++]];

				for (const auto& eachNeighbour : neighbourNodeList)
				{
					if (!alreadyAccessBool[eachNeighbour])
					{
						bfsQueue[endIt++] = eachNeighbour;
						alreadyAccessBool[eachNeighbour] = 1;
					}
				}
			}
		}

		for (int i = 0; i < endIt; i++)
		{
			alreadyAccessBool[bfsQueue[i]] = 0;
		}
	}



	long long basicCi(const vector<vector<int> > &adjListGraph, int ballRadius, int currentNode)
	{
		if (adjListGraph[currentNode].size() == 0)
		{
			return -1;
		}

		if (ballRadius == 0)
		{
			return adjListGraph[currentNode].size();
		}

		getNeighbourFrontierAndScope(adjListGraph, ballRadius, currentNode);

		long long ci = 0;

		for (int i = startIt; i < endIt; i++)
		{
			ci += (adjListGraph[bfsQueue[i]].size() - 1);
		}

		ci *= (adjListGraph[currentNode].size() - 1);

		return ci;
	}

	void deleteNode(vector<vector<int> > &adjListGraph, int node)
	{
		/*
		if (adjListGraph[node].size() == 0)
		{
		return;
		} // not must here
		*/

		for (auto neighbour : adjListGraph[node])
		{
			adjListGraph[neighbour].erase(remove(adjListGraph[neighbour].begin(), adjListGraph[neighbour].end(), node), adjListGraph[neighbour].end());
		}

		adjListGraph[node].clear();

	}

};






int main(int argc, char* argv[])
{
	unsigned int ballRadius = 1;
	unsigned int updateBatch = 1;
	unsigned int outputNumBatch = 1;


	string path = "";
	string output = "";

	if (argc > 4)
	{
		path = argv[1];
		output = path + "_out";

		ballRadius = stoi(argv[2]);
		updateBatch = stoi(argv[3]);
		outputNumBatch = stoi(argv[4]);

	}
	else
	{
		cout << "at least 3 parameters for csv path" << endl;
		cout << "e.g. 'C:/Users/zhfkt/Documents/Visual Studio 2013/Projects/ComplexCi/Release/karate.txt' 2 500 500" << endl;
		return 0;
	}

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	string modelID = "";

	{
		vector<string> fileName = util::split(path, '/');
		modelID = util::split(fileName[fileName.size() - 1], '.')[0];
	}

	cout << "path: " << path << endl;
	cout << "output: " << output << endl;
	cout << "modelID: " << modelID << endl;

	cout << "ballRadius: " << ballRadius << endl;
	cout << "updateBatch: " << updateBatch << endl;
	cout << "outputNumBatch: " << outputNumBatch << endl;


	cout << "First Read Start" << endl;

	unordered_set<int> allVex;
	string eachLine;
	ifstream is;
	is.open(path);

	while (is >> eachLine)
	{
		vector<string> csvEachLine = util::split(eachLine, ',');

		allVex.insert(stoi(csvEachLine[0]));
		allVex.insert(stoi(csvEachLine[1]));

	}
	is.close();
	int totalSize = allVex.size();

	cout << "First Read End/Second Read Start" << endl;


	vector<vector<int> > adjListGraph(totalSize);
	is.open(path);
	while (is >> eachLine)
	{
		vector<string> csvEachLine = util::split(eachLine, ',');

		adjListGraph[stoi(csvEachLine[0])].push_back(stoi(csvEachLine[1]));
		adjListGraph[stoi(csvEachLine[1])].push_back(stoi(csvEachLine[0]));
	}

	cout << "Second Read End" << endl;

	//--------------


	set<pair<long long, int> > allPQ; //ci/currentNode --- long is 32 bit on the win and long long is 64 bit / and long long can be multiple
	vector<long long> revereseLoopUpAllPQ(totalSize);

	graphUtil gu(totalSize);

	cout << "modelID: " << modelID << " First Cal CI" << endl;

	for (int i = 0; i < adjListGraph.size(); i++)
	{
		int currentNode = i;
		// core_ci
		long long ci = gu.basicCi(adjListGraph, ballRadius, currentNode);

		allPQ.insert(make_pair(ci, currentNode));
		revereseLoopUpAllPQ[currentNode] = ci;
	}

	vector<bool> candidateUpdateNodesBool(totalSize, 0);
	vector<int> candidateUpdateNodesVector(totalSize, -1);
	
	int candidateEnd = 0;

	vector<int> finalOutput;
	int loopCount = 0;
	while (true)
	{
		if ((updateBatch != 1) || ((loopCount%outputNumBatch == 0) && (updateBatch == 1)))
		{
			//restrict flood output when updateBatch == 1
			cout << "modelID: " << modelID << " loopCount: " << loopCount << " totalSize: " << totalSize << " maxCi: " << allPQ.rbegin()->first << " node: " << allPQ.rbegin()->second << endl;
		}
		
		loopCount += updateBatch;

		candidateEnd = 0;

		vector<int> batchList;
		unsigned int batchLimiti = 0;

		//pair<long long, int> debugPreviousMax = *(allPQ.rbegin());

		for (auto rit = allPQ.rbegin(); batchLimiti < updateBatch && (rit != allPQ.rend()); rit++, batchLimiti++)
		{
			if (rit->first < 0)//try -1 and batchList is the min point causing Zero Component
			{
				// ci algorithm ends
				goto CIEND;
			}

			batchList.push_back(rit->second);
			finalOutput.push_back(rit->second);
			allVex.erase(rit->second);  //remove key
		}
		
		for (int i : batchList)
		{
			gu.getNeighbourFrontierAndScope(adjListGraph, ballRadius + 1, i);
			const vector<int>& bfsQueue = gu.getBfsQueue();
			int endIt = gu.getEndIt();

			for (auto bfsIt = bfsQueue.begin(); bfsIt != bfsQueue.begin() + endIt; bfsIt++)
			{
				if (!candidateUpdateNodesBool[*bfsIt])
				{
					candidateUpdateNodesVector[candidateEnd++] = (*bfsIt);
					candidateUpdateNodesBool[*bfsIt] = 1;
				}
			}
		}


		for (int i : batchList)
		{
			gu.deleteNode(adjListGraph, i);
			//candidateUpdateNodesWithCi.insert(make_pair(i, -1));// no need to because self will be included in the candidateUpdateNodes and updated in the below
		}

		for (auto canIt = candidateUpdateNodesVector.begin(); canIt != (candidateUpdateNodesVector.begin() + candidateEnd); canIt++)
		{
			long long updatedCi = gu.basicCi(adjListGraph, ballRadius, *canIt);

			long long olderCi = revereseLoopUpAllPQ[*canIt];

			allPQ.erase(make_pair(olderCi, *canIt));
			allPQ.insert(make_pair(updatedCi, *canIt));

			revereseLoopUpAllPQ[*canIt] = updatedCi;

			candidateUpdateNodesBool[*canIt] = 0; //recover candidateUpdateNodesBool to zero

		}

	}

CIEND:

	//add left random

	cout << "Before Random adding the left CI equals zero: " << finalOutput.size() << endl;
	for (auto leftVex : allVex)
	{
		finalOutput.push_back(leftVex);
	}
	cout << "After Random adding the left CI equals zero: " << finalOutput.size() << endl;

	while (true)
	{
		if (finalOutput.size() % outputNumBatch == 0)
		{
			break;
		}
		else
		{
			finalOutput.push_back(-1);
		}
	}

	//--------------

	cout << "Outputing Start.." << endl;

	ofstream os(output);


	string output500 = "";
	for (unsigned int i = 0; i < finalOutput.size(); i++)
	{
		if (i % outputNumBatch == 0)
		{
			output500 = modelID;
		}


		if (finalOutput[i] != -1)
		{
			output500 += (',' + to_string(finalOutput[i]));
		}
		else
		{
			output500 += ',';
		}

		if (i % outputNumBatch == (outputNumBatch - 1) || i == (finalOutput.size() - 1))
		{
			os << output500 << endl;
			output500.clear();
		}

	}


	os.close();

	cout << "Outputing End.." << endl;

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(t2 - t1).count();

	cout << "duration: " << duration << "s" << endl;

	system("pause");
	return 0;
}

