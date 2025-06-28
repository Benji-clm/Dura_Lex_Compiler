%option noyywrap

%{
  // A lot of this lexer is based off the ANSI C grammar:
  // https://www.lysator.liu.se/c/ANSI-C-grammar-l.html#MUL-ASSIGN
  // Avoid error "error: `fileno' was not declared in this scope"
  extern "C" int fileno(FILE *stream);

  #include "parser.tab.hpp"
  #include <unordered_map>
  #include <unordered_set>
  #include <stdio.h>
  #include <string>
  #include <cstdlib>
  #include <iostream>
  #include <sstream>

  // Suppress warning about unused function
  [[maybe_unused]] static void yyunput (int c, char * yy_bp );

  // Lexer hack
  std::unordered_set<std::string> typedefs;

  void updateTypeDefs(std::string type)
  {
    printf("PARSER: Parsed a TypeDef: %s\n", type.c_str());
    typedefs.insert(type);
  }

  int checkType(void)
  {
    // If we have typedef'd a string, return the type
    if (typedefs.find(*yylval.string) != typedefs.end())
    {
      printf("PARSER: Found a TypeDef:\n");
      return TYPE_NAME;
    }

    return IDENTIFIER;
  }

int process_char_literal(const char* text) {
    std::string str(text);

    // Remove enclosing single quotes
    if (str.size() >= 2 && str.front() == '\'' && str.back() == '\'') {
        str = str.substr(1, str.size() - 2);
    }

    // Handle common escape sequences
    if (str == "\\n") return '\n';
    if (str == "\\t") return '\t';
    if (str == "\\r") return '\r';
    if (str == "\\b") return '\b';
    if (str == "\\f") return '\f';
    if (str == "\\v") return '\v';
    if (str == "\\0") return '\0';
    if (str == "\\\\") return '\\';
    if (str == "\\'") return '\'';
    if (str == "\\\"") return '\"';

    // Handle octal escapes (e.g., '\101' -> 'A')
    if (str[0] == '\\' && str.size() > 1 && isdigit(str[1])) {
        int value = std::stoi(str.substr(1), nullptr, 8);
        return (char)value;
    }

    // Handle hexadecimal escapes (e.g., '\x41' -> 'A')
    if (str.substr(0, 2) == "\\x" && str.size() > 2) {
        int value = std::stoi(str.substr(2), nullptr, 16);
        return (char)value;
    }

    // Invalid escape sequence
    if (str[0] == '\\') {
        std::cerr << "Warning: Unrecognized escape sequence '" << str << "'" << std::endl;
        return '?';  // Default to a placeholder
    }

    return str[0];  // Normal case: return the character
  }

%}

D	  [0-9]
L	  [a-zA-Z_]
H   [a-fA-F0-9]
E	  [Ee][+-]?{D}+
FS  (f|F|l|L)
IS  (u|U|l|L)*

%%
"/*"			{/* consumes comment - TODO you might want to process and emit it in your assembly for debugging */}
"//"(.)*     								{;}

"auto"			{return(AUTO);}
"break"			{return(BREAK);}
"case"			{return(CASE);}
"char"			{return(CHAR);}
"const"			{return(CONST);}
"continue"  {return(CONTINUE);}
"default"		{return(DEFAULT);}
"do"			  {return(DO);}
"double"		{return(DOUBLE);}
"else"			{return(ELSE);}
"enum"			{return(ENUM);}
"extern"		{return(EXTERN);}
"float"			{return(FLOAT);}
"for"			  {return(FOR);}
"goto"			{return(GOTO);}
"if"			  {return(IF);}
"int"			  {return(INT);}
"long"			{return(LONG);}
"register"	{return(REGISTER);}
"return"		{return(RETURN);}
"short"			{return(SHORT);}
"signed"		{return(SIGNED);}
"sizeof"		{return(SIZEOF);}
"static"		{return(STATIC);}
"struct"		{return(STRUCT);}
"switch"		{return(SWITCH);}
"typedef"		{return(TYPEDEF);}
"union"			{return(UNION);}
"unsigned"	{return(UNSIGNED);}
"void"			{return(VOID);}
"volatile"	{return(VOLATILE);}
"while"			{return(WHILE);}

{L}({L}|{D})*		{ yylval.string = new std::string(yytext); return(checkType()); }

0[xX]{H}+{IS}?		{yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
0{D}+{IS}?		    {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
{D}+{IS}?		      {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
L?'(\\.|[^\\'])+'	{yylval.number_int = process_char_literal(yytext); return(CHAR_LITERAL);}

{D}+{E}{FS}?		        {yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}
{D}*"."{D}+({E})?{FS}?	{yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}
{D}+"."{D}*({E})?{FS}?	{yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}

L?\"(\\.|[^\\"])*\"	{yylval.string = new std::string(yytext + 1, yytext + yyleng - 1); return(STRING_LITERAL);

}

"..."      {return(ELLIPSIS);}
">>="			 {return(RIGHT_ASSIGN);}
"<<="      {return(LEFT_ASSIGN);}
"+="			 {return(ADD_ASSIGN);}
"-="       {return(SUB_ASSIGN);}
"*="       {return(MUL_ASSIGN);}
"/="			 {return(DIV_ASSIGN);}
"%="			 {return(MOD_ASSIGN);}
"&="       {return(AND_ASSIGN);}
"^="			 {return(XOR_ASSIGN);}
"|="       {return(OR_ASSIGN);}
">>"       {return(RIGHT_OP);}
"<<"       {return(LEFT_OP);}
"++"			 {return(INC_OP);}
"--"			 {return(DEC_OP);}
"->"			 {return(PTR_OP);}
"&&"			 {return(AND_OP);}
"||"			 {return(OR_OP);}
"<="			 {return(LE_OP);}
">="			 {return(GE_OP);}
"=="			 {return(EQ_OP);}
"!="			 {return(NE_OP);}
";"			   {return(';');}
("{"|"<%") {return('{');}
("}"|"%>") {return('}');}
","			   {return(',');}
":"			   {return(':');}
"="			   {return('=');}
"("		     {return('(');}
")"			   {return(')');}
("["|"<:") {return('[');}
("]"|":>") {return(']');}
"."			   {return('.');}
"&"			   {return('&');}
"!"			   {return('!');}
"~"			   {return('~');}
"-"			   {return('-');}
"+"			   {return('+');}
"*"			   {return('*');}
"/"			   {return('/');}
"%"			   {return('%');}
"<"			   {return('<');}
">"			   {return('>');}
"^"			   {return('^');}
"|"			   {return('|');}
"?"			   {return('?');}

[ \a\b\t\v\f\n\r]		{/* ignore new lines and special sequences */}
.			              {/* ignore bad characters */}

%%

void yyerror (char const *s)
{
  fprintf(stderr, "Lexing error: %s\n", s);
  exit(1);
}
