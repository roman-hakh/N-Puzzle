#ifndef NPUZZLE_SOLVER_HPP
#define NPUZZLE_SOLVER_HPP

#include <exception>
#include <list>
#include <queue>
#include <unordered_set>
#include "State.hpp"

typedef std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CompareState>  NPqueue;
typedef std::unordered_set<std::shared_ptr<State>, HashState, EqualState>                               NPset;

class NP_retVal {
public:
	std::list<int>	path;
	size_t			maxOpen;
	size_t			closedNodes;
	size_t			usedMemory;
};

class NPuzzleSolver {

private:
	void	aStar(const int *map, NP_retVal &result);
	void	checkPath(const State &root, const NP_retVal &result) const;
	bool	isSolvable(const int *map, int mapSize, int solutionType);

public:
	NPuzzleSolver();
	~NPuzzleSolver() {};
	void	solve(int heuristic, int solutionType, const int *map, const int mapSize, NP_retVal &result);

	class	NP_MapisNullException : public std::exception {
	public:
		virtual const char	*what() const throw() {return ("Map is NULL");};
	};

	class	NP_InvalidMapSize : public std::exception {
	public:
		virtual const char	*what() const throw() {return ("Invalid map size");};
	};

	class	NP_InvalidMap : public std::exception {
	public:
		virtual const char	*what() const throw() {return ("Map is unsolvable or not squared");};
	};

	class	NP_InvalidHeuristic : public std::exception {
	public:
		virtual const char	*what() const throw() {return ("Invalid heuristic");};
	};
};

#endif /* NPUZZLE_SOLVER_HPP */
