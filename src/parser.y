// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
    #include "ast.hpp"

	using namespace ast;

    extern Node* g_root;
    extern FILE* yyin;
    int yylex(void);
    void yyerror(const char*);
	int yylex_destroy(void);

	void updateTypeDefs(std::string type);
}

// Represents the value associated with any kind of AST node.
%union{
  Node*        	node;
  NodeList*    	node_list;
  int          	number_int;
  double       	number_float;
  std::string* 	string;
  TypeSpecifier	type_specifier;
  AssignmentOp assignment_operator;
  UnaryOp unary_op;
  yytokentype  	token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL CHAR_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
%token <string> TYPE_NAME

%type <node> external_declaration function_definition primary_expression postfix_expression
%type <node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <node> conditional_expression assignment_expression expression constant_expression declaration init_declarator_list
%type <node> init_declarator struct_specifier struct_declaration specifier_qualifier_list struct_declarator_list
%type <node> struct_declarator enum_specifier enumerator declarator direct_declarator pointer parameter_declaration
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator initializer statement labeled_statement
%type <node> compound_statement expression_statement selection_statement iteration_statement jump_statement

%type <node_list> statement_list declaration_list parameter_list translation_unit argument_expression_list initializer_list enumerator_list struct_declaration_list

%type <string> storage_class_specifier

%type <number_int> INT_CONSTANT CHAR_LITERAL
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER STRING_LITERAL
%type <assignment_operator> assignment_operator
%type <unary_op> unary_operator
// TODO: Make a better type for this (only needed for advanced features)
%type <node> declaration_specifiers type_specifier



%start ROOT
%%

ROOT
  : translation_unit { g_root = $1; }

translation_unit
	: external_declaration { $$ = new NodeList(NodePtr($1)); }
	| translation_unit external_declaration { $1->PushBack(NodePtr($2)); $$ = $1; }
	;

external_declaration
	: function_definition { $$ = $1; }
	| declaration { $$ = $1; }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement {
		$$ = new FunctionDefinition(NodePtr($1), NodePtr($2), NodePtr($3));
	}
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;


primary_expression
	: IDENTIFIER { $$ = new Identifier(std::move(*$1)); }
	| INT_CONSTANT {
		$$ = new IntConstant($1);
	}
	| CHAR_LITERAL { $$ = new CharConstant($1); }
	// is this a double, is this a float? I guess the context will tell us...
    | FLOAT_CONSTANT { $$ = new FloatDoubleConstant($1); }
	| STRING_LITERAL { $$ = new String(std::move(*$1)); delete $1;}
	| '(' expression ')' { $$ = $2; }
	;

postfix_expression
	: primary_expression { $$ = $1; }
	| postfix_expression '[' expression ']' { $$ = new ArrayIndex(NodePtr($1), NodePtr($3)); }
    /* f(), looks similar? its because it's a function call! (I hope lol) */
	| postfix_expression '(' ')' { $$ = new FunctionCall(NodePtr($1), NodePtr(nullptr)); }
	| postfix_expression '(' argument_expression_list ')' { $$ = new FunctionCall(NodePtr($1), NodePtr($3)); }
	| postfix_expression '.' IDENTIFIER { $$ = new StructAccess(NodePtr($1), NodePtr(new Identifier(*$3))); }
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP { $$ = new Increment(NodePtr($1), IncrType::INC_OP); }
	| postfix_expression DEC_OP { $$ = new Increment(NodePtr($1), IncrType::DEC_OP); }
	;

argument_expression_list
	: assignment_expression { $$ = new NodeList(NodePtr( new FunctionArgument(NodePtr($1)))); }
	| argument_expression_list ',' assignment_expression { $1->PushBack( NodePtr(new FunctionArgument(NodePtr($3)))); $$ = $1; }
	;

/* These are all *pre-fix* meaning we alter the value **before it is used** */
unary_expression
	: postfix_expression { $$ = $1; }
	| INC_OP unary_expression { $$ = new PreIncrement(IncrType::INC_OP, NodePtr($2)); }
	| DEC_OP unary_expression { $$ = new PreIncrement(IncrType::DEC_OP, NodePtr($2)); }
	| unary_operator cast_expression { $$ = new UnaryExpression($1, NodePtr($2)); }
	| SIZEOF unary_expression { $$ = new SizeOf(NodePtr($2)); }
	| SIZEOF '(' type_name ')' { $$ = new SizeOf(NodePtr($3)); }
	;

unary_operator
	: '&' { $$ = UnaryOp::ADDRESSOF; }
	| '*' { $$ = UnaryOp::INDIRECTION; } // gets the value from address
	| '+' { $$ = UnaryOp::U_PLUS; }
	| '-' { $$ = UnaryOp::U_MINUS; }
	| '~' { $$ = UnaryOp::BIT_NOT; }
	| '!' { $$ = UnaryOp::LOG_NOT; }
	;

cast_expression
	: unary_expression { $$ = $1; }
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression { $$ = $1; }
	| multiplicative_expression '*' cast_expression { $$ = new Operation(NodePtr($1), OpType::MUL, NodePtr($3)); }
	| multiplicative_expression '/' cast_expression { $$ = new Operation(NodePtr($1), OpType::DIV, NodePtr($3)); }
	| multiplicative_expression '%' cast_expression { $$ = new Operation(NodePtr($1), OpType::REM, NodePtr($3)); }
	;

additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression '+' multiplicative_expression { $$ = new Operation(NodePtr($1), OpType::ADD, NodePtr($3)); }
	| additive_expression '-' multiplicative_expression { $$ = new Operation(NodePtr($1), OpType::SUB, NodePtr($3)); }
	;

shift_expression
	: additive_expression { $$ = $1; }
	| shift_expression LEFT_OP additive_expression { $$ = new Operation(NodePtr($1), OpType::LEFT_SHIFT, NodePtr($3)); }
	| shift_expression RIGHT_OP additive_expression { $$ = new Operation(NodePtr($1), OpType::RIGHT_SHIFT, NodePtr($3)); }
	;

relational_expression
	: shift_expression { $$ = $1; }
	| relational_expression '<' shift_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::L_OP, NodePtr($3)); }
	| relational_expression '>' shift_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::G_OP, NodePtr($3)); }
	| relational_expression LE_OP shift_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::LE_OP, NodePtr($3)); }
	| relational_expression GE_OP shift_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::GE_OP, NodePtr($3)); }
	;

