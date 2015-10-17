/*---------1. Pre-processor Directives---------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

/*---------2. Function Prototypes--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void fchk(FILE*,FILE*,char[],char[],int*);
void fwrt(FILE*,FILE*,char[],char[]);
int fcompile(FILE*,FILE*,FILE*,FILE*,FILE*,FILE*,FILE*,char[],char[],int*);
void flblcompile(FILE*,FILE*,char[],char[],int*);
void regnerr(FILE*,FILE*,char[],int,int*);
void dec2bin(FILE*,int);
void fprt(FILE*);

/*---------3. Main Function (goes as 2nd PASS)-------------------------------------------------------------------------------------------------------------------------------------------------------*/

void main(void) {
	FILE *finp,*fout,*fopc,*freg,*fhex,*flbl,*fhlp;
	char inp[17],op[17];
	int stat,icount;
	finp = fopen("Input.txt", "r");
	fout = fopen("Output.txt", "w");
	fopc = fopen("ISAopcode.txt", "r");
	freg = fopen("ISAreg.txt", "r");
	fhex = fopen("hex2bin.txt", "r");
	flbl = fopen("ISAlabel.txt", "w+");
	fhlp = fopen("ISAerrhelp.txt", "w+");
	if((!finp)||(!fout)||(!fopc)||(!freg)||(!fhex)||(!flbl)||(!fhlp)) return;
	icount=fcompile(finp,fout,fopc,freg,fhex,flbl,fhlp,inp,op,&stat);
	if(stat) flblcompile(finp,flbl,inp,op,&stat);
	if(stat) {
		while(!feof(finp)) {
			fscanf(finp,"%s",inp);
			fwrt(fopc,fout,inp,op);
			fwrt(freg,fout,inp,op);
			fwrt(fhex,fout,inp,op);
			fwrt(flbl,fout,inp,op);
		}
		printf("\n%d Instructions processed.\n",icount);
		printf("\nTask Successfully done!\n");
	}
	fclose(finp);
	fclose(fout);
	fclose(fopc);
	fclose(freg);
	fclose(fhex);
	fclose(finp);
	fclose(fhlp);
	return;
}

/*---------4. Checks for undeclared labels after scanning complote input file------------------------------------------------------------------------------------------------------------------------*/

void flblcompile(FILE *finp,FILE *flbl,char inp[], char op[], int* z) {
	int a,b;
	while(!feof(finp)) {
		fscanf(finp,"%s",inp);
		if((strcasecmp(inp,"JMP")==0)||(strcasecmp(inp,"JC")==0)||(strcasecmp(inp,"JNC")==0)||(strcasecmp(inp,"JZ")==0)||(strcasecmp(inp,"JNZ")==0)||(strcasecmp(inp,"JP")==0)||(strcasecmp(inp,"JM")==0)||(strcasecmp(inp,"JPE")==0)||(strcasecmp(inp,"JPO")==0)) {
			fchk(finp,flbl,inp,op,&b);
			if(b==0) {
				printf("\n%s Label not found!\n\nAvailable declared labels are:-\n",inp);
				z=0;
				fprt(flbl);
			}
		}
	}
	rewind(finp);
}

/*---------5. Checks for all types of errors in Input file. (Ist PASS)-------------------------------------------------------------------------------------------------------------------------------*/

