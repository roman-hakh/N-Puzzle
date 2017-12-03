#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include "State.h"

#define DBG_PRINT 0
#define BIG_VAL 68761468
int	n_size = 0;
int	dsize = 0;
int max_open = 0;

std::priority_queue<State, std::vector<State>, CompareState>   open;

#if 0 /* to use unordered sed instead for set */
std::unordered_set<State, ?, ?>  closed;
#else
std::set<State>  closed;
#endif /* 0 */

void	print_queue(std::priority_queue<State, std::vector<State>, CompareState> *open) {
	State curr;

	while (!open->empty()) {
		curr = open->top();
		open->pop();
		curr.printState();
		printf("cost = %d, length = %d\n", curr.getCost(), curr.getLength());
	}
}

void	print_set(std::set<State> *closed) {
	std::set<State>::iterator iter;
	State	elem;
	for (iter = closed->begin(); iter != closed->end(); iter++) {
		elem = *iter;
		elem.printState();
		printf("cost = %d, length = %d\n", elem.getCost(), elem.getLength());
	}
}

int		hammiltonFunction(std::vector<int>  *map) {
	int inversions = 0;

	for (int i = 0; i < map->size(); i++) {
		if (map->at(i) != (i + 1))
			inversions++;
	}
	return (inversions);
}

#define swap_int(a, b) \
	(a) = (a) ^ (b); \
	(b) = (a) ^ (b); \
	(a) = (a) ^ (b); \

State	doMove(std::vector<int> map, moves_e move) {
	int	pos = 0;
	int	x, y, up, down, right, left;

	for (int i = 0; i < map.size(); i++)
		if (map[i] == 0 && (pos = i) >= 0)
			break ;
	x = pos % n_size;
	y = pos / n_size;
	up = x + ((y - 1) * n_size);
	down = x + ((y + 1) * n_size);
	right = (x + 1 == n_size) ? (BIG_VAL) : ((x + 1) + (y * n_size));
	left = (x - 1 < 0) ? (BIG_VAL) : ((x - 1)  + (y * n_size));

	if (move == UP && up >=0 && up < dsize) {
		swap_int(map[pos], map[up]);
		return (State(map, hammiltonFunction(&map), 0));
	}
	else if (move == DOWN && down >= 0 && down < dsize) {
		swap_int(map[pos], map[down]);
		return (State(map, hammiltonFunction(&map), 0));

	}
	else if (move == LEFT && left >= 0 && left < dsize) {
		swap_int(map[pos], map[left]);
		return (State(map, hammiltonFunction(&map), 0));

	}
	else if (move == RIGHT && right >=0 && right < dsize) {
		swap_int(map[pos], map[right]);
		return (State(map, hammiltonFunction(&map), 0));

	}
	return (State());
}

State	aStar(State *start) {
	State	curr;
	State	afterMove;

	start->cost = hammiltonFunction(&start->map);
	open.push(*start);
	while (!open.empty()) {
		curr = open.top();
		open.pop();

		/* check if it solved \
		 * 1 insversion == solved*/
		if (hammiltonFunction(curr.getptrMap()) == 1) {
			printf("FINISH\n");
			printf("max open size = %d, closed.size = %d\n", max_open, closed.size());
			curr.printState();
			printf("cost = %d, length = %d\n", curr.getCost(), curr.getLength());
			return (curr);
		}

		if (closed.find(curr) != closed.end()) {
			/* curr already exist in closed set,
			 * pop another node from open set*/
			continue;
		}

		closed.insert(curr);

		for (int i = UP; i < LAST; i++) {
			afterMove = doMove(curr.getMap(), (moves_e)i);
			if (afterMove.cost == 0)
				continue ;
			else {
				afterMove.length = curr.length + 1;
				open.push(afterMove);
				max_open++;
			}
		}

#if DBG_PRINT
		printf("########### POP FROM QUEUE #########\n");
		printf("max open size = %d, closed.size = %d\n", max_open, closed.size());
		curr.printState();
		printf("cost = %d, length = %d\n", curr.getCost(), curr.getLength());
		printf("########### END #########\n");
#endif /* EN_DBG_PRINT */
		
	}
	return (State());
}

int		main(void) {
	std::vector<int>	map6 ({1, 14, 2, 4, 6, 18,
							  9, 13, 3, 17, 11, 33,
							  19, 7, 16, 10, 5, 12,
							  8, 26, 20, 15, 22, 24,
							  21, 31, 27, 29, 23, 30,
							  25, 0, 32, 28, 34, 35});

	std::vector<int>	map4 ({	11,  0,  9,  4,
								2, 15,  7,  1,
								13,  3, 12,  5,
								8,  6, 10, 14,});

	std::vector<int>	*map = 0;
	std::vector<int>	path;

	map = &map6;
	n_size = (int)sqrt(map->size());
	dsize = n_size * n_size;
	State	start(*map, 0, 0);
	State	solved = aStar(&start);
}
