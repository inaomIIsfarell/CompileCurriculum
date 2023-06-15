#include <stdio.h>
#include <math.h>
#include <string.h>

int main(){

    char dna[100];
    int length;
	int total = 0;
	float res;
    scanf("%s", dna);
    length = strlen(dna);
    int i;
	for (i = 0; i < length; i++){
		if (dna[i] == 'G' || dna[i] == 'C'){
			total++;
		}
	}
	res = (float)total / (float)length;
	printf("%d\n", total);
	printf("%d\n", length);
	printf("%f\n", res);
    return 0;
}
