#ifndef __GRAPH_PROCESSING_H__
#define __GRAPH_PROCESSING_H__

#include <vector>
#include "VIDECLexer.h"
#include "./Workspace/Node.h"

#ifdef snprintf		//boost 1.69 error redefinition, undef and then include Boost.Threading system
#undef snprintf
#endif

#include <boost/thread.hpp>

namespace VIDEKit {

	class GraphProcessing
	{
		struct BlockSaveData {
			std::string uuid;
			std::string name;
			std::string sectionName;
			std::string typestr;
			VIDEKit::Node::NodeType type;
			double x;
			double y;
			uint32_t nextConnectionsDataCount;
		};

		struct ConnectionSaveData {
			std::string fromuuid;
			std::string fromconnectionname;
			std::string touuid;
			std::string toconnectionname;
			VIDEKit::Connection::SelfLinkType linkageType;
		};

		struct DebugControl {
			bool stepNext;
			bool active;
			std::string currentBlockUUID;
			int debugStepCheckMS;
		};

		std::vector<VIDEKit::Node*> *nodesListReference;

		class Window *window;

		bool __internalThreadRun;

		boost::thread *executionThread;

		DebugControl debugControl;

	public:
		GraphProcessing(std::vector<VIDEKit::Node*> *nodes, class Window *referenceWindw);
		~GraphProcessing();

		void clear();

		void load(const std::string &path);

		void save(const std::string &path);

		void newGraph();

		void runInThread(VIDECLexer *videc, bool debug = false);

		void step();
	private:
		void recursive_block_execution(VIDEKit::Node *currentNode, VIDECLexer *videc, VIDECLexer::BehaviorDefinition *definition, DebugControl *debug = nullptr);

		void saveBlockSettings(std::ofstream &outputFile,VIDEKit::Node * nodeReference);

		bool readBlockSettings(std::ifstream &inputFile, VIDEKit::Node * outputNode = nullptr, std::vector<ConnectionSaveData> *nodeConnections = nullptr);
	
		void linkConnections(std::vector<ConnectionSaveData> connectionsInfos);

		void __simpleExec(VIDECLexer *videc);
	public:
		bool hasEndedRun();

		boost::thread *thread();
	};

}

#endif //__GRAPH_PROCESSING_H__