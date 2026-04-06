#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_set>
#include <stack>
#include <cctype>
#include "parser.h"

using namespace std;

static const char LABEL = '_';

//Construction for the NFA representation with no outgoing edges
static REG_node* new_node() {
    REG_node* node = new REG_node();
    node->first_neighbour = nullptr;
    node->second_neighbour = nullptr;
    node->first_label = 0;
    node->second_label = 0;
    return node;
}

// this syntax error function needs to be 
// modified to produce the appropriate message
void Parser::syntax_error()
{
    cout << "SNYTAX ERROR";
    exit(1);
}

void Parser::syntax_error_in_expr() {
    //Syntax error which happens while the parsing an expression of a token
    cout << expr_owner_name << " HAS A SYNTAX ERROR IN ITS EXPRESSION  NOOOOOOOOOOOOOOOO!" << endl;
    exit(1);
}

//Consume token from the lexer and make sure it matches the expected type (if not, show appropriate error message)
Token Parser::expect(TokenType expected_type) {
    Token token = lexer.GetToken();
    if (token.token_type != expected_type) {
        if (in_expr) {
            syntax_error_in_expr();
        }
        syntax_error();
    }
    return token;
}

//Adding a labelled edge from one node REG node to another
static void add_edge(REG_node* from, REG_node* to, char label) {
    if (from->first_neighbour == nullptr) {
        from->first_neighbour = to;
        from->first_label = label;
        return;
    }

    if (from->second_neighbour == nullptr) {
        from->second_neighbour = to;
        from->second_label = label;
        return;
    }
}

//Build for a single character token, which is the base case for building the NFA for a token expression
static REG* make_char_reg(char character) {
    REG* reg = new REG();
    reg->start = new_node();
    reg->accept = new_node();
    add_edge(reg->start, reg->accept, character);
    return reg;
}

//Build for epsilon token, which is the base case for building the NFA for a token expression
static REG* make_epsilon_reg() {
    REG* reg = new REG();
    reg->start = new_node();
    reg->accept = new_node();
    add_edge(reg->start, reg->accept, LABEL);
    return reg;
}

//Build for concatenation of reg1 and reg2 (accept using epsilon edge)
static REG* make_concat_reg(REG* reg1, REG* reg2) {
    //connecting reg1 accept to the reg2
    add_edge(reg1->accept, reg2->start, LABEL);

    REG* reg = new REG();
    reg->start = reg1->start;
    reg->accept = reg2->accept;
    return reg;
}

//Build for reg1 OR reg2
static REG* make_or_reg (REG* reg1, REG* reg2) {
    REG* reg = new REG();
    reg->start = new_node();
    reg->accept = new_node();

    add_edge(reg->start, reg1->start, LABEL);
    add_edge(reg->start, reg2->start, LABEL);

    add_edge(reg1->accept, reg->accept, LABEL);
    add_edge(reg2->accept, reg->accept, LABEL);

    return reg;
}

//Build for Kleene star of reg1
static REG* make_star_reg(REG* reg1) {
    REG* reg = new REG();
    reg->start = new_node();
    reg->accept = new_node();

    add_edge(reg->start, reg1->start, LABEL);
    add_edge(reg->start, reg->accept, LABEL);

    add_edge(reg1->accept, reg1->start, LABEL);
    add_edge(reg1->accept, reg->accept, LABEL);

    return reg;

}
// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.

REG* Parser::parse_expr(bool &nullable) {
    Token token = lexer.peek(1);
    //Regarding CHAR
    if (token.token_type == CHAR) { 
        Token charToken = expect(CHAR);
        nullable = false;
        return make_char_reg(charToken.lexeme[0]);
    }

    //Regarding UNDERSCORE
    if (token.token_type == UNDERSCORE) {
        expect(UNDERSCORE);
        nullable = true;
        return make_epsilon_reg();
    }

    //Regarding LPAREN
    expect(LPAREN);
    bool null1 = false;
    REG* reg1 = parse_expr(null1);
    expect(RPAREN);

    Token op = lexer.peek(1);

    if (op.token_type == DOT) {
        expect(DOT);
        expect(LPAREN);
        bool null2 = false;
        REG* reg2 = parse_expr(null2);
        expect(RPAREN);

        nullable = (null1 && null2);
        return make_concat_reg(reg1, reg2);
    }

    if (op.token_type == OR) {
        expect(OR);
        expect(LPAREN);
        bool null2 = false;
        REG* reg2 = parse_expr(null2);
        expect(RPAREN);

        nullable = (null1 || null2);
        return make_or_reg(reg1, reg2);
    }

    if (op.token_type == STAR) {
        expect(STAR);
        nullable = true;
        return make_star_reg(reg1);
    }

    //If no matches, then there's a syntax error in the expresssion
    syntax_error_in_expr();
    return nullptr;
}

//parse single token and track any duplicate declarations
void Parser::parse_token() {
    Token nameToken = expect(ID);
    expr_owner_name = nameToken.lexeme;

    auto iterator = first_decl_line.find(expr_owner_name);
    if (iterator == first_decl_line.end()) {
        first_decl_line[expr_owner_name] = nameToken.line_no;
    }
    else {
        DuplicateError error;
        error.name = expr_owner_name;
        error.dup_line = nameToken.line_no;
        error.first_line = iterator->second;
        dup_errors.push_back(error);
    }

    bool nullable = false;

    in_expr = true;
    REG* reg = parse_expr(nullable);
    in_expr = false;

    TokenSpec spec;
    spec.name = expr_owner_name;
    spec.reg = reg;
    spec.nullable = nullable;
    token_specs.push_back(spec);
}

//Parse list of tokens separated by semicolons
void Parser::parse_token_list() {
    parse_token();
    Token token = lexer.peek(1);
    if (token.token_type == SEMICOLON) {
        expect(SEMICOLON);
        parse_token_list();
    }
}

