#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define N 13
#define LEN 50

using namespace std;
int count_lines = 1; 						//��¼��ʼ�����������ӵ�һ�п�ʼ 
string keywords[N] = {"begin", "end", "if", "then", "else", "while", "write", "read", "do", "call",  "const", "var", "procedure"};

struct Lex_Symbol
{ 
	string symbol_record;		    //���汣���֣���ʶ����������� 
	int count_lines_num;			//�����к� 
};

struct Lex_Symbol symbols[200];		 

int grammar=1; // �﷨�Ƿ���ȷ 

struct Quad{	//��Ԫʽ 
	string type;
	string op1;
	string op2;
	string result;
}; 

struct Quad Quad[LEN];


struct table{	//���ű� 
	string kind;
	string name;
	string size;
}; 
struct table table[LEN];

int record_idx = 0;	// ���toekn���±� 
int read_idx = 0;   // ��ȡtoken���±� 
int quad_idx=0;     // ��Ԫʽ������ 
int tab=0;          // ���ű������ 

int semantic=1;		// ��Ϊ1���ʾ����������ȷ��Ϊ0��ʾ�������
int if_true;		// �����if�ж������ȷʱִ����������
int if_false;		// �����if�ж�������ʱִ����������
int while_true;		// �����while�ж������ȷʱִ����������
int while_false;	// �����while�ж������ȷʱִ����������


void Iden(string Lex_str);
void Scanner();

void P();
void P_plus();	
void I();	
void V();		
void I_plus();	
void D();			
void S();	
void stepAdd();		// ��ֹ��, d 
void unsInt();	// �޷�������, ��дΪn 
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
int isKeyword(string str);	// �ж����Ƿ��ǹؼ��� 
string Int_to_String(int n);

string Int_to_String(int n)
{
	ostringstream stream;
	stream<<n;
	return stream.str();
}

//����--><�ֳ���>.
// P -> P' . 
void P(){
	P_plus();
	if(symbols[read_idx].symbol_record==".")
	{
		read_idx++;
	}
	return;
}

// �ֳ���-->[<����˵������>][<����˵������>][<����˵������>]<���>  
/*
	 P' -> IVI'S
	����I,V,I'��3���ֶ�����ѡ��, �ɳ��ֿɲ����� 
*/
void P_plus(){
	//����˵������
	if(symbols[read_idx].symbol_record=="const")
	{
		I();
	}
	//����˵������
 	if (symbols[read_idx].symbol_record == "var")
	{
		V();
	}
	//����˵������
	if (symbols[read_idx].symbol_record== "procedure")
	{
		I_plus();
	}
	S();//���
}

// ����˵������-->const<��������>{,<��������>}
// I -> const DA'| ��
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
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}	
}

// ����˵������ -->var<��ʶ��>{,<��ʶ��>}
// V -> vbB' | ��
void V()
{
	// �жϱ����Ƿ�Ϸ���flag��Ϊ1������������
	// ��������ѱ�����ı�������flag1��Ϊ1
	// ֻ����һ��������ʱ��ֻ�õ�flag1 
	int flag1=0;
	// ��һ���������������ϵı�������ʹ�õ�flag2
	//  
	int flag2=0;
	if (symbols[read_idx].symbol_record == "var")
	{
		table[tab].kind="var";
		Quad[quad_idx].type="var"; 
		read_idx++;
		stepAdd();
		for(int i=0;i<tab;i++)//�жϸñ�����֮ǰ�Ƿ񱻶���� 
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
			cout<<"(�������,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
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
			for(int i=0;i<tab;i++)//�жϸñ�����֮ǰ�Ƿ񱻶���� 
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
				cout<<"(�������,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
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
			cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
			read_idx=read_idx+2;
		}
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

//����˵������--><�����ײ�><�ֳ���>{;<����˵������>};
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
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
		
}

