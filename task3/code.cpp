#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define N 13
#define LEN 50

using namespace std;
int count_lines = 1; 						//记录初始行数，行数从第一行开始 
string keywords[N] = {"begin", "end", "if", "then", "else", "while", "write", "read", "do", "call",  "const", "var", "procedure"};

struct Lex_Symbol
{ 
	string symbol_record;		    //储存保留字，标识符，运算符等 
	int count_lines_num;			//储存行号 
};

struct Lex_Symbol symbols[200];		 

int grammar=1; // 语法是否正确 

struct Quad{	//四元式 
	string type;
	string op1;
	string op2;
	string result;
}; 

struct Quad Quad[LEN];


struct table{	//符号表 
	string kind;
	string name;
	string size;
}; 
struct table table[LEN];

int record_idx = 0;	// 存放toekn的下标 
int read_idx = 0;   // 读取token的下标 
int quad_idx=0;     // 四元式的索引 
int tab=0;          // 符号表的索引 

int semantic=1;		// 若为1则表示程序语义正确，为0表示语义错误
int if_true;		// 程序的if判断语句正确时执行语句的行数
int if_false;		// 程序的if判断语句错误时执行语句的行数
int while_true;		// 程序的while判断语句正确时执行语句的行数
int while_false;	// 程序的while判断语句正确时执行语句的行数


void Iden(string Lex_str);
void Scanner();

void P();
void P_plus();	
void I();	
void V();		
void I_plus();	
void D();			
void S();	
void stepAdd();		// 终止符, d 
void unsInt();	// 无符号整数, 改写为n 
void A();		
void S_plus();	
void E();		
void F();	
void C_plus();		
void L();	
void T();			
void T_plus();				
void M();
void C();
void R();		
void V_plus();
void H();
void W();
void D_plus();
int isKeyword(string str);	// 判断是是否是关键字 
string Int_to_String(int n);

string Int_to_String(int n)
{
	ostringstream stream;
	stream<<n;
	return stream.str();
}

//程序--><分程序>.
// P -> P' . 
void P(){
	P_plus();
	if(symbols[read_idx].symbol_record==".")
	{
		read_idx++;
	}
	return;
}

// 分程序-->[<常数说明部分>][<变量说明部分>][<过程说明部分>]<语句>  
/*
	 P' -> IVI'S
	其中I,V,I'这3部分都是任选项, 可出现可不出现 
*/
void P_plus(){
	//常量说明部分
	if(symbols[read_idx].symbol_record=="const")
	{
		I();
	}
	//变量说明部分
 	if (symbols[read_idx].symbol_record == "var")
	{
		V();
	}
	//过程说明部分
	if (symbols[read_idx].symbol_record== "procedure")
	{
		I_plus();
	}
	S();//语句
}

