#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;
static int n = 0;

const char * TW[] = {
        "", "program", "string", "if",
        "else", "read", "write",
        "not", "and", "or", "undefined",
        "true", "false", "int", "real", "boolean",
        "while", "goto", "return", "number",
        "case", "of", "end",
        NULL
};

const char * TD[] = {
        ";", ",", ":", ".",
        "(",")", "{", "}",
        "=", "==", "<", ">",
        "+","-", "++", "--", "+=",
        "-=", "*", "/", "*=", "/=",
        "%","<=", "!=", ">=",
        NULL
};

enum LexType {
    // TW 0-21
    LEX_NULL,
    LEX_PROGRAM,
    LEX_STRING,
    LEX_IF,
    LEX_ELSE,
    LEX_READ,
    LEX_WRITE,
    LEX_NOT,
    LEX_AND,
    LEX_OR,
    LEX_UNDEFINED,
    LEX_TRUE,
    LEX_FALSE,
    LEX_INT,
    LEX_REAL,
    LEX_BOOLEAN,
    LEX_WHILE,
    LEX_GOTO,
    LEX_RETURN,
    LEX_NUMBER,
    LEX_CASE,
    LEX_OF,
    LEX_ENDP,
    // TD 22-47
    LEX_SEMICOLON,
    LEX_COMMA,
    LEX_COLON,
    LEX_DOT,
    LEX_LPAREN,
    LEX_RPAREN,
    LEX_BEGIN,
    LEX_END,
    LEX_EQ,
    LEX_DEQ,
    LEX_LSS,
    LEX_GTR,
    LEX_PLUS,
    LEX_MINUS,
    LEX_PLUSP,
    LEX_MINUSM,
    LEX_PLUSE,
    LEX_MINUSE,
    LEX_TIMES,
    LEX_SLASH,
    LEX_TIMESE,
    LEX_SLASHE,
    LEX_PERCENT,
    LEX_LEQ,
    LEX_NEQ,
    LEX_GEQ,
    // 48-51
    LEX_ID,
    LEX_NUMB,
    LEX_STR_CONST,
    LEX_ERROR
};

enum state {
    START,
    IDE,
    NUM,
    COM,
    HELPCOM,
    SLS,
    MUL,
    DOUBLET1,
    DOUBLET2,
    PLUS,
    MINUS,
    QUOTE
};

class Ident
{
    string id_name;
    LexType id_type;
    int id_value;
    bool declare;
public:

    Ident (string n) {
        id_name = n;
        declare = false;
    }
    bool operator==(const string& s) const {
        return id_name == s;
    }

    LexType GetType() const {
        return id_type;
    }
    int GetValue() const {
        return id_value;
    }
    string GetName() const  {
        return id_name;
    }
    bool GetDeclare() const {
        return declare;
    }
    void SetType(LexType t) {
        id_type = t;
    }
    void SetValue(int v) {
        id_value = v;
    }
    void SetName(string str) {
        id_name = str;
    }
    void SetDeclare() {
        declare = true;
    }
};

vector <Ident> TID;
int addtoTID(const string& str)
{
    vector<Ident>::iterator i;
    i = find(TID.begin(), TID.end(), str);
    if (i != TID.end()) return(i - TID.begin());
    else
    {
        TID.push_back(str);
        return(TID.size() - 1);
    }
}

class Lex
{
    LexType l_type;
    int l_value;
    string l_str;
public:
    Lex(){
        l_type = static_cast<LexType>(NULL);
        l_value = 0;
        l_str = "";
    }
    Lex (LexType t, int v, string str) {
        l_type = t;
        l_value = v;
        l_str = str;
    }

    LexType GetType() {
        return l_type;
    }
    int GetValue() const {
        return l_value;
    }
    string GetStr() const {
        return l_str;
    }

    void SetType(LexType t) {
        l_type = t;
    }
    void SetValue(int v) {
        l_value = v;
    }
    void SetString(string s) {
        l_str = s;
    }

