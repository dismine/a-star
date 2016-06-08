#include "stdafx.h"
#include "astar.h"
#include "global.h"

#include <queue>
#include <set>

using std::priority_queue;
using std::set;

class Node
{
public:
	Node(int index);
	Node(int index, double g, double f);

	int Index() const;

    double GetFX() const;
    void   SetFX(int dest);

	double GetGX() const;
    void   SetGX(double g);
        
    // Estimation function for the remaining distance to the goal.
    double Estimate(int dest) const;
        
private:
	// index of point
    int index;
    // total distance already travelled to reach the node
    double gX;
    // fX = gX + remaining distance estimate
    double fX;  // smaller: higher priority
};

Node::Node(int index)
	: index(index), gX(0), fX(0)
{
}

Node::Node(int index, double g, double f)
	: index(index), gX(g), fX(f)
{
}

inline int Node::Index() const
{
	return index;
}

inline double Node::GetFX() const 
{
	return fX;
}

inline void Node::SetFX(int dest)
{
	fX = gX + Estimate(dest);
}

inline double Node::GetGX() const 
{
	return gX;
}

inline void Node::SetGX(double g)
{
	gX = g;
}

double Node::Estimate(int dest) const
{
	return Length(index, dest) * onePixelWeight;
}

// Determine priority (in the priority queue)
static bool operator<(const Node &a, const Node &b)
{
  return a.GetFX() > b.GetFX();
}

static vector<int> Directions(int from)
{
	vector<int> dir;
	for(unsigned int i = 0; i < coordinates.size(); ++i)
	{
		if (map[from][i] > 0)
		{
			dir.push_back(i);
		}
	}
	return dir;
}

vector<pair<int, int>> PathFind(int start, int finish)
{
	priority_queue<Node> pq[2]; // list of open (not-yet-tried) nodes
	int pqi = 0; // pq index
	set<int> closedNodes; // set of closed (tried-out) nodes
	set<int> openNodes; // set of open (not-yet-tried) nodes
	vector<int> from(coordinates.size(),-1); //contain shortes path from vertex to vertex

	{//Compiler can free this memory
	// create the start node and push into set of open nodes
    Node startNode = Node(start);
	startNode.SetFX(finish);
	pq[pqi].push(startNode);
	openNodes.insert(startNode.Index()); // add it to the open nodes set
	}

	// A* search
    while(!pq[pqi].empty())
    {
		// get the current node with the highest fx
        // from the list of open nodes
        const Node current = Node(pq[pqi].top().Index(), pq[pqi].top().GetGX(), pq[pqi].top().GetFX());

		int index = current.Index();

		pq[pqi].pop(); // remove the node from the open list
		openNodes.erase(index);
        // mark it on the closed nodes map
        closedNodes.insert(index);

		// quit searching when the goal state is reached
        if(index == finish) 
        {
			vector<pair<int, int>> path;

			// generate the path from finish to start
            // by following the directions
			while(!(index == start))
            {
				path.push_back(std::make_pair(index, from[index]));
				index = from[index];
			}

			return path;
		}

		const vector<int> dir = Directions(index);
		// generate moves (child nodes) in all possible directions
        for(unsigned int i=0; i < dir.size(); ++i)
        {
			auto search = closedNodes.find(dir[i]);
			if (search == closedNodes.end())
			{// Not found
				// generate a child node
				Node neighbour = Node(dir[i]);
				neighbour.SetGX(current.GetGX() + map[index][dir[i]]);
				neighbour.SetFX(finish);

				search = openNodes.find(dir[i]);
				if (search == openNodes.end())
				{// Not found
					openNodes.insert(dir[i]);
                    pq[pqi].push(neighbour);
                    // mark its parent node direction
					from[dir[i]] = index; 
				}
				else if (current.GetGX() > neighbour.GetGX())
                {
                    // update the parent direction info
                    from[dir[i]] = index;

                    // replace the node
                    // by emptying one pq to the other one
                    // except the node to be replaced will be ignored
                    // and the new node will be pushed in instead
                    while (!(pq[pqi].top().Index() == dir[i]))
                    {                
                        pq[1-pqi].push(pq[pqi].top());
                        pq[pqi].pop();       
                    }
                    pq[pqi].pop(); // remove the wanted node
                    
                    // empty the larger size pq to the smaller one
                    if (pq[pqi].size() > pq[1-pqi].size()) 
					{
						pqi=1-pqi;
					}

                    while (!pq[pqi].empty())
                    {                
                        pq[1-pqi].push(pq[pqi].top());
                        pq[pqi].pop();       
                    }
                    pqi=1-pqi;
                    pq[pqi].push(neighbour); // add the better node instead
				}
			}
		}
	}

	return vector<pair<int, int>>(); // no route found
}
