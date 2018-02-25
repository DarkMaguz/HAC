/*
 * MathExpression.cpp
 *
 *  Created on: 25. nov. 2017
 *      Author: Peter S. Balling
 */

#include "MathExpression.h"

#include <iostream>


MathExpression::MathExpression(const std::string& expression) :
	expr(expression)
{
}

MathExpression::~MathExpression()
{
}

const std::string& MathExpression::Evaluate(void)
{
	
	for (auto opIt : MATH_SYMBOLS)
	{
		
		std::cout << "##############################" << std::endl;
		
		std::cout << "opIt: " << opIt << std::endl;
		uint64_t opPos;
		char op = opIt;
		FindOp(opPos, op);
		
		std::cout << "Op: " << op << std::endl;
		
		while (opPos != std::string::npos)
		{
			std::cout << "-----------------------------------" << std::endl;
			std::cout << "Pos: " << opPos << std::endl;
			
			bool rightIsPositive = true;
			uint64_t rightSign = std::string(MATH_SYMBOLS).find(expr[opPos + 1]);
			
			std::cout << "rightSign: " << rightSign << std::endl;
			if (rightSign != std::string::npos)
			{
				if (rightSign < 2)
					throw "Misformed expression!";
				else if (rightSign == 2)
					rightIsPositive = false;
				//else
				//	
				std::cout << expr << std::endl;
				expr.erase(opPos, 1);
				std::cout << expr << std::endl;
				
			}
			std::cout << "rightIsPositive: " << rightIsPositive << std::endl;
			
			uint64_t exprStart = expr.find_last_of(MATH_SYMBOLS, opPos - 1);
			if (exprStart == std::string::npos)
				exprStart = 0;
			else
				exprStart += 1;
			std::cout << "Start: " << exprStart << std::endl;
			
			uint64_t exprEnd = expr.find_first_of(MATH_SYMBOLS, opPos + 1);
			if (exprEnd == std::string::npos)
				exprEnd = expr.length();
			std::cout << "End: " << exprEnd << std::endl;
			
			long double left = std::stold(expr.substr(exprStart, opPos));
			std::cout << "Left: " << left << std::endl;
			
			long double right = std::stold(expr.substr(opPos + 1, exprEnd));
			if (!rightIsPositive)
				right *= -1;
			std::cout << "Right: " << right << std::endl;
			
			uint64_t len = exprEnd - exprStart;
			std::cout << "len: " << len << std::endl;
			
			long double res;
			switch (op)
			{
				case '/':
					if (right == 0)
						throw "Division by zero is not defined!";
					res = left / right;
					break;
				case '*':
					res = left * right;
					break;
				case '-':
					res = left - right;
					break;
				case '+':
					res = left + right;
					break;
				default:
					break;
			}
			
			std::cout << "res: " << res << std::endl;
			
			expr.replace(exprStart, len, std::to_string(res));
			std::cout << "Expr: " << expr << std::endl;
			op = opIt;
			FindOp(opPos, op);
		}
	}
	
	return expr;
}

void MathExpression::FindOp(uint64_t& opPos, char& op)
{
	
	if (op == '-' || op == '+' )
	{
		opPos = expr.find_first_of("-+");
		op = expr[opPos];
	}
	else
	{
		opPos = expr.find_first_of(op);
	}
	
}

int main(int argc, char **argv)
{
	//                 0123456789*123456789
	MathExpression me("15+-15*+5");
	
	std::cout << me.Evaluate() << std::endl;
	
	return 0;
}
