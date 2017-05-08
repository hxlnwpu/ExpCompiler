#include<fstream>
#include <cctype>
#include<cstring>
#include<stack>
#include<stdlib.h>
#include <iostream>
#include<queue>
#include<vector>
#include<ctime>

#define MAXCHILDREN 2

using namespace std;

typedef enum
{
    ID,NUM,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,END
} TokenType;

typedef struct scanToken
{
    TokenType op;
    char stringValue[256];
} TokenNode;


typedef struct treeNode
{
    struct treeNode * child[MAXCHILDREN];
    TokenNode attr;
} TreeNode;



TokenNode token;   //����token�����ڹ����﷨������expr��Ԫ��
TokenNode expr[256]; //��һ���ṹ��������ʶ�����token,����<attr,value>
int  flag=0;    //����ָʾexpr�±�
stack<TokenNode> OPND;  //��Ų�������������
vector<TokenNode> rpn;   //��ŵõ����沨��ʽ���Ա��������
TokenNode Vtemp[50]={ };

void scan(TokenNode* tokenlist,char* filename);    //ɨ����token
void fscan(TokenNode* expression,char* filename);   ////ɨ���ļ����token,֧�ֶ��ַ���ʶ��������
void printToken(TokenNode* expression);  //�����ʶ���token
TreeNode * exp();   //exp->term+term||term-term
TreeNode * term();  //term->factor*factor||term->factor/factor
TreeNode * factor();//factor->NUM||ID||(exp)
TreeNode * newNode(); //�����﷨���Ĺ����з������ڵ�
void TreePrint(TreeNode * T,int level);  //�������δ�ӡ
void postOrderTraverse(TreeNode *T); //��������﷨��
void printFef(vector<TokenNode> fexp); //�����Ԫʽ�����ָ��
char *rand_str(char *str); //����ַ�������




int main()
{
    TreeNode* t;
    fscan(expr,"exp1");
    printToken(expr);
    token=expr[flag];
    char name[3];
    for(int p=0;p<10;p++)  //��ʼ������м����Ľṹ������
    {
        Vtemp[p].op=ID;
        strcpy(Vtemp[p].stringValue,rand_str(name));
    }
    while(token.op!=END)
        t=exp();
    cout<<endl;
    cout<<"*********************************************"<<endl;
    cout<<"�ɹ������﷨����******************************"<<endl;
    cout<<"��ǰ���ṹ����********************************"<<endl;
    TreePrint(t,0);
    cout<<endl;
    cout<<"�����沨��ʽ����*******************************"<<endl;
    postOrderTraverse(t);
    cout<<endl;
    cout<<"������Ԫʽ����*******************************"<<endl;
    //initVtemp(Ltemp);
    printFef(rpn);
    return 0;

}

void fscan(TokenNode* expression,char* filename)
{
    fstream source;
    int m=0;
    source.open(filename);
    if(source.is_open())
    {
        while(!source.eof())
        {
            char ch;
            source.get(ch);
            if(isalpha(ch)) //ʶ���ʶ��
            {
                char Iden[256];
                int i=0;
                Iden[i]=ch;
                source.get(ch);
                expr[m].op=ID;
                while((!source.eof())&&isalnum(ch))
                {
                    Iden[++i]=ch;
                    source.get(ch);
                }
                if(!isalnum(ch)) source.unget(); //�ַ�����
                strncpy(expr[m].stringValue,Iden,i+1);
                m++;
            }
            else if(isdigit(ch))   //ʶ�����֣�δ�ж��쳣�����������Ϊ0
            {
                char number[256];
                int i=0;
                number[i]=ch;
                source.get(ch);
                expr[m].op=NUM;
                while((!source.eof())&&isdigit(ch))
                {
                    number[++i]=ch;
                    source.get(ch);
                }

                strncpy(expr[m].stringValue,number,i+1);
                if(!isdigit(ch)) source.unget(); //�ַ�����
                m++;
            }
            else if(ch=='+')
            {
                expr[m].op=PLUS;
                    strncpy(expr[m++].stringValue,&ch,1);
            }
            else if(ch=='-')
            {
                expr[m].op=MINUS;
                    strncpy(expr[m++].stringValue,&ch,1);
            }
            else if(ch=='*')
            {
                expr[m].op=TIMES;
                    strncpy(expr[m++].stringValue,&ch,1);
            }
            else if(ch=='/')
            {
                expr[m].op=OVER;
                    strncpy(expr[m++].stringValue,&ch,1);
            }
            else if(ch=='(')
            {
                expr[m].op=LPAREN;
                    strncpy(expr[m++].stringValue,&ch,1);
            }
            else if(ch==')')
            {
                expr[m].op=RPAREN;
                    strncpy(expr[m++].stringValue,&ch,1);
            }

            else   //�޷�ʶ�����ʱ�򱨴�
            {
                cout<<ch<<"�����޷�ʶ��,����Դ�ļ���"<<endl;
            }

        }
    }
    expr[m].op=END;   //ĩβ���ϱ�־λ����Ϊ�����ݹ���ֹ����
}

