class Tokenizer{
private:

  std::vector<token> tokens;

  struct token {
    char typ; // 's' := Strichoperator; 'p' := Punktoperator; 'o' := Operand
    union {
        char operator;
        double Operand;
        }op;
    };

  /**
   * retruns a priority of a giving token
   *
   * returs a 0 for 'p'
   *          1 for 's'
   *          2 for 'o'
   */

  int getPrio(token){
    switch (token.typ) {
      
      case 'p':
          return 0;

      case 's':
          return 1;

      case 'o':
          return 2;

      default:
         
          return -1;
    }    
  }

public:

    std::vector<token> tokenize(const std::string& expression) {
        std::vector<token> tokens;

        for (size_t i = 0; i < expression.length(); ++i) {
            char current_char = expression[i];

            // fall Leerzeichen
            if (std::isspace(current_char)) {
                continue;
            }

            // welcher Operator ist es
            if (current_char == '+' || current_char == '-') {
                token t;
                t.typ = 's'; t.op.operator_val = current_char; tokens.push_back(t);
            } else if (current_char == '*' || current_char == '/'){
              token t; t.typ = 'p'; 
              t.op.operator_val = current_char; 
              tokens.push_back(t); 
            } else if (current_char.isdigit() || current_char == '.') { // wenn es eine zahl ist
                std::string num_str;

                while (i < expression.length() && (expression[i].isdigit() || expression[i] == '.')) {
                    num_str += expression[i];
                    i++;
                }
                i--; // Zahl abgeatbeitet, der index ist jetzt im nächsten char (korrektur)

                try {
                    // String zu double (std::stod(...))
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


  void shunting-yurd-algorithmus(){

    while (!tokens.isEmpty()) {

      //nächsten Token lesen und aus Eingabe entfernen;
      token current = tokens.begin();
      tokens.erase(tokens.begin());
      Stack operator;


      if (current.typ == 'o') {
        Token zur Ausgabe;
      } else {
      if (current.typ == 'p' || current.typ == 's' && !(current.op.operand_val == '(' || current.op.operand_val == ')') {

        while(!operator.empty() && getPrio(operator.top() >= getPrio(current))) {
          operator.top() //zur Ausgabe;
          operator.pop();
        }
        operator.push(current);
      } else {
        if (Token ist öffnende Klammer) {
          operator.push(Token);
        } else {
          if (Token ist schließende Klammer) {
            while (operator.top() != öffnende Klammer) {
              operator.top()// zur Ausgabe;
              operator.pop();
            }
            operator.pop(); //öffnende Klammer
          }
        }
      }:
     }
    } 

    while (!Stack.isEmpty()) {
      operator.top()// zur Ausgabe;
      operator.pop();
    }
  }
}