    friend ostream& operator<< (ostream &out, Lex l) {
        string type, type_of_table;
        if (l.l_type <= LEX_ENDP)
        {
            type = (string)TW[l.l_type];
            type_of_table = "TW: ";

        }
        else if ((l.l_type >= LEX_SEMICOLON) && (l.l_type <= LEX_GEQ))
        {
            type = (string)TD[l.l_type - LEX_SEMICOLON];
            type_of_table = "TD: ";
        }
        if (l.l_type == LEX_ID)
        {
            type = TID[l.l_value].GetName();
            out << " < " << type << " | " << "TID: " << l.l_value  << " > " <<  "\n";
        }
        else if (l.l_type == LEX_NUMB)
        {
            type = "NUM";
            type_of_table = "";
            out << " < " << type << " | " << l.l_value  << " > " <<  "\n";
        }
        else if (l.l_type == LEX_STR_CONST)
        {
            type = "STR";
            type_of_table = "";
            out << " < " << type << " | " << l.l_str  << " > " <<  "\n";
        }
        else if (l.l_type == LEX_ERROR)
        {
            type = "ERROR";
            out << " < " << type << " | " << l.l_str << " > " << "\n";
        }
        else {
            out << " < " << type << " | " << type_of_table << l.l_str << " > " << "\n";
        }
        return out;
    }
};

class Scanner
{
    char c;
    char gc() {
        cin.read(&c, 1);
        return(c);
    }
    int find(string s, const char** table) {
        int i = 0;
        while (table[i] != NULL) {
            if (s == table[i])
                return(i);
            i++;
        }
        return(0);
    }
public:
    bool flag = true;
    Scanner() {
        ;
    }


    Lex GetLex() {
        int dig, j;
        state CS = START;
        string str = "";
        while (1)
        {
            if (flag) {
                if(gc() == '.') {
                    n = 1;
                    break;
                }
            }
            else {
                flag = true;
            }
            switch(CS)
            {
                case START:
                    if ( c == ' ' || c == '\n' || c == '\r' || c == '\t' )
                    {
                        ;
                    }
                    else if (isalpha(c))
                    {
                        str.push_back(c);
                        CS = IDE;
                    }
                    else if (isdigit(c))
                    {
                        dig = c - '0';
                        str.push_back(c);
                        CS = NUM;
                    }
                    else if (c == '+')
                    {
                        str.push_back(c);
                        CS = PLUS;
                    }
                    else if (c == '-')
                    {
                        str.push_back(c);
                        CS = MINUS;
                    }
                    else if (c == '/')
                    {
                        str.push_back(c);
                        CS = SLS;
                    }
                    else if(c == '*' || c == '%')
                    {
                        str.push_back(c);
                        CS = MUL;
                    }
                    else if (c == '!' || c == '=')
                    {
                        str.push_back(c);
                        CS = DOUBLET1;
                    }
                    else if (c == '<' || c == '>')
                    {
                        str.push_back(c);
                        CS = DOUBLET2;
                    }
                    else if (c == '"')
                    {
                        CS = QUOTE;
                    }
                    else
                    {
                        str.push_back(c);
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j, str);
                    }
                    break;
                case IDE:
                    if (isalpha(c) || isdigit(c))
                    {
                        str.push_back(c);
                    }
                    else
                    {
                        flag = false;
                        if ((j = find(str, TW)))
                        {
                            return Lex((LexType)j, j, str);
                        }
                        else
                        {
                            j = addtoTID(str);
                            return Lex(LEX_ID, j, str);
                        }
                    }
                    break;
                case NUM:
                    if (isdigit(c))
                    {
                        dig = 10 * dig + (c - '0');
                        str.push_back(c);
                    }
                    else if(isalpha(c)){
                        str.push_back(c);
                        while(isalpha(gc())){
                            str.push_back(c);
                        }
                        return(Lex(LEX_ERROR, 0, str));
                    }
                    else
                    {
                        flag = false;
                        return Lex(LEX_NUMB, dig, str);
                    }
                    break;
                case PLUS:
                    if (c == '=' || c == '+')
                    {
                        str.push_back(c);
                    }
                    else
                    {
                        flag = false;
                    }
                    j = find(str, TD);
                    return Lex((LexType)(j + (int)LEX_SEMICOLON), j, str);
                    break;
                case MINUS:
                    if (c == '=' || c == '-')
                    {
                        str.push_back(c);
                    }
                    else
                    {
                        flag = false;
                    }
                    j = find(str, TD);
                    return Lex((LexType)(j + (int)LEX_SEMICOLON), j, str);
                    break;
                case MUL:
                    if (c == '=')
                    {
                        str.push_back(c);
                    }
                    else
                    {
                        flag = false;
                    }
                    j = find(str, TD);
                    return Lex((LexType)(j + (int)LEX_SEMICOLON), j, str);
                    break;
                case QUOTE:
                    if (c == '"')
                    {
                        return Lex(LEX_STR_CONST, 0, str);
                    }
                    str.push_back(c);
                    break;
                case SLS:
                    if (c == '*')
                    {
                        str.pop_back();
                        CS = COM;
                    }
                    else
                    {
                        if (c == '=')
                        {
                            str.push_back(c);
                        }
                        else
                        {
                            flag = false;
                        }
                        j = find(str, TD);
                        return Lex((LexType)(j + (int)LEX_SEMICOLON), j, str);
                    }
                    break;
                case COM:
                    if (c == '*')
                        CS = HELPCOM;
                    break;
                case HELPCOM:
                    if (c == '/')
                        CS = START;
                    else
                        CS = COM;
                    break;
                case DOUBLET1:
                    if (c == '=')
                    {
                        str.push_back(c);
                    }
                    else{
                        flag = false;
                    }
                    j = find(str, TD);
                    return Lex((LexType)(j + (int)LEX_SEMICOLON), j, str);
                    break;
                case DOUBLET2:
                    if (c == '=')
                    {
                        str.push_back(c);
                    }
                    else
                    {
                        flag = false;
                    }
                    j = find(str, TD);
                    return Lex((LexType)(j + (int)LEX_SEMICOLON), j, str);
                    break;
            }
        }
    }
};


