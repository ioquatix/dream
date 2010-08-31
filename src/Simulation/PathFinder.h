/*
 *  PathFinder.h
 *  Dream
 *
 *  Created by Samuel Williams on 14/09/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_SIMULATION_PATHFINDER_H
#define _DREAM_SIMULATION_PATHFINDER_H

#include <Dream/Numerics/Numerics.h>

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
			virtual void notifyCost (const StepT& location, const CostT& costFromStart, const CostT& costToEnd) {}
			
			virtual void addStepsFrom (const Node *node, PathFinder & pf) const abstract;
			
			// The cost from one location to another
			virtual CostT estimatePathCost (const StepT& from, const StepT& to) const abstract;
			virtual CostT exactPathCost (const StepT& from, const StepT& to) const { 
				return estimatePathCost(from, to);
			}
			
			virtual bool isGoalState (PathFinder & pf, const StepT & from, const StepT &to) const {
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
				const CostT costFromStart;
				const CostT costToGoal;
				const Node * parent;
				
				Node (const StepT& _step, const CostT& _costFromStart, const CostT& _costToGoal, const Node* _parent = NULL)
					: step(_step), costFromStart(_costFromStart), costToGoal(_costToGoal), parent(_parent), closed(false) {
						//std::cout << "Total Cost (" << step << "): " << total() << std::endl;
				}
				
				CostT total () const {
					return costFromStart+costToGoal;
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
			void openPush(Node * node) {
				m_open.push_back(node);
				std::push_heap(m_open.begin(), m_open.end(), typename Node::greater());
			}
			
			void openPop() {
				std::pop_heap(m_open.begin(), m_open.end(), typename Node::greater());
				m_open.pop_back();
			}

			typedef std::map<StepT, Node*> TouchedT;
			typedef std::pair<typename TouchedT::iterator, bool> TouchedResultT;
			TouchedT m_touched;
			
			OpenT m_open;
			ClosedT m_closed;

			const StepT m_startLocation;
			const StepT m_endLocation;
			
			InterfaceT* m_interface;
			
			/* Recursively create the path */
			void constructForwardPath (const Node* node, PathT& path) const {
				if (node->parent != NULL)
					constructForwardPath(node->parent, path);
				
				path.push_back(node->step);
			}

	public:
			PathFinder (const StepT& startLocation, const StepT& endLocation, InterfaceT* interface)
			: m_startLocation(startLocation), m_endLocation(endLocation), m_interface(interface) {
				CostT estimateToGoal = m_interface->estimatePathCost(startLocation, endLocation);
				
				openPush (new Node(startLocation, 0, estimateToGoal));
			}
			
			~PathFinder () {
				//std::cout << __PRETTY_FUNCTION__ << std::endl;
				while (!m_open.empty()) {
					delete m_open.front();
					openPop();
				}
				
				for (iterateEach (m_closed, closedNode)) {
					delete *closedNode;
				}
			}
			
			const OpenT open () const {
				return m_open;
			}
			
			const ClosedT closed () const {
				return m_closed;
			}
			
			const Node * top () const {
				return m_open.front();
			}
			
			Node * top () {
				return m_open.front();
			}
			
			StepT goal () {
				return m_endLocation;
			}
			
			void addStep (const StepT & step, const Node * top) {
				CostT estimateToGoal = m_interface->estimatePathCost(step, m_endLocation);
				CostT stepCost = m_interface->exactPathCost(top->step, step);
				CostT costFromStart = top->costFromStart + stepCost;
				
				/* Is this step in the priority queue already? */
				bool insertNode = false;
				Node* nextNode = new Node(step, costFromStart, estimateToGoal, top);
				
				typename TouchedT::iterator touchedIter = m_touched.find(step);
				if (touchedIter != m_touched.end()) {
					/*	If we have already been to the node, only 
					insert it if it costs less to get here this way */
					Node* currentNext = (*touchedIter).second;
					if (*currentNext > *nextNode) {
						currentNext->closed = true;
						insertNode = true;
					}
				} else {
					insertNode = true;
				}
				
				if (insertNode) {
					m_interface->notifyCost(step, costFromStart, estimateToGoal);
					openPush(nextNode);
					m_touched[step] = nextNode;
				} else {
					delete nextNode;
				}
			}
		
			bool findPath (int iterations) {
				while (!m_open.empty() && iterations--) {
					const Node * top = this->top();
					
					if (top->closed) {
						/*	We cannot remove a value from the open list,
							so we mark it as closed and skip it here.
							This is only done if a faster way to the same
							node is found. */
						openPop();

					} else {
						if (m_interface->isGoalState(*this, top->step, m_endLocation)) {
							/* We have reached the goal and no longer need to do any searching */
							return true;
						}
						
						/* We are going to process this node, so remove it from the open list */
						openPop();
						m_interface->addStepsFrom(top, *this);
						
						//std::set<StepT> nextSteps = m_interface->stepsFrom(top);
						
						//for (iterateEach (nextSteps, nextStep)) {
						//	addStep(*nextStep, top);
						//}
					}
					
					/* Once we have processed the node, place it on the closed list */
					m_closed.insert(top);
				}
				
				return m_open.empty();
			}
			
			PathT constructCurrentPath () const {
				PathT path;
				
				if (!m_open.empty()) {
					const Node* top = this->top();
					constructForwardPath(top, path);
				}
				
				return path;
			}

		};
	}
}

#endif
