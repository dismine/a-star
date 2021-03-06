#include "stdafx.h"
#include "parsing.h"
#include "global.h"

#include <fstream>
#include <vector>
#include <utility>
#include <limits>

using std::vector;
using std::pair;

static const int MAX_CHARS_PER_LINE = 512;
static const int MAX_TOKENS_PER_LINE = 3;
static const char* const DELIMITER = " ";

vector<pair<int, int>> coordinates; // point coordinates
vector<pair<int, int>> edges; // all connections
DynamicArray<double> map;
double onePixelWeight;

static bool ParseLong(const char *str, long &val)
{
    char *temp;
    bool res = true;
    errno = 0;
    val = strtol(str, &temp, 10);

    if (temp == str || *temp != '\0' || ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE))
	{
        res = false;
	}

    return res;
}

static int GetNumberObjects(std::ifstream &data, bool *ok)
{
	long numberPoints = -1;

	do
	{ 
		// read an entire line into memory
		char buf[MAX_CHARS_PER_LINE];
		data.getline(buf, MAX_CHARS_PER_LINE);

		// array to store memory addresses of the tokens in buf
		const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0

		char *next_token;

		// parse number of points
		token[0] = strtok_s(buf, DELIMITER, &next_token);

		if (token[0]) // zero if line is blank
		{
			if (ParseLong(token[0], numberPoints))
			{
				*ok = true;
				return static_cast<int>(numberPoints);
			}
			else
			{
				*ok = false;
				return 0;
			}
		}
	} while(!data.eof());

	*ok = false;
	return 0;
}

static std::vector<std::pair<int, int>> GetCoordinates(std::ifstream &data, int nPoints, bool *ok)
{
	std::vector<std::pair<int, int>> points;

	int processed = 0;
	do
	{
		// read an entire line into memory
		char buf[MAX_CHARS_PER_LINE];
		data.getline(buf, MAX_CHARS_PER_LINE);

		// array to store memory addresses of the tokens in buf
		const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0

		char *nextToken;

		// parse number of points
		token[0] = strtok_s(buf, DELIMITER, &nextToken);

		if (token[0]) // zero if line is blank
		{
			token[1] = strtok_s(nullptr, DELIMITER, &nextToken);
		}

		long x = 0;
		if (!ParseLong(token[0], x))
		{
			*ok = false;
			std::cout << "Can't get X coordinate. Line " << processed + 1 << ".\n";
			return points;
		}

		long y = 0;
		if (!ParseLong(token[1], y))
		{
			*ok = false;
			std::cout << "Can't get Y coordinate. Line " << processed + 1 << ".\n";
			return points;
		}

		if (x > WINDOW_WIDTH || y > WINDOW_HEIGHT)
		{
			*ok = false;
			std::cout << "X or Y coordinate out of size. Line " << processed + 1 << ".\n";
			std::cout << "Max X = " << WINDOW_WIDTH << " Y = " << WINDOW_HEIGHT << ".\n";
			return points;
		}

		for (unsigned int i = 0; i < points.size(); ++i)
		{
			const int xP = points[i].first;
			const int yP = points[i].second;
			if (xP == static_cast<int>(x) && yP == static_cast<int>(y))
			{
				*ok = false;
				std::cout << "All points should be unique. Line " << processed + 1 << ".\n";
				return points;
			}
		}

		std::pair<int, int> p(static_cast<int>(x), static_cast<int>(y));
		points.push_back(p);
		++processed;

	} while (processed < nPoints && !data.eof());

	if (points.empty() || points.size() != nPoints)
	{
		*ok = false;
		std::cout << "Not enough points were processd.\n"; 
		return points;
	}
	else
	{
		*ok = true;
		return points;
	}
}

