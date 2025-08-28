#include <cstdint>
#include <vector>
namespace MiniEngine::Backend {

class Node;

class DependencyGraph
{
public:
	DependencyGraph() noexcept;
	~DependencyGraph();

	struct Edge {
		const uint32_t to;
		const uint32_t from;

		Edge(DependencyGraph &graph, Node *from, Node *to);
	};

	class Node {
		const uint32_t target = 0x80000000u;
		uint32_t m_refCount;
		const uint32_t m_id;
	};

	void clear();

	std::vector<Edge> getEdges();
	std::vector<Node> getNodes();

	std::vector<Edge> getIncomingEdges(Node *node);
	std::vector<Edge> getOutgoingEdges(Node *node);

	Node *getNode(uint32_t id);

	void cull();


};

}
