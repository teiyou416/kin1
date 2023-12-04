#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _person person;
struct _person {
  person *next;
  char gender;
  char *name;
  int age;
};

person *phead = NULL, *ptail = NULL; 

void insert(void);
void delete(void);
void printout(void);
void help(void);

int main(int argc, char **argv)
{
  char buf[10];

  while(1) {
    fputs("Command(I/D/P/h)=", stdout);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
      printf("EOF\n");
      break;
    }
    if (strcmp("I\n", buf) == 0)
      insert();
    else if (strcmp("D\n", buf) == 0)
      delete();
    else if (strcmp("P\n", buf) == 0)
      printout();
    else if (strcmp("h\n", buf) == 0)
      help();
    else printf("Unknown command =%s\n", buf);
  }

}

void insert()
{
  char buf[50];
  person *p;

  p = (person *)malloc(sizeof(person));
  p->next= NULL;
  while(1) {
    fputs("Gender(M/F)=", stdout);
    fgets(buf, sizeof(buf), stdin);
    if (strcmp("M\n", buf) == 0) {
      p->gender='M';
      break;
    } if (strcmp("F\n", buf) == 0) {
      p->gender='F';
      break;
    } else printf("Input error: Please input M or F\n");
  }
  while(1) {
    fputs("Name(max 20 bytes)=", stdout);
    fgets(buf, sizeof(buf), stdin);
    if (strlen(buf)>=20) {
      printf("Input error: Over 20 bytes\n");
      continue;
    }
    p->name=malloc(strlen(buf));
    strncpy(p->name, buf, strlen(buf));
    *(p->name+(strlen(buf)-1))='\0';
    break;
  }
  while(1) {
    fputs("Age(integer)=", stdout);
    fgets(buf, sizeof(buf), stdin);
    p->age=atoi(buf);
    if(p->age >= 0 && p->age < 300) break;
    printf("Input error: age is from 0 to 299\n");
  }

  if (phead == NULL) {
    phead=p;
  } else {
    ptail->next=p;
  }
  ptail=p;

  printf("++++++Inserted++++\n");

  return;

}
void printout()
{ person *p;
for(p=phead;p!=NULL;p=p->next){
  printf("%c,%s,%d\n",p->gender,p->name,p->age);
}return;
}

void delete()
{
    char targetName[50];
    printf("Enter name to delete: ");
    fgets(targetName, sizeof(targetName), stdin);
    targetName[strcspn(targetName, "\n")] = '\0'; 

    person *p = phead;
    person *prev = NULL;

    while (p != NULL) {
        if (strcmp(p->name, targetName) == 0) {
            if (prev == NULL) {
                phead = p->next;
            } else {
                prev->next = p->next;
            }

            free(p->name);
            free(p);
            printf("Deleted: %s\n", targetName);
            return;
        }

        prev = p;
        p = p->next;
    }

    printf("Name not found: %s\n", targetName);



} 
void help()
{
  printf("\"I\" is insert data.\n \"D\" is delete data.\n \"P\" is print stored data.\n");

  return;
}

