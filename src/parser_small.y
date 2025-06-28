// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

// TODO: you can either continue adding to this grammar file or
// rename parser_full.y to parser.y once you're happy with
// how this example works.

%code requires{
    #include "ast.hpp"

	using namespace ast;

    extern Node* g_root;
    extern FILE* yyin;
    int yylex(void);
    void yyerror(const char*);
	int yylex_destroy(void);
}

%union{
  Node*				node;
  NodeList*			node_list;
  int          		number_int;
  double       		number_float;
  std::string*		string;
  TypeSpecifier 	type_specifier;
  yytokentype  		token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <node> translation_unit external_declaration function_definition primary_expression postfix_expression argument_expression_list
%type <node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <node> conditional_expression assignment_expression expression constant_expression declaration init_declarator_list
%type <node> init_declarator struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list
%type <node> struct_declarator enum_specifier enumerator_list enumerator declarator direct_declarator pointer parameter_list parameter_declaration
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator initializer initializer_list statement labeled_statement
%type <node> compound_statement expression_statement selection_statement iteration_statement jump_statement

%type <node_list> statement_list declaration_list

%type <number_int> INT_CONSTANT STRING_LITERAL
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER
%type <type_specifier> type_specifier declaration_specifiers


%start ROOT
%%

ROOT
    : translation_unit { g_root = $1; }

translation_unit
	: external_declaration { $$ = $1; }
	;

external_declaration
	: function_definition { $$ = $1; }
	;

function_definition
	: declaration_specifiers declarator compound_statement {
		$$ = new FunctionDefinition($1, NodePtr($2), NodePtr($3));
	}
	;

declaration_specifiers
	: type_specifier { $$ = $1; }
	;

type_specifier
	: INT {
		$$ = TypeSpecifier::INT;
	}
	;

declarator
	: direct_declarator { $$ = $1; }
	;

direct_declarator
	: IDENTIFIER {
		$$ = new Identifier(std::move(*$1));
		delete $1;
	}
	| direct_declarator '(' ')' {
		$$ = new DirectDeclarator(NodePtr($1));
	}
	;

statement
	: jump_statement { $$ = $1; }
	;

compound_statement
	: '{' '}' {
		// TODO: correct this
		$$ = new Scope(NodePtr(nullptr), NodePtr(nullptr));
	}
	| '{' statement_list '}' {
		$$ = new Scope(NodePtr(nullptr), NodePtr($2));
	}
	| '{' declaration_list '}' {
		// TODO: correct this
		$$ = new Scope(NodePtr($2), NodePtr(nullptr));
	}
	| '{' declaration_list statement_list '}'  {
		// TODO: correct this
		$$ = new Scope(NodePtr($2), NodePtr($3));
	}
	;

declaration_list
	: declaration { $$ = new NodeList(NodePtr($1)); }
	| declaration_list declaration { $1->PushBack(NodePtr($2)); $$=$1; }
	;

statement_list
	: statement { $$ = new NodeList(NodePtr($1)); }
	| statement_list statement { $1->PushBack(NodePtr($2)); $$=$1; }
	;

jump_statement
	: RETURN ';' {
		$$ = new ReturnStatement(nullptr);
	}
	| RETURN expression ';' {
		$$ = new ReturnStatement(NodePtr($2));
	}
	;

primary_expression
	: IDENTIFIER { $$ = new Identifier(std::move(*$1)); }
	| INT_CONSTANT {
		$$ = new IntConstant($1);
	}
	;

postfix_expression
	: primary_expression { $$ = $1; }
	;

unary_expression
	: postfix_expression { $$ = $1; }
	;

cast_expression
	: unary_expression { $$ = $1; }
	;

multiplicative_expression
	: cast_expression { $$ = $1; }
	;

additive_expression
	: multiplicative_expression { $$ = $1; }
	;

shift_expression
	: additive_expression { $$ = $1; }
	;

relational_expression
	: shift_expression { $$ = $1; }
	;

equality_expression
	: relational_expression { $$ = $1; }
	;

and_expression
	: equality_expression { $$ = $1; }
	;

exclusive_or_expression
	: and_expression { $$ = $1; }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = $1; }
	;

logical_and_expression
	: inclusive_or_expression { $$ = $1; }
	;

logical_or_expression
	: logical_and_expression { $$ = $1; }
	;

conditional_expression
	: logical_or_expression { $$ = $1; }
	;

assignment_expression
	: conditional_expression { $$ = $1; }
	;

expression
	: assignment_expression { $$ = $1; }
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';' { $$ = new Declaration($1, NodePtr($2)); }

init_declarator_list
	: init_declarator { $$ = $1; }
	/* assuming only single declarator (for the moment) */
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator { $$ = $1; }
	| declarator '=' initializer { $$ = new InitDeclarator(NodePtr($1), NodePtr($3)); }
	;

initializer
	: assignment_expression { $$ = new NodeList(NodePtr($1)); }
	;

%%

Node* g_root;

NodePtr ParseAST(std::string file_name)
{
  yyin = fopen(file_name.c_str(), "r");
  if(yyin == NULL){
    std::cerr << "Couldn't open input file: " << file_name << std::endl;
    exit(1);
  }
  g_root = nullptr;
  yyparse();
  fclose(yyin);
  yylex_destroy();
  return NodePtr(g_root);
}