void printToken(TokenNode* expression)
{
    int n=0;
    cout<<"����token���£�"<<endl;
    cout<<"********************************************"<<endl;
    while(expression[n].op!=END)
    {
        if(expression[n].op==ID)
            cout<<"<"<<"ID "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==NUM)
            cout<<"<"<<"NUM "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==PLUS)
            cout<<"<"<<"PLUS "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==MINUS)
            cout<<"<"<<"MINUS "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==OVER)
            cout<<"<"<<"OVER "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==TIMES)
            cout<<"<"<<"TIMES "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==LPAREN)
            cout<<"<"<<"LPAREN "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==RPAREN)
            cout<<"<"<<"RPAREN "<<expression[n].stringValue<<">"<<endl;
        n++;
    }
}

void scan(TokenNode* expression,char* filename)  //ɨ����token
{
    static int i=0,j=0,m=0;
    static char str[256];   //���ڴ�Ŷ�������ÿ���ַ�
    fstream source;
    source.open(filename);
    if(source.is_open())
    {
        cout<<"���ʽΪ��";
        while(!source.eof())
        {
            source.get(str[i]);
            cout<<str[i];
            i++;
        }
        cout<<endl;
    }
    for(j=0; j<i-1;) //����token
    {

        if(isdigit(str[j]))
        {
            expression[m].op=NUM;
            expression[m].stringValue[0]=str[j];
            j++,m++;
        }
        else if(str[j]=='-')
        {
            expression[m].op=MINUS;
            expression[m].stringValue[0]='-';
            j++,m++;
        }

        else if(str[j]=='+')
        {
            expression[m].op=PLUS;
            expression[m].stringValue[0]='+';
            j++,m++;
        }

        else if(str[j]=='*')
        {
            expression[m].op=TIMES;
            expression[m].stringValue[0]='*';
            j++,m++;
        }

        else if(str[j]=='/')
        {
            expression[m].op=OVER;
            expression[m].stringValue[0]='/';
            j++,m++;
        }

        else if(str[j]=='(')
        {
            expression[m].op=LPAREN;
            expression[m].stringValue[0]='(';
            j++,m++;
        }

        else if(str[j]==')')
        {
            expression[m].op=RPAREN;
            expression[m].stringValue[0]=')';
            j++,m++;
        }
        else if(isalpha(str[j]))
        {
            expression[m].op=ID;
            expression[m].stringValue[0]=str[j];
            j++;
            m++;
        }
    }
    cout<<m<<endl;
    expression[m].op=END;
    for(int n=0; n<=m; n++)
    {

        if(expression[n].op==ID)
            cout<<"<"<<"ID "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==NUM)
            cout<<"<"<<"NUM "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==PLUS)
            cout<<"<"<<"PLUS "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==MINUS)
            cout<<"<"<<"MINUS "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==OVER)
            cout<<"<"<<"OVER "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==TIMES)
            cout<<"<"<<"TIMES "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==LPAREN)
            cout<<"<"<<"LPAREN "<<expression[n].stringValue<<">"<<endl;
        else if(expression[n].op==RPAREN)
            cout<<"<"<<"RPAREN "<<expression[n].stringValue<<">"<<endl;

    }

}


TreeNode * exp()
{
    TreeNode * t = term();
    while ((token.op==PLUS)||(token.op==MINUS))    //�Ӽ�
    {
        TreeNode * p = newNode();
        if (p!=NULL)
        {
            p->child[0] = t;
            p->attr.op = token.op;
            strcpy(p->attr.stringValue,token.stringValue);
            t = p;
            token=expr[++flag];
            t->child[1] = term();
        }
    }
    return t;
}

TreeNode * term()
{
    TreeNode * t = factor();
    while ((token.op==TIMES)||(token.op==OVER))   //�˳�
    {
        TreeNode * p = newNode();
        if (p!=NULL)
        {
            p->child[0] = t;
            p->attr.op = token.op;
            strcpy(p->attr.stringValue,token.stringValue);
            t = p;
            token=expr[++flag];
            p->child[1] = factor();
        }
    }
    return t;
}

