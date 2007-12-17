/* speedcrunch single line expression grammar */

%{
int yylex(void)
void yyerror(const char*);
%}

%token LITERAL

%%
input: /* empty */
       | LITERAL
;
%%