template <class T, class Q>
void from_st(T& st, Q& i)
{
    i = st.top();
    st.pop();
}

class SYNCH
{
    Lex     c_lex;
    LexType c_type;
    int     c_val;
    string  c_str;
    Scanner scan;

    stack<int>     st_int;
    stack<LexType> st_lex;

    void ST_PL();

    void IN_BR();
    void DESC();
    void BOOLCHECK();
    void PMCHECK();
    void LEXCHECK();

    void GT();
    void RD();
    void WR();

    void twdesc(LexType type, int i);
    void check_id();
    void check_readid();
    void check_op();
    void check_not();
    void eq_bool(LexType&);

    void gl()
    {
        c_lex = scan.GetLex();
        c_type = c_lex.GetType();
        c_val = c_lex.GetValue();
        c_str = c_lex.GetStr();
        cout << c_lex;
    }

public:
    SYNCH() : scan() {}
    void analyze();
};

void SYNCH::analyze()
{
    gl();
    ST_PL();
    cout << endl << "end of work" << endl;
}

void SYNCH::ST_PL()
{
    if (c_type == LEX_PROGRAM)
    {
        gl();
        IN_BR();
    }
    else if (c_type == LEX_IF)
    {
        LexType new_val;
        gl();
        if (c_type != LEX_LPAREN)
        {
            throw c_lex;
        }
        else
        {
            gl();
            BOOLCHECK();
            eq_bool(new_val);
            if (c_type == LEX_RPAREN)
            {
                gl();
                IN_BR();
                if (c_type == LEX_ELSE)
                {
                    gl();
                    IN_BR();
                }
            }
            else
            {
                throw c_lex;
            }
        }
    }
    else if (c_type == LEX_ID)
    {
        check_id();
        gl();
        if (c_type == LEX_EQ)
        {
            gl();
            BOOLCHECK();
            if (c_type == LEX_SEMICOLON)
            {
                gl();
            }
            else
            {
                throw c_lex;
            }
        }
        else
        {
            throw c_lex;
        }
    }
    else if (c_type == LEX_INT || c_type == LEX_STRING || c_type == LEX_BOOLEAN || c_type == LEX_REAL)
    {
        st_lex.push(c_type);
        gl();
        DESC();
    }
    else if (c_type == LEX_NUMB)
    {
        st_lex.push(LEX_NUMB);
        BOOLCHECK();
        gl();
    }
    else if (c_type == LEX_GOTO)
    {
        gl();
        GT();
    }
    else if (c_type == LEX_END)
    {
        return;
    }
    else if (c_type == LEX_READ)
    {
        RD();
        check_readid();
        gl();
    }
    else if (c_type == LEX_WRITE)
    {
        WR();
        gl();
    }
    ST_PL();
}

