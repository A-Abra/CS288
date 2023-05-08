#include<stdio.h>
#include<stdlib.h>

void binary(unsigned int n, int binnum[]){
	for(int i =0; i < 32; i++){
		binnum[31-i] = n%2;
		n=n/2;
	}
}

unsigned int invertBits(unsigned int n){
	unsigned int revnum;
	for(int i=0; i < 32; i++){
		revnum <<= 1;
		revnum = revnum | (n & 1);
		n >>= 1;
	}
	return revnum;
}

int diglen(unsigned int n){
	int count = 0;
	while( n != 0){
		count++;
		n /= 10;
	}
	return count;
}

void print(unsigned int n, int bits[]){
	double spacer = 15 - diglen(n);
	binary(n,bits);

	printf("%u:",n);
	for(int i=0; i<spacer; i++){
		printf(" ");
	}
	for(int i=0;i<32;i++){
		if(i%4==0)
			printf(" ");
		printf("%d",bits[i]);
	}
	printf("\n");
}

int main(int argc, char **argv){
	if(argc==2){
		char * num = argv[1];
		unsigned int uint = strtol(argv[1], &num, 10);
		int bits[32];
		unsigned int revuint = invertBits(uint);
		
		print(uint,bits);
		print(revuint,bits);
	}
	else
		printf("Single argument needed\n");
	return 0;
}

