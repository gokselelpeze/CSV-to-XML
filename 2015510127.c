#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *inputFile;
const char *outputFile;
char *seperator;
char *opsys;
char seperOpsys[10];  // I had to concat seperator and opsys for getting specified column in getColumn part, so I put that unique name
int elementCount = 0;
// Getting specified column
char* getColumn(char* line, int num)
{
    char* token;
    for (token = strtok(line, seperator); token && token; token = strtok(NULL, seperOpsys))
    {
        if (!--num)
            return token;
    }
    return NULL;
}
// For learning the number of columns
int getColumnCount(char* line)
{
    int count = 0;
    const char* token;
    for (token = strtok(line, seperator); token && token; token = strtok(NULL, seperOpsys))
    {
        count++;
    }
    return count;
}

int main(int argc, char * argv[])
{
    // Help menu
    if (strcmp(argv[1],"-h") == 0)
    {
        printf("--------------------------------------------------------------------------------------------------------------------\n");
        printf("Hi, Welcome to CSV to XML converter. You can convert your CSV file to XML file with this following format.\n");
        printf("Input format must be like this: <inputfile> <outputfile> <separator> <P1> <-opsys> <P2>\n");
        printf("For example: CVS2XML Contacts.csv Contacts.xml -separator 1 -opsys 1");
        printf("P1 and P2 values can have the following values:\nP1 may be 1 = Comma, 2 = Tab or 3 = Semicolon\nP2 may be 1 = Windows, 2 = Linux or 3 = MacOs\n");
        printf("--------------------------------------------------------------------------------------------------------------------");
        exit (1);
    }
    // Convert case
    else{
        inputFile = argv[1];
        outputFile = argv[2];
        
        // SEPERATORS
        // comma
        if (atoi(argv[4]) == 1)
            seperator = ",";
        //Tab
        else  if (atoi(argv[4]) == 2)
            seperator = "\t";
        // Semicolon
        else  if (atoi(argv[4]) == 3)
            seperator = ";";
        
        // OPSYS 
        // Windows
        if (atoi(argv[6]) == 1)
            opsys = "\r\n";
        // Linux
        else  if (atoi(argv[6]) == 2)
            opsys = "\n";
        // Mac
        else  if (atoi(argv[6]) == 3)
            opsys = "\r";
    }  

    // I had to concat seperator and opsys for getting specified column in getColumn part, so I put that unique name
    strcat(seperOpsys, "");
    strcat(seperOpsys, seperator);
    strcat(seperOpsys, opsys);
 
    // For reading csv file
    FILE *stream = fopen(inputFile, "r");
    char line[1024];
   
    // For writing xml file
    FILE *fp;
    fp = fopen (outputFile,"w");

    if (stream == NULL)
    {
        printf("Check your file's name please.\n");
        exit(1);
    }
   
    // Getting First Line
    fgets(line, 1024, stream);
    char* firstLine = strdup(line);
   
    // Getting element Count from firstLine
    elementCount = getColumnCount(firstLine);

    // Cretating an array to keep column headers
    const char *headers[elementCount];

    for (size_t i = 1; i <= elementCount; i++)
    {
        firstLine = strdup(line);
        char* element = getColumn(firstLine, i);
        for (size_t i = 0; element[i] != '\0'; i++) {
            // Space to underline
            if (element[i] == ' ') {
                element[i] = '_';
                
            }
            // element to lower
            element[i] = tolower(element[i]);
        }
        
        headers[i - 1] = strdup(element);
    }
    // Encoding style
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    // Root
    fprintf (fp, "<%s>\n", inputFile);
    free(firstLine);
    
    int id=1;

    while (fgets(line, 1024, stream))
    {
        int i = 1;
        fprintf (fp, "\t<row id=\"%d\">\n",id);
        
        while (i <= elementCount)
        {   
            char* tmp = strdup(line);
            // Taking variables 1 by 1 in every loop
            char* item = getColumn(tmp, i);
            // If variable is empty
            if(item == NULL){
                // If it is not last variable
                if(i != elementCount)
                    fprintf (fp, "\t\t<%s></%s>\t\n",headers[i - 1],headers[i - 1]);
                // If it is last variable
                else
                    fprintf (fp, "\t\t<%s></%s>\t",headers[i - 1],headers[i - 1]);
            }
            // If variable is not empty
            else{
                // If it is not last variable
                if(i != elementCount)
                    fprintf (fp, "\t\t<%s>%s</%s>\t\n",headers[i - 1],item,headers[i - 1]);
                // If it is last variable
                else
                    fprintf (fp, "\t\t<%s>%s</%s>\t",headers[i - 1],item,headers[i - 1]);
            }
            free(tmp);
            i++;
        }
        fprintf(fp, "\n");
        id++;
        fprintf (fp, "\t</row>\n");

    }
    //End root
    fprintf (fp, "</%s>\n", inputFile);
    
    // Close file pointers
    fclose (fp);
    fclose (stream);
    printf("%s file is converted to %s file", inputFile, outputFile);
    return 0;
}