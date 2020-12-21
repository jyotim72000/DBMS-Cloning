	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <regex.h>
	#include <dirent.h>
	#include <errno.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	
	const char *r1 = "use [a-zA-Z_][a-zA-Z0-9_]+;$";
	const char *r2 = "show databases;$";
	const char *r3 = "create database [a-zA-Z_][a-zA-Z0-9_]+;$";
	const char *r4 = "create table [a-zA-Z_][a-zA-Z0-9_]+ ?\\(([a-zA-Z_][a-zA-Z0-9_]+ (int|string|double)\\,? ?)+\\);$";
	const char *r5 = "insert into [a-zA-Z_][a-zA-Z0-9_]+ values ?\\(([a-zA-Z0-9_\\.]+\\,?)+\\);$";
	const char *r6 = "show tables;$";
	const char *r7 = "select \\* ?from [a-zA-Z_][a-zA-Z0-9_]+;$";
	const char *r8 = "drop table [a-zA-Z_][a-zA-Z0-9_]+;$";
	
	struct key
	{
	    char *input;
	    struct key *next;
	}*head=NULL;
	
	char database[10] = "default";
	
	void createDatabase(char *d)
	{
	    chdir("database");
	    int check = mkdir(d,0700);//0 success -1 fail
	    chdir("..");
	    if(!check)
	    {
	        printf("%s Created\n",d);
	    }
	    else
	    {
	        printf("Database already exists\n");
	        return;
	    }
	    
	}
	
	void openDatabase(char *d)
	{
	    chdir("database");
	    DIR *check = opendir(d);//traversing
	    chdir("..");
	    if(check)
	    {
	        closedir(check);
	        strcpy(database,d);
	        printf("Switched to %s\n",database);
	    }
	    else
	    {
	        printf("%s does not exist\n",d);
	    }
	}
	
	void showDatabases()
	{
	    chdir("database");
	    struct dirent *dir;
	    DIR *d = opendir(".");
	    chdir("..");
	    if(d)
	    {
	        while((dir=readdir(d)) != NULL)
	        {
	            printf("%s\n",dir->d_name);
	        }
	        //close(d);
	    }
	    else
	    {
	        printf("No database created\n");
	    }
	}
	
	void createTable(struct key *K)
	{
	    chdir("database");
	    chdir(database);
	    FILE *fp;
	    if(access(K->input,F_OK) == -1)
	    {
	        fp = fopen(K->input,"w");
	        K = K->next;
	        while(K != NULL)
	        {
	            fputs(K->input,fp);
	            K = K->next;
	            if(K != NULL)
	                fputc(',',fp);
	        } 
	        fclose(fp);
	        printf("Table created\n");
	    }
	    else
	    {
	        printf("Table already exists\n");
	    }
	    chdir("..");
	    chdir("..");
	}
	
	void showTables()
	{
	    chdir("database");
	    chdir(database);
	    struct dirent *dir;
	    DIR *d = opendir(".");
	    if(d != NULL)
	    {
	        while((dir=readdir(d)) != NULL)
	        {
	            printf("%s\n",dir->d_name);
	        }
	        //close(d);
	    }
	    else
	    {
	        printf("No tables created\n");
	    }
	    chdir("..");
	    chdir("..");
	}
	
	void deleteTable(char *t)
	{
	    chdir("database");
	    chdir(database);
	    if(remove(t) == 0)
	    {
	        printf("Table deleted\n");
	    }
	    else
	    {
	        printf("Table does not exist\n");
	    }
	    chdir("..");
	    chdir("..");
	}
	
	int validateData(struct key *K)
	{
	    
	    FILE *fp;
	    fp = fopen(K->input,"r");
	    char s[100];
	    fgets(s,100,fp);
	    fclose(fp);
	    K = K->next->next;
	    char *t = strtok(s,",");
	    int i=0;
	    while(t != NULL)
	    {
	        i++;
	        t= strtok(NULL,",");
	        if(strcmp("int",t)==0)
	        {
	            char p[20];
	            snprintf(p,20,"%d",atoi(K->input));
	            if(strcmp(p,K->input) != 0)
	            {
	                printf("Error: '%s' is not of type 'int'\n",K->input);
	                return 0;
	            }
	        }
	        else if(strcmp("double",t)==0)
	        {
	            char p[20];
	            snprintf(p,20,"%g",atof(K->input));
	            if(strcmp(p,K->input) != 0)
	            {
	                printf("Error: '%s' is not of type 'double'\n",K->input);
	                return 0;
	            }
	        }
	        K = K->next;
	        t= strtok(NULL,",");
	        if(K == NULL && t != NULL)
	        {
	            printf("Error: Less number of attributes are given\n");
	            return 0;
	        }
	    }
	    if(K != NULL)
	    {
	        printf("Error: Table has %d attributes\n",i);
	        return 0;
	    }
	    return 1;
	}
	
	void insertTable(struct key *K)
	{
	    chdir("database");
	    chdir(database);
	    if(access(K->input,F_OK) != -1)
	    {
	        if(validateData(K)==1)
	        {
	            FILE *fp;
	            fp = fopen(K->input,"a");
	            K = K->next->next;
	            fputc('\n',fp);
	            while(K != NULL)
	            {
	                fputs(K->input,fp);
	                K = K->next;
	                if(K != NULL)
	                    fputc(',',fp);
	            } 
	            fclose(fp);
	            printf("1 row inserted\n");
	        }
	    }
	    else
	    {
	        printf("Table does not exist\n");
	    }
	    chdir("..");
	    chdir("..");
	}
	
	void selectAllCol(char *f)
	{
	    chdir("database");
	    chdir(database);
	    if(access(f,F_OK) != -1)
	    {
	        FILE *fp;
	        fp = fopen(f,"r");
	        char s[100];
	        fgets(s,100,fp);
	        char *t = strtok(s,",");
	        while(t != NULL)
	        {
	            printf("\t%s",t);
	            t= strtok(NULL,",");
	            t= strtok(NULL,",");
	        }
	        printf("\n");
	        int i=0;
	        while(fgets(s,100,fp) != NULL)
	        {
	            t = strtok(s,",");
	            while(t != NULL)
	            {
	            printf("\t%s",t);
	            t= strtok(NULL,",");
	            }
	            i++;
	        }
	        printf("\n%d row(s) selected\n",i);
	        fclose(fp);
	    }
	    else
	    {
	        printf("Table does not exist\n");
	    }
	    chdir("..");
	    chdir("..");
	}
	
	void selectTable(struct key *K)
	{
	    if (strcmp("from",K->input)==0)
	    {
	        K = K->next;
	        selectAllCol(K->input);
	    }
	}
	
	char query[100];
	int match(const char *string, const char *pattern)
	{
	    regex_t re; 
	    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0; //Use Extended Regular Expressions. //Report only success/fail in regexec()
	    int status = regexec(&re, string, 0, NULL, 0);//regular expression matching
	    regfree(&re);
	    if (status != 0) return 0;
	    return 1;
	}
	
	int checkSyntax()
	{
	    char *p = &query[0];
	    if(match(p,r1)==1)
	        return 1;
	    else if(match(p,r2)==1)
	        return 1;
	    else if(match(p,r3)==1)
	        return 1;
	    else if(match(p,r4)==1)
	        return 1;
	    else if(match(p,r5)==1)
	        return 1;
	    else if(match(p,r6)==1)
	        return 1;
	    else if(match(p,r7)==1)
	        return 1;
	    else if(match(p,r8)==1)
	        return 1;
	    else
	        return 0;
	}
	
	void storeKeywords(char *p)
	{
	    struct key *K = (struct key*)malloc(sizeof(struct key));
	    K->input=p;
	    K->next = NULL;
	    if(head == NULL)
	        head = K;
	    else
	    {
	        struct key *t = head;
	        while(t->next != NULL)
	            t = t->next;
	        t->next = K;  
	    }
	}
	
	void findKeywords()
	{
	    char *t = strtok(query," (),;*");
	    while(t != NULL)
	    {
	        storeKeywords(t);
	        t= strtok(NULL," (),;*");
	    }
	}
	
	void processQuery()
	{
	    struct key *K = head;
	    if(strcmp("use",K->input)==0)
	    {
	        K = K->next;
	        openDatabase(K->input);
	    }
	    else if(strcmp("show",K->input)==0)
	    {
	        K = K->next;
	        if (strcmp("databases",K->input)==0)
	            showDatabases();
	        else if (strcmp("tables",K->input)==0);
	            showTables();
	    }
	    else if(strcmp("drop",K->input)==0)
	    {
	        K = K->next;
	        deleteTable(K->next->input);
	    }
	    else if(strcmp("create",K->input)==0)
	    {
	        K = K->next;
	        if(strcmp("database",K->input)==0)
	        {   
	            K = K->next;
	            createDatabase(K->input);
	        }
	        else if(strcmp("table",K->input)==0)
	        {   
	            K = K->next;
	            createTable(K);
	        }
	    }
	    else if(strcmp("insert",K->input)==0)
	    {
	        K = K->next->next;
	        insertTable(K);
	    }
	    else if(strcmp("select",K->input)==0)
	    {
	        K = K->next;
	        selectTable(K);
	    }
	}
	
	void deleteKeywords()
	{
	    struct key *current = head;
	    struct key *next;
	    while(current != NULL)
	    {
	        next = current->next;
	        free(current);
	        current = next;
	    }
	    head = NULL;
	}
	
	int main()
	{
	    int loop = 1;
	    printf("\n\t                              MINOR 1 PROJECT\t\n\n\t                                CLONING OF DBMS\t\n\n");
	    
	    while (loop)
	    {
	        printf("DBMS> ");
	        // step 1
	        gets(query);
	        if(strcmp(query,"exit") == 0)
	            loop=0;
	        else if(strcmp(query,"clear") == 0)
	            system("clear");
	        else
	        {
	            // Step 2
	            if(checkSyntax()==1)
	            {
	                //printf("No Error\n");     
	                findKeywords();
	                // Step 4
	                processQuery();
	                deleteKeywords();
	            }
	            else
	                printf("Syntax Error\n");
	        }
	    }
	    return 0;
	}




