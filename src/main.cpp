#include "main.hpp"
#include <client_http.hpp>
#include <server_http.hpp>
#include "NPuzzleSolver.hpp"
#include <array>

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <fstream>
#include <vector>

#include "CSC.hpp"

using namespace std;
// Added for the json-example:
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

void	constructTaskRequest(std::string &requestStr) {
	namespace pt = boost::property_tree;
	// std::array<int, 9>		map = {{0, 3, 5, 6, 7, 1, 4, 2, 8}};
	std::array<int, 16>		map = {{11, 0, 9, 4, 2, 15, 7, 1, 13, 3, 12, 5, 8, 6, 10, 14}};
	// std::array<int, 16>		map = {{5, 6, 11, 14, 1, 15, 3, 4, 8, 2, 10, 12, 0, 9, 7, 13}};
	// std::array<int, 36>		map = {{1, 14, 2, 4, 6, 18,
	// 								9, 13, 3, 17, 11, 33,
	// 								19, 7, 16, 10, 5, 12,
	// 								8, 26, 20, 15, 22, 24,
	// 								21, 31, 27, 29, 23, 30,
	// 								25, 0, 32, 28, 34, 35}};

	// snail solution
	// std::array<int, 16>		map = {{0,	10,	5,	7,
	// 								11,	14,	4,	8,
	// 								1,	2,	6,	13,
	// 								12,	3,	15,	9}};

	// regular solution
	// std::array<int, 16>		map = {{12, 1, 11, 0,
	// 								3, 2, 14, 10,
	// 								15, 6, 4, 5,
	// 								9, 13, 8, 7}};
	pt::ptree		taskJson;
	pt::ptree		dataNode;
	pt::ptree		mapNode;

	taskJson.put("messageType", NP_TASK);

	for (size_t i = 0; i < map.size(); i++) {
		pt::ptree	mapElem;

		mapElem.put("", map[i]);
		mapNode.push_back(std::make_pair("", mapElem));
	}
	dataNode.add_child("map", mapNode);

	dataNode.put("algorithm", ASTAR);

	dataNode.put("heuristicFunction", HAMMILTON_DISTANCE);

	dataNode.put("solutionType", SNAIL_SOLUTION);

	taskJson.add_child("data", dataNode);

	std::stringstream	ss;
	boost::property_tree::json_parser::write_json(ss, taskJson, false);
	requestStr = ss.str();
}

void	constructStopRequest(std::string &requestStr) {
	namespace pt = boost::property_tree;

	pt::ptree		taskJson;
	pt::ptree		dataNode;

	taskJson.put("messageType", NP_STOP);

	taskJson.add_child("data", dataNode);

	std::stringstream	ss;
	boost::property_tree::json_parser::write_json(ss, taskJson, false);
	requestStr = ss.str();
}

void	clientCode() {
	HttpClient client("localhost:8080");

	// Synchronous request examples
	try {
		std::string	requestStr;

		constructTaskRequest(requestStr);
		std::cout << "Client send to server json: " << requestStr << std::endl;

		auto r2 = client.request("POST", "/message", requestStr);
		cout << "Client recv json: " << r2->content.rdbuf() << endl;
	}
	catch(const SimpleWeb::system_error &e) {
		cerr << "Client request error: " << e.what() << endl;
	}
}

int		main(int argc, char **argv) {
	CSC				mp;
	boost::thread	*server_thread;

	server_thread = mp.serverStart();

	// TODO: process arguments here

	// *** this code for tests
	this_thread::sleep_for(chrono::seconds(1));
	thread	client_thread([]() {
		clientCode();
	});
	client_thread.join();
	// *** delete this

	server_thread->join();
	delete server_thread;
	return (0);
}

/*
	Code conventions:
		1. Everything in camelCase
		2. 'using' / 'using namespace' can be applied in scope of function
		3. Short alias for this namespace -> namespace pt = boost::property_tree;
		4. Names of enumes and defines starts with 'NP_'

	TODO:
		+1. Makefile for macos
		2. -std=c++11 in Makefile ?
		3. Start new thread to solve puzzle, and send signal if client
			was send a stop message
		4. Use unsigned int in everywhere in NPuzzleSolver
		5. Make an API of NPuzzleSolver, means in header files must be
			everything what needs for NPuzzleSolver
		6. Think about using enum as argument in NPuzzleSolver class
		7. Use incline function in NPuzzleSolver.cpp instead of SWAP
		8. Leaks in NPsolver
		9. Add -O2 flag to Makefile
		10. Includes must be in cpp files, not in hpp
		*11. Use multithreads to find solution in NPuzzleSolver.cpp
		12. Put to NPuzzleSolver opennodes, closednodes, momory used
		13. rvalue reference, use it to return value from function
		14. Add mutex to NPuzzleSolver: stop task only when task is running
		15. Snail solution, discovery
		16. Asynchronus response from server
		17. Figure out how to build finishState
		18. Fix manhattanDistance for snail solution
		19. Redesign class State
		20. Abort when solving 3*3 puzzle
*/