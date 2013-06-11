/*
 * forest.h
 * Contains the class Forest, which is the central class in scrm
 *
 * The central data structure of scrm is a forest, which is a collection of
 * trees. This class on the one hand contains a NodeContainer object with all
 * nodes building the trees, and on the other hand functions to manipulate to
 * forest.
 *
 * Most functions are defined in forest.cc with exception of pure debugging
 * functions, with are in forest-debug.cc.
 */

#ifndef scrm_src_forest
#define scrm_src_forest

//Unless compiled with options NDEBUG, we will produce a debug output using 
//'dout' instead of cout and execute (expensive) assert statements.
#ifndef NDEBUG
#define dout std::cout
#else
#pragma GCC diagnostic ignored "-Wunused-value"
#define dout 0 && std::cout
#endif

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cfloat>
#include <cassert>
#include <boost/assign/std/vector.hpp>

#include "node.h"
#include "model.h"
#include "node_container.h"
#include "time_interval.h"
#include "tree_point.h"
#include "random/random_generator.h"
#include "random/constant_generator.h"
#include "random/mersenne_twister.h"

class TimeInterval;

class Forest
{
 public:

#ifdef UNITTEST
  friend class TestForest;
  friend class TestNode;
  friend class TestTimeInterval;
  friend class TestModel;
  friend class TestNodeContainer;
#endif

  friend class TimeInterval;
  friend class TimeIntervalIterator;
  //friend class NodeContainer;	
	
	
  Forest();
  Forest(Model *model, RandomGenerator *random_generator);
  Forest(Forest * current_forest, bool entire_ARG=true);
  ~Forest();

	
  //Getters & Setters
  const Model model() const { return *model_; }
  Model* writable_model() { return this->model_; };
  void set_model(Model* model) { this->model_ = model; }

  Node* local_root() const { return local_root_; }
  void set_local_root(Node* local_root) { local_root_ = local_root; };
  
  Node* primary_root() const { return primary_root_; }
  void set_primary_root(Node* primary_root) { primary_root_ = primary_root; };

  size_t sample_size() const { return this->sample_size_; }
  void set_sample_size(const size_t &size ) { sample_size_ = size; }
  
  double current_base() const { return current_base_; }
  void set_current_base(double base) { current_base_ = base; }

  double local_tree_length() const { return this->local_root()->length_below(); }
  
  void set_random_generator(RandomGenerator *rg) {
    this->random_generator_ = rg; }
  RandomGenerator* random_generator() const { return this->random_generator_; }
  
  NodeContainer const *getNodes() const { return nodes_; };
  
  double expo_sample() const {return expo_sample_;}
  void set_expo_sample(double expo_sample){expo_sample_=expo_sample;}      // Placeholder for exp(1) sampled values
  
  size_t prune_countdown() const{return prune_countdown_;}  // We will prune once this countdown reaches 0
  void set_prune_countdown(size_t  prune_countdown){prune_countdown_=prune_countdown_;}
  
  bool pruning() const{return pruning_;}
  void set_pruning(bool pruning){pruning_=pruning;}

  // Central functions
  void buildInitialTree();
  void sampleNextGenealogy();
  
  //Debugging Tools
  void addNodeToTree(Node *node, Node *parent, Node *first_child, Node *second_child);
  void createExampleTree();
  bool checkLeafsOnLocalTree(Node const* node=NULL) const;
  bool checkTree(Node const* root = NULL) const;
  double calcTreeLength() const;
  bool checkTreeLength() const;
  bool checkInvariants(Node const* node = NULL) const;
  bool checkNodeProperties() const;
  bool printNodes() const;

  //Tree printing
  int countLinesLeft(Node const* node) const;
  int countLinesRight(Node const* node) const;
  int countBelowLinesLeft(Node const* node) const;
  int countBelowLinesRight(Node const* node) const;
  bool printTree();
  std::vector<Node const*> determinePositions() const;
  void printPositions(const std::vector<Node const*> &positions) const;

  NodeContainer *nodes() { return this->nodes_; }

  //segegrating sites
  void seg_data(string treefile, int total_mut);
  
 private:
  //segegrating sites  
  void find_descndnt();
  void exp_mut_num(int total_mut);

  //Operations on the Tree
  Node* cut(const TreePoint &cut_point);
  void updateAbove(Node* node, 
                   bool above_local_root = false,
                   bool recursive = true,
                   bool local_only = false);

  // Tools for doing coalescence & recombination
  void sampleCoalescences(Node *start_node, bool pruning);
  TreePoint samplePoint(Node* node = NULL, double length_left = -1);
  size_t getNodeState(Node const *node, const double &current_time) const;
  double calcRate(Node* node, const int &state, const int &other_state, const TimeInterval &event) const;
  Node* updateBranchBelowEvent(Node* node, const TreePoint &event_point); 




  double sampleExpTime(double rate, double intervall_length);
  size_t sampleWhichRateRang(const double &rate_1, const double &rate_2) const;

  Node* possiblyMoveUpwards(Node* node, const TimeInterval &event);
 
  // Implementation of the different events
  Node* implementCoalescence(Node *coal_node, const TreePoint &coal_point);
  void  implementPwCoalescence(Node* root_1, Node* root_2, const double &time);

  // Pruning
  bool isPrunable(Node const* node) const;
  void prune(Node* node); 

  //Private variables
  NodeContainer* nodes_;    // The nodes of the Tree/Forest

  // We have 2 different roots that are important:
  // - local_root: root of the smallest subtree containing all local sequences
  // - primary_root: root of the tree that contains all local sequences (do we
  //   need this one?)

  Node* local_root_;
  Node* primary_root_;

  double current_base_;     // The current position of the sequence we are simulating
  size_t sample_size_;      // The number of sampled nodes (changes while building the initial tree)
  double expo_sample_;      // Placeholder for exp(1) sampled values
  size_t prune_countdown_;  // We will prune once this countdown reaches 0
  bool pruning_;

  Model* model_;
  RandomGenerator* random_generator_;

  
  void initialize(Model *model = new Model(),
                  RandomGenerator *rg = NULL);

  




  void createSampleNodes();

};

bool areSame(const double &a, const double &b, 
             const double &epsilon = std::numeric_limits<double>::epsilon());

#endif
