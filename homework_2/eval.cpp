#include <iostream>
#include <string>
#include <stack>
#include <cassert>
using namespace std;

bool is_number_char(char the_char) {
  return the_char >= '0' && the_char <= '9';
}

bool validate(string infix) {
  string infix_trimmed = "";
  for (int i = 0; i < infix.size(); i++) {
    if (infix.at(i) != ' ') infix_trimmed += infix.at(i);
  }

  // Must contain an operand.
  if (infix_trimmed.size() == 0) return false;
  if (infix_trimmed.size() == 1) return is_number_char(infix_trimmed.at(0));
  bool has_operand = false;

  {
    char first_char = infix_trimmed.at(0);
    char last_char = infix_trimmed.at(infix_trimmed.size() - 1);
    if (first_char == '|' || first_char == '&' || last_char == '|' ||
        last_char == '&')
      return false;
  }

  // All parentheses must be closed.
  int parentheses_sum = 0;

  for (int i = 0; i < infix_trimmed.size(); i++) {
    char current_char = infix_trimmed.at(i);
    char char_before, char_after;
    if (i > 0) char_before = infix_trimmed.at(i - 1);
    if (i < infix_trimmed.size() - 1) char_after = infix_trimmed.at(i + 1);
    if (current_char == '(') parentheses_sum++;
    if (current_char == ')') parentheses_sum--;

    if (is_number_char(current_char)) has_operand = true;

    if (current_char == '&' || current_char == '|') {
      if ((i > 0 && !is_number_char(char_before) && char_before != ')') ||
          (i < infix_trimmed.size() - 1 && !is_number_char(char_after) &&
           char_after != '(' && char_after != '!')) {
        // Allow operands before operators.
        return false;
      }
    } else if (is_number_char(current_char)) {
      if (i < infix_trimmed.size() - 1 &&
          (is_number_char(char_after) || char_after == '(' ||
           char_after == '!')) {
        return false;
      }
    }
  }

  if (!has_operand || parentheses_sum != 0) return false;

  return true;
}

string generatePostfix(string infix) {
  // Generate postfix.
  string postfix = "";
  stack<char> operators;
  for (int i = 0; i < infix.size(); i++) {
    char current_char = infix.at(i);

    if (is_number_char(current_char)) {
      postfix += current_char;
      continue;
    }

    switch (current_char) {
      case '(':
        operators.push(current_char);
        break;

      case ')':
        while (operators.top() != '(') {
          postfix += operators.top();
          operators.pop();
        }

        operators.pop();  // Pop (
        break;

      case '&':
      case '|':
      case '!':
        string precedence = "|&!";
        while (!operators.empty() && operators.top() != '(' &&
               precedence.find(current_char) <=
                   precedence.find(operators.top())) {
          postfix += operators.top();
          operators.pop();
        }

        operators.push(current_char);
        break;
    }
  }

  while (!operators.empty()) {
    postfix += operators.top();
    operators.pop();
  }

  return postfix;
}

int evaluate(string infix, const bool values[], string &postfix, bool &result) {
  if (!validate(infix)) return 1;

  postfix = generatePostfix(infix);

  stack<bool> operands;
  for (int i = 0; i < postfix.size(); i++) {
    char current_char = postfix.at(i);

    if (is_number_char(current_char)) {
      int current_char_to_num = current_char - '0';
      operands.push(values[current_char_to_num]);

      continue;
    } else if (current_char == '!') {
      bool flipped_operand = !operands.top();
      operands.pop();
      operands.push(flipped_operand);

      continue;
    }

    bool operand_2 = operands.top();
    operands.pop();
    if (operands.empty()) return 1;
    bool operand_1 = operands.top();
    operands.pop();

    if (current_char == '&') {
      operands.push(operand_1 && operand_2);
    } else if (current_char == '|') {
      operands.push(operand_1 || operand_2);
    }
  }

  result = operands.top();

  return 0;
}

int main() {
  assert(validate("2| 3"));
  assert(validate("4  |  !3 & (0&3) "));
  assert(validate("2"));
  assert(validate("(3)"));
  assert(validate("2&(3)"));
  assert(validate("0 & !9"));
  assert(validate("!(7|8)"));
  assert(validate("!7|8"));
  assert(validate("6|4&5"));
  assert(validate("1&!(9|1&1|9) | !!!(9&1&9)"));
  assert(!validate("8|"));
  assert(!validate("4 5"));
  assert(!validate("01"));
  assert(!validate("()"));
  assert(!validate("((((((()))))))"));
  assert(!validate(")("));
  assert(!validate(")))))))((((((("));
  assert(!validate("2(9|8)"));
  assert(!validate("2(&8)"));
  assert(!validate("(6&(7|7)"));
  assert(!validate(""));
  assert(!validate("1&!(9^|1&1|9) | !!!(9&1&9)"));

  bool ba[10] = {
      //  0      1      2      3      4      5      6      7      8      9
      true, true, true, false, false, false, true, false, true, false};
  string pf;
  bool answer;
  assert(evaluate("2| 3", ba, pf, answer) == 0 && pf == "23|" && answer);
  assert(evaluate("8|", ba, pf, answer) == 1);
  assert(evaluate("4 5", ba, pf, answer) == 1);
  assert(evaluate("01", ba, pf, answer) == 1);
  assert(evaluate("()", ba, pf, answer) == 1);
  assert(evaluate("2(9|8)", ba, pf, answer) == 1);
  assert(evaluate("2(&8)", ba, pf, answer) == 1);
  assert(evaluate("(6&(7|7)", ba, pf, answer) == 1);
  assert(evaluate("", ba, pf, answer) == 1);
  assert(evaluate("4  |  !3 & (0&3) ", ba, pf, answer) == 0 &&
         pf == "43!03&&|" && !answer);
  assert(evaluate(" 9  ", ba, pf, answer) == 0 && pf == "9" && !answer);
  ba[2] = false;
  ba[9] = true;
  assert(evaluate("((9))", ba, pf, answer) == 0 && pf == "9" && answer);
  assert(evaluate("2| 3", ba, pf, answer) == 0 && pf == "23|" && !answer);
  cout << "Passed all tests" << endl;
}
