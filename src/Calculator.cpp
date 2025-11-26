#include <cctype>
#include <cmath> // Für std::pow oder ähnliches, falls benötigt
#include <iostream>
#include <stack>
#include <string>
#include <vector>

class Calculator {
private:
  struct token {
    char typ; // 's' := Strichoperator (+-); 'p' := Punktoperator (*/); 'o' := Operand; '(' oder ')'
    union {
      char operator_val;
      double operand_val;
    } op;
  };

  // für Operatoren Prioritäten
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


  // Shunting-Yard Algorithmus
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
    // Alle verbleibenden Operatoren auf den Stack verschieben
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

  // Hilfsfunktion zum Ausführen der Berechnung
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
      return 0; // Sollte nicht passieren
    }
  }

  // Auswertung des Postfix (RPN) Ausdrucks
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
  // Tokenisierung der Eingabezeichenkette
  std::vector<token> tokenize(const std::string &expression) {
    std::vector<token> tokens;
    bool expect_operand = true; // Flag zur Erkennung von unärem Minus/Plus

    for (size_t i = 0; i < expression.length(); ++i) {
      char current_char = expression[i];

      if (std::isspace(current_char))
        continue;

      // Operatoren
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
              expect_operand = false; // Nach dem Operanden wird ein Operator erwartet
            } catch (const std::exception &e) {
              std::cerr << "Fehler beim Konvertieren der Zahl: " << num_str
                        << std::endl;
            }
       } else { // Binärer Operator
          token t;
          t.typ = 's';
          t.op.operator_val = current_char;
          tokens.push_back(t);
          expect_operand = true; // Nach dem Operator wird ein Operand erwartet
        }
      } else if (current_char == '*' || current_char == '/') {
        token t;
        t.typ = 'p';
        t.op.operator_val = current_char;
        tokens.push_back(t);
        expect_operand = true; // Nach dem Operator wird ein Operand erwartet
      } else if (current_char == '(') {
        token t;
        t.typ = current_char;
        tokens.push_back(t);
        expect_operand = true; // Nach '(' wird ein Operand erwartet (z.B. in
                               // (5*2) oder (-5))
      } else if (current_char == ')') {
        token t;
        t.typ = current_char;
        tokens.push_back(t);
        expect_operand = false; // Nach ')' wird ein Operator erwartet
      } else if (std::isdigit(static_cast<unsigned char>(current_char)) ||
                 current_char == '.') {
        // Zahl parsen
        std::string num_str;
        // Zahl-Parsing (aus Ihrem Originalcode, mit Bounds Check)
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



	double calculate(const std::string &expression) {
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




// mögliche klausurfrage: Worauf ein Zeiger zeigen kann
