#include <cmath>
#include <iostream>
#include <stack>
#include <vector>

class Calculator {
private:
  /**
   * Tokens are used to differentiate between an addtion/substraction operator,
   * a multiplication/division operator, or an operand.
   */
  struct token {
    char typ; // 's' := Strichoperator (+-); 'p' := Punktoperator (*/); 'o' :=
              // Operand; '(' oder ')'
    union {
      char operator_val;
      double operand_val;
    } op;
  };

  /**
   * Priority of the operators.
   */
  int getPrio(const token &t) const {
    switch (t.typ) {
    case 'p':
      return 2; // * /
    case 's':
      return 1; // + -
    default:
      return -1; // z.B. für '('
    }
  }

  /**
   * Tokenization of an expression
   */
  std::vector<token> tokenize(const std::string &expression) {
    std::vector<token> tokens;
    bool expect_operand = true;

    for (size_t i = 0; i < expression.length(); ++i) {
      char current_char = expression[i];

      if (std::isspace(static_cast<unsigned char>(current_char)))
        continue;

      if (current_char == '+' || current_char == '-') {
        if (expect_operand) {
          if (current_char == '-') {
            std::string num_str;
            num_str += current_char;
            i++;

            while (i < expression.length() &&
                   (std::isdigit(static_cast<unsigned char>(expression[i])) ||
                    expression[i] == '.')) {
              num_str += expression[i];
              i++;
            }
            i--; // index zurücksetzen

            try {
              double value = std::stod(num_str);
              token t;
              t.typ = 'o';
              t.op.operand_val = value;
              tokens.push_back(t);
              expect_operand = false;
            } catch (const std::exception &e) {
              std::cerr << "Fehler beim Konvertieren der Zahl: " << num_str
                        << std::endl;
            }
          } else if (current_char == '+') {
            continue;
          }
          if (tokens.back().op.operator_val == '+') {
            if (current_char == '-') {
              tokens.pop_back();
              token t;
              t.typ = 's';
              t.op.operator_val = '-';
              tokens.push_back(t);
            }
          } else if (tokens.back().op.operator_val == '-') {
            if (current_char == '-') {
              tokens.pop_back();
              token t;
              t.typ = 's';
              t.op.operator_val = '+';
              tokens.push_back(t);
            }
          }

        } else {
          token t;
          t.typ = 's';
          t.op.operator_val = current_char;
          tokens.push_back(t);
          expect_operand = true;
        }
      } else if (current_char == '*' || current_char == '/') {
        token t;
        t.typ = 'p';
        t.op.operator_val = current_char;
        tokens.push_back(t);
        expect_operand = true;
      } else if (current_char == '(') {
        token t;
        t.typ = current_char;
        tokens.push_back(t);
        expect_operand = true;
      } else if (current_char == ')') {
        token t;
        t.typ = current_char;
        tokens.push_back(t);
        expect_operand = false;
      } else if (std::isdigit(static_cast<unsigned char>(current_char)) ||
                 current_char == '.') {
        std::string num_str;
        while (i < expression.length() &&
               (std::isdigit(static_cast<unsigned char>(expression[i])) ||
                expression[i] == '.')) {
          num_str += expression[i];
          i++;
        }
        i--; // index zurücksetzen

        try {
          double value = std::stod(num_str);
          token t;
          t.typ = 'o';
          t.op.operand_val = value;
          tokens.push_back(t);
          expect_operand =
              false; // Nach dem Operanden wird ein Operator erwartet
        } catch (const std::exception &e) {
          std::cerr << "Fehler beim Konvertieren der Zahl: " << num_str
                    << std::endl;
        }
      } else {
        std::cerr << "Unbekanntes Zeichen übersprungen: " << current_char
                  << std::endl;
      }
    }
    return tokens;
  }

  /**
   * Reorder the tokens using the Shunting-Yard algorithm (Infix to Postfix).
   */
  std::vector<token> shuntingYard(const std::vector<token> &input) const {
    std::vector<token> output;
    std::stack<token> ops;

    for (const token &tok : input) {
      if (tok.typ == 'o') {
        output.push_back(tok);

      } else if (tok.typ == 's' || tok.typ == 'p') {
        while (!ops.empty() && ops.top().typ != '(' &&
               getPrio(ops.top()) >= getPrio(tok)) {
          output.push_back(ops.top());
          ops.pop();
        }

        ops.push(tok);

      } else if (tok.typ == '(') {
        ops.push(tok);

      } else if (tok.typ == ')') {
        while (!ops.empty() && ops.top().typ != '(') {
          output.push_back(ops.top());
          ops.pop();
        }

        if (!ops.empty() && ops.top().typ == '(')
          ops.pop();
        else
          std::cerr << "Fehlende öffnende Klammer\n";
      }
    }
    while (!ops.empty()) {
      if (ops.top().typ == '(' || ops.top().typ == ')') {
        std::cerr << "Fehlende schließende Klammer\n";
        ops.pop();
      } else {
        output.push_back(ops.top());
        ops.pop();
      }
    }
    return output;
  }

  /**
   * Helper fuction for the calculation.
   */
  double applyOp(char op, double b, double a) const {
    switch (op) {
    case '+':
      return a + b;
    case '-':
      return a - b;
    case '*':
      return a * b;
    case '/':
      if (b == 0)
        throw std::runtime_error("Division durch Null!");
      return a / b;
    default:
      return 0.0;
    }
  }

  /**
   * Evaluation of the postfix expression.
   */
  double evaluatePostfix(const std::vector<token> &postfix) const {
    std::stack<double> operands;
    for (const token &tok : postfix) {
      if (tok.typ == 'o') {
        operands.push(tok.op.operand_val);
      } else if (tok.typ == 's' || tok.typ == 'p') {
        if (operands.size() < 2)
          throw std::runtime_error(
              "Ungültiger Ausdruck: Zu wenige Operanden für Operator.");
        double b = operands.top();
        operands.pop();
        double a = operands.top();
        operands.pop();
        operands.push(applyOp(tok.op.operator_val, b, a));
      }
    }
    if (operands.size() != 1)
      throw std::runtime_error("Ungültiger Ausdruck: Restliche Operanden.");
    return operands.top();
  }

public:
  /**
   * Public fuction to provide an expression and get a result.
   */
  double calculate(
      const std::string
          &expression) { // what is the differenz between std::string and string
    try {
      std::vector<token> infix_tokens = tokenize(expression);
      std::vector<token> postfix_tokens = shuntingYard(infix_tokens);
      return evaluatePostfix(postfix_tokens);
    } catch (const std::exception &e) {
      std::cerr << "Fehler bei der Berechnung: " << e.what() << std::endl;
      return NAN; // Not a Number
    }
  }
};
