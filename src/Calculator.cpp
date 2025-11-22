#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <cctype>

class Tokenizer{
private:
  struct token {
    char typ; // 's' := Strichoperator; 'p' := Punktoperator; 'o' := Operand; '(' or ')'
    union {
        char operator_val;
        double operand_val;
        }op;
    };

  std::vector<token> tokens;

  // minimal change: take by const ref and set correct precedence (p = */ higher than s = +-)
  int getPrio(const token& t){
    switch (t.typ) {
      case 'p': return 2; // * /
      case 's': return 1; // + -
      case 'o': return 3; // operands (not used for ops comparison)
      default:  return -1;
    }
  }

public:
    std::vector<token> tokenize(const std::string& expression) {
        std::vector<token> tokens;
        for (size_t i = 0; i < expression.length(); ++i) {
            char current_char = expression[i];

            // whitespace (use unsigned char to avoid UB)
            if (std::isspace(static_cast<unsigned char>(current_char))) {
                continue;
            }

            // operators
            if (current_char == '+' || current_char == '-') {
                token t;
                t.typ = 's';
                t.op.operator_val = current_char;
                tokens.push_back(t);
            } else if (current_char == '*' || current_char == '/'){
                token t; t.typ = 'p';
                t.op.operator_val = current_char;
                tokens.push_back(t);
            } else if (current_char == '(' || current_char == ')') {
                token t; t.typ = current_char; // use '(' or ')' as typ
                tokens.push_back(t);
            } else if (std::isdigit(static_cast<unsigned char>(current_char)) || current_char == '.') { // number
                std::string num_str;

                // ensure bounds check includes both parts
                while (i < expression.length() && (std::isdigit(static_cast<unsigned char>(expression[i])) || expression[i] == '.')) {
                    num_str += expression[i];
                    i++;
                }
                i--; // adjust index after overshoot

                try {
                    double value = std::stod(num_str);
                    token t;
                    t.typ = 'o';
                    t.op.operand_val = value;
                    tokens.push_back(t);
                } catch (const std::exception& e) {
                    std::cerr << "Fehler beim Konvertieren der Zahl: " << num_str << std::endl;
                }
            }
            else {
                std::cerr << "Unbekanntes Zeichen übersprungen: " << current_char << std::endl;
            }
        }
      return tokens;
    }


  std::vector<token> shuntingYard(const std::vector<token>& input) {
        std::vector<token> output;
        std::stack<token> ops;
        for (const token& tok : input) {
            if (tok.typ == 'o') {
                output.push_back(tok);
            } else if (tok.typ == 's' || tok.typ == 'p') {
                while (!ops.empty() && ops.top().typ != '(' && getPrio(ops.top()) >= getPrio(tok)) {
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
                if (!ops.empty() && ops.top().typ == '(') ops.pop();
                else std::cerr << "Unmatched parenthesis\n";
            }
        }
        while (!ops.empty()) {
            if (ops.top().typ == '(' || ops.top().typ == ')') {
                std::cerr << "Unmatched parenthesis\n";
                ops.pop();
            } else {
                output.push_back(ops.top());
                ops.pop();
            }
        }
        return output;
    }
};