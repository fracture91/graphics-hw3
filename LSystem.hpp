
#ifndef __LSYSTEM_H_
#define __LSYSTEM_H_

#include <string>
#include <map>
#include <stdexcept>
#include <stack>

#include "Angel.h"

using std::string;
using std::map;
using std::pair;
using std::runtime_error;
using std::cout;
using std::endl;
using std::stack;

class Turtle {
	public:
		unsigned segmentLength;
		vec3 rotations;
		stack<mat4>* ctm;
		enum Axis { X, Y, Z };

		Turtle() {
			segmentLength = 0;
			rotations = vec3(0, 0, 0);
			ctm = NULL;
		}

		void rotate(Axis axis, bool positive) {
			
		}
};

Turtle* tur = new Turtle();
tur->rotate(0, true);

class LSystem {
	private:
		string name;
		map<char, char> replacements;
		map<char, string> grammar;
		string turtleString;

		// apply rules to turtleString one time
		void iterateTurtleString() {
			string nextTurtle = "";
			for(string::iterator it = turtleString.begin(); it != turtleString.end(); ++it) {
				char currentChar = *it;
				map<char, string>::iterator rule = grammar.find(currentChar);
				if(rule != grammar.end()) {
					// if there is a rule for this character, replace with rhs
					nextTurtle += rule->second;
				} else {
					// no rule, char is same in newTurtle
					nextTurtle += currentChar;
				}
			}
			turtleString = nextTurtle;
		}

		// replace characters in turtleString according to replacements map
		void applyReplacements() {
			string repTurtle = "";
			for(string::iterator it = turtleString.begin(); it != turtleString.end(); ++it) {
				char currentChar = *it;
				map<char, char>::iterator rep = replacements.find(currentChar);
				if(rep != replacements.end()) {
					// if there is a replacement for this character, replace
					if(rep->second == ' ') {
						// just remove character from string
					} else {
						repTurtle += rep->second;
					}
				} else {
					// no replacement, char is same in repTurtle
					repTurtle += currentChar;
				}
			}
			turtleString = repTurtle;
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
			string lastTurtle;
			unsigned i = 0;
			while(i < iterations) {
				lastTurtle = turtleString;
				iterateTurtleString();
				if(turtleString == lastTurtle) {
					break; // no longer changing
				}
				i++;
			}
			applyReplacements();
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
				"turtleString=" << getTurtleString() << endl;
		}

};

#endif

