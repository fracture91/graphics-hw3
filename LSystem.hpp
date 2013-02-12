
#ifndef __LSYSTEM_H_
#define __LSYSTEM_H_

#include <string>
#include <map>
#include <stdexcept>

#include "Angel.h"

using std::string;
using std::map;
using std::pair;
using std::runtime_error;
using std::cout;
using std::endl;

class LSystem {
	private:
		string name;
		map<char, char> replacements;
		map<char, string> grammar;
		string turtleString;

		// apply rules to turtleString one time
		void iterateTurtleString() {
			// TODO
		}

	public:
		unsigned segmentLength;
		unsigned iterations;
		vec3 rotations;
		string start;

		LSystem(string name) {
			this->name = name;
			turtleString = "";
			start = "";
			segmentLength = iterations = 0;
			rotations = vec3(0, 0, 0);
		}

		string getName() {
			return name;
		}

		// add a replacement rule - use space for empty replacements
		void addReplacement(char target, char replacement) {
			replacements.insert(pair<char, char>(target, replacement));
		}

		// add a rule to this lsystem's grammar
		void addRule(char lhs, string rhs) {
			grammar.insert(pair<char, string>(lhs, rhs));
		}

		// get the generated turtle string
		string getTurtleString() {
			if(turtleString != "") { // already computed
				return turtleString;
			}
			if(start == "") {
				throw runtime_error("Empty start string");
			}

			turtleString = start;
			string lastTurtle = turtleString;
			unsigned i = 0;
			while(i < iterations) {
				iterateTurtleString();
				if(turtleString == lastTurtle) {
					break; // no longer changing
				}
				i++;
			}
			// TODO: apply replacements
			return turtleString;
		}

		void print() {
			cout << "LSystem " << name << ": " << endl <<
				"len=" << segmentLength << ", " << endl <<
				"iter=" << iterations << ", " << endl <<
				"rot=" << rotations << ", " << endl <<
				"reps=(";
			for (map<char, char>::const_iterator i = replacements.begin(); i != replacements.end(); ++i) {
				cout << i->first << "->" << i->second << ", ";
			}
			cout << "), " << endl <<
				"start=" << start << ", " << endl <<
				"rules=(";
			for (map<char, string>::const_iterator i = grammar.begin(); i != grammar.end(); ++i) {
				cout << i->first << " -> " << i->second << ", ";
			}
			cout << "), " << endl <<
				"turtleString=" << turtleString << endl;
		}

};

#endif