void SYNCH::IN_BR()
{
    if (c_type == LEX_BEGIN)
    {
        gl();
        ST_PL();
        if (c_type == LEX_END)
        {
            gl();
        }
        else
        {
            throw c_lex;
        }
    }
    else
    {
        throw c_lex;
    }
}

void SYNCH::GT()
{
    if (c_type != LEX_ID)
    {
        throw c_lex;
    }
    else
    {
        twdesc(LEX_ID, c_val);
        gl();
    }
    if (c_type != LEX_SEMICOLON)
    {
        throw c_lex;
    }
    else if (c_type == LEX_SEMICOLON)
    {
        gl();
    }
}

void SYNCH::DESC()
{
    if (c_type != LEX_ID)
    {
        throw c_lex;
    }
    else{
        int l_v_index = c_val;
        gl();
        LexType i;
        from_st(st_lex, i);
        if (c_type == LEX_EQ)
        {
            gl();
            BOOLCHECK();
            twdesc(i, l_v_index);
        }
        else
        {
            twdesc(i, l_v_index);
        }
        st_lex.push(i);

        while (c_type == LEX_COMMA)
        {
            gl();
            if (c_type != LEX_ID)
            {
                throw c_lex;
            }
            else
            {
                l_v_index = c_val;
                gl();
                from_st(st_lex, i);
                if (c_type == LEX_EQ)
                {
                    gl();
                    BOOLCHECK();
                    twdesc(i, l_v_index);
                }
                else
                {
                    twdesc(i, l_v_index);
                }
                st_lex.push(i);
            }
            from_st(st_lex, i);
        }
        if (c_type != LEX_SEMICOLON)
        {
            throw c_lex;
        }
        else
        {
            gl();
        }
    }
}

void SYNCH::BOOLCHECK()
{
    PMCHECK();
    while (c_type == LEX_OR || c_type == LEX_AND || c_type == LEX_NOT || c_type == LEX_EQ)
    {
        st_lex.push(c_type);
        gl();
        PMCHECK();
        check_op();
    }
}

void SYNCH::PMCHECK()
{
    LEXCHECK();
    while ( c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_PERCENT
    || c_type == LEX_LSS || c_type == LEX_GTR || c_type == LEX_LEQ ||
            c_type == LEX_GEQ || c_type == LEX_NEQ || c_type == LEX_DEQ)
    {
        st_lex.push(c_type);
        gl();
        LEXCHECK();
        check_op();
    }
}

void SYNCH::LEXCHECK()
{
    if (c_type == LEX_ID)
    {
        check_id();
        gl();
    }
    else if (c_type == LEX_NUMB)
    {
        st_lex.push(LEX_NUMB);
        gl();
    }
    else if (c_type == LEX_TRUE)
    {
        st_lex.push(LEX_BOOLEAN);
        gl();
    }
    else if (c_type == LEX_FALSE)
    {
        st_lex.push(LEX_BOOLEAN);
        gl();
    }
    else if (c_type == LEX_STR_CONST)
    {
        st_lex.push(LEX_STR_CONST);
        gl();
    }
    else if (c_type == LEX_NOT)
    {
        gl();
        LEXCHECK();
        check_not();
    }
    else if (c_type == LEX_LPAREN)
    {
        gl();
        BOOLCHECK();
        if (c_type == LEX_RPAREN)
        {
            gl();
        }
        else
        {
            throw c_lex;
        }
    }
    else
    {
        throw c_lex;
    }
}