TreeNode * factor()
{
    TreeNode * t = NULL;
    switch (token.op)
    {
    case NUM :      //����
        t = newNode();
        if ((t!=NULL) && (token.op==NUM))
        {
            t->attr.op = token.op;
            strcpy(t->attr.stringValue,token.stringValue);
        }
        token=expr[++flag];
        break;
    case ID :      //��ʶ��
        t = newNode();
        if ((t!=NULL) && (token.op==ID))
        {
            t->attr.op = token.op;
            strcpy(t->attr.stringValue,token.stringValue);
        }
        token=expr[++flag];
        break;
    case LPAREN :    //������
        token=expr[++flag];
        t = exp();
        token=expr[++flag];   //������
        break;
    default:
        cout<<"unexpected token"<<endl;
        token=expr[++flag];
        break;
    }
    return t;
}

TreeNode * newNode()
{
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i=0;
    if (t==NULL)
        cout<<"Out of memory error\n"<<endl;
    else
    {
        for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;
    }
    return t;
}

void TreePrint(TreeNode * T,int level)
{
    if (!T)             //���ָ��Ϊ�գ�������һ��
    {
        return;
    }
    TreePrint(T->child[1],level+1);    //��ӡ��������������μ�1
    for (int i=0; i<level; i++)  //���յݹ�Ĳ�δ�ӡ�ո�
    {
        printf("   ");
    }
    printf("%s\n",T->attr.stringValue);  //��������
    TreePrint(T->child[0],level+1);    //��ӡ��������������μ�1
}

void postOrderTraverse(TreeNode *T)   //�ݹ�ʵ�ֺ�������﷨��
{
        if(!T) return;
        postOrderTraverse(T->child[0]);
        postOrderTraverse(T->child[1]);
        cout<<T->attr.stringValue<<" ";
        rpn.push_back(T->attr);
}

char *rand_str(char *str)
{
    int i;
    for(i=0;i<2;++i)
        str[i]='A'+rand()%26;
    str[++i]='\0';
    return str;
}

void printFef(vector<TokenNode> fexp)
{
    int m=0,n=0;
    TokenNode temp,optemp;
    char sAsm[4][50];
    while(m<fexp.size())
    {
        if(fexp.at(m).op>=2)    //�����Ԫʽ<op s1,s2,dest>
        {
            //optemp=fexp.at(m);
            cout<<fexp.at(m).stringValue<<" ";
            strcpy(sAsm[0],fexp.at(m).stringValue);
            temp=OPND.top();
            OPND.pop();
            cout<<OPND.top().stringValue<<",";
            strcpy(sAsm[1],OPND.top().stringValue);
            cout<<temp.stringValue<<",";
            strcpy(sAsm[2],temp.stringValue);
            cout<<Vtemp[n].stringValue<<endl;
            strcpy(sAsm[3],Vtemp[n].stringValue);
            OPND.pop();
            OPND.push(Vtemp[n]);
            cout<<"���ָ������:"<<endl;
            if(fexp.at(m).op==PLUS)
            {
                cout<<"      "<<"mov "<<"ax "<<sAsm[1]<<endl;
                cout<<"      "<<"add "<<"ax "<<sAsm[2]<<endl;
                cout<<"      "<<"mov "<<sAsm[3]<<" "<<"ax"<<endl;
            }
            else if(fexp.at(m).op==MINUS)
            {
                cout<<"      "<<"mov "<<"ax "<<sAsm[1]<<endl;
                cout<<"      "<<"sub "<<"ax "<<sAsm[2]<<endl;
                cout<<"      "<<"mov "<<sAsm[3]<<" "<<"ax"<<endl;
            }
            else if(fexp.at(m).op==TIMES)
            {
                cout<<"      "<<"mov "<<"ax "<<sAsm[1]<<endl;
                cout<<"      "<<"mov "<<"bx "<<sAsm[2]<<endl;
                cout<<"      "<<"mul "<<"bx"<<endl;
                cout<<"      "<<"mov "<<sAsm[3]<<" "<<"ax"<<endl;

            }
            else if(fexp.at(m).op==OVER)
            {
                cout<<"      "<<"mov "<<"ax "<<sAsm[1]<<endl;
                cout<<"      "<<"mov "<<"bx "<<sAsm[2]<<endl;
                cout<<"      "<<"div "<<"bx"<<endl;
                cout<<"      "<<"mov "<<sAsm[3]<<" "<<"ax"<<endl;
            }

            m++,n++;
        }
        else
        {
            OPND.push(fexp.at(m++));
        }

    }
}