int fcompile(FILE *finp,FILE *fout,FILE *fopc,FILE *freg,FILE *fhex,FILE *flbl,FILE *fhlp,char inp[17],char op[17],int* z) {
	int a,b,c,d,e,f,g,i=0,icnt=0;
	*z=1;
	char inpp[17];
	rewind(finp);
	while(!feof(finp)) {
		rewind(fopc);
		fscanf(finp,"%s",inp);
		fseek(fhlp,0,SEEK_SET);
		while(!feof(fopc)) {
			fscanf(fopc,"%s",op);
			if(strcasecmp(inp,op)==0) {
				fscanf(fopc,"%s",op);
				a=1; break;
			} else {
				i=0;
				while(i<strlen(inp)) {
					inp[i]=toupper(inp[i]);
					i++;
				}
				if ((strstr(inp,op)!=0)||(strstr(op,inp)!=0)) {
					fprintf(fhlp, "\n%s %s",op,inp);
				}
				fscanf(fopc,"%s",op);
				a=0;
			}
		}
		if(icnt==0 && (strcasecmp(inp,"start")!=0)) {
			fprintf(fout, "%s", "\n1111111111101111");
			printf("\nLINE 0: 'START' instruction not found at beginning of input\n");
			icnt++;
		}
		if(icnt==1 && (strcasecmp(inp,"rst")!=0)) {
			fprintf(fout, "%s", "\n1111111111101110");
			printf("\nLINE 0: 'RST' instruction not found at beginning of input following START\n");
			icnt++;
		}
		if(a==0) {
			if(inp[strlen(inp)-1]==':') {
				inp[strlen(inp)-1]='\0';
				rewind(flbl);
				while(!feof(flbl)) {
					fscanf(flbl,"%s",op);
					if(strcasecmp(inp,op)==0) {
						fscanf(flbl,"%s",op);
						d=1; break;
					} else {
						fscanf(flbl,"%s",op);
						d=0;
					}
				}
				if(d==1) {
					printf("\nLine %d: Label '%s' already declared earlier!\n",icnt+1,inp);
					*z=0;
				}
				else {
					fprintf(flbl,"\n%s ",inp);
					dec2bin(flbl, icnt+1);
				}
				continue;
			}
			strcpy(inpp,inp);
			fscanf(finp,"%s",inp);
			if(strcasecmp(inp,":")==0) {
				printf("\nLine %d: Incorrect syntax for label declaration. Space between 'label_Name' and ':' not allowed.",icnt+1);
				*z=0;
			}
			else {
				fseek(finp,-strlen(inp)-1,SEEK_CUR);
				strcpy(inp,inpp);
				if(inp[1]>47&&inp[1]<58) printf("\nLine %d: INVALID Instruction\n",icnt);
				else {
					printf("\nLine %d: '%s' Instruction not found!\n\nAvailable options are:-\n",icnt+1,inp);
					fprt(fhlp);
					//-------------------------------------------------------
				}
				*z=0; return 0;
			}
		}
		else {
			icnt++;
			if((strcasecmp(inp,"JMP")==0)||(strcasecmp(inp,"JC")==0)||(strcasecmp(inp,"JNC")==0)||(strcasecmp(inp,"JZ")==0)||(strcasecmp(inp,"JNZ")==0)||(strcasecmp(inp,"JP")==0)||(strcasecmp(inp,"JM")==0)||(strcasecmp(inp,"JPE")==0)||(strcasecmp(inp,"JPO")==0)) {
				fscanf(finp,"%s",inp);
			}
			else if(strlen(op)==4){
				fchk(finp,freg,inp,op,&b);
				if(b==0) regnerr(finp,freg,inp,icnt,&g);
				if(g==0) {
					printf("\nLINE %d: INVALID Instruction\n",icnt);
					*z=0; return 0;
				}
				fchk(finp,freg,inp,op,&e);
				if(e==0) regnerr(finp,freg,inp,icnt,&g);
				if(g==0) {
					printf("\nLINE %d: INVALID Instruction\n",icnt);
					*z=0; return 0;
				}
				fchk(finp,freg,inp,op,&f);
				if(f==0) regnerr(finp,freg,inp,icnt,&g);
				if(g==0) {
					printf("\nLINE %d: INVALID Instruction\n",icnt);
					*z=0; return 0;
				}
				if(b==0||e==0||f==0) *z=0;
			}
			else if(strlen(op)==8){
				fchk(finp,freg,inp,op,&b);
				if(b==0) regnerr(finp,freg,inp,icnt,&g);
				if(g==0) {
					printf("\nLINE %d: INVALID Instruction\n",icnt);
					*z=0; return 0;
				}
				fchk(finp,freg,inp,op,&e);
				if(e==0) regnerr(finp,freg,inp,icnt,&g);
				if(g==0) {
					printf("\nLINE %d: INVALID Instruction\n",icnt);
					*z=0; return 0;
				}
				if(b==0||e==0) *z=0;
			}
			else if(strlen(op)==12){
				char ins[17];
				strcpy(ins,inp);
				fchk(finp,freg,inp,op,&b);
				if(b==0) {
					regnerr(finp,freg,inp,icnt,&g);
					if(g==0) {
						printf("\nLINE %d: INVALID Instruction\n",icnt);
						*z=0; return 0;
					}
					*z=0;
				}
				if((ins[strlen(ins)-1]=='i')||(ins[strlen(ins)-1]=='I')){
					fchk(finp,fhex,inp,op,&c);
					strcpy(inpp,inp);
					if(c==1) icnt++;
					else {
						fchk(finp,fhex,inp,op,&g);
						if(g==0) {
							printf("\nLINE %d: INVALID Instruction\n",icnt);
							*z=0; return 0;
						}
						fseek(finp,-strlen(inp)-1,SEEK_CUR);
						strcpy(inp,inpp);
						printf("\nLine %d: '%s' Invalid Value!\n\nPlease input the value in hexadecimal in range (0,FFFF)\n",icnt,inp);
						*z=0;
					}
				}
			}
		}
	}
	if(strcasecmp(inp,"HLT")!=0) {
		printf("\nCheck whether you have included 'HLT' at the end of your program or your source code is not EMPTY.\n");
		*z=0;
	}
	rewind(finp);
	return icnt;
}