//�������� --> <��ʶ��>=<�޷�������>
/*
	D -> b=n | �� 
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
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
		read_idx=read_idx+2;
	}
		
}

// ��ʶ��
void stepAdd()
{
	read_idx++;
}


void unsInt()//�޷�������
{
	if ((symbols[read_idx].symbol_record.at(0)>='0')&&(symbols[read_idx].symbol_record.at(0)<='9'))
	{
		read_idx++;
		return;
	}		
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

//�����ײ�-->procedure<��ʶ��>;
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
			cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
			read_idx=read_idx+2;
		}
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

//��� --> <��ֵ���>|<�������>|<����ѭ�����>|<���̵������>|<�����>|<д���>|<�������>|<�����>
void S() 
{
	if(isKeyword(symbols[read_idx].symbol_record))
		S_plus(); 
	if (symbols[read_idx].symbol_record== "if")//�������
		C();
	else if (symbols[read_idx].symbol_record == "while")//Wѭ��
		W();
	else if (symbols[read_idx].symbol_record == "call")//���̵���
		V_plus();
	else if (symbols[read_idx].symbol_record == "read")//�����
		H();
	else if (symbols[read_idx].symbol_record == "write")//д���
		D_plus();
	else if (symbols[read_idx].symbol_record == "begin")//�������
		F(); 
}

//��ֵ���--><��ʶ��>:=<���ʽ>
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
			cout<<"(�﷨����,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			grammar=0;
		}
	}
	else {
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
		read_idx=read_idx+4;
	}
}

//���ʽ-->[+|-]<��>{<�Ӽ�����><��>}
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

void F()//�������-->begin<���>{;<���>}end 
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
			cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
		}
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

//���� --> <���ʽ><��ϵ�����><���ʽ>|odd<���ʽ>
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

//�� --> <����>{<�˳������><����>}
void T()
{
	// ��flagΪ1������ñ���δ�ڷ��ű��У���δ���� 
	int flag1=1;
	int flag2=1;
	T_plus();
	
	for(int i=0;i<tab;i++)//�жϸñ�����֮ǰ�Ƿ񱻶������û������򱨴�  
	{
		if (symbols[read_idx-1].symbol_record.at(0) <= '9' && symbols[read_idx-1].symbol_record.at(0) >= '0')//����ǳ����������ж� 
		{
			flag1=0;
			break; 
		} 
		// ���ҷ��ű���kindΪconst��var�ķ���, ���������, ��flag1Ϊ0������falg1Ϊ1, ������ʹ��δ�����ĳ�������� 
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
		cout<<"(�������,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
		semantic=0;
	}
	while(symbols[read_idx].symbol_record=="*"|| symbols[read_idx].symbol_record == "/")
	{
		Quad[quad_idx].op1=symbols[read_idx-1].symbol_record;
		M();
		Quad[quad_idx].type=symbols[read_idx-1].symbol_record;
		T_plus();
		
		for(int i=0;i<tab;i++)//�жϸñ�����֮ǰ�Ƿ񱻶����,û������򱨴� 
		{
			if ((symbols[read_idx-1].symbol_record.at(0) <= '9' && symbols[read_idx-1].symbol_record.at(0) >= '0'))   //����ǳ����������ж� 
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
			cout<<"(�������,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			semantic=0;
		}
		
		Quad[quad_idx].op2=symbols[read_idx-1].symbol_record;
		Quad[quad_idx].result="T1";
		quad_idx++;
	}
}

//����--><��ʶ��>|<�޷�������>
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
			cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0; 
		}
 	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

void L()//�Ӽ������-->+|-
{
	if (symbols[read_idx].symbol_record == "+" || symbols[read_idx].symbol_record == "-")
	{
		read_idx++;
		return;
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

void M()//�˳������-->*|/
{
	if (symbols[read_idx].symbol_record== "*" ||symbols[read_idx].symbol_record== "/")
	{
		read_idx++;
		return;
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0;
	}
}

//��ϵ����� -> =|#|<|<=|>|>=
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
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

void C()//�������-->if<����>then<���> 
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
			cout<<"(�﷨����,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			grammar=0;
			S();
		}
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
		
}

//���̵������-->call<��ʶ��>
void V_plus() 
{
	int flag=1;	//�ж�call()�Ƿ��д� 
	if (symbols[read_idx].symbol_record=="call")
	{
		Quad[quad_idx].type="call";
		read_idx++;
		stepAdd();
		for(int i=0;i<tab;i++)	//�ж�call��ı����Ƿ���֮ǰ������ 
			{
				// ����ű� 
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
			cout<<"(�������,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			semantic=0;
		}
		quad_idx++;
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

void W()//����ѭ��-->while<����>do<���>
{
	if (symbols[read_idx].symbol_record == "while")
	{
		read_idx++;
		C_plus();
		if (symbols[read_idx].symbol_record == "do")
		{
			while_true=quad_idx+1; //��Ϊ�м�����������0��ʼ���Ǵ�1��ʼ 
			read_idx++;
			S();
			while_false=quad_idx+1;//��Ϊ�м�����������0��ʼ���Ǵ�1��ʼ
		}
		else 
		{
			cout<<"(�﷨����,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
			grammar=0;
			S();
		}
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
	
}

void H()//�����-->read ( <��ʶ��>{,<��ʶ��>} ) 
{
	// �ж�read()���������еĲ����Ƿ��д� 
	int flag=1;
	if (symbols[read_idx].symbol_record== "read")
	{
		Quad[quad_idx].type="read";
		read_idx++;
		if (symbols[read_idx].symbol_record== "(")
		{
			read_idx++;
			stepAdd();
			// �ж϶��ı����Ƿ���֮ǰ����
			// ���δ����flagΪ1, ��ʹ��δ�����ı��� 
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
				cout<<"(�������,�к�:"<<symbols[read_idx-1].count_lines_num<<")"<<endl;
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
				cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
				grammar=0;
			}	
		}
		else
		{
			cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0; 
		}
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
		grammar=0; 
	}
}

// д���-->write ( <���ʽ>{,<���ʽ>} )
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
			cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
			grammar=0;
			read_idx=read_idx+4;
		}
	}
	else
	{
		cout<<"(�﷨����,�к�:"<<symbols[read_idx].count_lines_num<<")"<<endl;
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

//����ʶ�����ֻ��ʶ�� 
void Iden(string Lex_str){
    /*-------------------ʶ�����ֻ��ʶ��--------------------*/
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
     	cout<<"(�ʷ�����,�к�:"<<count_lines<<")"<<endl;
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
    while((input_char=getchar())!=EOF){ 								//�������� 
        input_str = input_str+input_char;
    }
   
    int length=input_str.length();
    //cout<<"�������������£�"<<endl;                                   //��ʼ�������Ĵ���
    
    for(int i=0;i<length;i++){
        if(input_str[i]==' '||input_str[i]=='	')						//�������ո�,tabʱ����������ִ��
            continue;
        else if(input_str[i]=='\n')										//�����س�ʱ��ͳ����������������ִ�� 
        {
        	count_lines++;
        	continue;
		}

    /*-------------------ʶ�������/��ʶ��--------------------*/
        else if(isalpha(input_str[i])){
            //��ĸ
            string lett;
            // ֻ��¼��ĸ�������� 
            while(isalpha(input_str[i])||isdigit(input_str[i])){
                lett += input_str[i];
                i++;
            }
            i--;  
			Iden(lett);         
        }
        
     /*-------------------ʶ����-------------------*/
        else if(isdigit(input_str[i]))
		{											  //����
            string digit;
            int flag=0;									//��¼�Ƿ������ֿ�ͷ��������ĸ��� 
            while(isdigit(input_str[i])||isalpha(input_str[i])){
                digit +=input_str[i];
                if(isalpha(input_str[i]))
				{
					flag=1;
				} 
                i++;								//����2a��֮��Ĵʷ����� 
            }
            i--;									//�±�ص� 
            if(flag==0)
            {
             	if((digit.length())>8)				//���ȹ����������� 
             	cout<<"(�޷�������Խ��,"<<digit<<",�к�:"<<count_lines<<")"<<endl;
             	else {
				 	symbols[record_idx].symbol_record=digit;
	    			symbols[record_idx].count_lines_num=count_lines;
					record_idx++; 
				 	} 
			}
             
            else if(flag==1)                      //Υ��������ж� 
			{
             	cout<<"(�Ƿ��ַ�(��),"<<digit<<",�к�:"<<count_lines<<")"<<endl;
			}
			
        }
        
        else{
            switch(input_str[i]){
    /*-------------------ʶ�������--------------------*/
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

     /*-------------------ʶ����--------------------*/
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
   /*-------------------�Ƿ��ַ�������--------------------*/
            default:
                cout<<"(�ʷ�����,�к�:"<<count_lines<<")"<<endl;
                break;
            }
        }
    }
}


int main()
{
	for(int i=0;i<LEN;i++)//��ʼ�� 
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
			cout<<"������ȷ"<<endl;
			Quad[quad_idx].type="syse"; 
			cout<<"�м����:"<<endl;
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
			cout<<"���ű�:"<<endl;
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