void SYNCH::RD()
{
    gl();
    if (c_type != LEX_LPAREN)
    {
        throw c_lex;
    }
    else
    {
        gl();
        BOOLCHECK();
        if (c_type != LEX_RPAREN)
        {
            throw c_lex;
        }
        else
        {
            gl();
        }
    }
}

void SYNCH::WR()
{
    gl();
    if (c_type != LEX_LPAREN)
    {
        throw c_lex;
    }
    else
    {
        gl();
        BOOLCHECK();
        while (c_type == LEX_COMMA)
        {
            gl();
            BOOLCHECK();
        }
        if (c_type != LEX_RPAREN)
        {
            throw c_lex;
        }
        else
        {
            gl();
        }
    }
}

void SYNCH::twdesc(LexType type, int i)
{
    if (TID[i].GetDeclare())
    {
        throw "twice";
    }
    else
    {
        TID[i].SetDeclare();
        TID[i].SetType(type);
    }
}

void SYNCH::check_id()
{
    if (TID[c_val].GetDeclare()) {
        st_lex.push(TID[c_val].GetType());
    }
    else
    {
        throw "not declared";
    }
}

void SYNCH::check_readid()
{
    if (!TID[c_val].GetDeclare())
        throw "not declared";
}

void SYNCH::check_op()
{
    LexType t1, t2, op;
    LexType operation_type = LEX_INT;
    LexType put_in_stack = LEX_BOOLEAN;

    from_st(st_lex, t2);
    from_st(st_lex, op);
    from_st(st_lex, t1);


    if((t1 == LEX_INT) && (t1 == t2)) {
        operation_type = LEX_INT;
    }

    if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH || op == LEX_PERCENT){
        if((t1 == LEX_INT) && (t1 == t2)){
            operation_type = LEX_INT;
            put_in_stack = LEX_INT;
        }
        else{
            if(op != LEX_PERCENT){
                operation_type = LEX_REAL;
                put_in_stack = LEX_REAL;
            }
            else throw "Use percent with real";
        }
    }

    if (op == LEX_OR || op == LEX_AND)
        operation_type = LEX_BOOLEAN;

    if (t1 == LEX_STRING) {
        operation_type = LEX_STRING;
        if(op == LEX_PLUS){
            put_in_stack = LEX_STRING;
        }
    }

    if ((t1 == operation_type || t2 == operation_type) && ((t1 == t2) || ((t1 == LEX_INT && t2 == LEX_REAL)) || ((t1 == LEX_REAL && t2 == LEX_INT)))){
        st_lex.push(put_in_stack);
    }
    else {
        throw "wrong types in last operation";
    }
}

void SYNCH::check_not()
{
    LexType t;
    from_st(st_lex, t);
    if (t == LEX_BOOLEAN)
    {
        st_lex.push(LEX_BOOLEAN);
    }
    else
        throw "wrong type is in not";
}

void SYNCH::eq_bool(LexType& new_val)
{
    from_st(st_lex, new_val);
    if (new_val != LEX_BOOLEAN)
        throw "expression is not boolean";
}

class Translator
{
    SYNCH t;
public:
    Translator() : t() {}
    void Translate ()
    {
        t.analyze();
    }
};


int main(){
    ifstream in_stream;
    char filename[31];
    cout << "write filename :: ";
    cin >> filename;
    in_stream.open(filename);
    if (in_stream.fail())
    {
        cout << "Could not open file to read.""\n";
        exit(1);
    }
    cin.rdbuf(in_stream.rdbuf());

    Translator t;
    try{
        t.Translate();
    }
    catch(Lex L){
        cout << L;
    }
    catch (const char* str)
    {
        cout << "ERROR: " << str;
        return 1;
    }
}
