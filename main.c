#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

typedef struct{
    char courseName[64];
    char courseSched[4];
    unsigned int courseHours;
    unsigned int courseSize;
} COURSE;

void readCourse(char *fileNm){
    int courseNum;
    COURSE course;
    struct stat statbuf;
    printf("Enter a CS course number:\n");
    scanf("%d%*c", &courseNum);
    stat(fileNm, &statbuf);
    if (courseNum * sizeof(COURSE) > statbuf.st_size){
        printf("ERROR: course not found\n");
        return;
    }
    FILE *data = fopen(fileNm, "rb");
    fseek(data, (courseNum * sizeof(COURSE)), SEEK_SET);
    fread(&course, sizeof(COURSE), 1L, data);
    fclose(data);
    if (course.courseName[0] == 0){
        printf("ERROR: course not found\n");
        return;
    }
    printf("Course number:  %d\n", courseNum);
    printf("Course name:  %s\n", course.courseName);
    printf("Schedule days:  %s\n", course.courseSched);
    printf("Credit hours:  %d\n", course.courseHours);
    printf("Enrolled Students:  %d\n", course.courseSize);
}

void create(char *fileNm){
    int courseNum;
    unsigned int creditHours, enrollment;
    char name[64], schedule[4];
    COURSE course;
    struct stat statbuf;
    stat(fileNm, &statbuf);
    scanf("%d%*c", &courseNum);
    if (courseNum * sizeof(COURSE) > statbuf.st_size){
        scanf("%[^\n]s%*c", name);
        scanf("%s%*c", schedule);
        scanf("%u%*c", &creditHours);
        scanf("%u%*c", &enrollment);
        strcpy(course.courseName, name);
        strcpy(course.courseSched, schedule);
        course.courseHours = creditHours;
        course.courseSize = enrollment;
        FILE *data = fopen(fileNm, "rb+");
        fseek(data, (courseNum * sizeof(COURSE)), SEEK_SET);
        fwrite(&course, sizeof(COURSE), 1L, data);
        fclose(data);
        return;
    }
    FILE *data = fopen(fileNm, "rb+");
    fseek(data, (courseNum * sizeof(COURSE)), SEEK_SET);
    fread(&course, sizeof(COURSE), 1L, data);
    if (course.courseName[0] != 0){
        printf("ERROR: course already exists\n");
        fclose(data);
        return;
    }
    scanf("%[^\n]s%*c", name);
    scanf("%s%*c", schedule);
    scanf("%u%*c", &creditHours);
    scanf("%u%*c", &enrollment);
    strcpy(course.courseName, name);
    strcpy(course.courseSched, schedule);
    course.courseHours = creditHours;
    course.courseSize = enrollment;
    fseek(data, -sizeof(COURSE), SEEK_CUR);
    fwrite(&course, sizeof(COURSE), 1L, data);
    fclose(data);
}

void update(char *fileNm){
    
    int courseNum;
    unsigned int creditHours, enrollment;
    char name[64], schedule[5], hours[3], enroll[5];
    COURSE course;
    struct stat statbuf;
    stat(fileNm, &statbuf);
    scanf("%d%*c", &courseNum);
    if (courseNum * sizeof(COURSE) > statbuf.st_size){
        printf("ERROR: course not found\n");
        return;
    }
    FILE *data = fopen(fileNm, "rb+");
    fseek(data, (courseNum * sizeof(COURSE)), SEEK_SET);
    fread(&course, sizeof(COURSE), 1L, data);
    if (course.courseName[0] == 0){
        printf("ERROR: course not found\n");
        fclose(data);
        return;
    }
    fgets(name, 65, stdin);
    fgets(schedule, 5, stdin);
    fgets(hours, 3, stdin);
    fgets(enroll, 5, stdin);
    if (name[0] != '\n'){
        name[strlen(name)-1] = '\0';
        strcpy(course.courseName, name);
    }
    if (schedule[0] != '\n'){
        schedule[strlen(schedule)-1] = '\0';
        strcpy(course.courseSched, schedule);
    }
    if (hours[0] != '\n'){
        sscanf(hours, "%u", &creditHours);
        course.courseHours = creditHours;
    }
    if (enroll[0] != '\n'){
        sscanf(enroll, "%u", &enrollment);
        course.courseSize = enrollment;
    }
    fseek(data, -sizeof(COURSE), SEEK_CUR);
    fwrite(&course, sizeof(COURSE), 1L, data);
    fclose(data);
}

void delete(char *fileNm){
    int courseNum;
    char zero = 0;
    COURSE course;
    struct stat statbuf;
    stat(fileNm, &statbuf);
    printf("Enter a course number: \n");
    scanf("%d%*c", &courseNum);
    if (courseNum * sizeof(COURSE) > statbuf.st_size){
        printf("ERROR: course not found\n");
        return;
    }
    FILE *data = fopen(fileNm, "rb+");
    fseek(data, (courseNum * sizeof(COURSE)), SEEK_SET);
    fread(&course, sizeof(COURSE), 1L, data);
    if (course.courseName[0] == 0){
        printf("ERROR: course not found\n");
        fclose(data);
        return;
    }
    fseek(data, -sizeof(COURSE), SEEK_CUR);
    fwrite(&zero, 1, 76L, data);
    fclose(data);
    printf("%d was successfully deleted.\n", courseNum);
}

int main(int argc, char* argv[]){
    char prompt[1000] = "Enter one of the following actions or press CTRL-D to exit.\n"
    "C - create a new course record\n"
    "R - read an existing course record\n"
    "U - update an existing course record\n"
    "D - delete an existing course record\n";
    printf("%s", prompt);
    char ch[1000];
    while (fgets(ch, 1000, stdin)){
        if (strlen(ch) > 2){
            printf("ERROR: invalid option\n");
            printf("%s", prompt);
            continue;
        }
        char c = toupper(ch[0]);
        switch(c){
            case 'C':
                create(argv[1]);
                break;
            case 'R':
                readCourse(argv[1]);
                break;
            case 'U':
                update(argv[1]);
                break;
            case 'D':
                delete(argv[1]);
                break;
            default:
                printf("ERROR: invalid option\n");
        }
        printf("%s", prompt);
    }
    return 0;
}