static bool GetConnections(std::ifstream &data, int connections, DynamicArray<double> &map, vector<pair<int, int>> &edges)
{
	int processed = 0;
	do
	{
		// read an entire line into memory
		char buf[MAX_CHARS_PER_LINE];
		data.getline(buf, MAX_CHARS_PER_LINE);

		// array to store memory addresses of the tokens in buf
		const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0

		char *nextToken;

		// parse number of points
		token[0] = strtok_s(buf, DELIMITER, &nextToken);

		if (token[0]) // zero if line is blank
		{
			token[1] = strtok_s(nullptr, DELIMITER, &nextToken);
			if (!token[1]) 
			{
				std::cout << "Can't parse an index the second endge. Line " << processed << ".\n";
				break; // can't get the second edge
			}

			token[2] = strtok_s(nullptr, DELIMITER, &nextToken);
		}

		long indx1 = 0;
		if (!ParseLong(token[0], indx1))
		{
			std::cout << "Can't get an index the first endge. Line " << processed << ".\n";
			return false;
		}

		long indx2 = 0;
		if (!ParseLong(token[1], indx2))
		{
			std::cout << "Can't get an index the second endge. Line " << processed << ".\n";
			return false;
		}

		double weight = 0;
		char *end;
		weight = strtod(token[2], &end);
		if(end == token[2]) 
		{
			std::cout << "Can't get an edge weight. Line " << processed << ".\n";
			return false;
		} 

		if (!(weight > 0))
		{
			std::cout << "Weight should be more than 0. Line " << processed << ".\n";
			return false;
		}

		map[static_cast<int>(indx1)][static_cast<int>(indx2)] = weight;
		map[static_cast<int>(indx2)][static_cast<int>(indx1)] = weight; // Revers order also important

		pair<int, int> edge(static_cast<int>(indx1), static_cast<int>(indx2));
		edges.push_back(edge);

		++processed;

	} while (processed < connections && !data.eof());

	if (processed != connections)
	{
		std::cout << "Not enough edges were processd.\n";
		return false;
	}
	else
	{
		return true;
	}
}

bool Parse(int argc, char *argv[])
{
	if (argc < 2) 
	{ // Check the value of argc. If not enough parameters have been passed, inform user and exit.
        std::cout << "Please, provide file with graph description.\n\n"; // Inform the user of how to use the program
		std::cout << "The file format for graph description is:\n";
		std::cout << "<number_of_vertices>\n";
		std::cout << "<x> <y>\n";
		std::cout << " ....  ....\n";
		std::cout << "<x> <y>\n";
		std::cout << "<number_of_connections>\n";
		std::cout << "<0_point_idx> <1_point_idx> <weight>\n";
		std::cout << " ....  ....  ....\n";
		std::cout << "<2_point_idx> <1_point_idx> <weight>\n\n";
		std::cout << "Example:\n";
		std::cout << "4\n";
		std::cout << "1 1\n";
		std::cout << "100 100\n";
		std::cout << "100 200\n";
		std::cout << "300 300\n";
		std::cout << "4\n";
		std::cout << "0 1 1\n";
		std::cout << "0 2 0.5\n";
		std::cout << "1 3 1\n";
		std::cout << "2 3 0.5\n";
        return false;
	}

	// create a file-reading object
	std::ifstream data;
	data.open(argv[1]); // open a file
	if (!data.good())
	{
		std::cout << "File not found.\n";
		return false;
	}

	if (data.eof())
	{
		std::cout << "Unexpected end of file.\n";
		return false;
	}

	bool ok = false;
	const int numberPoints = GetNumberObjects(data, &ok);

	if (!ok)
	{
		std::cout << "Can't get number of points.\n";
		return false;
	}

	if (numberPoints < 2)
	{
		std::cout << "Not enough points.\n";
		return false;
	}

	ok = false;
	coordinates = GetCoordinates(data, numberPoints, &ok);

	if (!ok)
	{
		std::cout << "Can't get coordinates.\n";
		return false;
	}

	ok = false;
	const int numberEdges = GetNumberObjects(data, &ok);

	if (!ok)
	{
		std::cout << "Can't get number of edges.\n";
		return false;
	}

	// Note that you dont have to take care about a degenerate cases like no
    // connection between selected vertices.

	map.Resize(numberPoints, numberPoints);
	map.Init();
	ok = GetConnections(data, numberEdges, map, edges);

	if (!ok)
	{
		std::cout << "Can't get edges.\n";
		return false;
	}

	// For heuristic function we somehow should estimate the remaining distance to the goal.
	// Problem is how to understand weight value. These values do not depend on geometry positions, as i see it.
	// I came to conlusion that we could calcualte weigh for one pixel for each edge and take the smaller value.
	// It will not break the main rule for a heuristic function, it should not produce the value bigger than real.

#undef max
	onePixelWeight = std::numeric_limits<double>::max(); // First make value max possible.

	for(unsigned int i = 0; i < edges.size(); ++i)
	{
		const int indx1 = edges[i].first;
		const int indx2 = edges[i].second;

		const double length = Length(indx1, indx2);
		const double weight = map[indx1][indx2];
		onePixelWeight = min(weight / length, onePixelWeight);
	}
	
	return true;
}