/*---------6. Displays all the Name contents of specified file excluding binary opcodes.------------------------------------------------------------------------------------------------------------------*/

void fprt(FILE *fp) {
	char str[17];
	rewind(fp);
	while(!feof(fp)) {
		fscanf(fp,"%s",str);
		printf("%s\t",str);
		fscanf(fp,"%s",str);
	}
	printf("\n");
}

/*---------7. Checks for errors in register part of instruction.------------------------------------------------------------------------------------------------------------------*/

void regnerr(FILE *finp,FILE *freg,char inp[],int icnt, int* chk) {
	int flag=0;
	char c,stray[17],op[17],org[17];
	strcpy(stray,inp);
	if(((inp[0]=='r'||inp[0]=='R')&&(inp[1]>47&&inp[1]<58))||(strcasecmp(inp,",")==0)||(strcasecmp(inp,".")==0)) {
		c=stray[strlen(stray)-1];
		if (c<48 || c>57) {
			stray[strlen(stray)-1]='\0';
			rewind(freg);
			while(!feof(freg)) {
				fscanf(freg,"%s",op);
				if(strcasecmp(stray,op)==0) {
					fscanf(freg,"%s",op);
					flag=1; break;
				} else {
					fscanf(freg,"%s",op);
					flag=0;
				}
			}
		}
		if (flag==1) {
			printf("\nLine %d: Invalid character found after %s.\n",icnt,stray);
		}
		else if((strcasecmp(inp,",")==0)||(strcasecmp(inp,".")==0)) {
			fseek(finp,-(strlen(inp)+4),SEEK_CUR);
			fscanf(finp,"%s",inp);
			strcpy(org,inp);
			fscanf(finp,"%s",inp);
			printf("\nLine %d: Invalid character found after %s. '%s' not allowed between instructions.\n",icnt,org,inp);
			fchk(finp,freg,inp,op,&flag);
			if(flag==0) regnerr(finp,freg,inp,icnt,chk);
		}
		else {
			printf("\nLine %d: %s Register not found!\n\nAvailable registers are:-\n",icnt,inp);
			fprt(freg);
		}
	}
	else *chk=0;
}

/*---------8. Checks whether given word inp (from Input file) exists in specified file (fop) or not.-------------------------------------------------------------------------------------------------*/

void fchk(FILE *finp,FILE *fop,char inp[],char op[],int* flag) {
	rewind(fop);
	fscanf(finp,"%s",inp);
	while(!feof(fop)) {
		fscanf(fop,"%s",op);
		if(strcasecmp(inp,op)==0) {
			fscanf(fop,"%s",op);
			*flag=1; break;
		} else {
			fscanf(fop,"%s",op);
			*flag=0;
		}
	}
}

/*---------9. Called in 2nd PASS, it prints the binary opcode assigned to specified word inp (from Input file)---------------------------------------------------------------------------------------*/

void fwrt(FILE *fop,FILE *fout,char inp[],char op[]){
	rewind(fop);
	while(!feof(fop)) {
		fscanf(fop,"%s",op);
		if(strcasecmp(inp,op)==0) {
			fscanf(fop,"%s",op);
			if((inp[0]=='r'||inp[0]=='R')&&(inp[1]>47&&inp[1]<58)) fprintf(fout,"%s",op);
			else fprintf(fout,"\n%s",op);
			break;
		}
		else fscanf(fop,"%s",op);
	}
}

/*---------10. Converts given integer into its 16-bit binary format.----------------------------------------------------------------------------------------------------------------------------------*/

void dec2bin(FILE *fop,int k) {
	int l=15;
	char b[16]="0000000000000000";
	while(k>0)
	{
		b[l]=48+k%2;
		k=k>>1;
		l--;
	}
	for(l=0;l<16;l++){
		fprintf(fop,"%c",b[l]);
	}
}

/*-------------------Assembler Program ends here.----------------------------------------------------------------------------------------------------------------------------------------------------*/
