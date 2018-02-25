/*
 * MathExpression.h
 *
 *  Created on: 25. nov. 2017
 *      Author: Peter S. Balling
 */

#ifndef SRC_MATHEXPRESSION_H_
#define SRC_MATHEXPRESSION_H_

#include <string>

#define MATH_SYMBOLS "/*-+"

class MathExpression
{
	public:
		MathExpression(const std::string& expression);
		virtual ~MathExpression();
		
		const std::string& Evaluate();
		
	protected:
		
		void FindOp(uint64_t& opPos, char& op);
		
		void Divide(void);
		void Multiply(void);
		void Subtract(void);
		void Add(void);
		
	private:
		std::string expr;
};

#endif /* SRC_MATHEXPRESSION_H_ */
