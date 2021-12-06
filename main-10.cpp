#include "std_lib_facilities.h"




struct Token {
 char kind;
 double value;
 string name;
 Token(char ch) :kind(ch), value(0) { }     
 Token(char ch, double val) :kind(ch), value(val) { }
 Token(char ch, string n) :kind(ch), name(n) { }
};

class Token_stream {
 bool full;
 Token buffer;
public:
 Token_stream() :full(0), buffer(0) { }
 Token get();
 void unget(Token t) { buffer = t; full = true; }
 void ignore(char);
};

const char isConst = 'C';
const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char squareR = 's';
const char findPow = 'p';

const string constKey = "const";
const string declKey = "let";
const string quitKey = "quit";
const string sqrtKey = "sqrt";
const string powKey = "pow";

Token Token_stream::get()
{
 if (full) 
 { 
  full = false; 
  return buffer; 
 }

 char ch;
 cin >> ch;  
 switch (ch) 
 {
  case '(': 
  case ')': 
  case '+': 
  case '-':
  case '*': 
  case '/': 
  case '%': 
  case ';':
  case '=': 
  case ',':
   return Token(ch);     
  case '.':
  case '0': 
  case '1': 
  case '2': 
  case '3':
  case '4': 
  case '5': 
  case '6': 
  case '7':
  case '8': 
  case '9':
  { 
   cin.unget();      
   double val;
   cin >> val;       
   return Token(number, val);  
  }

  default:
   if (isalpha(ch)|| ch == '_') 
   {
    string s;
    s += ch;

    while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) 
     s += ch;
    cin.unget();

    if(s == constKey)
      return Token(isConst);
    if (s == declKey) 
     return Token(let);
    if (s == quitKey) 
     return Token(quit);
    if (s == sqrtKey)
     return Token(squareR);
    if (s == powKey)
     return Token(findPow);

    return Token(name, s);
   }

   error("Bad token");
 }
}


void Token_stream::ignore(char c)
{
 if (full && c == buffer.kind) 
 {
  full = false;
  return;
 }

 full = false;
 char ch;
 while (cin >> ch)
  if (ch == c) return;
}

struct Variable 
{
 string name;
 double value;
 bool isConst;
 Variable(string n, double v, bool ic) :name(n), value(v), isConst(ic)  { }
};

vector<Variable> names;

double get_value(string s, double val)
{
 for (int i = 0; i < names.size(); ++i)
 {
  if (names[i].name == s)
  {
   return names[i].value;
  } 
 }

 error("get: undefined name ", s);
 return 0.0;
}

void set_value(string s, double d)
{
 for (int i = 0; i <= names.size(); ++i)
  if (names[i].name == s && names[i].isConst == false) 
  {
   names[i].value = d;
   return;
  }

 error("set: undefined name ", s);
}

bool is_declared(string s)
{
 for (int i = 0; i < names.size(); ++i)
 {
  if (names[i].name == s && names[i].isConst == true)
    error("Cannot reassign const variable");
  else if (names[i].name == s && names[i].isConst == false)
    return true;
 }

 return false;
}

double define_name(string var, double val, bool isConst)
{
 if (is_declared(var))
  error(var, " declared twice");

 names.push_back(Variable(var, val, isConst));

 return val;
}

Token_stream ts;

double expression();  

Token checkForChar(Token t, char ch)
{
  string chstring = "";
  chstring += ch;
  error("'" + chstring + "' expected");
}


double primary()
{
 Token t = ts.get();
 switch (t.kind) 
 {
  case '(':
  { 
   double d = expression();
   t = ts.get();
   checkForChar(t, ')');
   return d;
  }

  case '-':
   return -primary();

  case number:
   return t.value;

  case name:
   return get_value(t.name, 0);
   
  case squareR: 
  { 
   //get next char after 'sqrt' 
   t = ts.get();
   checkForChar(t, '(');

    
   double d = primary();
   if (d < 0)
    error("Cannot squareroot negative integers");

    //get next char after expression
   t = ts.get();
   checkForChar(t, ')');

    
   return sqrt(d);
  }

   
  case findPow:
  {
   // next char after 'pow'    
   t = ts.get();
   checkForChar(t, '(');

    //get the expression after '('
   double x = expression();

    // next char after 'expression'    
   t = ts.get();
   checkForChar(t, ',');

    // the expression after ','
   double i = expression();

    // char after expression
   t = ts.get();
   checkForChar(t, ')');

    
   return pow(x, i);
  }
  default:
   error("primary expected");
 }
}


double term()
{
 double left = primary();
 while (true) 
 {
  Token t = ts.get();
  switch (t.kind) 
  {
   case '*':
    left *= primary();
    break;
   case '/':
   { 
    double d = primary();
    if (d == 0) 
     error("divide by zero");
    left /= d;
    break;
   }
   case '%':
   {
    double d = primary();
    if (d == 0)
     error("%:divide by zero");
    left = fmod(left, d);
    break;
   }
   default:
    ts.unget(t);
    return left;
  }
 }
}


double expression()
{
 double left = term();
 while (true) 
 {
  Token t = ts.get();

  switch (t.kind) 
  {
   case '+':
    left += term();
    break;
   case '-':
    left -= term();
    break;
   default:
    ts.unget(t);
    return left;
  }
 }
}

// name definition errors
double declaration()
{
Token t = ts.get();

 bool isC;
 if (t.kind == 'C')
 {
  isC = true;
  t = ts.get();  
 }
 else
  isC = false;

 if (t.kind != 'a') 
  error("name expected in declaration;");

 string name = t.name;
 if (is_declared(name))
 {
  cout << name + ", declared twice. Reassign? y/n > ";
  cin.clear();
  cin.ignore(10000, '\n'); 
  string ans;
  getline(cin, ans);
  if (ans == "n")
   error(name, ", will not be reassigned ");
  if (ans == "y")
  {
   cout << "enter new value: ";
   int val;
   cin >> val;
   set_value(name, val);
   double d = val; 
   return d;
  }
 }

}


double statement()
{
 Token t = ts.get();
 switch (t.kind) 
 {
  case let:
   return declaration();
  default:
   ts.unget(t);
   return expression();
 }
}

void clean_up_mess()
{
 ts.ignore(print);
}

const string prompt = " > ";
const string result = " = ";

void calculate()
{
 while (true) try 
 {
  cout << prompt;
  Token t = ts.get();

  while (t.kind == print) 
   t = ts.get();       

  if (t.kind == quit) 
   return;

  ts.unget(t);

  cout << result << statement() << endl;
 }
 catch (runtime_error & e) 
 {
  cerr << e.what() << endl;
  clean_up_mess();
 }
}

int main()
try {

 calculate();
 return 0;
}

catch (exception& e) {
 cerr << "exception: " << e.what() << endl;
 char c;
 while (cin >> c && c != ';');
 return 1;
}

catch (...) {
 cerr << "exception\n";
 char c;
 while (cin >> c && c != ';');
 return 2;

}


