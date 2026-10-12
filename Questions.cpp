#include "Questions.h"
#include <string>
#include <iostream>
#include <random>
#include <chrono>

using namespace std;

Difficulty Questions::getDifficulty()
{
	return difficulty;
}
void Questions::generateEasyQuestion() {

	difficulty = Difficulty::EASY;

	enum easyQuestionTypes {
		ONE_STEP_OPERATION,
		ONE_STEP_ALGEBRA,
		PERIMETER_OF_RECTANGLE,
		EASYCOUNT
	};

	int min = 2;
	int small = 10;
	int max = 25;
	int top = 10;

	static random_device random;
	static mt19937 gen(random());
	uniform_int_distribution<> dist(small, max);
	uniform_int_distribution<> newdist(min, top);
	uniform_int_distribution<> enumdist(0, static_cast<int>(EASYCOUNT - 1));
	uniform_int_distribution<> opdist(0, static_cast<int>(OPERATIONCOUNT - 1));

	int firstNumber = dist(gen);
	int secondNumber = dist(gen); // 10-25
	int numberOne = newdist(gen); // 2-10
	int numberTwo = newdist(gen);

	Operations operation = static_cast<Operations>(opdist(gen));

	easyQuestionTypes questionType = static_cast<easyQuestionTypes>(enumdist(gen));
	

	if (questionType == PERIMETER_OF_RECTANGLE) {
		correctAnswer = (numberOne * 2) + (numberTwo * 2);
		questionPrompt = "What is the perimeter of a rectangle when the length is " + to_string(numberOne) + 
			" and the width is " + to_string(numberTwo) + "?";
	}
	else if (questionType == ONE_STEP_OPERATION) {
		switch (operation) {
		case ADD:
			correctAnswer = firstNumber + secondNumber;
			questionPrompt = to_string(firstNumber) + " + " + to_string(secondNumber) + " = ";
			break;
		case SUBTRACT:
			correctAnswer = firstNumber - secondNumber;
			questionPrompt = to_string(firstNumber) + " - " + to_string(secondNumber) + " = ";
			break;
		case MULTIPLY:
			correctAnswer = numberOne * numberTwo;
			questionPrompt = to_string(numberOne) + " x " + to_string(numberTwo) + " = ";
			break;
		case DIVIDE:
			correctAnswer = numberTwo;
			questionPrompt = to_string(numberOne * numberTwo) + " / " + to_string(numberOne) + " = ";
			break;
		}
	}
	else if (questionType == ONE_STEP_ALGEBRA) {
		switch (operation) {
			case ADD: 
				correctAnswer = secondNumber - firstNumber;
				questionPrompt = "x + " + to_string(firstNumber) + " = " + to_string(secondNumber);
				break;
			case SUBTRACT:
				correctAnswer = secondNumber + firstNumber;
				questionPrompt = "x - " + to_string(firstNumber) + " = " + to_string(secondNumber);
				break;
			case MULTIPLY:
				correctAnswer = numberOne;
				questionPrompt = "X x " + to_string(numberTwo) + " = " + to_string(numberOne * numberTwo);
				break;
			case DIVIDE:				
				correctAnswer = numberOne * numberTwo;
				questionPrompt = "x / " + to_string(numberOne) + " = " + to_string(numberTwo);
				break;
		}
	}	
}
void Questions::generateMediumQuestion() {

	difficulty = Difficulty::MEDIUM;

	enum mediumQuestionTypes {
		TWO_STEP_OPERATION,
		TWO_STEP_ALGEBRA,
		AREA_OF_RECTANGLE,
		MEDIUMCOUNT
	};

	int min = 2;
	int max = 8;
	int scaleMax = 12;

	static random_device random;
	static mt19937 gen(random());
	uniform_int_distribution<> dist(min, max);
	uniform_int_distribution<> scaledist(min, scaleMax);
	uniform_int_distribution<> enumdist(0, static_cast<int>(MEDIUMCOUNT - 1));
	uniform_int_distribution<> opdist(0, static_cast<int>(OPERATIONCOUNT - 1));


	int num1 = dist(gen);
	int num2 = dist(gen); 
	int num3 = dist(gen); // 2-8
	int scale = scaledist(gen); // 2-12


	mediumQuestionTypes questionType = static_cast<mediumQuestionTypes>(enumdist(gen));

	Operations operation2;
	Operations operation1;
	if (questionType == TWO_STEP_ALGEBRA) {
		do {
			 operation1 = static_cast<Operations>(opdist(gen)); // first operation is multiplication or division
		} while (operation1 == ADD || operation1 == SUBTRACT);

		do {
			operation2 = static_cast<Operations>(opdist(gen)); // second operation is addition or subtraction
		} while (operation2 == MULTIPLY || operation2 == DIVIDE);

		if (operation1 == MULTIPLY) {
			if (operation2 == ADD) {
				num3 = (num1 * scale) + num2;
				questionPrompt = "(X x " + to_string(num1) + ") + " + to_string(num2) + " = " + to_string(num3);
				correctAnswer = scale;
			}
			else {
				num3 = (num1 * scale) - num2;
				questionPrompt = "(X x " + to_string(num1) + ") - " + to_string(num2) + " = " + to_string(num3);
				correctAnswer = scale;
			}
		}
		else {
			if (operation2 == ADD) {
				questionPrompt = "(X / " + to_string(num1) + ") + " + to_string(num2) + " = " + to_string(num3);
				correctAnswer = (num3 - num2) * num1;
			}
			else {
				questionPrompt = "(X / " + to_string(num1) + ") - " + to_string(num2) + " = " + to_string(num3);
				correctAnswer = (num2 + num3) * num1;
			}
		}
		
	}
	else if (questionType == TWO_STEP_OPERATION) {
		operation1 = static_cast<Operations>(opdist(gen));
		do {
			operation2 = static_cast<Operations>(opdist(gen));

		} while (operation1 == DIVIDE && operation2 == DIVIDE);

		if (operation1 == DIVIDE) {
			if (operation2 == ADD) {
				correctAnswer = num1 + num3;
				questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " + " + to_string(num3) + " = ";
			}
			else if (operation2 == SUBTRACT) {
				questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " - " + to_string(num3) + " = ";
				correctAnswer = num1 - num3;
			}
			else {
				questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " x " + to_string(num3) + " = ";
				correctAnswer = num1 * num3;
			}
		}
		else if (operation1 == MULTIPLY) {
			if (operation2 == ADD) {
				questionPrompt = to_string(num1) + " x " + to_string(num2) + " + " + to_string(num3) + " = ";
				correctAnswer = (num1 * num2) + num3;
			}
			else if (operation2 == SUBTRACT) {
				questionPrompt = to_string(num1) + " x " + to_string(num2) + " - " + to_string(num3) + " = ";
				correctAnswer = (num1 * num2) - num3;
			}
			else if (operation2 == MULTIPLY) {
				questionPrompt = to_string(num1) + " x " + to_string(num2) + " x " + to_string(num3) + " = ";
				correctAnswer = num1 * num2 * num3;
			}
			else if (operation2 == DIVIDE) {
				questionPrompt = to_string(num1) + " x " + to_string(num2 * num3) + " / " + to_string(num3) + " = ";
				correctAnswer = num1 * num2;
			}
		}
		else if (operation1 == ADD) {
			if (operation2 == ADD) {
				questionPrompt = to_string(num1) + " + " + to_string(num2) + " + " + to_string(num3) + " = ";
				correctAnswer = num1 + num2 + num3;
			}
			else if (operation2 == SUBTRACT) {
				questionPrompt = to_string(num1) + " + " + to_string(num2) + " - " + to_string(num3) + " = ";
				correctAnswer = num1 + num2 - num3;
			}
			else if (operation2 == MULTIPLY) {
				questionPrompt = to_string(num1) + " + " + to_string(num2) + " * " + to_string(num3) + " = ";
				correctAnswer = num1 + (num2 * num3);
			}
			else if (operation2 == DIVIDE) {
				questionPrompt = to_string(num1) + " + " + to_string(num2 * num3) + " / " + to_string(num3) + " = ";
				correctAnswer = num1 + num2;
			}
		}
		else if (operation1 == SUBTRACT) {
			if (operation2 == ADD) {
				questionPrompt = to_string(num1) + " - " + to_string(num2) + " + " + to_string(num3) + " = ";
				correctAnswer = num1 - num2 + num3;
			}
			else if (operation2 == SUBTRACT) {
				questionPrompt = to_string(num1) + " - " + to_string(num2) + " - " + to_string(num3) + " = ";
				correctAnswer = num1 - num2 - num3;
			}
			else if (operation2 == MULTIPLY) {
				questionPrompt = to_string(num1) + " - " + to_string(num2) + " x " + to_string(num3) + " = ";
				correctAnswer = num1 - (num2 * num3);
			}
			else if (operation2 == DIVIDE) {
				questionPrompt = to_string(num1) + " - " + to_string(num2 * num3) + " / " + to_string(num3) + " = ";
				correctAnswer = num1 - num2;
			}
		}
	}
	else {
		num1 = scaledist(gen);
		num2 = scaledist(gen);
		questionPrompt = "What is the area of a rectangle when the length is " + to_string(num1) +
			" and the width is " + to_string(num2) + "?";
		correctAnswer = num1 * num2;
	}
}
void Questions::generateHardQuestion() {

	difficulty = Difficulty::HARD;

	enum hardQuestionTypes {
		THREE_STEP_OPERATIONS,
		THREE_STEP_ALGEBRA,
		VOLUME_OF_RECTANGULAR_PRISM,
		HARDCOUNT
	};

	int min = 2;
	int max = 12;

	static random_device random;
	static mt19937 gen(random());
	uniform_int_distribution<> scaledist(min, max);
	uniform_int_distribution<> enumdist(0, static_cast<int>(HARDCOUNT - 1));
	uniform_int_distribution<> opdist(0, static_cast<int>(OPERATIONCOUNT - 1));

	hardQuestionTypes questionType = static_cast<hardQuestionTypes>(enumdist(gen));

	Operations operation2;
	Operations operation1;
	Operations operation3;
	Operations operation4;

	int num1 = scaledist(gen);
	int num2 = scaledist(gen);
	int num3 = scaledist(gen);
	int num4 = scaledist(gen);

	if (questionType == VOLUME_OF_RECTANGULAR_PRISM) {

		questionPrompt = "What is the volume of a rectangular prism when the length is " + to_string(num1) +
			" and the width is " + to_string(num2) + " and the height is " + to_string(num3) + " ? ";
		correctAnswer = num1 * num2 * num3;
	}
	else if (questionType == THREE_STEP_ALGEBRA) {
		do {
			operation1 = static_cast<Operations>(opdist(gen)); // only multiplication or division

		} while (operation1 == ADD || operation1 == SUBTRACT);

		do {
			operation2 = static_cast<Operations>(opdist(gen)); // only addition or subtraction

		} while (operation2 == MULTIPLY || operation2 == DIVIDE);

		do {
			operation3 = static_cast<Operations>(opdist(gen)); // only multiplication or division 
																// operation 1 and operation 3 can not both be division
		} while ((operation3 == ADD || operation3 == SUBTRACT) && (operation1 == DIVIDE && operation3 == DIVIDE));

		do {
			operation4 = static_cast<Operations>(opdist(gen)); // only addition or subtraction

		} while (operation4 == MULTIPLY || operation4 == DIVIDE);

		if (operation1 == MULTIPLY) {
			if (operation3 == MULTIPLY) {
				if (operation2 == ADD) {
					if (operation4 == ADD) {
									// (*)(+)(*)(+)
					}
					else if (operation4 == SUBTRACT) {
									// (*)(+)(*)(-)
					}
				}
				else if (operation2 == SUBTRACT) {
					if (operation4 == ADD) {
									// (*)(-)(*)(+)
					}
					else if (operation4 == SUBTRACT) {
									// (*)(-)(*)(-)
					}
				}
			}
			else if (operation3 == DIVIDE) {
				if (operation2 == ADD) {
					if (operation4 == ADD) {
									// (*)(+)(/)(+)
					}
					else if (operation4 == SUBTRACT) {
									// (*)(+)(/)(-)
					}
				}
				else if (operation2 == SUBTRACT) {
					if (operation4 == ADD) {
									// (*)(-)(/)(+)
					}
					else if (operation4 == SUBTRACT) {
									// (*)(-)(/)(-)
					}
				}
			}
		}
		else if (operation1 == DIVIDE) { // operation 3 must be multiplication
			if (operation2 == ADD) {
				if (operation4 == ADD) {
					// (/)(+)(*)(+)
				}
				else if (operation4 == SUBTRACT) {
					// (/)(+)(*)(-)
				}
			}
			else if (operation2 == SUBTRACT) {
				if (operation4 == ADD) {
					// (/)(-)(*)(+)
				}
				else if (operation4 == SUBTRACT) {
					// (/)(-)(*)(-)
				}
			}
		}
	}
	else if (questionType == THREE_STEP_OPERATIONS) {
		operation1 = static_cast<Operations>(opdist(gen));

		do {
			operation2 = static_cast<Operations>(opdist(gen));
		} while (operation1 == DIVIDE && operation2 == DIVIDE);

		do {
			operation3 = static_cast<Operations>(opdist(gen));
		} while ((operation1 == DIVIDE && operation2 == DIVIDE) || (operation2 == DIVIDE && operation3 == DIVIDE) || (operation1 == DIVIDE && operation3 == DIVIDE));

		if (operation1 == DIVIDE) {
			if (operation2 == ADD) {
				if (operation3 == ADD) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " + " + to_string(num3) + " + " + to_string(num4) + " = ";
					correctAnswer = num1 + num3 + num4;
				}
				else if (operation3 == SUBTRACT) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " + " + to_string(num3) + " - " + to_string(num4) + " = ";
					correctAnswer = num1 + num3 - num4;
				}
				else if (operation3 == MULTIPLY) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " + " + to_string(num3) + " x " + to_string(num4) + " = ";
					correctAnswer = num1 + (num3 * num4);
				}
			}
			else if (operation2 == SUBTRACT) {
				if (operation3 == ADD) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " - " + to_string(num3) + " + " + to_string(num4) + " = ";
					correctAnswer = num1 - num3 + num4;
				}
				else if (operation3 == SUBTRACT) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " - " + to_string(num3) + " - " + to_string(num4) + " = ";
					correctAnswer = num1 - num3 - num4;
				}
				else if (operation3 == MULTIPLY) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " - " + to_string(num3) + " x " + to_string(num4) + " = ";
					correctAnswer = num1 - (num3 * num4);
				}
			}
			else if (operation2 == MULTIPLY) {
				if (operation3 == ADD) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " x " + to_string(num3) + " + " + to_string(num4);
					correctAnswer = (num1 * num3) + num4;
				}
				else if (operation3 == SUBTRACT) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " x " + to_string(num3) + " - " + to_string(num4);
					correctAnswer = (num1 * num3) - num4;
				}
				else if (operation3 == MULTIPLY) {
					questionPrompt = to_string(num1 * num2) + " / " + to_string(num2) + " x " + to_string(num3) + " x " + to_string(num4);
					correctAnswer = num1 * num3 * num4;
				}
			}
		}
		else if (operation1 == ADD) {
			if (operation2 == ADD) {
				if (operation3 == ADD) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " + " + to_string(num3) + " + " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 + num3 + num4;
				}
				else if (operation3 == SUBTRACT) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " + " + to_string(num3) + " - " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 + num3 - num4;
				}
				else if (operation3 == MULTIPLY) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " + " + to_string(num3) + " x " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 + (num3 * num4);
				}
				else if (operation3 == DIVIDE) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " + " + to_string(num3 * num4) + " / " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 + num3;
				}
			}
			else if (operation2 == SUBTRACT) {
				if (operation3 == ADD) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " - " + to_string(num3) + " + " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 - num3 + num4;
				}
				else if (operation3 == SUBTRACT) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " - " + to_string(num3) + " - " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 - num3 - num4;
				}
				else if (operation3 == MULTIPLY) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " - " + to_string(num3) + " x " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 - (num3 * num4);
				}
				else if (operation3 == DIVIDE) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " - " + to_string(num3 * num4) + " / " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 - num3;
				}
			}
			else if (operation2 == MULTIPLY) {
				if (operation3 == ADD) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " x " + to_string(num3) + " + " + to_string(num4) + " = ";
					correctAnswer = num1 + (num2 * num3) + num4;
				}
				else if (operation3 == SUBTRACT) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " x " + to_string(num3) + " - " + to_string(num4) + " = ";
					correctAnswer = num1 + (num2 * num3) - num4;
				}
				else if (operation3 == MULTIPLY) {
					questionPrompt = to_string(num1) + " + " + to_string(num2) + " x " + to_string(num3) + " x " + to_string(num4) + " = ";
					correctAnswer = num1 + ((num2 * num3) * num4);
				}
				else if (operation3 == DIVIDE) {
					questionPrompt = to_string(num1) + " + " + to_string(num2 * num4) + " x " + to_string(num3) + " / " + to_string(num4) + " = ";
					correctAnswer = num1 + (((num2 * num4) * num3) / num4);
				}
			}
			else if (operation2 == DIVIDE) {
				if (operation3 == ADD) {
					questionPrompt = to_string(num1) + " + " + to_string(num2 * num3) + " / " + to_string(num3) + " + " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 + num4;
				}
				else if (operation3 == SUBTRACT) {
					questionPrompt = to_string(num1) + " + " + to_string(num2 * num3) + " / " + to_string(num3) + " - " + to_string(num4) + " = ";
					correctAnswer = num1 + num2 - num4;
				}
				else if (operation3 == MULTIPLY) {
					questionPrompt = to_string(num1) + " + " + to_string(num2 * num3) + " / " + to_string(num3) + " x " + to_string(num4) + " = ";
					correctAnswer = num1 + (num2 * num4);
				}
			}
		}
		else if (operation1 == SUBTRACT) {
			if (operation2 == ADD) {
				if (operation3 == ADD) {

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
				else if (operation3 == DIVIDE) {

				}
			}
			else if (operation2 == SUBTRACT) {
				if (operation3 == ADD) {

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
				else if (operation3 == DIVIDE) {

				}
			}
			else if (operation2 == MULTIPLY) {
				if (operation3 == ADD) {

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
				else if (operation3 == DIVIDE) {

				}
			}
			else if (operation2 == DIVIDE) {
				if (operation3 == ADD) {

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
			}
		}
		else if (operation1 == MULTIPLY) {
			if (operation2 == ADD) {
				if (operation3 == ADD) {

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
				else if (operation3 == DIVIDE) {

				}
			}
			else if (operation2 == SUBTRACT) {
				if (operation3 == ADD) {

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
				else if (operation3 == DIVIDE) {

				}
			}
			else if (operation2 == MULTIPLY) {
				if (operation3 == ADD) {

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
				else if (operation3 == DIVIDE) {

				}
			}
			else if (operation2 == DIVIDE) {
				if (operation3 == ADD) {               

				}
				else if (operation3 == SUBTRACT) {

				}
				else if (operation3 == MULTIPLY) {

				}
			}
		}

	}
}
string Questions::getQuestionPrompt()
{
	return questionPrompt;
}
void Questions::setTimeLimit(int endurance)
{
	timeLimit = 30 + (endurance * 10); // 30 seconds to start and 10 extra seconds per endurance token
}
int Questions::getTimeLimit()
{
	return timeLimit;
}
bool Questions::checkAnswer(int playerAnswer)
{
	return playerAnswer == correctAnswer;
}
bool Questions::isTimedOut(int elapsedTime)
{
	if (elapsedTime > timeLimit) {
		timedOut = true;
	}
	else {
		timedOut = false;
	}
	return timedOut;
}
int Questions::getPoints()
{
	int easyPoints = 100;
	int mediumPoints = 200;
	int hardPoints = 300;

	if (getDifficulty() == Difficulty::EASY) {
		return easyPoints;
	}
	else if (getDifficulty() == Difficulty::MEDIUM) {
		return mediumPoints;
	}
	else {
		return hardPoints;
	}
}