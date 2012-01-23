//
//  Simulation/PathFinder.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_SIMULATION_PATHFINDER_H
#define _DREAM_SIMULATION_PATHFINDER_H

#include "../Numerics/Numerics.h"

#include <queue>
#include <vector>
#include <set>
#include <map>
#include <iostream>

namespace Dream {
	namespace Simulation {
	
	/*
		class Interface {
	public:
			// The cost to get to a certain location
			virtual void notify_cost (const StepT& location, const CostT& cost_from_start, const CostT& cost_to_end) {}
			
			virtual void add_stepsFrom (const Node *node, PathFinder & pf) const abstract;
			
			// The cost from one location to another
			virtual CostT estimate_path_cost (const StepT& from, const StepT& to) const abstract;
			virtual CostT exact_path_cost (const StepT& from, const StepT& to) const { 
				return estimate_path_cost(from, to);
			}
			
			virtual bool is_goal_state (PathFinder & pf, const StepT & from, const StepT &to) const {
				return from == to;
			}
		};
	*/
		
		template <typename _InterfaceT, typename _StepT, typename _CostT = Numerics::RealT>
		class PathFinder {
	public:
			typedef _InterfaceT InterfaceT;
			typedef _StepT StepT;
			typedef _CostT CostT;
			typedef std::set<StepT> StepsT;
			typedef std::vector<StepT> PathT;
			
			struct Node {
				bool closed;
				
				const StepT step;
				const CostT cost_from_start;
				const CostT cost_to_goal;
				const Node * parent;
				
				Node (const StepT& _step, const CostT& _cost_from_start, const CostT& _cost_to_goal, const Node* _parent = NULL)
					: step(_step), cost_from_start(_cost_from_start), cost_to_goal(_cost_to_goal), parent(_parent), closed(false) {
						//std::cout << "Total Cost (" << step << "): " << total() << std::endl;
				}
				
				CostT total () const {
					return cost_from_start+cost_to_goal;
				}
				
				bool operator< (const Node &other) const {
					return total() < other.total();
				}
				
				bool operator<= (const Node &other) const {
					return total() <= other.total();
				}
				
				bool operator> (const Node &other) const {
					return total() > other.total();
				}
				
				bool operator>= (const Node &other) const {
					return total() >= other.total();
				}
				
				struct greater : public std::binary_function<Node*, Node*, bool> {
					bool operator()(const Node* a, const Node* b) const { return *a > *b; }
				};
			};
			
			typedef std::vector<Node *> OpenT;
			typedef std::set<const Node *> ClosedT;
			
	protected:
			void open_push(Node * node) {
				_open.push_back(node);
				std::push_heap(_open.begin(), _open.end(), typename Node::greater());
			}
			
			void open_pop() {
				std::pop_heap(_open.begin(), _open.end(), typename Node::greater());
				_open.pop_back();
			}

			typedef std::map<StepT, Node*> TouchedT;
			typedef std::pair<typename TouchedT::iterator, bool> TouchedResultT;
			TouchedT _touched;
			
			OpenT _open;
			ClosedT _closed;

			const StepT _startLocation;
			const StepT _endLocation;
			
			InterfaceT* _interface;
			
			/* Recursively create the path */
			void construct_forward_path (const Node* node, PathT& path) const {
				if (node->parent != NULL)
					construct_forward_path(node->parent, path);
				
				path.push_back(node->step);
			}

	public:
			PathFinder (const StepT& start_location, const StepT& end_location, InterfaceT* interface)
			: _startLocation(start_location), _endLocation(end_location), _interface(interface) {
				CostT estimate_to_goal = _interface->estimate_path_cost(start_location, end_location);
				
				open_push (new Node(start_location, 0, estimate_to_goal));
			}
			
			~PathFinder () {
				//std::cout << __PRETTY_FUNCTION__ << std::endl;
				while (!_open.empty()) {
					delete _open.front();
					open_pop();
				}
				
				foreach(closed_node, _closed) {
					delete *closed_node;
				}
			}
			
			const OpenT open () const {
				return _open;
			}
			
			const ClosedT closed () const {
				return _closed;
			}
			
			const Node * top () const {
				return _open.front();
			}
			
			Node * top () {
				return _open.front();
			}
			
			StepT goal () {
				return _endLocation;
			}
			
			void add_step (const StepT & step, const Node * top) {
				CostT estimate_to_goal = _interface->estimate_path_cost(step, _endLocation);
				CostT step_cost = _interface->exact_path_cost(top->step, step);
				CostT cost_from_start = top->cost_from_start + step_cost;
				
				/* Is this step in the priority queue already? */
				bool insert_node = false;
				Node* next_node = new Node(step, cost_from_start, estimate_to_goal, top);
				
				typename TouchedT::iterator touched_iter = _touched.find(step);
				if (touched_iter != _touched.end()) {
					/*	If we have already been to the node, only 
					insert it if it costs less to get here this way */
					Node* current_next = (*touched_iter).second;
					if (*current_next > *next_node) {
						current_next->closed = true;
						insert_node = true;
					}
				} else {
					insert_node = true;
				}
				
				if (insert_node) {
					_interface->notify_cost(step, cost_from_start, estimate_to_goal);
					open_push(next_node);
					_touched[step] = next_node;
				} else {
					delete next_node;
				}
			}
		
			bool find_path (int iterations) {
				while (!_open.empty() && iterations--) {
					const Node * top = this->top();
					
					if (top->closed) {
						/*	We cannot remove a value from the open list,
							so we mark it as closed and skip it here.
							This is only done if a faster way to the same
							node is found. */
						open_pop();

					} else {
						if (_interface->is_goal_state(*this, top->step, _endLocation)) {
							/* We have reached the goal and no longer need to do any searching */
							return true;
						}
						
						/* We are going to process this node, so remove it from the open list */
						open_pop();
						_interface->add_stepsFrom(top, *this);
						
						//std::set<StepT> next_steps = _interface->steps_from(top);
						
						//for (iterate_each (next_steps, next_step)) {
						//	add_step(*next_step, top);
						//}
					}
					
					/* Once we have processed the node, place it on the closed list */
					_closed.insert(top);
				}
				
				return _open.empty();
			}
			
			PathT construct_current_path () const {
				PathT path;
				
				if (!_open.empty()) {
					const Node* top = this->top();
					construct_forward_path(top, path);
				}
				
				return path;
			}

		};
	}
}

#endif