//Parse token section, specifically ending with HASH token
void Parser::parse_tokens_section() {
    parse_token_list();
    expect(HASH);
}

//Removes quotes from the input texts
string Parser::strip_quotes(const std::string &s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

//Compute epsilon closure of a set of REG nodes
static vector<REG_node*> epsilon_closure(const vector<REG_node*> &start) {
    vector<REG_node*> result;
    unordered_set<REG_node*> seen;
    stack<REG_node*> stack;

    for (REG_node* node : start) {
        if (!node) {
            continue;
        }

        if (seen.insert(node).second) {
            result.push_back(node);
            stack.push(node);
        }
    }

    while (!stack.empty()) {
        REG_node* current = stack.top();
        stack.pop();

        if (current->first_neighbour && current->first_label == LABEL) {
            if (seen.insert(current->first_neighbour).second) {
                result.push_back(current->first_neighbour);
                stack.push(current->first_neighbour);
            }
        }

        if (current->second_neighbour && current->second_label == LABEL) {
            if (seen.insert(current->second_neighbour).second) {
                result.push_back(current->second_neighbour);
                stack.push(current->second_neighbour);
            }
        }
    }

    return result;
}

//Move on from set of REG nodes utilising a single character transition
static vector<REG_node*> move_on_char(const vector<REG_node*> &S, char character) {
    vector<REG_node*> direct;
    unordered_set<REG_node*> seenNode;

    for (REG_node* node : S) {
        if (!node) {
            continue;
        }

        if (node->first_neighbour && node->first_label == character) {
            if (seenNode.insert(node->first_neighbour).second) {
                direct.push_back(node->first_neighbour);
            }
        }

        if (node->second_neighbour && node->second_label == character) {
            if (seenNode.insert(node->second_neighbour).second) {
                direct.push_back(node->second_neighbour);
            }
        }
    }

    if (direct.empty()) {
        return direct;
    }

    return epsilon_closure(direct);
}

//Attempt to match NFA reg against the input string at the startPosition
//This returns teh last index part of the longest match/startPosition - 1 if no match
int Parser::match(REG *r, const string &s, int startPosition) {
    if (!r || !r->start || !r->accept) {
        return startPosition - 1;
    }

    if (startPosition < 0 || startPosition >= (int)s.size()) {
        return startPosition - 1;
    }

    vector<REG_node*> init;
    init.push_back(r->start);
    vector<REG_node*> current = epsilon_closure(init);

    int bestEndPosition = startPosition - 1;

    for (int i = startPosition; i < (int)s.size(); i++) {
        char character = s[i];
        if (isspace((unsigned char)character)) {
            break;
        }

        vector<REG_node*> next = move_on_char(current, character);
        if (next.empty()) {
            break;
        }

        current.swap(next);

        for (REG_node* node : current) {
            if (node == r->accept) {
                bestEndPosition = i;
                break;
            }
        }
    }

    return bestEndPosition;
}

//Run lexical anaylsis on raw input text using the token specifications parsed from the token section
void Parser::run_lexical_analysis(const string &input_text) {
    int i = 0;
    int n = (int)input_text.size();

    while (i < n) {
        while (i < n && isspace((unsigned char)input_text[i])) {
            i++;
        }

        if (i >= n) {
            break;
        }

        int bestLen = 0;
        int bestEnd = i - 1;
        int bestIndex = -1;

        for (int t = 0; t < (int)token_specs.size(); t++) {
            int endPosition = match(token_specs[t].reg, input_text, i);
            if (endPosition >= i) {
                int len = endPosition - i + 1;
                if (len > bestLen) {
                    bestLen = len;
                    bestEnd = endPosition;
                    bestIndex =  t;
                }
            }
        }

        if (bestIndex == -1) {
            cout << "ERROR" << endl;
            return;
        }

        string lexeme = input_text.substr(i, bestLen);
        cout << token_specs[bestIndex].name << ", \"" << lexeme << "\"" << endl;

        i = bestEnd + 1;
    }
}

//top level parse routine, parse, read input text, cehck for duplicates and epsilon tokens, lex the input
void Parser::parse_input() {
    in_expr = false;
    expr_owner_name = "";

    parse_tokens_section();

    Token inputToken = expect(INPUT_TEXT);
    Token token = lexer.GetToken();
    if (token.token_type != END_OF_FILE) {
        syntax_error();
    }

    if (!dup_errors.empty()) {
        for (int i = 0; i < (int)dup_errors.size(); i++) {
            cout << "Line " << dup_errors[i].dup_line << ": " << dup_errors[i].name << " already declared on line " << dup_errors[i].first_line << endl;
        }
        return;
    }

    vector<string> eps;
    for (int i = 0; i < (int)token_specs.size(); i++) {
        if (token_specs[i].nullable) {
            eps.push_back(token_specs[i].name);
        }
    }

    if (!eps.empty()) {
        cout << "EPSILON IS NOOOOOOOT A TOKEN !!!!";
        for (int i = 0; i < (int)eps.size(); i++) {
            cout << " " << eps[i];
        }
        cout << endl;
        return;
    }

    string rawText = strip_quotes(inputToken.lexeme);
    run_lexical_analysis(rawText);
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
// This function is not needed for the solution and it is only provided to
// illustrate the functionality of getToken() and the Token type.

void Parser::readAndPrintAllInput()
{
    Token token;

    // get a token
    token = lexer.GetToken();

    // while end of input is not reached
    while (token.token_type != END_OF_FILE) 
    {
        token.Print();         	// pringt token
        token = lexer.GetToken();	// and get another one
    }
        }

int main()
{
    Parser parser;
    parser.parse_input();
    return 0;	
}
