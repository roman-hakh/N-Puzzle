#include "NPuzzleSolver.hpp"
#include "heuristicFunctions.hpp"
#include "State.hpp"
#include "main.hpp"

#include <queue>
#include <unordered_set>
#include <cassert>
#include <cmath>
#include <functional>
#include <map>

typedef std::priority_queue<State *, std::vector<State *>, CompareState>	NPqueue;
typedef std::unordered_set<State *, HashState, EqualState>	NPset;

void	NPuzzleSolver::checkPath(const State *root, const std::list<int> &result) const {
	std::string ss[] = {"ROOT", "UP", "DOWN", "LEFT", "RIGHT"};
	State *state;
	State *prev = nullptr;

	std::cout << "###### PRINT PATH ######" << std::endl;
	for (auto const &move: result) {
		std::cout << "Move: " << ss[move] << std::endl << std::flush;
		if (move == 0)
			state = new State(*root, move);
		else // if can't create State with this move, then constructor throw exception
			state = new State(*prev, move);
		prev = state;
		state->printState();
	}
	std::cout << "###### END PRINT PATH ######" << std::endl;
	// if you see this message, then path is correct
}

void    freeMem(NPqueue *open, NPset *closed) {
	State	*curr = nullptr;

	while (!open->empty()) {
		curr = open->top();
		open->pop();
		delete curr;
	}

	State	*element = nullptr;
	std::unordered_set<State*, HashState, EqualState>::iterator i;
	for (i = closed->begin(); i != closed->end(); i++) {
		element = *i;
		delete element;
	}
}

void 	NPuzzleSolver::createPath(std::list<int> &result, const State *curr) const {
	while (curr->getMove() != ROOT) {
		result.push_front(curr->getMove());
		curr = curr->getPrev();
	}
	result.push_front(ROOT);
}

std::tuple<size_t, size_t, size_t>
constructRetVal(NPqueue *open, NPset *closed, unsigned int maxOpen, const int mapLength) {
	size_t	summ = open->size() + closed->size();

	// used memory
	summ = summ * (sizeof(State) + sizeof(int) * mapLength);
	return (std::make_tuple(maxOpen, closed->size(), summ));
}

std::tuple<size_t, size_t, size_t>
NPuzzleSolver::aStar(const int *map, std::list<int> &result) {
	std::tuple<size_t, size_t, size_t>	retVal;
	NPqueue	open;
	NPset	closed;
	State	*curr = nullptr;
	State	*root;
	State	*newState;
	size_t	maxOpen = 0;

	root = new State(map);
	open.push(root);

	if (verboseLevel) {
		std::cout << "******* ROOT" << std::endl;
		root->printState();
	}

	while (!open.empty()) {
		curr = open.top();
		open.pop();

		if (closed.find(curr) != closed.end()) {
			// curr already exist in closed set
			// pop another one
			delete curr;
			continue;
		}
		closed.insert(curr);

		if (curr->getPrice() == 0) {
			if (verboseLevel) {
				std::cout << "******* SOLVED" << std::endl;
				curr->printState();
			}
			createPath(result, const_cast<const State *>(curr));
			retVal = constructRetVal(&open, &closed, maxOpen, State::mapLength);

			// del allocated mem
			freeMem(&open, &closed);
			return retVal;
		}

		for (int move = UP; move < LAST; move++) {
			try {
				newState = new State(*curr, move);
				open.push(newState);
			}
			catch (State::NP_InvalidMove &e) {
				// can't create State with this move, let's try next move
			}
			catch (std::exception &e) {
				std::cout << "Error:" << __func__ << ":" << __LINE__ << ":"
							<< e.what() << std::endl;

				// del allocated mem
				freeMem(&open, &closed);
				throw e;
			}
		}

		if (open.size() > maxOpen)
			maxOpen = open.size();
	}
	//del allocated mem
	freeMem(&open, &closed);
	// can't find solution, throw an exception
	throw NP_InvalidMap();
	return (constructRetVal(&open, &closed, 0, 0));
}

NPuzzleSolver::NPuzzleSolver() {
}

static int getInversions(const int *map, int mapSize) {
	int inversions = 0;

	for (int i = 0; i < mapSize; i++)
		for (int j = i + 1; j < mapSize; j++) {
			if (map[i] == 0 || map[j] == 0)
				continue;
			if (map[i] > map[j])
				inversions++;
		}

	return (inversions);
}

bool NPuzzleSolver::isSolvable(const int *map, int mapSize, int solutionType) {
	State	finishState(solutionType, mapSize);
	int		inversionsFin = getInversions(finishState.getMapPtr(), mapSize);
	int		inversionsMap = getInversions(map, mapSize);
	int		size = (int)std::sqrt(mapSize);

	auto	isEven = [](const int number) { return ((number & 0x1) == 0); };
	auto findIndexInMap = [](const int *map, const int mapSize, const int value) {
		for (int i = 0; i < mapSize; i++)
			if (value == map[i])
				return (i);
		return (-1);
	};

	if (isEven(size)) {
		int zeroIdx;

		zeroIdx = findIndexInMap(map, mapSize, 0);
		assert(zeroIdx != -1);
		inversionsMap += zeroIdx;
		zeroIdx = findIndexInMap(finishState.getMapPtr(), mapSize, 0);
		assert(zeroIdx != -1);
		inversionsFin += zeroIdx;
	}
	return !(isEven(inversionsMap) ^ isEven(inversionsFin));
}

std::tuple<size_t, size_t, size_t>
NPuzzleSolver::solve(int heuristic, int solutionType,
		const int *map, const int mapLength, std::list<int> &result) {

	if (mapLength < 9)
		throw NP_InvalidMapSize();

	State::mapLength = mapLength;
	State::mapSize = (int)std::sqrt(mapLength);
	State::finishState = new State(solutionType);

	if (map == nullptr)
		throw NP_MapisNullException();

	if (std::sqrt(mapLength) - (int)(std::sqrt(mapLength)) != 0.0)
		throw NP_InvalidMapSize();

	if (!isSolvable(map, mapLength, solutionType))
		throw NP_InvalidMap();

	switch (heuristic) {
		case MISPLACED_TILES:
			State::heuristicFunc = &Heuristic::misplacedTiles;
			break;
		case MANHATTAN_DISTANCE:
			State::heuristicFunc = &Heuristic::manhattanDistance;
			break;
		case MANHATTAN_DISTANCE_PLUS_LINEAR_CONFLICTS:
			State::heuristicFunc = &Heuristic::MDplusLinearConflicts;
			break;
		case MISPLACED_TILES_PLUS_LINEAR_CONFLICTS:
			State::heuristicFunc = &Heuristic::MTplusLinearConflicts;
			break;
		case N_MAXSWAP:
			State::heuristicFunc = &Heuristic::nMaxSwap;
			break;
		default:
			throw NP_InvalidHeuristic();
			break;
	}

	return (aStar(map, result));
}
