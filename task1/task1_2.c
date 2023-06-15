#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TEXT_LENGTH 999
#define MAX_WORD_LENGTH 64
// 单个单词最长限制为64 

void tokenizeText(char *text){
	char currentWord[MAX_WORD_LENGTH] = "";
	int i = 0;
	
	while (*text) {
		if (isalpha(*text)) { // 字母字符
            currentWord[i++] = *text;
        } else if (isdigit(*text)) { // 数字字符
            currentWord[i++] = *text;
        } else if (*text != ' ' && *text != '\n') { // 符号字符
            printf("符号: %c\n", *text);
        } else if (i > 0) { // 当前单词结束
        	if (isalpha(currentWord)){ // 单词 
        		currentWord[i] = '\0';
            	printf("单词: %s\n", currentWord);
            	i = 0;
			}else{ // 数字 
				currentWord[i] = '\0';
            	printf("数字: %s\n", currentWord);
            	i = 0;
			}
            
        }
        ++text;
	}
	if (i > 0) { // 处理最后一个单词
        currentWord[i] = '\0';
        printf("单词: %s\n", currentWord);
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