// 常量说明部分-->const<常量定义>{,<常量定义>}
// I -> const DA'| ε
void I()
{
	if (symbols[read_idx].symbol_record == "const")
	{
		table[tab].kind="const";
		Quad[quad_idx].type="const";
		read_idx++;
		D();
		while(symbols[read_idx].symbol_record == ",")
		{
			tab++;
			table[tab].kind="const";
			read_idx++;
			D();
		}	
		if (symbols[read_idx].symbol_record == ";")
		{
			read_idx++;
			return;
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}	
}

// 变量说明部分 -->var<标识符>{,<标识符>}
// V -> vbB' | ε
void V()
{
	// 判断变量是否合法，flag若为1则出现语义错误
	// 如果声明已被定义的变量，则flag1变为1
	// 只声明一个变量的时候只用到flag1 
	int flag1=0;
	// 若一次声明两个或以上的变量，则使用到flag2
	//  
	int flag2=0;
	if (symbols[read_idx].symbol_record == "var")
	{
		table[tab].kind="var";
		Quad[quad_idx].type="var"; 
		read_idx++;
		stepAdd();
		for(int i=0;i<tab;i++)//判断该变量在之前是否被定义过 
		{
			if(table[i].kind=="const"||table[i].kind=="var")
			{
				if(symbols[read_idx-1].symbol_record==table[i].name)
				{
					flag1=1;
					break;
				} 
			}
		}
		if(flag1)
		{
			cout<<"(语义错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			semantic=0;
		}
		table[tab].name=symbols[read_idx-1].symbol_record;
		tab++;
		Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
		quad_idx++;
		while(symbols[read_idx].symbol_record == ",")
		{
			table[tab].kind="var";
			Quad[quad_idx].type="var";
			read_idx++;
			stepAdd();
			for(int i=0;i<tab;i++)//判断该变量在之前是否被定义过 
			{
				if(table[i].kind=="const"||table[i].kind=="var")
				{
					if(symbols[read_idx-1].symbol_record==table[i].name)
					{
						flag2=1;
						break;
					} 
				}
			}
			if(flag2)
			{
				cout<<"(语义错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
				semantic=0;
			}
			table[tab].name=symbols[read_idx-1].symbol_record;	
			tab++;
			Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
			quad_idx++;	    
		}
		if(symbols[read_idx].symbol_record== ";")
		{
			read_idx++;
			return;
		}
		else 
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
			read_idx=read_idx+2;
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

//过程说明部分--><过程首部><分程序>{;<过程说明部分>};
void I_plus()											
{
	A();
	P_plus();
	if (symbols[read_idx].symbol_record == ";")
	{
		read_idx++;	
		while(symbols[read_idx].symbol_record == "procedure")
		{
			I_plus();
		}
		if (symbols[read_idx].symbol_record == ";")
		{
			read_idx++;
			return;
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
		
}

//常量定义 --> <标识符>=<无符号整数>
/*
	D -> b=n | ε 
*/
void D()
{
	stepAdd();
	table[tab].name=symbols[read_idx-1].symbol_record;
	Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
	quad_idx++;
	if (symbols[read_idx].symbol_record == "=")
	{
		read_idx++;
		unsInt();
		table[tab].size=symbols[read_idx-1].symbol_record;
		tab++;
		Quad[quad_idx].type="=";
		Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
		Quad[quad_idx].result=Quad[quad_idx-1].op1;
		quad_idx++;
	}
	else {
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
		read_idx=read_idx+2;
	}
		
}

// 标识符
void stepAdd()
{
	read_idx++;
}


void unsInt()//无符号整数
{
	if ((symbols[read_idx].symbol_record.at(0)>='0')&&(symbols[read_idx].symbol_record.at(0)<='9'))
	{
		read_idx++;
		return;
	}		
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

//过程首部-->procedure<标识符>;
void A()
{
	if (symbols[read_idx].symbol_record== "procedure")
	{
		table[tab].kind="procedure";
		Quad[quad_idx].type="procedure";
		read_idx++;
		stepAdd();
		table[tab].name=symbols[read_idx-1].symbol_record;
		Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
		if(symbols[read_idx].symbol_record == ";"){
			read_idx++;
			tab++;
			quad_idx++;
			return;
		}
		else 
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
			read_idx=read_idx+2;
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

//语句 --> <赋值语句>|<条件语句>|<当型循环语句>|<过程调用语句>|<读语句>|<写语句>|<复合语句>|<空语句>
void S() 
{
	if(isKeyword(symbols[read_idx].symbol_record))
		S_plus(); 
	if (symbols[read_idx].symbol_record== "if")//条件语句
		C();
	else if (symbols[read_idx].symbol_record == "while")//W循环
		W();
	else if (symbols[read_idx].symbol_record == "call")//过程调用
		V_plus();
	else if (symbols[read_idx].symbol_record == "read")//读语句
		H();
	else if (symbols[read_idx].symbol_record == "write")//写语句
		D_plus();
	else if (symbols[read_idx].symbol_record == "begin")//复合语句
		F(); 
}

//赋值语句--><标识符>:=<表达式>
void S_plus()
{
	stepAdd();
	if(symbols[read_idx].symbol_record==":=")
	{
		Quad[quad_idx].result=symbols[read_idx-1].symbol_record;
		read_idx++;
		E();
		if(symbols[read_idx].symbol_record==";")
		{
			read_idx++;
		}
		else {
			cout<<"(语法错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			grammar=0;
		}
	}
	else {
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
		read_idx=read_idx+4;
	}
}

//表达式-->[+|-]<项>{<加减运算><项>}
void E()
{
	if (symbols[read_idx].symbol_record == "+" || symbols[read_idx].symbol_record == "-")
	{
		read_idx++;
	}
	T();
	while(symbols[read_idx].symbol_record == "+" || symbols[read_idx].symbol_record == "-")
	{
		Quad[quad_idx].type=symbols[read_idx].symbol_record;
		Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
		L();
		T();
		Quad[quad_idx].op2=symbols[read_idx-1].symbol_record;
		quad_idx++;
	}
}

void F()//复合语句-->begin<语句>{;<语句>}end 
{
	if (symbols[read_idx].symbol_record == "begin")
	{
		read_idx++;
		S();
		while(symbols[read_idx].symbol_record == ";")
		{
			read_idx++;
			S();
		}
		if (symbols[read_idx].symbol_record == "end")
		{
			read_idx++;
			return;
		}
		else
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

//条件 --> <表达式><关系运算符><表达式>|odd<表达式>
void C_plus() 
{ 
	 if (symbols[read_idx].symbol_record == "odd")
	{
		E();
	}
	 else 
	{
		Quad[quad_idx].type="j";
		E();
		Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
		R();
		E();
		Quad[quad_idx].op2=symbols[read_idx-1].symbol_record;
		if(Quad[quad_idx].type=="j#")
		{
			quad_idx++;
			Quad[quad_idx].type="j=";
			Quad[quad_idx].op1=Quad[quad_idx-1].op1;
			Quad[quad_idx].op2=Quad[quad_idx-1].op2;
		}
		quad_idx++;
		
	}
}

//项 --> <因子>{<乘除运算符><因子>}
void T()
{
	// 若flag为1则表明该变量未在符号表中，即未声明 
	int flag1=1;
	int flag2=1;
	T_plus();
	
	for(int i=0;i<tab;i++)//判断该变量在之前是否被定义过，没定义过则报错  
	{
		if (symbols[read_idx-1].symbol_record.at(0) <= '9' && symbols[read_idx-1].symbol_record.at(0) >= '0')//如果是常数，则不用判断 
		{
			flag1=0;
			break; 
		} 
		// 查找符号表中kind为const或var的符号, 如果出现有, 则flag1为0，否则falg1为1, 即出现使用未声明的常量或变量 
		if(table[i].kind=="const"||table[i].kind=="var")
		{
			if(symbols[read_idx-1].symbol_record == table[i].name)
			{
				flag1=0;
				break;
			} 
		}
	}
	if(flag1)
	{
		cout<<"(语义错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
		semantic=0;
	}
	while(symbols[read_idx].symbol_record=="*"|| symbols[read_idx].symbol_record == "/")
	{
		Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
		M();
		Quad[quad_idx].type=symbols[read_idx-1].symbol_record;
		T_plus();
		
		for(int i=0;i<tab;i++)//判断该变量在之前是否被定义过,没定义过则报错 
		{
			if ((symbols[read_idx-1].symbol_record.at(0) <= '9' && symbols[read_idx-1].symbol_record.at(0) >= '0'))   //如果是常数，则不用判断 
			{
				flag2=0;
				break; 
			}
			if(table[i].kind=="const"||table[i].kind=="var")
			{
				if(symbols[read_idx-1].symbol_record==table[i].name)
				{
					flag2=0;
					break;
				} 
			}
		}
		if(flag2)
		{
			cout<<"(语义错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			semantic=0;
		}
		
		Quad[quad_idx].op2=symbols[read_idx-1].symbol_record;
		Quad[quad_idx].result="T1";
		quad_idx++;
	}
}

//因子--><标识符>|<无符号整数>
void T_plus() 
{
	if (symbols[read_idx].symbol_record.at(0) <= 122 && symbols[read_idx].symbol_record.at(0) >= 97)
	{
		stepAdd();
	}
	else if ((symbols[read_idx].symbol_record.at(0) <= '9' && symbols[read_idx].symbol_record.at(0) >= '0'))
	{
		unsInt();
	}
	else if ((symbols[read_idx].symbol_record == "("))
	{
		E();
		if (symbols[read_idx].symbol_record == ")")
		{
			read_idx++;
			return;
		}
		else
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0; 
		}
 	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

void L()//加减运算符-->+|-
{
	if (symbols[read_idx].symbol_record == "+" || symbols[read_idx].symbol_record == "-")
	{
		read_idx++;
		return;
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

void M()//乘除运算符-->*|/
{
	if (symbols[read_idx].symbol_record== "*" ||symbols[read_idx].symbol_record== "/")
	{
		read_idx++;
		return;
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

//关系运算符 -> =|#|<|<=|>|>=
void R()
{
	if (symbols[read_idx].symbol_record== "=" || symbols[read_idx].symbol_record== "#"||symbols[read_idx].symbol_record=="<"||symbols[read_idx].symbol_record == "<="||symbols[read_idx].symbol_record == ">"
		||symbols[read_idx].symbol_record == ">=")
	{
		Quad[quad_idx].type+=symbols[read_idx].symbol_record;
		read_idx++;
		return;
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

void C()//条件语句-->if<条件>then<语句> 
{
	if(symbols[read_idx].symbol_record == "if")
	{
		read_idx++;
		C_plus();
		if (symbols[read_idx].symbol_record == "then")
		{
			if_true=quad_idx+1;
			read_idx++; 
			S();
			if_false=quad_idx+1;
			Quad[quad_idx].type="ret";
			quad_idx++;
		}
		else 
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			grammar=0;
			S();
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
		
}

//过程调用语句-->call<标识符>
void V_plus() 
{
	int flag=1;	//判断call()是否有错 
	if (symbols[read_idx].symbol_record=="call")
	{
		Quad[quad_idx].type="call";
		read_idx++;
		stepAdd();
		for(int i=0;i<tab;i++)	//判断call里的变量是否在之前定义了 
			{
				// 查符号表 
				if(table[i].kind=="procedure")
				{
					if(symbols[read_idx-1].symbol_record==table[i].name)
					{
						flag=0;
						Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
						break;
					}
				}
			}
		if(flag)
		{
			cout<<"(语义错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			semantic=0;
		}
		quad_idx++;
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

void W()//当型循环-->while<条件>do<语句>
{
	if (symbols[read_idx].symbol_record == "while")
	{
		read_idx++;
		C_plus();
		if (symbols[read_idx].symbol_record == "do")
		{
			while_true=quad_idx+1; //因为中间代码输出不从0开始，是从1开始 
			read_idx++;
			S();
			while_false=quad_idx+1;//因为中间代码输出不从0开始，是从1开始
		}
		else 
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			grammar=0;
			S();
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
	
}

void H()//读语句-->read ( <标识符>{,<标识符>} ) 
{
	// 判断read()函数括号中的参数是否有错 
	int flag=1;
	if (symbols[read_idx].symbol_record== "read")
	{
		Quad[quad_idx].type="read";
		read_idx++;
		if (symbols[read_idx].symbol_record== "(")
		{
			read_idx++;
			stepAdd();
			// 判断读的变量是否在之前声明
			// 如果未定义flag为1, 即使用未声明的变量 
			for(int i=0;i<tab;i++) 
			{
				if(table[i].kind=="var")
				{
					if(symbols[read_idx-1].symbol_record==table[i].name)
					{
						flag=0;
						Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
						break;
					}
				}
			}
			if(flag)
			{
				cout<<"(语义错误,行号:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
				semantic=0;
			}
			while(symbols[read_idx].symbol_record == ",")
			{
				read_idx++;
				stepAdd();
				
			}
			if (symbols[read_idx].symbol_record== ")")
			{
				quad_idx++;
				read_idx++;
			}
			else 
			{
				cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
				grammar=0;
			}	
		}
		else
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0; 
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

// 写语句-->write ( <表达式>{,<表达式>} )
void D_plus() 
{
 
	if (symbols[read_idx].symbol_record == "write")
	{
		read_idx++;
		if (symbols[read_idx].symbol_record == "(")
		{
			read_idx++;
			E();
			while(symbols[read_idx].symbol_record == ",")
			{
				read_idx++;
				E();
				
			}
			if (symbols[read_idx].symbol_record == ")")
			{
				Quad[quad_idx].type="write";
				Quad[quad_idx].op1=Quad[quad_idx-1].result;
				quad_idx++;
				read_idx++;
			}
		}
		else
		{
			cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
			read_idx=read_idx+4;
		}
	}
	else
	{
		cout<<"(语法错误,行号:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

int isKeyword(string str)
{
	for(int j=0;j<13;j++)
	{
		if(str == keywords[j])
		{
			return 0;
		}
	}
	return 1;
}

//用于识别保留字或标识符 
void Iden(string Lex_str){
    /*-------------------识别保留字或标识符--------------------*/
    if(Lex_str=="begin")
    {
	    symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	} 
    else if(Lex_str=="call")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="const")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="do")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="end")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="if")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="procedure")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="read")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="then")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="while")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="var")
	{
		symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str=="write")
    {
    	symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
    else if(Lex_str.length()>8)
    {
     	cout<<"(词法错误）,行号:"<<count_lines<<")"<<endl;
	}
	else {
		symbols[record_idx].symbol_record=Lex_str;
	    symbols[record_idx].count_lines_num=count_lines;
		record_idx++;
	}
}

void Scanner()
{
	string input_str;
    char input_char;
    while((input_char=getchar())!=EOF){ 								//读入数据 
        input_str = input_str+input_char;
    }
   
    int length=input_str.length();
    //cout<<"语义分析结果如下："<<endl;                                   //开始处理读入的代码
    
    for(int i=0;i<length;i++){
        if(input_str[i]==' '||input_str[i]=='	')						//当遇到空格,tab时，跳过继续执行
            continue;
        else if(input_str[i]=='\n')										//遇到回车时，统计行数，跳过继续执行 
        {
        	count_lines++;
        	continue;
		}

    /*-------------------识别基本字/标识符--------------------*/
        else if(isalpha(input_str[i])){
            //字母
            string lett;
            // 只记录字母或者数字 
            while(isalpha(input_str[i])||isdigit(input_str[i])){
                lett += input_str[i];
                i++;
            }
            i--;  
			Iden(lett);         
        }
        
     /*-------------------识别常数-------------------*/
        else if(isdigit(input_str[i]))
		{											  //常数
            string digit;
            int flag=0;									//记录是否是数字开头的数字字母组合 
            while(isdigit(input_str[i])||isalpha(input_str[i])){
                digit +=input_str[i];
                if(isalpha(input_str[i]))
				{
					flag=1;
				} 
                i++;								//处理“2a”之类的词法错误 
            }
            i--;									//下标回调 
            if(flag==0)
            {
             	if((digit.length())>8)				//长度过长给出警告 
             	cout<<"(无符号整数越界,"<<digit<<",行号:"<<count_lines<<")"<<endl;
             	else {
				 	symbols[record_idx].symbol_record=digit;
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++; 
				 	} 
			}
             
            else if(flag==1)                      //违规输入的判断 
			{
             	cout<<"(非法字符(串),"<<digit<<",行号:"<<count_lines<<")"<<endl;
			}
			
        }
        
        else{
            switch(input_str[i]){
    /*-------------------识别运算符--------------------*/
            case '+':
                symbols[record_idx].symbol_record="+";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case '-':
                symbols[record_idx].symbol_record="-";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case '*':
                symbols[record_idx].symbol_record="*";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case '#':
                symbols[record_idx].symbol_record="#";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
				break;
            case '/':
                { i++;
                if(input_str[i]=='/')
                {
                	while(1)
                	{
                		i++;
                		if(input_str[i]=='\n')
                		{
                			count_lines++;
                			break;
						}
					}	
				}
				else if (input_str[i]=='*')
				{
					while(1)
					{
						if(input_str[i]=='\n')
						{
							count_lines++;
						}
						i++;
						if(input_str[i]=='*')
						{
							if(input_str[i]=='\n')
							{
								count_lines++;
							}
							i++;
							if(input_str[i]=='/')
							break;
						}
					}
				}
				else{
					i--;
					symbols[record_idx].symbol_record="/";
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++;
				}
				break;
				} 
            case '=':
                symbols[record_idx].symbol_record="=";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case '<':
                i++;
//                if(input_str[i]=='>'){
//                    symbols[record_idx].symbol_record="<>";
//	    			symbols[record_idx].count_lines_num=count_lines;
//					record_idx++;
//                }
                if(input_str[i]=='='){
                    symbols[record_idx].symbol_record="<=";
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++;
                }
                else {
                    i--;
                    symbols[record_idx].symbol_record="<";
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++;
                }
                break;
            case '>':
                i++;
                if(input_str[i]=='='){
                    symbols[record_idx].symbol_record=">=";
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++;
                }
                else{
                    i--;
                    symbols[record_idx].symbol_record=">";
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++;
                }
                break;
            case ':':
                i++;
                if(input_str[i]=='='){
                    symbols[record_idx].symbol_record=":=";
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++;
                }
                break;

     /*-------------------识别界符--------------------*/
            case '(':
                symbols[record_idx].symbol_record="(";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case ')':
                symbols[record_idx].symbol_record=")";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case ',':
                symbols[record_idx].symbol_record=",";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case ';':
                symbols[record_idx].symbol_record=";";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
            case '.':
                symbols[record_idx].symbol_record=".";
	    		symbols[record_idx].count_lines_num=count_lines;
				record_idx++;
                break;
   /*-------------------非法字符错误处理--------------------*/
            default:
                cout<<"(词法错误,行号:"<<count_lines<<")"<<endl;
                break;
            }
        }
    }
}


int main()
{
	for(int i=0;i<LEN;i++)//初始化 
	{
		Quad[i].type="_";
		Quad[i].op1="_";
		Quad[i].op2="_";
		Quad[i].result="_";
		table[i].size="0";
	}
	Quad[quad_idx].type="syss";
	quad_idx++;
	Scanner();
	P();
	if(grammar)
	{
		if(semantic)
		{
			cout<<"语义正确"<<endl;
			Quad[quad_idx].type="syse"; 
			cout<<"中间代码:"<<endl;
			for(int k=0;k<quad_idx+1;k++)
			{
				if(Quad[k].type=="j<=")
				{
					Quad[k].result="$"+Int_to_String(if_true);
				}
				else if(Quad[k].type=="j#")
				{
					Quad[k].result="$"+Int_to_String(while_true);
				}
				else if(Quad[k].type=="j=")
				{
					Quad[k].result="$"+Int_to_String(while_false);
				}
				cout<<"("<<k+1<<")("<<Quad[k].type<<","<<Quad[k].op1<<","<<Quad[k].op2<<","<<Quad[k].result<<")"<<endl;
			}
			cout<<"符号表:"<<endl;
			for(int i=0;i<tab;i++)
			{
				if(table[i].kind=="procedure")
				{
					cout<<""<<table[i].kind<<" "<<table[i].name<<"";
				}
				else cout<<""<<table[i].kind<<" "<<table[i].name<<" "<<table[i].size<<""<<endl;
			}
		}
	}
    return 0;
}
