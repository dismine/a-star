#include "stdafx.h"
#include "parsing.h"
#include "global.h"

#include <fstream>
#include <vector>
#include <utility>

using std::vector;
using std::pair;

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 3;
const char* const DELIMITER = " ";

vector<pair<int, int>> coordinates; // point coordinates
vector<pair<int, int>> edges; // all connections
DynamicArray<double> map;

bool ParseLong(const char *str, long &val)
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

int GetNumberObjects(std::ifstream &data, bool *ok)
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

std::vector<std::pair<int, int>> GetCoordinates(std::ifstream &data, int nPoints, bool *ok)
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
			std::cout << "Can't get X coordinate. Line " << processed << ".\n";
			points.clear(); 
			return points;
		}

		long y = 0;
		if (!ParseLong(token[1], y))
		{
			*ok = false;
			std::cout << "Can't get Y coordinate. Line " << processed << ".\n";
			points.clear();
			return points;
		}

		std::pair<int, int> p(static_cast<int>(x), static_cast<int>(y));
		points.push_back(p);
		++processed;

	} while (processed < nPoints && !data.eof());

	if (points.empty() || points.size() != nPoints)
	{
		*ok = false;
		std::cout << "Not enough points were processd.\n";
		points.clear(); 
		return points;
	}
	else
	{
		*ok = true;
		return points;
	}
}

bool GetConnections(std::ifstream &data, int connections, DynamicArray<double> &map, vector<pair<int, int>> &edges)
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
				std::cout << "Can't parse an index the second endge." << processed << ".\n";
				break; // can't get the second edge
			}

			token[2] = strtok_s(nullptr, DELIMITER, &nextToken);
		}

		long indx1 = 0;
		if (!ParseLong(token[0], indx1))
		{
			std::cout << "Can't get an index the first endge." << processed << ".\n";
			return false;
		}

		long indx2 = 0;
		if (!ParseLong(token[1], indx2))
		{
			std::cout << "Can't get an index the second endge." << processed << ".\n";
			return false;
		}

		double weight = 0;
		char *end;
		weight = strtod(token[2], &end);
		if(end == token[2]) 
		{
			std::cout << "Can't get an edge weight." << processed << ".\n";
			return false;
		} 

		map[static_cast<int>(indx1)][static_cast<int>(indx2)] = weight;

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
        std::cout << "Please, provide file with graph description.\n"; // Inform the user of how to use the program
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
	
	return true;
}
