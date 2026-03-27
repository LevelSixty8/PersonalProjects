#ifndef __PARSER_H__
#define __PARSER_H__

#include <unordered_map>
#include <vector>
#include <string>
#include "lexer.h"

struct REG_node {
  REG_node *first_neighbour;
  char first_label;
  REG_node *second_neighbour;
  char second_label;
  
  REG_node() : first_neighbour(nullptr), first_label(0), second_neighbour(nullptr), second_label(0) {}
  };

struct REG {
  REG_node *start;
  REG_node *accept;
  REG() : start(nullptr), accept(nullptr) {}
};

struct DuplicateError {
  std::string name;
  int dup_line;
  int first_line;
};

struct TokenSpec { 
  std::string name;
  REG *reg;
  bool nullable;
  TokenSpec() : name(""), reg(nullptr), nullable(false) {}
};

class Parser {
  public:
    void parse_input();
    void readAndPrintAllInput();

  private:
    LexicalAnalyzer lexer;

    std::string expr_owner_name;
    bool in_expr;

    void syntax_error();
    void syntax_error_in_expr();
    Token expect(TokenType expected_type);

    void parse_tokens_section();
    void parse_token_list();
    void parse_token();


    REG* parse_expr(bool &nullable);

    std::unordered_map<std::string, int> first_decl_line;
    std::vector<DuplicateError> dup_errors;
    std::vector<TokenSpec> token_specs;

    std::string strip_quotes(const std::string &s);
    int match(REG *r, const std::string &s, int start_pos);
    void run_lexical_analysis(const std::string &input_text);
};

#endif

