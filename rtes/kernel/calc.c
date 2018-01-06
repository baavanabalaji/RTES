#include <linux/calc.h>
//#include <linux/string.h>
//#include <linux/syscalls.h>
//#define MAX_CHAR_LENGTH 64

void rtes_appendZeros(char *s, int n)
{
    int i;
    for(i=0;i<n;i++){
        strcat(s,"0");
    }
}

void rtes_prependZeros(char *str, int n)
{
    int i;
    char s[MAX_CHAR_LENGTH] = "\0";
    char s1[MAX_CHAR_LENGTH] = "\0";

    for(i=0;i<n;i++){
        strcat(s,"0");
    }
    
    sprintf(s1,"%s%s",s,str);

    strcpy(str,s1);
}

char *rtes_strrev(char *str)
{
    int i = strlen(str)-1,j=0;

    char ch;
    while(i>j)
    {
        ch = str[i];
        str[i]= str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
}

void rtes_placeDecimal(char *str, int i)
{  

   int l = strlen(str);
   int j = l-i;
    
    if(j<0){

        if(strcmp(str,"0") == 0){
            strcat(str,".0");
            return;
        }
        rtes_prependZeros(str,-j);
        l = strlen(str);
        j = l-i;
        
    }

   if(l == i){

      i++;
      memmove (str+j+2,str+j,i+1);
      str[j] = '0';
      str[j+1] = '.';
   }
   else{
      memmove (str+j+1,str+j,i+1);
      str[j] = '.';
   }
}

void rtes_removeCharacter(char *str,int i)
{
   int l = strlen(str);
   memmove (str+i,str+i+1,l-i);
}

int rtes_findIndex(char *str,char c)
{
    char *subStr = strchr(str,c);
    if(subStr == NULL)
    {  
       strcat(str,".0"); 
       subStr = strchr(str,c);
    }
    return subStr-str;
}

int rtes_resultPower(char op,int i1, int i2)
{
    int result = 0;
    if((op == '+') || (op == '-')){
        result = i2;
    }
    else if(op == '*'){
        result = i1+i2;
    }
    else if(op == '/'){
        result = 0;
    }
    return result;
}

int rtes_equalizeFracLengths(char *str1,char *str2, char op)
{
  int i1 = rtes_findIndex(str1,'.');
  int i2 = rtes_findIndex(str2,'.');  

	if(i1 == 0){
    rtes_prependZeros(str1,1);
    i1++;
  }
  if(i2 == 0){
    rtes_prependZeros(str2,1);
    i2++;
  }

  int numFrac1 = strlen(str1)-i1-1;
  int numFrac2 = strlen(str2)-i2-1;

    if(i1>0)   
    rtes_removeCharacter(str1,i1);
    if(i2>0)   
     rtes_removeCharacter(str2,i2);
     
  if(numFrac1>numFrac2){
    rtes_appendZeros(str2,numFrac1-numFrac2);
    numFrac2 = numFrac1;  
  }
  else{
    rtes_appendZeros(str1,numFrac2-numFrac1);
    numFrac1 = numFrac2;  
  }
  
  return rtes_resultPower(op,numFrac1,numFrac2);
}

void rtes_prettifyResult(char *str)
{  
    int l = strlen(str);
    int i = l-1;
    if(strchr(str,'.') != NULL){
       
       while(str[i] == '0'){
           str[i] = str[i+1];
           i--;
       }
       l = strlen(str);
       if(str[l-1] == '.')
            str[l-1] = str[l];
    }  
}


int rtes_equalizeStringLengths(char *str1,char *str2)
{

    int l1 = strlen(str1);
    int l2 = strlen(str2);
    int L = l1;
         
    if(l1>l2){
        rtes_prependZeros(str2,l1-l2);
        L = l1;
    }
    else if(l1<l2){
        rtes_prependZeros(str1,l2-l1);
        L = l2;  
    }
  
    return L;
}




void rtes_add(char *str1, char *str2, char *result)
{
    int l1,l2,L;
    
   
    if(strcmp(str1,"") == 0){
        strcpy(result,str2);
        return;    
    }
    if(strcmp(str2,"") == 0){
        strcpy(result,str1);
        return;    
    }
    
    l1 = strlen(str1);
    l2 = strlen(str2);
    

    L = rtes_equalizeStringLengths(str1,str2);

    
    long x = 0;
    long y = 0;
    long r = 0;
    long c = 0;
    char str3[MAX_CHAR_LENGTH];
    int k = 0;
    int i = 0;
    
    for(i=L-1;i>=0;i--){
    
        x = str1[i]-48;
        y = str2[i]-48;
        r = x+y+c;
        
        c = r/10;
        r = r%10;

        str3[k] = '0'+r;
        k++;
        
        
        if(k>MAX_CHAR_LENGTH){
            printk("overflowing");
            return;
        }
    }
    
    if(c != 0){
        str3[k] = '0'+c;
        c = 0;
        k++;
    }

    str3[k] = '\0';
    rtes_strrev(str3);
    strcpy(result,str3);
    rtes_trimLeadingZeros(result);
}

void rtes_subtract(char *str1, char *str2, char *result)
{
    int l1,l2,L;
    char temp[MAX_CHAR_LENGTH] = "";
        
    if(strcmp(str1,"") == 0){
        strcpy(result,str2);
        return;    
    }
    if(strcmp(str2,"") == 0){
        strcpy(result,str1);
        return;    
    }
    
    l1 = strlen(str1);
    l2 = strlen(str2);
    L = rtes_equalizeStringLengths(str1,str2);
    
    if(strcmp(str1,str2) < 0){
        
        strcpy(temp,str1);
        strcpy(str1,str2);
        strcpy(str2,temp);
        strcpy(temp,"-");
    }


    long x = 0;
    long y = 0;
    long r = 0;
    long b = 0;
    char str3[MAX_CHAR_LENGTH];
    int k = 0;
    int i = 0;

    
    for(i=L-1;i>=0;i--){
    
        x = str1[i]-48;
        y = str2[i]-48;
        r = x-y-b;
        if(r<0){
            r += 10;
            b = 1;
        }
        else
            b = 0;
        str3[k] = '0'+r;

        k++;
        
        if(k>MAX_CHAR_LENGTH){
            printk("overflowing");
            return;
        }
    }
    
    str3[k] = '\0';
    rtes_strrev(str3);

    strcpy(result,str3);
    rtes_trimLeadingZeros(result);
}



void rtes_multiply(char *str1, char *str2, char *result)
{
    int l1 = strlen(str1);
    int l2 = strlen(str2);
    int i,j;
    long x = 0;
    long y = 0;
    long r = 0;
    long c = 0;
    char str3[MAX_CHAR_LENGTH];
    int k = 0;
    int l = 0;
    char *temp;
    strcpy(result,"\0");
    
    for(j=l2-1;j>=0;j--){
        
        y = str2[j]-48;
        
        for(i=l1-1;i>=0;i--){
            x = str1[i]-48;
            r = x*y+c;
            
            c = r/10;
            r = r%10;
            
            str3[k] = '0'+r;
            k++;
            if(k>MAX_CHAR_LENGTH){
                printk("overflowing");
                return;
            }
            
        }
        if(c != 0){
            str3[k] = '0'+c;
            c = 0;
            k++;
        }
        str3[k] = '\0';
        rtes_strrev(str3);
        rtes_add(result,str3,result);

        strcpy(str3,"");
        rtes_appendZeros(str3,l2-j);

        k=l2-j;
    }

    rtes_trimLeadingZeros(result);
}

int rtes_isAllZeros(char *str){
    
    int l = strlen(str);
    int i;
    
    for(i=0;i<l;i++){
        if(str[i] != '0')
            return 0;
    }
    return 1;
}

void rtes_trimLeadingZeros(char *str){
    
    int l = strlen(str);
    int i;
    
    for(i=0;i<l;i++){
        if(str[i] != '0')
            break;
    }
    memmove(str,str+i,l-i+1);
    
}
void rtes_divide(char *str1, char *str2, char *result){
    
    char quotient[MAX_CHAR_LENGTH] = "0";
   
    char tempFrac[MAX_CHAR_LENGTH] = "0";
    char frac[MAX_CHAR_LENGTH] = ".";

    char inc[MAX_CHAR_LENGTH] = "1";
    int precision = 7;
    int x;
    strcpy(result,str1);
    rtes_equalizeStringLengths(result,str2);

    while(strcmp(result,str2)>=0){
        rtes_subtract(result,str2,result);
        rtes_add(quotient,inc,quotient);
        rtes_equalizeStringLengths(result,str2);

    }
    

    if(rtes_isAllZeros(result)){
        strcpy(result,quotient);
        return;
    }
    
    while(precision>0){
        
        rtes_trimLeadingZeros(result);
        rtes_trimLeadingZeros(str2);
 
        rtes_appendZeros(result,1); 
        rtes_equalizeStringLengths(result,str2);

        while(strcmp(result,str2)>=0){
            rtes_subtract(result,str2,result);
            rtes_add(tempFrac,inc,tempFrac);
            rtes_equalizeStringLengths(result,str2);
        }
            
            if(strlen(tempFrac)>1)
                rtes_trimLeadingZeros(tempFrac);
        
            strcat(frac,tempFrac);
            strcpy(tempFrac,"0");
            
            
            if(rtes_isAllZeros(result))
                break;
        precision--;
    }
    strcpy(result,"");
    sprintf(result,"%s%s",quotient,frac);
}
void rtes_ceil(char *str){
    
    int i;
    char *subStr;
    char inc[MAX_CHAR_LENGTH] = "1";
    char result[MAX_CHAR_LENGTH];
    
    subStr = strchr(str,'.');
    if(subStr == NULL)
        return;
    i = subStr-str;
    str[i] = '\0';
    rtes_add(str,inc,result);
    strcpy(str,result);
}

int rtes_strcmp(char *str1,char *str2){
    
    char s1[MAX_CHAR_LENGTH] = "1";
    char s2[MAX_CHAR_LENGTH] = "1";
    char *subStr1,*subStr2;
    int i1,i2,l1,l2,d;
    
    strcpy(s1,str1);
    strcpy(s2,str2);
    
    rtes_trimLeadingZeros(s1);
    rtes_trimLeadingZeros(s2);
    
    l1 = strlen(s1);
    l2 = strlen(s2);
    
    subStr1 = strchr(str1,'.');
    if(subStr1 == NULL)
        i1 = l1;
    else
        i1 = subStr1-str1;
    
    subStr2 = strchr(str2,'.');
    if(subStr2 == NULL)
        i2 = l2;
    else
        i2 = subStr2-str2;
    
    d = i1-i2;
    
    if(d > 0)
       return 1;
    if(d < 0)
        return -1;
        
    rtes_equalizeFracLengths(s1,s2,'-');
    return strcmp(s1,s2);
}

void rtes_calculateResult(char *str1,char *str2,char op,char *result)
{
     char str1_reset[MAX_CHAR_LENGTH] = "";
    char str2_reset[MAX_CHAR_LENGTH] = "";
    
    strcpy(str1_reset, str1);
    strcpy(str2_reset, str2);

    int pow = rtes_equalizeFracLengths(str1,str2,op);
    rtes_equalizeStringLengths(str1,str2);
    
    if(op == '+'){
        rtes_add(str1,str2,result);
        rtes_placeDecimal(result, pow);
        rtes_prettifyResult(result);
    }
    else if(op == '-'){
        rtes_subtract(str1,str2,result);
        rtes_placeDecimal(result, pow);
        rtes_prettifyResult(result);
    }
    else if(op == '*'){
        rtes_multiply(str1,str2,result);
        rtes_placeDecimal(result, pow);
        rtes_prettifyResult(result);
    }
    else if(op == '/'){
        rtes_divide(str1,str2,result);
    }

    strcpy(str1, str1_reset);
    strcpy(str2, str2_reset);

}

asmlinkage long sys_calc(const char* param1, const char* param2, char operation, char* result)
{
    rtes_calculateResult(param1,param2,operation,result);
    printk("result in kernel space: %s",result);
    return 0;
}
