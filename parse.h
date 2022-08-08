/*This header provides interface to parse files with assembly source code and control the parser module*/

/*distinct parse functions*/
int parse_punc(char *word);
int parse_endl(char *line);
int parse_blank(char *line);
int parse_name(char *word);
int parse_oper(char *word);
int parse_direc(char *word);
int parse_reg(char *word);
int parse_strlit(char *line);
int parse_intlit(char *line, int *res);

Token parse_token(char *line);
void parse_file(char *fname);

/*general parser control*/
void restart_parser();
void free_parser();
