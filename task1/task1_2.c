#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TEXT_LENGTH 999
#define MAX_WORD_LENGTH 64
// �������������Ϊ64 

void tokenizeText(char *text){
	char currentWord[MAX_WORD_LENGTH] = "";
	int i = 0;
	
	while (*text) {
		if (isalpha(*text)) { // ��ĸ�ַ�
            currentWord[i++] = *text;
        } else if (isdigit(*text)) { // �����ַ�
            currentWord[i++] = *text;
        } else if (*text != ' ' && *text != '\n') { // �����ַ�
            printf("����: %c\n", *text);
        } else if (i > 0) { // ��ǰ���ʽ���
        	if (isalpha(currentWord)){ // ���� 
        		currentWord[i] = '\0';
            	printf("����: %s\n", currentWord);
            	i = 0;
			}else{ // ���� 
				currentWord[i] = '\0';
            	printf("����: %s\n", currentWord);
            	i = 0;
			}
            
        }
        ++text;
	}
	if (i > 0) { // �������һ������
        currentWord[i] = '\0';
        printf("����: %s\n", currentWord);
    }
}

int main (){
	char *pText;
	char text[MAX_TEXT_LENGTH];
	fgets(text, sizeof(text), stdin);
	pText = text;
//	char *pText = "Hello 123 World! @";
    tokenizeText(pText);
    return 0;
} 
