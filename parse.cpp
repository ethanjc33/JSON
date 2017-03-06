//Ethan Clark <ejc49@zips.uakron.edu>
//JSON Parser

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <map>
#include <iterator>


//Concatenates the Num object with the next number in the string
double concat(double p, int q) {
	double hold = 1;
	while (hold <= q) hold *= 10;
	return (p * hold + q);
}

//Concatenates all numbers past a decimal point with their decimal value
double toDouble(double p, int q) {
	double hold = p;
	double hold2 = q;
	hold += (hold2 / 10);
	return hold;
}

//Converts JSON text to int value
int toInt(std::string::iterator f) {
	char c = *f;
	int hold = c - '0';
	return hold;
}


//Classes
struct value {
	virtual ~value() { }
	virtual void parse(std::string::iterator &f, std::string::iterator l,
					   std::vector<std::unique_ptr<value>> * v, int &weight) { }
};

void massParse(std::string::iterator & f, std::string::iterator l,
			   std::vector<std::unique_ptr<value>> * v, int &weight);

struct Null : value {
	Null() = default;

	virtual void parse(std::string::iterator &f, std::string::iterator l,
					   std::vector<std::unique_ptr<value>> * v, int &weight) {
		int hold = v->size() + 1;
		v->resize(hold);
		v->at(hold).reset(new Null());
		f + 4;
		++weight;
	}
};

struct Boolean : value {
	bool cond;

	Boolean() = default;
	Boolean(const bool & b) : cond(b) { }

	void parse(std::string::iterator &f, std::string::iterator l,
			   std::vector<std::unique_ptr<value>> * v, int &weight) {
		if (*f == 't') {
			int hold = v->size() + 1;
			v->resize(hold);
			v->at(hold).reset(new Boolean(true));
			f + 4;
			++weight;
		}

		else if (*f == 'f') {
			int hold = v->size() + 1;
			v->resize(hold);
			v->at(hold).reset(new Boolean(false));
			f + 5;
			++weight;
		}
	}
};

struct Num : value {
	double ber;

	Num() = default;

	virtual void parse(std::string::iterator &f, std::string::iterator l,
					   std::vector<std::unique_ptr<value>> * v, int &weight) {
		int x;
		Num y;
		while (isdigit(*f)) {
			x = toInt(f);
			y.ber = concat(y.ber, x);
			f++;
			if (*f == '.') {
				++f;
				while (isdigit(*f)) {
					int p = toInt(f);
					y.ber = toDouble(y.ber, p);
					++f;
				}
			}
		}
		Num * z = new Num();
		z = &y;
		int hold = v->size() + 1;
		v->resize(hold);
		v->at(hold).reset(z);
		++weight;

	}
};

struct String : value, std::string {
	using std::string::string;

	virtual void parse(std::string::iterator &f, std::string::iterator l,
					   std::vector<std::unique_ptr<value>> * v, int &weight) {
		String text;
		while (*f != '\"') {
			text += *f;
			++f;
		}
		String * x = new String();
		x = &text;
		int hold = v->size() + 1;
		v->resize(hold);
		v->at(hold).reset(x);
		++weight;
	}
};

struct Array : value, std::vector<value *> {
	std::vector<std::unique_ptr<value>> * cont;
	Array() = default;
	~Array() {
		for (value * v : *this) delete v;
	}

	using std::vector<value *>::push_back;

	virtual void parse(std::string::iterator &f, std::string::iterator l,
					   std::vector<std::unique_ptr<value>> * v, int &weight) {
		std::vector<std::unique_ptr<value>> u;
		while (*f != ']') massParse(f, l, &u, weight);
		Array * x = new Array();
		x->cont = &u;
		int hold = v->size() + 1;
		v->resize(hold);
		v->at(hold).reset(x);
		++weight;
	}
};

struct Object : value, std::map<std::string, value *> {
	std::map<String, value *> o;

	Object() = default;
	~Object() { o.clear(); }

	virtual void parse(std::string::iterator &f, std::string::iterator l,
					   std::vector<std::unique_ptr<value>> * v, int &weight) {
		std::vector<std::unique_ptr<value>> u;
		String text;
		if (*f == '"') {
			while (*f != '"') {
				text += *f;
				++f;
			}
		}
		++f;
		while (*f != '}') massParse(f, l, &u, weight);
		Object * x = new Object();
		int hold = v->size() + 1;
		v->resize(hold);
		v->at(hold).reset(x);
		++weight;
	}
};


//Skips spaces and unnecessary special characters
void skip(std::string::iterator & f, std::string::iterator l) {
	while ((f != l) || std::isspace(*f) || (*f == ',') || (*f == ':')) ++f;
}


void massParse(std::string::iterator & f, std::string::iterator l,
			   std::vector<std::unique_ptr<value>> * v, int &weight) {

	skip(f, l);

	if (*f == 'n') {
		Null hold;
		hold.parse(f, l, v, weight);
		return;
	}

	else if (*f == 't' || *f == 'f') {
		Boolean hold;
		hold.parse(f, l, v, weight);
		return;
	}

	else if (*f == '"') {
		String hold;
		hold.parse(f, l, v, weight);
		return;
	}

	else if (isdigit(*f)) {
		Num hold;
		hold.parse(f, l, v, weight);
		return;
	}

	else if (*f == '[') {
		Array hold;
		hold.parse(f, l, v, weight);
		return;
	}

	else if (*f == '{') {
		Object hold;
		hold.parse(f, l, v, weight);
		return;
	}

}


//Application Implementation
int main() {

	std::string name;						//Name of file
	int weight = 0;							//Weight counter for JSON objects
	std::vector<std::unique_ptr<value>> v;	//Vector will hold all JSON objects

	//Obtain file name from user (NOTE: Must provide format as "filename.txt")
	std::cout << "Enter JSON txt file:\n";
	std::cin >> name;
	std::ifstream file(name);

	//String constructor takes two iterators
	//These iterators will read from beginning to end of file
	std::string json{ std::istreambuf_iterator<char>(file),
										std::istreambuf_iterator<char>() };

	//Iterators for parsing through the entire JSON string
	std::string::iterator f = json.begin();
	std::string::iterator l = json.end();

	//Parsing loop looks at each character and determines which object to parse
	while (f != l) massParse(f, l, &v, weight);

	//Displays the JSON object's weight
	std::cout << "\nParsing Completed...\nWeight: " << weight << '\n';

	//TODO: Print out contents of vector
	//Will sequentially read elements and print according to types

	return 0;
}