equality_expression
	: relational_expression { $$ = $1; }
	| equality_expression EQ_OP relational_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::EQ_OP, NodePtr($3)); }
	| equality_expression NE_OP relational_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::NE_OP, NodePtr($3)); }
	;

and_expression
	: equality_expression { $$ = $1; }
	| and_expression '&' equality_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::AND_EXP, NodePtr($3)); }
	;

exclusive_or_expression
	: and_expression { $$ = $1; }
	| exclusive_or_expression '^' and_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::XOR_OP, NodePtr($3)); }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::OR_EXP, NodePtr($3)); }
	;

logical_and_expression
	: inclusive_or_expression { $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::AND_OP, NodePtr($3)); }
	;

logical_or_expression
	: logical_and_expression { $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression { $$ = new LogicalOperation(NodePtr($1), LogicalOpType::OR_OP, NodePtr($3)); }
	;

conditional_expression
	: logical_or_expression { $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression { $$ = new Ternary(NodePtr($1), NodePtr($3), NodePtr($5)); }
	;

assignment_expression
	: conditional_expression { $$ = $1; }
	| unary_expression assignment_operator assignment_expression {$$ = new Assignment(NodePtr($1), $2, NodePtr($3)); }
	;

assignment_operator
	: '=' { $$ = AssignmentOp::ASSIGN; }
	| MUL_ASSIGN { $$ = AssignmentOp::MUL_ASSIGN; }
	| DIV_ASSIGN { $$ = AssignmentOp::DIV_ASSIGN; }
	| MOD_ASSIGN { $$ = AssignmentOp::MOD_ASSIGN; }
	| ADD_ASSIGN { $$ = AssignmentOp::ADD_ASSIGN; }
	| SUB_ASSIGN { $$ = AssignmentOp::SUB_ASSIGN; }
	| LEFT_ASSIGN { $$ = AssignmentOp::LEFT_ASSIGN; }
	| RIGHT_ASSIGN { $$ = AssignmentOp::RIGHT_ASSIGN; }
	| AND_ASSIGN { $$ = AssignmentOp::AND_ASSIGN; }
	| XOR_ASSIGN { $$ = AssignmentOp::XOR_ASSIGN; }
	| OR_ASSIGN { $$ = AssignmentOp::OR_ASSIGN; }
	;

expression
	: assignment_expression { $$ = $1; }
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';' { $$ = $1; }
	| declaration_specifiers init_declarator_list ';' { $$ = new Declaration(NodePtr($1), NodePtr($2)); }
	| TYPEDEF type_specifier init_declarator_list ';'
	{
		updateTypeDefs($3->getIdentifier());
		$$ = new TypeDef(NodePtr($2), NodePtr($3));
	}
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier { $$ = $1; }
	| type_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator { $$ = $1; }
	/* assuming only single declarator (for the moment) */
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator { $$ = new InitDeclarator(NodePtr($1), nullptr); }
	| declarator '=' initializer { $$ = new InitDeclarator(NodePtr($1), NodePtr($3)); }
	;

storage_class_specifier
	: EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID { $$ = new TypeSpecifierNode(TypeSpecifier::VOID); }
	| CHAR { $$ = new TypeSpecifierNode(TypeSpecifier::CHAR); }
	| SHORT
	| INT { $$ = new TypeSpecifierNode(TypeSpecifier::INT); }
	| LONG
	| FLOAT { $$ = new TypeSpecifierNode(TypeSpecifier::FLOAT); }
	| DOUBLE { $$ = new TypeSpecifierNode(TypeSpecifier::DOUBLE); }
	| SIGNED
   	| UNSIGNED { $$ = new TypeSpecifierNode(TypeSpecifier::UNSIGNED_INT); }  /* Add this */
  	| struct_specifier { $$ = $1; }
	| enum_specifier { $$ = $1; }
	| TYPE_NAME { $$ = new TypeDefNode(*$1); delete $1; }
	;

struct_specifier
	: STRUCT IDENTIFIER '{' struct_declaration_list '}'  { $$ = new StructDefinition(*$2, NodePtr($4)); }
	| STRUCT '{' struct_declaration_list '}'
	| STRUCT IDENTIFIER  { $$ = new StructInstance(*$2);}
	;

struct_declaration_list
	: struct_declaration { $$ = new NodeList(NodePtr($1)); }
	| struct_declaration_list struct_declaration { $1->PushBack(NodePtr($2)); $$=$1; }
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';' { $$ = new StructDeclaration(NodePtr($1), NodePtr($2)); }
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier { $$ = $1; }
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}' { $$ = new Enum("", NodePtr($3)); }
	| ENUM IDENTIFIER '{' enumerator_list '}' { $$ = new Enum(std::move(*$2), NodePtr($4)); delete $2; }
	| ENUM IDENTIFIER { $$ = new Enum(std::move(*$2), nullptr); delete $2; }
	;

enumerator_list
	: enumerator { $$ = new NodeList(NodePtr($1)); }
	| enumerator_list ',' enumerator { $1->PushBack(NodePtr($3)); $$=$1; }
	;

enumerator
	: IDENTIFIER { $$ = new EnumValue(std::move(*$1), nullptr); delete $1; }
	| IDENTIFIER '=' constant_expression { $$ = new EnumValue(std::move(*$1), NodePtr($3)); delete $1; }
	;

declarator
     : pointer direct_declarator { $$ = new PointerDeclarator(NodePtr($1),NodePtr($2)); }
     | direct_declarator { $$ = $1; }
     ;

direct_declarator
	: IDENTIFIER {
		$$ = new Identifier(std::move(*$1));
		delete $1;
	}
	| '(' declarator ')' { $$ = $2; }
	| direct_declarator '[' constant_expression ']' { $$ = new ArrayDeclarator(NodePtr($1), NodePtr($3)); }
	| direct_declarator '[' ']' { $$ = new ArrayDeclarator(NodePtr($1), nullptr); }
	/* below is the important part, capturing the arguments of a function i.e. "f(int x, int y)" */
	| direct_declarator '(' parameter_list ')' { $$ = new FunctionDeclarator(NodePtr($1), NodePtr($3)); }
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')' {
		$$ = new FunctionDeclarator(NodePtr($1), nullptr);
	}
	;

pointer
    : '*' { $$ = new Pointer(NULL); }
    | '*' specifier_qualifier_list
    | '*' pointer { $$ = new Pointer($2); }
    | '*' specifier_qualifier_list pointer
    ;

parameter_list
	: parameter_declaration { $$ = new NodeList(NodePtr($1)); }
	| parameter_list ',' parameter_declaration { $1->PushBack(NodePtr($3)); $$=$1; }
	;

parameter_declaration
	: declaration_specifiers declarator { $$ = new FunctionParameter(NodePtr($1), NodePtr($2)); }
	| declaration_specifiers abstract_declarator
	| declaration_specifiers { $$ = nullptr; }
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list                              { $$ = $1; }
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_list ')'
	;

initializer
	: assignment_expression { $$ = new NodeList(NodePtr($1)); }
	| '{' initializer_list '}' { $$ = new ArrayInit(NodePtr($2)); }
	| '{' initializer_list ',' '}' { $$ = new ArrayInit(NodePtr($2)); }
	;

initializer_list
	: initializer { $$ = new NodeList(NodePtr($1)); }
	| initializer_list ',' initializer { $1->PushBack(NodePtr($3)); $$=$1; }
	;

statement
	: labeled_statement { $$ = $1; }
	| compound_statement { $$ = $1; }
	| expression_statement { $$ = $1; }
	| selection_statement
	| iteration_statement
	| jump_statement { $$ = $1; }
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement { $$ = new Case(NodePtr($2), NodePtr($4)); }
	| DEFAULT ':' statement { $$ = new DefaultCase(NodePtr($3)); }
	;

compound_statement
	: '{' '}' {
		$$ = new Scope(NodePtr(nullptr), NodePtr(nullptr));
	}
	| '{' statement_list '}' {
		$$ = new Scope(NodePtr(nullptr), NodePtr($2));
	}
	| '{' declaration_list '}' {
		$$ = new Scope(NodePtr($2), NodePtr(nullptr));
	}
	| '{' declaration_list statement_list '}'  {
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

expression_statement
	: ';'
	| expression ';' { $$ = $1; }
	;

selection_statement
	: IF '(' expression ')' statement { $$ = new IfElse(NodePtr($3), NodePtr($5), nullptr); }
	| IF '(' expression ')' statement ELSE statement { $$ = new IfElse(NodePtr($3), NodePtr($5), NodePtr($7)); }
	| SWITCH '(' expression ')' statement { $$ = new Switch(NodePtr($3), NodePtr($5)); }
	;

iteration_statement
	: WHILE '(' expression ')' statement { $$ = new While(NodePtr($3), NodePtr($5)); }
	| DO statement WHILE '(' expression ')' ';' { $$ = new DoWhile(NodePtr($2), NodePtr($5)); }
	| FOR '(' expression_statement expression_statement ')' statement { $$ = new For(NodePtr($3), NodePtr($4), nullptr, NodePtr($6)); }
	| FOR '(' expression_statement expression_statement expression ')' statement { $$ = new For(NodePtr($3), NodePtr($4), NodePtr($5), NodePtr($7)); }
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';' { $$ = new Continue(); }
	| BREAK ';' { $$ = new Break(); }
	| RETURN ';' {
		$$ = new ReturnStatement(nullptr);
	}
	| RETURN expression ';' {
		$$ = new ReturnStatement(NodePtr($2));
	}
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
