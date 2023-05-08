#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	int opt;
	while((opt = getopt(argc,argv,":cduf:s:"))!=-1){
	  switch(opt){
	  	case 'c':
	  	case 'd':
  		case 'u':
			printf("option: %c\n", opt); break;
		case 'f':
			printf("option %c with fields ignored: %s\n", opt, optarg); break;
		case 's':
			printf("option %c with chars ignored: %s\n", opt, optarg); break;
		case ':':
			printf("option %c needs a value\n", optopt); break;
		case '?':
			printf("unknown option: %c\n", optopt); break;
	  }
	}
	
	int count = 0;
	int remArgs = argc - optind;
	for(; optind < argc; optind++){
		if (remArgs < 2 || count >= 2)
			printf("other argument: %s\n", argv[optind]);
		else if(remArgs >= 2 && count < 2){
			if(count==0){
				printf("Input file: %s\n", argv[optind]);
				count++;
			}
			else if(count==1){
				printf("Output file: %s\n", argv[optind]);
				count++;
			}
		}
	}
	exit(0);
}
