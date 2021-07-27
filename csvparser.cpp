#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <iomanip>
#include <set>
#include <map>
#include <vector>
#define separator ','
#define EPS 0.000001

//Args and operation in cell
struct CellArgs
{
	float farg;
	float sarg;
	char operation;
	bool state;
};

struct CalculationCell
{
	int col;
	int row;
	std::string value;
};

//Find the calculation exception
bool calculation_error(float sarg, char operation){
	
	bool haserr = 0;
	std::set<char> operators {'/','*','-','+'};
	if (!(operators.find(operation) != operators.end())){
		std::cout << "ERR:Undefined operation";
		haserr = 1;
	} 
	if ((fabs(sarg) < EPS) && (operation == '/')){
		std::cout << "ERR:Division by zero";
		haserr = 1;
	}
	return haserr;
}

//Calculating by two args and operation
std::string calculate_cell(float farg, float sarg, char operation){

	float result = 0.0;
			switch (operation){
	    case '-':
	        result = farg - sarg;
	        break;
	    case '+':
	        result = farg + sarg;
	        break;
	    case '*':
	        result = farg * sarg;
	        break;
	    case '/':
	        result = farg / sarg;
	        break;
	    default:
	        std::cout << "ERR:Undefined operation\n";
	    }
	    return std::to_string(result);
	}

//Parsing the filename with .csv ending
bool fname_error(std::string fname){
	bool haserr = 0;
	std::regex reg("([^_\\W])([\\w]+)([^_\\W])(\.)(csv)");

	if (!std::regex_match(fname.c_str(), reg)){
		std::cout << "ERR:Not supported file\n";
		haserr = 1;
	} 
	else {
		std::ifstream file(fname.c_str());
		if(!file.is_open()){
		std::cout << "ERR:The file does not exist\n";
		haserr = 1;
		}
		file.close();
	}

    return haserr;
}

bool is_digit(std::string cell){
	std::regex reg ("([-?\\d]+)([.\\d]?+)");
	return std::regex_match(cell.c_str(), reg);
}

//Parsing arguments and operation in cell and calculating all
CellArgs calculate_parser(std::string cell, std::map <std::string,int> &tablecolumns,
											std::map <std::string,int> &tablerows,
											std::vector<std::vector<std::string>> &table){
	CellArgs thisCell,recCell;
	bool haserr = 1;
	bool iseq,rarg,larg;
	bool isnnull;
	std::cmatch result;
	std::regex reg ("([=])([a-zA-Z]+)([\\d]+)([/*-+])([a-zA-Z]+)([\\d]+)");
	std::string farg, sarg, operation;
	thisCell.state = std::regex_match(cell.c_str(), result, reg);
	isnnull = tablecolumns.count(result[2]) && tablecolumns.count(result[5]) &&
		   tablerows.count(result[3]) && tablerows.count(result[6]) && thisCell.state;


	if (isnnull){
		farg = table[tablerows[result[3]]][tablecolumns[result[2]]];
		sarg = table[tablerows[result[6]]][tablecolumns[result[5]]];
		operation = result[4];
		if ((is_digit(farg))&&(is_digit(sarg))){
			thisCell.farg = std::stof(farg);
			thisCell.sarg = std::stof(sarg);
			thisCell.operation = operation[0];
			haserr = 0;
		}else if (!is_digit(farg)){
			recCell = calculate_parser(farg, tablecolumns, tablerows, table);
			haserr = !recCell.state;
			if (!haserr){
				table[tablerows[result[3]]][tablecolumns[result[2]]] = 
				calculate_cell(recCell.farg,
							   recCell.sarg,
							   recCell.operation);
				thisCell.farg = std::stof(table[tablerows[result[3]]][tablecolumns[result[2]]]);
				thisCell.sarg = std::stof(sarg);
				thisCell.operation = operation[0];
			}
		}else {
			recCell = calculate_parser(sarg, tablecolumns, tablerows, table);
			haserr = !recCell.state;
			if (!haserr){
				table[tablerows[result[6]]][tablecolumns[result[5]]] = 
				calculate_cell(recCell.farg,
							   recCell.sarg,
							   recCell.operation);
				thisCell.farg = std::stof(farg);
				thisCell.sarg = std::stof(table[tablerows[result[6]]][tablecolumns[result[5]]]);
				thisCell.operation = operation[0];
			}
		}
	}
	thisCell.state = (thisCell.state && !haserr);

	return thisCell;
}

//Parsing csv file in maps and array and calculating
void csv_parser(std::string fname){
	std::map <std::string,int> tablecolumns;
	std::map <std::string,int> tablerows;
	std::map <int,std::string> columnnames;
	std::map <int,std::string> rownames;
	std::vector<std::vector<std::string>> table;

	std::string finstring;
	std::ifstream csvfile(fname.c_str());

	std::vector<CalculationCell> tocalcvec;
	CalculationCell celltocalc;
	CellArgs args;
	//parsing
	for (int i = 0; std::getline(csvfile, finstring); i++){
		std::stringstream linestream(finstring);
		std::vector<std::string> temp;
		std::string cell;
		for(int j = 0; std::getline(linestream, cell, separator) ; j++){
			if (i == 0){
				tablecolumns[cell] = j;
				columnnames[j] = cell;
			}
			if (j == 0){
				tablerows[cell] = i;
				rownames[i] = cell;
			}
			if ((i > 0)&&(j > 0)){
				if (is_digit(cell))
					temp.push_back(cell);
				else{
					celltocalc.col = j;
					celltocalc.row = i;
					celltocalc.value = cell;
					temp.push_back(cell);
					tocalcvec.push_back(celltocalc);
				}
			}else
				temp.push_back(cell);
		}
		table.push_back(temp);
	}
	csvfile.close();

	//calculating

	for (int i = 0; i < tocalcvec.size(); i++){
		args = calculate_parser(tocalcvec[i].value,tablecolumns,tablerows,table);
		if (args.state){
			if (calculation_error(args.sarg,args.operation)){
				std::cout << " in cell: " << columnnames[tocalcvec[i].col] 
									      << rownames[tocalcvec[i].row] << std::endl;
				exit(1);
			} else
				table[tocalcvec[i].row][tocalcvec[i].col] = calculate_cell(args.farg, args.sarg, args.operation);

		} else {
			std::cout << "ERR:Bad equation in cell: " << columnnames[tocalcvec[i].col] 
													  << rownames[tocalcvec[i].row] << std::endl;
			exit(1);
		}
	}



	for (int i = 0; i < table.size(); i++){
		std::vector<std::string> temp;
		temp = table[i];
		for(int j = 0; j < temp.size(); j++){
			std::cout << temp[j];
			if (j != (temp.size() - 1))
				std::cout << ',';
		}
		std::cout << std::endl;
	}
}

int main(int argc, char *argv[]){

	//Args exception
	if (argc > 2){
		std::cout << "ERR:Too much arguments\n";
		exit(1);
	} else if (argc < 2){
		std::cout << "ERR:You didn't enter a file name\n";
		exit(1);
	}
	//Filename exception
	if (fname_error(argv[1])){
		exit(1);
	}
	else
		csv_parser(argv[1]);

	return 0;
}