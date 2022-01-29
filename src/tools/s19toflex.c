
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

unsigned int getNibble(char chr){
    if (chr >= '0' && chr <= '9') {
        chr = chr-'0';
    } 
    else{
        chr = toupper(chr);
        if (chr >= 'A' && chr <= 'F')
            chr = chr - 'A' + 10;
        else
            return -1;
    } 
    return chr;
}

unsigned int getByte(char *str){
    return (getNibble(str[0])<<4) | getNibble(str[1]);
}

unsigned int getAddress(char *str){
    return getByte(str)<<8 | getByte(str+2);
}

bool checkSumOk (char *str){
    
    long unsigned int checkSum = 0;

    for(unsigned int i=1; i<(strlen(str)/2-1); i++)
        checkSum += getByte(str+i*2);

    checkSum = (~checkSum) & 0x000000ff;
    return checkSum == getByte(str+strlen(str)-2);
}

int main (int argc, char *argv[]) {
    if(argc != 3){
        printf("usage s19toflex <s19-file> <flex-bin-file>\n");
        printf("Tool reads a 16-bit S-record text file and converts it to FLEX bin-File\n");
        printf("A S1-record is converted to 0x02, ADDRH, ADDRL, LEN, DATxx\n");
        printf("A S9-record is converted to 0x16, ADDRH, ADDRL\n");
        printf("          if no S9-record is found the transfer address is set to the first S1 address\n");
        printf("The output-file is filled to multiples of 252 with 0x00 to fill for complete FLEX-sectors\n");
        return 1;
    }

    FILE *infile = fopen(argv[1], "r");
    if(!infile){
        printf("Cannot open '%s'\n", argv[1]);
        return 1;
    }

    FILE *outfile = fopen(argv[2], "w");
    if(!outfile){
        printf("Cannot open '%s'\n", argv[2]);
        return 1;
    }

    unsigned int lineCount;
    unsigned int transferAddress;
    unsigned int actualAddress = 0x0000;

    lineCount = 0;

#define BUFLEN (256)
#define TRANSFER (0x0000)
    char lineBuffer[BUFLEN];
    unsigned int byteCount=0;

    bool finished = false;

    while(!finished){
        if(!fgets(lineBuffer, BUFLEN, infile)){
            finished=true;
        }
        else{
            lineCount++;
            if(strlen(lineBuffer)>0)
                if(lineBuffer[strlen(lineBuffer)-1]=='\n')
                    lineBuffer[strlen(lineBuffer)-1]='\0';

            if(strlen(lineBuffer)>0)
                if(lineBuffer[strlen(lineBuffer)-1]=='\r')
                    lineBuffer[strlen(lineBuffer)-1]='\0';
                    
            if(strlen(lineBuffer) < 10){
                printf("Line %u has illegal content %s (too short), aborted\n", lineCount, lineBuffer);
                fclose(infile);
                fclose(outfile);
                return 1;
            }
            if(lineBuffer[0]!='S'){
                printf("Line %u has illegal content %s (no S-record), aborted\n", lineCount, lineBuffer);
                fclose(infile);
                fclose(outfile);
                return 1;
            }
            if(strlen(lineBuffer) % 2 != 0){
                printf("Line %u has illegal content %s (not even length), aborted\n", lineCount, lineBuffer);
                fclose(infile);
                fclose(outfile);
                return 1;
            }
            if((strlen(lineBuffer)-4)/2 != getByte(lineBuffer+2)){
                printf("Line %u has illegal content %s (length not matching, %lu vs %u), aborted\n", lineCount, lineBuffer, (strlen(lineBuffer)-4)/2, getByte(lineBuffer+2));
                fclose(infile);
                fclose(outfile);
                return 1;
            }

            if(checkSumOk(lineBuffer)){
                if(lineBuffer[1]=='1'){
                    actualAddress = getAddress(lineBuffer+4);
                    if(lineCount==1)
                        transferAddress = actualAddress;
                    fputc(0x02, outfile);
                    fputc((actualAddress & 0x0000ff00) >> 8, outfile);
                    fputc((actualAddress & 0x000000ff), outfile);
                    fputc(strlen(lineBuffer)/2 - 5, outfile);
                    byteCount += 4;

                    for(unsigned int i=0; i<(strlen(lineBuffer)/2)-5; i++){
                        fputc(getByte(lineBuffer+(i+4)*2), outfile);
                        byteCount++;
                    }
                }
                else if(lineBuffer[1]=='9'){
                    transferAddress = getAddress(lineBuffer+4);
                    if(lineCount==1)
                        transferAddress = actualAddress;
                    fputc(0x16, outfile);
                    fputc((transferAddress & 0x0000ff00) >> 8, outfile);
                    fputc((transferAddress & 0x000000ff), outfile);
                    byteCount += 3;
                }
                else{
                    printf("Line %u has illegal content %s (unsupported S-record), aborted\n", lineCount, lineBuffer);
                    fclose(infile);
                    fclose(outfile);
                    return 1;                    
                }
            }
            else{
                printf("Line %u has illegal checksum %s, aborted\n", lineCount, lineBuffer);
                fclose(infile);
                fclose(outfile);
                return 1;
            }
        }
    }
    for(unsigned int i=0; i< 252 - (byteCount % 252); i++)
        fputc(0x00, outfile);

    fclose(infile);
    fclose(outfile);
    printf("Successfully read %u lines, written %u bytes and last transfer address %04x\n", lineCount, byteCount, transferAddress);
    return 0;
}